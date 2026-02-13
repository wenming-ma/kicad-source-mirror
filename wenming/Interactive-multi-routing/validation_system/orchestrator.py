"""Orchestration system for the validation battle (async)."""

import json
import os
from enum import Enum
from typing import Dict, Any, List

from claude_agent_sdk import query, ClaudeAgentOptions, ResultMessage
from agents import (
    ResearchAgent,
    CriticAgent,
    SolutionSynthesizerAgent,
    CoordinatorAgent
)
from agents.research_agent import ROUND_DIRECTIONS

# Number of research exploration rounds
RESEARCH_ROUNDS = 5

CHECKPOINT_FILE = "checkpoint.json"

# Maximum battle loop iterations (challenges -> solutions -> review, repeat)
MAX_BATTLE_ITERATIONS = 7

# JSON schema for the convergence-check agent's structured output
CONVERGENCE_SCHEMA = {
    "type": "object",
    "properties": {
        "converged": {
            "type": "boolean",
            "description": "True only if ALL solutions are APPROVE and NO challenges are OPEN",
        },
        "verdicts": {
            "type": "object",
            "properties": {
                "approve": {"type": "number"},
                "revise": {"type": "number"},
                "reject": {"type": "number"},
                "blocked": {"type": "number"},
            },
            "required": ["approve", "revise", "reject", "blocked"],
        },
        "open_challenges": {"type": "number"},
        "reason": {"type": "string"},
    },
    "required": ["converged", "verdicts", "open_challenges", "reason"],
}


class BattleStep(str, Enum):
    """Named steps within a battle iteration.

    Stored in checkpoints so the resume logic is human-readable.
    """
    CHALLENGES = "challenges"
    SOLUTIONS = "solutions"
    REVIEW = "review"


# Step order per iteration type
ITER1_STEPS: List[BattleStep] = [
    BattleStep.CHALLENGES, BattleStep.SOLUTIONS, BattleStep.REVIEW,
]
ITER2_STEPS: List[BattleStep] = [
    BattleStep.SOLUTIONS, BattleStep.REVIEW,
]


