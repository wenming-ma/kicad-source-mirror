"""Orchestration system for the validation battle (async)."""

import json
import os
from typing import Dict, Any, List
from agents import (
    ResearchAgent,
    ArchitectureCriticAgent,
    AlgorithmCriticAgent,
    ImplementationCriticAgent,
    CodeVerificationAgent,
    SolutionSynthesizerAgent,
    CoordinatorAgent
)
from agents.research_agent import ROUND_DIRECTIONS

# Number of research exploration rounds
RESEARCH_ROUNDS = 5

CHECKPOINT_FILE = "checkpoint.json"

# Maximum repair iterations for round 4 review loop
MAX_REPAIR_ITERATIONS = 7

# Ordered list of pipeline stages for checkpoint/resume
STAGES = ["research", "round1", "round2", "round3", "round4", "round5"]


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
        self.arch_critic = ArchitectureCriticAgent()
        self.algo_critic = AlgorithmCriticAgent()
        self.impl_critic = ImplementationCriticAgent()
        self.verifier = CodeVerificationAgent(self.kicad_repo_path)
        self.synthesizer = SolutionSynthesizerAgent()
        self.coordinator = CoordinatorAgent()

        self.agents = {
            "research": self.research,
            "arch_critic": self.arch_critic,
            "algo_critic": self.algo_critic,
            "impl_critic": self.impl_critic,
            "verifier": self.verifier,
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
        }

    async def run_research_phase(self) -> None:
        """Run 5 rounds of deep research exploration.

        Each round discovers new repos, clones them, and uses Explore
        subagents to analyze code.  Previous findings are read from
        files on disk by the agent (not passed as data).
        """
        print("\n" + "=" * 60)
        print("Research Phase: 5 Exploration Rounds")
        print("=" * 60)

        os.makedirs(self.repos_dir, exist_ok=True)
        research_dir = os.path.join(self.output_dir, "research")
        os.makedirs(research_dir, exist_ok=True)

        all_findings = []
        studied_repos = []

        for round_num in range(1, RESEARCH_ROUNDS + 1):
            print(f"\n--- Research Round {round_num}/{RESEARCH_ROUNDS} ---\n")

            directions = ROUND_DIRECTIONS.get(round_num, "")

            # Message carries only paths + lightweight metadata.
            # The agent reads previous findings from output_dir/research/.
            message = {
                "type": "research_request",
                "round": round_num,
                "total_rounds": RESEARCH_ROUNDS,
                "search_directions": directions,
                **self._file_paths(),
            }

            round_result = await self.research.process(message)

            # Track studied repos
            if isinstance(round_result, dict):
                for repo in round_result.get("repos_analyzed", []):
                    name = repo.get("name", "")
                    if name and name not in studied_repos:
                        studied_repos.append(name)

            all_findings.append(round_result)

            # Save each round's results
            self._save_json(
                round_result,
                os.path.join(research_dir, f"research_round_{round_num}.json")
            )
            # Update consolidated files after every round so the next
            # round (and crash-resume) can read them.
            self._save_json(
                all_findings,
                os.path.join(research_dir, "all_findings.json")
            )
            self._save_json(
                studied_repos,
                os.path.join(research_dir, "studied_repos.json")
            )

            print(f"Round {round_num} complete. "
                  f"Total repos studied: {len(studied_repos)}")

        print(f"\nResearch phase complete: {len(studied_repos)} repos analyzed "
              f"across {RESEARCH_ROUNDS} rounds")

    async def run_round_1_challenges(self) -> List[Dict]:
        """Round 1: Critics raise challenges informed by research.

        Each critic reads the design doc and research findings from disk.
        """
        print("\n=== Round 1: Initial Challenges ===\n")

        challenges = []
        errors = []

        critics = [
            ("arch_critic", self.arch_critic, "Architecture"),
            ("algo_critic", self.algo_critic, "Algorithm"),
            ("impl_critic", self.impl_critic, "Implementation"),
        ]

        for name, agent, label in critics:
            print(f"{label} critic reviewing design...")
            response = await agent.process({
                "type": "review_request",
                **self._file_paths(),
            })
            if response.get("error"):
                errors.append(response)
                print(f"  [WARN] {name} failed, continuing with others")
            elif "issues" in response:
                challenges.extend(response["issues"])

        for issue in challenges:
            self.coordinator.track_issue(issue)
        for err in errors:
            self.coordinator.track_error(err)

        print(f"\nRound 1 complete: {len(challenges)} challenges identified"
              f" ({len(errors)} agent errors)")
        return challenges

    async def run_round_2_verification(self) -> List[Dict]:
        """Round 2: Verify each challenge with research cross-references.

        The verifier reads round1_challenges.json and research from disk.
        """
        print("\n=== Round 2: Verification ===\n")

        # Load challenges from the file we saved in round 1
        challenges_path = os.path.join(self.output_dir, "round1_challenges.json")
        with open(challenges_path, 'r', encoding='utf-8') as f:
            challenges = json.load(f)

        verifications = []
        errors = []
        for i, challenge in enumerate(challenges, 1):
            print(f"Verifying challenge {i}/{len(challenges)}: {challenge.get('id', 'N/A')}")
            verification = await self.verifier.process({
                "type": "verify_challenge",
                "challenge": challenge,
                **self._file_paths(),
            })
            if verification.get("error"):
                errors.append(verification)
                print(f"  [WARN] Verification failed for challenge {i}")
            else:
                verifications.append(verification)

        for err in errors:
            self.coordinator.track_error(err)

        print(f"\nRound 2 complete: {len(verifications)} verifications completed"
              f" ({len(errors)} errors)")
        return verifications

    async def run_round_3_solutions(self) -> List[Dict]:
        """Round 3: Generate solutions grounded in research evidence.

        The synthesizer reads round2_verifications.json and research from disk.
        """
        print("\n=== Round 3: Solution Generation ===\n")

        # Load verifications from the file we saved in round 2
        verifications_path = os.path.join(
            self.output_dir, "round2_verifications.json"
        )
        with open(verifications_path, 'r', encoding='utf-8') as f:
            verified_issues = json.load(f)

        valid_issues = []
        for verification in verified_issues:
            if 'verifications' in verification:
                for v in verification['verifications']:
                    if v.get('status') in ['Valid', 'Partially Valid']:
                        valid_issues.append(v)

        print(f"Generating solutions for {len(valid_issues)} valid issues...")

        solutions = []
        errors = []
        for i, issue in enumerate(valid_issues, 1):
            print(f"Generating solution {i}/{len(valid_issues)}")
            solution = await self.synthesizer.process({
                "type": "generate_solution",
                "issue": issue,
                **self._file_paths(),
            })
            if solution.get("error"):
                errors.append(solution)
                print(f"  [WARN] Solution generation failed for issue {i}")
            else:
                solution["_source_issue"] = issue
                solutions.append(solution)

        for err in errors:
            self.coordinator.track_error(err)

        print(f"\nRound 3 complete: {len(solutions)} solutions generated"
              f" ({len(errors)} errors)")
        return solutions

    async def run_round_4_review(self) -> List[Dict]:
        """Round 4: Review solutions against research evidence.

        Critics read round3_solutions.json and research from disk.
        """
        print("\n=== Round 4: Solution Review ===\n")

        # Load solutions from the file we saved in round 3
        solutions_path = os.path.join(self.output_dir, "round3_solutions.json")
        with open(solutions_path, 'r', encoding='utf-8') as f:
            solutions = json.load(f)

        reviews = []
        for i, solution in enumerate(solutions, 1):
            print(f"Reviewing solution {i}/{len(solutions)}")

            review_msg = {
                "type": "review_solution",
                "solution": solution,
                "original_issue": solution.get("_source_issue"),
                **self._file_paths(),
            }

            review_entry = {
                "solution_id": solution.get('id', f'SOL-{i}'),
                "original_issue": solution.get("_source_issue"),
            }

            for name, agent in [
                ("arch_review", self.arch_critic),
                ("algo_review", self.algo_critic),
                ("impl_review", self.impl_critic),
            ]:
                result = await agent.process(review_msg)
                if result.get("error"):
                    self.coordinator.track_error(result)
                    print(f"  [WARN] {name} failed for solution {i}")
                review_entry[name] = result

            reviews.append(review_entry)

        print(f"\nRound 4 complete: {len(reviews)} solutions reviewed")
        return reviews

    def _extract_verdict(self, review: Dict) -> str:
        """Extract verdict string from a single critic response.

        Falls back to "revise" if the field is missing or the response
        contains an error.
        """
        if not isinstance(review, dict):
            return "revise"
        if review.get("error"):
            return "revise"
        verdict = review.get("verdict", "revise")
        if verdict not in ("approve", "reject", "revise"):
            return "revise"
        return verdict

    def _is_solution_approved(self, review_entry: Dict) -> bool:
        """Check whether a solution's review entry counts as approved.

        Policy: any "reject" means not approved. Otherwise needs at least
        2 out of 3 critics returning "approve".
        """
        verdicts = []
        for key in ("arch_review", "algo_review", "impl_review"):
            verdicts.append(self._extract_verdict(review_entry.get(key, {})))

        if "reject" in verdicts:
            return False
        return verdicts.count("approve") >= 2

    def _collect_review_feedback(self, review_entry: Dict) -> Dict:
        """Aggregate weaknesses, required_changes, and summaries from all
        three critics into a single feedback dict for the synthesizer."""
        feedback = {
            "weaknesses": [],
            "required_changes": [],
            "critic_summaries": [],
        }
        for key in ("arch_review", "algo_review", "impl_review"):
            critic = review_entry.get(key, {})
            if not isinstance(critic, dict) or critic.get("error"):
                continue
            for w in critic.get("weaknesses", []):
                feedback["weaknesses"].append(w)
            for rc in critic.get("required_changes", []):
                feedback["required_changes"].append(rc)
            summary = critic.get("summary", "")
            if summary:
                feedback["critic_summaries"].append(
                    f"{key}: {summary}"
                )
        return feedback

    async def run_round_4_repair_loop(
        self,
        reviews: List[Dict],
    ) -> List[Dict]:
        """Iteratively revise rejected solutions and re-review them.

        Approved solutions are kept as-is. Only rejected/revised solutions
        go through the synthesizer again and get re-reviewed by critics.
        Solutions and research are read from disk by agents.
        """
        current_reviews = list(reviews)

        # Load solutions from disk for the lookup
        solutions_path = os.path.join(self.output_dir, "round3_solutions.json")
        with open(solutions_path, 'r', encoding='utf-8') as f:
            solutions = json.load(f)

        # Build a solution lookup by solution_id
        sol_by_id = {}
        for sol in solutions:
            sid = sol.get("id", "")
            sol_by_id[sid] = sol
        # Also map by positional fallback id used in run_round_4_review
        for i, sol in enumerate(solutions, 1):
            fallback_id = f"SOL-{i}"
            if fallback_id not in sol_by_id:
                sol_by_id[fallback_id] = sol

        for iteration in range(1, MAX_REPAIR_ITERATIONS + 1):
            # Partition into approved / not-approved
            approved = []
            rejected = []
            for rev in current_reviews:
                if self._is_solution_approved(rev):
                    approved.append(rev)
                else:
                    rejected.append(rev)

            if not rejected:
                print(f"  All solutions approved, no repair needed.")
                break

            print(f"\n  --- Repair iteration {iteration}/{MAX_REPAIR_ITERATIONS} "
                  f"({len(rejected)} solution(s) to revise) ---")

            revised_reviews = []
            for rev in rejected:
                sol_id = rev.get("solution_id", "")
                original_solution = sol_by_id.get(sol_id, {})
                feedback = self._collect_review_feedback(rev)

                # Ask synthesizer to revise
                print(f"  Revising solution {sol_id}...")
                try:
                    revised = await self.synthesizer.process({
                        "type": "revise_solution",
                        "original_solution": original_solution,
                        "review_feedback": feedback,
                        "original_issue": rev.get("original_issue"),
                        **self._file_paths(),
                    })
                except Exception as exc:
                    print(f"  [WARN] Synthesizer revision failed for {sol_id}: {exc}")
                    rev["_repair_error"] = str(exc)
                    revised_reviews.append(rev)
                    continue

                if revised.get("error"):
                    print(f"  [WARN] Synthesizer returned error for {sol_id}")
                    self.coordinator.track_error(revised)
                    rev["_repair_error"] = revised.get("error")
                    revised_reviews.append(rev)
                    continue

                # Carry forward source issue
                revised["_source_issue"] = original_solution.get("_source_issue",
                                                                  rev.get("original_issue"))
                revised["_revision_iteration"] = iteration

                # Update solution lookup so further iterations use the revised version
                sol_by_id[sol_id] = revised

                # Re-review the revised solution
                print(f"  Re-reviewing revised solution {sol_id}...")
                review_msg = {
                    "type": "review_solution",
                    "solution": revised,
                    "original_issue": revised.get("_source_issue"),
                    **self._file_paths(),
                }

                new_review = {
                    "solution_id": sol_id,
                    "original_issue": revised.get("_source_issue"),
                    "revision_iteration": iteration,
                }

                for name, agent in [
                    ("arch_review", self.arch_critic),
                    ("algo_review", self.algo_critic),
                    ("impl_review", self.impl_critic),
                ]:
                    result = await agent.process(review_msg)
                    if result.get("error"):
                        self.coordinator.track_error(result)
                        print(f"    [WARN] {name} failed for revised {sol_id}")
                    new_review[name] = result

                revised_reviews.append(new_review)

            # Merge: approved stay, rejected replaced by new reviews
            current_reviews = approved + revised_reviews

        # Tag any still-rejected solutions after exhausting iterations
        for rev in current_reviews:
            if not self._is_solution_approved(rev):
                rev["_repair_exhausted"] = True

        return current_reviews

    async def run_round_5_consensus(self) -> Dict:
        """Round 5: Build consensus with full research context.

        The coordinator reads ALL previous round files from disk.
        """
        print("\n=== Round 5: Consensus ===\n")

        final_plan = await self.coordinator.process({
            "type": "build_consensus",
            "all_issues": self.coordinator.issues,
            **self._file_paths(),
        })

        if final_plan.get("error"):
            self.coordinator.track_error(final_plan)
            print("  [WARN] Consensus agent failed")

        print("\nRound 5 complete: Consensus reached")
        return final_plan

    async def execute(self, design_doc_path: str,
                      output_dir: str = "./validation_output",
                      resume: bool = False) -> str:
        """Execute full validation battle with checkpoint/resume support.

        Agents read previous outputs from files on disk.  The checkpoint
        only stores the name of the last completed stage -- no data blobs.
        """
        print(f"\n{'='*60}")
        print("KiCad Multi-Line Routing Validation Battle")
        print(f"{'='*60}\n")

        self.output_dir = os.path.abspath(output_dir)
        self.design_doc_path = os.path.abspath(design_doc_path)
        os.makedirs(self.output_dir, exist_ok=True)

        # Load checkpoint if resuming
        checkpoint = self._load_checkpoint(self.output_dir) if resume else None
        completed = checkpoint["completed_round"] if checkpoint else None

        if completed:
            print(f"Resuming from checkpoint: {completed} completed\n")

        def _stage_done(stage: str) -> bool:
            """Check if a stage was already completed in a previous run."""
            if completed is None:
                return False
            return STAGES.index(stage) <= STAGES.index(completed)

        # Research phase
        if not _stage_done("research"):
            await self.run_research_phase()
            self._save_checkpoint(self.output_dir, "research")
        else:
            print("Skipping research phase (already completed)")

        # Round 1
        if not _stage_done("round1"):
            challenges = await self.run_round_1_challenges()
            self._save_json(
                challenges,
                os.path.join(self.output_dir, "round1_challenges.json")
            )
            self._save_checkpoint(self.output_dir, "round1")
        else:
            print("Skipping round 1 (already completed)")

        # Round 2
        if not _stage_done("round2"):
            verifications = await self.run_round_2_verification()
            self._save_json(
                verifications,
                os.path.join(self.output_dir, "round2_verifications.json")
            )
            self._save_checkpoint(self.output_dir, "round2")
        else:
            print("Skipping round 2 (already completed)")

        # Round 3
        if not _stage_done("round3"):
            solutions = await self.run_round_3_solutions()
            self._save_json(
                solutions,
                os.path.join(self.output_dir, "round3_solutions.json")
            )
            self._save_checkpoint(self.output_dir, "round3")
        else:
            print("Skipping round 3 (already completed)")

        # Round 4
        if not _stage_done("round4"):
            reviews = await self.run_round_4_review()
            reviews = await self.run_round_4_repair_loop(reviews)
            self._save_json(
                reviews,
                os.path.join(self.output_dir, "round4_reviews.json")
            )
            self._save_checkpoint(self.output_dir, "round4")
        else:
            print("Skipping round 4 (already completed)")

        # Round 5
        if not _stage_done("round5"):
            final_plan = await self.run_round_5_consensus()
            self._save_json(
                final_plan,
                os.path.join(self.output_dir, "round5_consensus.json")
            )
            self._save_checkpoint(self.output_dir, "round5")
        else:
            print("Skipping round 5 (already completed)")

        report = self.coordinator.generate_report()
        report_path = os.path.join(self.output_dir, "validation_report.md")

        with open(report_path, 'w', encoding='utf-8') as f:
            f.write(report)

        print(f"\n{'='*60}")
        print(f"Validation complete!")
        print(f"Report saved to: {report_path}")
        print(f"{'='*60}\n")

        return report_path

    def _save_json(self, data: Any, filepath: str):
        with open(filepath, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=2, ensure_ascii=False)

    def _save_checkpoint(self, output_dir: str, round_name: str):
        """Save pipeline progress after a completed round.

        Only stores the stage name -- agents read data from files.
        """
        checkpoint = {"completed_round": round_name}
        path = os.path.join(output_dir, CHECKPOINT_FILE)
        self._save_json(checkpoint, path)
        print(f"  [Checkpoint saved: {round_name}]")

    def _load_checkpoint(self, output_dir: str) -> Dict[str, Any] | None:
        """Load checkpoint from a previous run, if it exists."""
        path = os.path.join(output_dir, CHECKPOINT_FILE)
        if os.path.exists(path):
            with open(path, 'r', encoding='utf-8') as f:
                return json.load(f)
        return None