class ValidationBattle:
    """Orchestrates the multi-agent validation process."""

    def __init__(self, kicad_repo_path: str, repos_dir: str = None):
        self.kicad_repo_path = os.path.abspath(kicad_repo_path)
        self.repos_dir = os.path.abspath(
            repos_dir or os.path.join(
                os.path.dirname(__file__), "research_repos"
            )
        )
        # Set during execute()
        self.output_dir: str = ""
        self.design_doc_path: str = ""

        self.research = ResearchAgent()
        self.critic = CriticAgent(self.kicad_repo_path)
        self.synthesizer = SolutionSynthesizerAgent()
        self.coordinator = CoordinatorAgent()

        self.agents = {
            "research": self.research,
            "critic": self.critic,
            "synthesizer": self.synthesizer,
            "coordinator": self.coordinator
        }

    def _file_paths(self) -> Dict[str, str]:
        """Return the standard path dict that agents use to locate files."""
        return {
            "output_dir": self.output_dir,
            "design_doc_path": self.design_doc_path,
            "kicad_repo_path": self.kicad_repo_path,
            "research_repos_dir": self.repos_dir,
            "research_agent_md": os.path.join(self.output_dir, "research_agent.md"),
            "critic_md": os.path.join(self.output_dir, "critic.md"),
            "solution_synth_md": os.path.join(self.output_dir, "solution_synth.md"),
            "coordinator_md": os.path.join(self.output_dir, "validation_report.md"),
        }

    def _log(self, msg: str):
        """Print a log message."""
        print(msg)

    def _verify_md_file(self, file_key: str) -> bool:
        """Check that an agent's md file exists and is non-empty."""
        path = self._file_paths()[file_key]
        if os.path.exists(path) and os.path.getsize(path) > 0:
            self._log(f"  [OK] {file_key} written ({os.path.getsize(path)} bytes)")
            return True
        self._log(f"  [WARN] {file_key} not found or empty")
        return False

    async def _check_convergence(self) -> bool:
        """Check if the battle has converged using a lightweight LLM agent.

        Sends the critic report to a haiku model with a JSON schema,
        which returns a structured convergence judgment.  This replaces
        the previous regex approach that was brittle against free-form
        critic output.
        """
        path = self._file_paths()["critic_md"]
        if not os.path.exists(path):
            self._log("  critic_md: file missing, not converged")
            return False

        with open(path, "r", encoding="utf-8") as f:
            critic_content = f.read()

        if not critic_content.strip():
            self._log("  critic_md: empty, not converged")
            return False

        options = ClaudeAgentOptions(
            model="claude-opus-4-6",
            output_format={"type": "json_schema", "schema": CONVERGENCE_SCHEMA},
            permission_mode="bypassPermissions",
            max_turns=1,
        )

        prompt = (
            "Analyze the following critic report and determine if the battle has converged.\n\n"
            "RULES:\n"
            "- converged=true ONLY if every solution verdict is APPROVE and "
            "there are zero OPEN challenges.\n"
            "- REVISE, REJECT, BLOCKED verdicts all mean NOT converged.\n"
            "- Any challenge with Status: OPEN means NOT converged.\n"
            "- Count each verdict type and open challenges exactly.\n\n"
            "CRITIC REPORT:\n"
            f"{critic_content}"
        )

        result = None
        async for msg in query(prompt=prompt, options=options):
            if isinstance(msg, ResultMessage):
                result = msg

        if not result or result.is_error or not result.structured_output:
            self._log("  Convergence agent failed, assuming not converged")
            return False

        output = result.structured_output
        v = output["verdicts"]
        self._log(
            f"  Convergence check: APPROVE={v['approve']} REVISE={v['revise']} "
            f"REJECT={v['reject']} BLOCKED={v['blocked']} "
            f"OPEN={output['open_challenges']}"
        )
        self._log(f"  Reason: {output['reason']}")

        return output["converged"]

    async def run_research_phase(self) -> None:
        """Run 5 rounds of deep research exploration.

        Each round discovers new repos, clones them, and uses Explore
        subagents to analyze code.  The research agent maintains its own
        markdown file (research_agent.md) across rounds.
        """
        print("\n" + "=" * 60)
        print("Research Phase: 5 Exploration Rounds")
        print("=" * 60)

        os.makedirs(self.repos_dir, exist_ok=True)
        research_dir = os.path.join(self.output_dir, "research")
        os.makedirs(research_dir, exist_ok=True)

        studied_repos = []

        for round_num in range(1, RESEARCH_ROUNDS + 1):
            print(f"\n--- Research Round {round_num}/{RESEARCH_ROUNDS} ---\n")

            directions = ROUND_DIRECTIONS.get(round_num, "")

            message = {
                "type": "research_request",
                "round": round_num,
                "total_rounds": RESEARCH_ROUNDS,
                "search_directions": directions,
                **self._file_paths(),
            }

            round_result = await self.research.process(message)

            if isinstance(round_result, dict):
                for repo in round_result.get("repos_analyzed", []):
                    name = repo.get("name", "")
                    if name and name not in studied_repos:
                        studied_repos.append(name)

            self._save_json(
                studied_repos,
                os.path.join(research_dir, "studied_repos.json")
            )

            print(f"Round {round_num} complete. "
                  f"Total repos studied: {len(studied_repos)}")

        print(f"\nResearch phase complete: {len(studied_repos)} repos analyzed "
              f"across {RESEARCH_ROUNDS} rounds")

    async def run_challenges(self, battle_iter: int = 1) -> None:
        """Critic raises initial challenges (iteration 1 only)."""
        print(f"\n=== Initial Challenges ===\n")

        print("Critic reviewing design...")
        await self.critic.process({
            "type": "review_request",
            "battle_iteration": battle_iter,
            **self._file_paths(),
        })
        self._verify_md_file("critic_md")

        print("\nChallenges complete: critic_md written")

    async def run_solutions(self, battle_iter: int = 1) -> None:
        """Generate all solutions in a single batch call."""
        print("\n=== Solution Generation ===\n")

        print("Synthesizer reading critic challenges and generating solutions...")
        await self.synthesizer.process({
            "type": "generate_all_solutions",
            "battle_iteration": battle_iter,
            **self._file_paths(),
        })
        self._verify_md_file("solution_synth_md")

        print("\nSolutions complete")

    async def run_review(self, battle_iter: int = 1) -> None:
        """Critic reviews solutions (single pass).

        The outer battle loop handles iteration; no internal repair loop.
        """
        print("\n=== Solution Review ===\n")

        print("Critic reviewing all solutions...")
        await self.critic.process({
            "type": "review_solutions",
            "battle_iteration": battle_iter,
            **self._file_paths(),
        })
        self._verify_md_file("critic_md")

        print("\nReview complete")

    async def run_battle_loop(self, start_iter: int = 1,
                              resume_after: str | None = None) -> None:
        """Run the battle loop until convergence or max iterations.

        Iteration 1:  challenges -> solutions -> review  (ITER1_STEPS)
        Iteration 2+: solutions -> review                (ITER2_STEPS)

        Args:
            start_iter:    Battle iteration to start from (for resume).
            resume_after:  BattleStep value that was last completed.
                           Steps up to and including this one are skipped
                           on the start_iter only.
        """
        for battle_iter in range(start_iter, MAX_BATTLE_ITERATIONS + 1):
            print(f"\n{'='*60}")
            print(f"Battle Iteration {battle_iter}/{MAX_BATTLE_ITERATIONS}")
            print(f"{'='*60}")

            steps = ITER1_STEPS if battle_iter == 1 else ITER2_STEPS

            # On the resume iteration, figure out which steps to skip
            skip_count = 0
            if battle_iter == start_iter and resume_after:
                try:
                    skip_count = steps.index(BattleStep(resume_after)) + 1
                except ValueError:
                    skip_count = 0  # unknown step, run everything

            for i, step in enumerate(steps):
                if i < skip_count:
                    continue

                if step == BattleStep.CHALLENGES:
                    await self.run_challenges(battle_iter)
                elif step == BattleStep.SOLUTIONS:
                    await self.run_solutions(battle_iter)
                elif step == BattleStep.REVIEW:
                    await self.run_review(battle_iter)

                self._save_checkpoint(
                    stage="battle", battle_iteration=battle_iter,
                    last_step=step.value)

            print("\nChecking convergence...")
            if await self._check_convergence():
                print(f"\nBattle converged at iteration {battle_iter}!")
                break
        else:
            print(f"\nMax battle iterations ({MAX_BATTLE_ITERATIONS}) reached")

    async def run_consensus(self) -> None:
        """Build consensus with full research context.

        The coordinator reads ALL agent markdown files from disk and writes
        the consensus report to coordinator_md. No inline data is sent.
        """
        print("\n=== Consensus ===\n")

        print("Coordinator building consensus from all md files...")
        await self.coordinator.process({
            "type": "build_consensus",
            **self._file_paths(),
        })
        self._verify_md_file("coordinator_md")

        print("\nConsensus complete")

    async def execute(self, design_doc_path: str,
                      output_dir: str = "./validation_output",
                      resume: bool = False) -> str:
        """Execute full validation battle with checkpoint/resume support.

        Pipeline: Research (once) -> Battle loop -> Consensus (once).
        MD files on disk are the single source of truth.
        """
        print(f"\n{'='*60}")
        print("KiCad Multi-Line Routing Validation Battle")
        print(f"{'='*60}\n")

        self.output_dir = os.path.abspath(output_dir)
        self.design_doc_path = os.path.abspath(design_doc_path)
        os.makedirs(self.output_dir, exist_ok=True)

        # Load checkpoint if resuming
        checkpoint = self._load_checkpoint(self.output_dir) if resume else None

        if checkpoint:
            print(f"Resuming from checkpoint: {checkpoint}\n")

        # Research phase (once)
        stage = checkpoint.get("stage") if checkpoint else None
        if not stage or stage == "init":
            await self.run_research_phase()
            self._save_checkpoint(stage="research")
        else:
            print("Skipping research phase (already completed)")

        # Battle loop (challenges -> solutions -> review, repeat)
        if not stage or stage in ("init", "research"):
            await self.run_battle_loop()
        elif stage == "battle":
            # Resume mid-battle
            await self.run_battle_loop(
                start_iter=checkpoint.get("battle_iteration", 1),
                resume_after=checkpoint.get("last_step"),
            )

        # Consensus (once)
        if stage != "done":
            await self.run_consensus()
            self._save_checkpoint(stage="done")
        else:
            print("Skipping consensus (already completed)")

        report_path = self._file_paths()["coordinator_md"]

        print(f"\n{'='*60}")
        print(f"Validation complete!")
        print(f"Report saved to: {report_path}")
        print(f"{'='*60}\n")

        return report_path

    def _save_json(self, data: Any, filepath: str):
        with open(filepath, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=2, ensure_ascii=False)

    def _save_checkpoint(self, stage: str = "battle",
                         battle_iteration: int = 0,
                         last_step: str = ""):
        """Save pipeline progress.

        Only stores stage + battle position. MD files on disk ARE the state.
        """
        checkpoint = {
            "stage": stage,
            "battle_iteration": battle_iteration,
            "last_step": last_step,
        }
        path = os.path.join(self.output_dir, CHECKPOINT_FILE)
        self._save_json(checkpoint, path)
        self._log(f"  [Checkpoint saved: stage={stage}, "
                  f"battle_iter={battle_iteration}, step={last_step}]")

    def _load_checkpoint(self, output_dir: str) -> Dict[str, Any] | None:
        """Load checkpoint from a previous run, if it exists."""
        path = os.path.join(output_dir, CHECKPOINT_FILE)
        if os.path.exists(path):
            with open(path, 'r', encoding='utf-8') as f:
                data = json.load(f)

            # Migrate old-format checkpoints
            if "completed_round" in data:
                old_stage = data["completed_round"]
                stage_map = {
                    "research": "research",
                    "round1": "battle", "round2": "battle",
                    "round3": "battle", "round4": "battle",
                    "round5": "done",
                }
                data = {
                    "stage": stage_map.get(old_stage, "init"),
                    "battle_iteration": 1,
                    "last_step": "",
                }

            if "last_round" in data:
                # Migrate numeric round -> step name
                round_to_step = {
                    1: BattleStep.CHALLENGES.value,
                    2: BattleStep.SOLUTIONS.value,
                    3: BattleStep.REVIEW.value,
                }
                data["last_step"] = round_to_step.get(
                    data.pop("last_round"), "")

            data.pop("state", None)
            return data
        return None
