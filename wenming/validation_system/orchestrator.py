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
        self.kicad_repo_path = kicad_repo_path
        self.repos_dir = repos_dir or os.path.join(
            os.path.dirname(__file__), "research_repos"
        )

        self.research = ResearchAgent()
        self.arch_critic = ArchitectureCriticAgent()
        self.algo_critic = AlgorithmCriticAgent()
        self.impl_critic = ImplementationCriticAgent()
        self.verifier = CodeVerificationAgent(kicad_repo_path)
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

    async def run_research_phase(self, design_doc: str, output_dir: str) -> List[Dict]:
        """Run 5 rounds of deep research exploration.

        Each round discovers new repos, clones them, and uses Explore
        subagents to analyze code. Previous findings are passed forward
        to avoid redundancy and guide new directions.
        """
        print("\n" + "=" * 60)
        print("Research Phase: 5 Exploration Rounds")
        print("=" * 60)

        os.makedirs(self.repos_dir, exist_ok=True)
        research_dir = os.path.join(output_dir, "research")
        os.makedirs(research_dir, exist_ok=True)

        all_findings = []
        studied_repos = []

        for round_num in range(1, RESEARCH_ROUNDS + 1):
            print(f"\n--- Research Round {round_num}/{RESEARCH_ROUNDS} ---\n")

            directions = ROUND_DIRECTIONS.get(round_num, "")

            # Build message with cumulative context
            message = {
                "type": "research_request",
                "round": round_num,
                "total_rounds": RESEARCH_ROUNDS,
                "repos_dir": self.repos_dir,
                "design_context": design_doc[:2000],
                "search_directions": directions,
                "already_studied_repos": studied_repos,
                "previous_findings_summary": self._summarize_findings(all_findings),
            }

            # Add recommended directions from previous round
            if all_findings:
                last = all_findings[-1]
                if isinstance(last, dict) and "recommended_next_directions" in last:
                    message["agent_recommended_directions"] = last["recommended_next_directions"]

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

            print(f"Round {round_num} complete. "
                  f"Total repos studied: {len(studied_repos)}")

        # Save consolidated research
        self._save_json(all_findings, os.path.join(research_dir, "all_findings.json"))
        self._save_json(studied_repos, os.path.join(research_dir, "studied_repos.json"))

        print(f"\nResearch phase complete: {len(studied_repos)} repos analyzed "
              f"across {RESEARCH_ROUNDS} rounds")

        return all_findings

    def _summarize_findings(self, findings: List[Dict]) -> str:
        """Create a concise summary of findings so far for the next round."""
        if not findings:
            return "No previous findings yet."

        parts = []
        for i, f in enumerate(findings, 1):
            if isinstance(f, dict):
                summary = f.get("summary", "")
                repos = [r.get("name", "?") for r in f.get("repos_analyzed", [])]
                insights = f.get("key_insights", [])
                parts.append(
                    f"Round {i}: Analyzed repos [{', '.join(repos)}]. "
                    f"Summary: {summary}. "
                    f"Key insights: {'; '.join(insights[:3]) if insights else 'N/A'}"
                )
            else:
                parts.append(f"Round {i}: {str(f)[:200]}")

        return "\n".join(parts)

    async def run_round_1_challenges(self, design_doc: str,
                                     research_findings: List[Dict]) -> List[Dict]:
        """Round 1: Critics raise challenges informed by research."""
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
                "design": design_doc,
                "research": research_findings
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

    async def run_round_2_verification(self, challenges: List[Dict],
                                       research_findings: List[Dict]) -> List[Dict]:
        """Round 2: Verify each challenge with research cross-references."""
        print("\n=== Round 2: Verification ===\n")

        verifications = []
        errors = []
        for i, challenge in enumerate(challenges, 1):
            print(f"Verifying challenge {i}/{len(challenges)}: {challenge.get('id', 'N/A')}")
            verification = await self.verifier.process({
                "type": "verify_challenge",
                "challenge": challenge,
                "kicad_repo_path": self.kicad_repo_path,
                "research_repos_dir": self.repos_dir,
                "research_findings": research_findings
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

    async def run_round_3_solutions(self, verified_issues: List[Dict],
                                    research_findings: List[Dict]) -> List[Dict]:
        """Round 3: Generate solutions grounded in research evidence."""
        print("\n=== Round 3: Solution Generation ===\n")

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
                "research_repos_dir": self.repos_dir,
                "research_findings": research_findings
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

    async def run_round_4_review(self, solutions: List[Dict],
                                 research_findings: List[Dict]) -> List[Dict]:
        """Round 4: Review solutions against research evidence."""
        print("\n=== Round 4: Solution Review ===\n")

        reviews = []
        for i, solution in enumerate(solutions, 1):
            print(f"Reviewing solution {i}/{len(solutions)}")

            review_msg = {
                "type": "review_solution",
                "solution": solution,
                "original_issue": solution.get("_source_issue"),
                "research_findings": research_findings
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
        solutions: List[Dict],
        research_findings: List[Dict],
    ) -> List[Dict]:
        """Iteratively revise rejected solutions and re-review them.

        Approved solutions are kept as-is. Only rejected/revised solutions
        go through the synthesizer again and get re-reviewed by critics.
        """
        current_reviews = list(reviews)

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
                        "research_findings": research_findings,
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
                    "research_findings": research_findings,
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

    async def run_round_5_consensus(self, reviews: List[Dict],
                                    research_findings: List[Dict]) -> Dict:
        """Round 5: Build consensus with full research context."""
        print("\n=== Round 5: Consensus ===\n")

        final_plan = await self.coordinator.process({
            "type": "build_consensus",
            "reviews": reviews,
            "all_issues": self.coordinator.issues,
            "research_findings": research_findings
        })

        if final_plan.get("error"):
            self.coordinator.track_error(final_plan)
            print("  [WARN] Consensus agent failed")

        print("\nRound 5 complete: Consensus reached")
        return final_plan

    async def execute(self, design_doc_path: str,
                      output_dir: str = "./validation_output",
                      resume: bool = False) -> str:
        """Execute full validation battle with checkpoint/resume support."""
        print(f"\n{'='*60}")
        print("KiCad Multi-Line Routing Validation Battle")
        print(f"{'='*60}\n")

        os.makedirs(output_dir, exist_ok=True)

        with open(design_doc_path, 'r', encoding='utf-8') as f:
            design_doc = f.read()

        # Load checkpoint if resuming
        checkpoint = self._load_checkpoint(output_dir) if resume else None
        completed = checkpoint["completed_round"] if checkpoint else None
        state = checkpoint["state"] if checkpoint else {}

        if completed:
            print(f"Resuming from checkpoint: {completed} completed\n")

        def _stage_done(stage: str) -> bool:
            """Check if a stage was already completed in a previous run."""
            if completed is None:
                return False
            return STAGES.index(stage) <= STAGES.index(completed)

        # Research phase
        if not _stage_done("research"):
            research_findings = await self.run_research_phase(
                design_doc, output_dir
            )
            state["research_findings"] = research_findings
            self._save_checkpoint(output_dir, "research", state)
        else:
            research_findings = state["research_findings"]
            print("Skipping research phase (already completed)")

        self.coordinator.set_research_findings(research_findings)

        # Round 1
        if not _stage_done("round1"):
            challenges = await self.run_round_1_challenges(
                design_doc, research_findings
            )
            state["challenges"] = challenges
            self._save_checkpoint(output_dir, "round1", state)
            self._save_json(
                challenges,
                os.path.join(output_dir, "round1_challenges.json")
            )
        else:
            challenges = state["challenges"]
            print("Skipping round 1 (already completed)")

        # Round 2
        if not _stage_done("round2"):
            verifications = await self.run_round_2_verification(
                challenges, research_findings
            )
            state["verifications"] = verifications
            self._save_checkpoint(output_dir, "round2", state)
            self._save_json(
                verifications,
                os.path.join(output_dir, "round2_verifications.json")
            )
        else:
            verifications = state["verifications"]
            print("Skipping round 2 (already completed)")

        # Round 3
        if not _stage_done("round3"):
            solutions = await self.run_round_3_solutions(
                verifications, research_findings
            )
            state["solutions"] = solutions
            self._save_checkpoint(output_dir, "round3", state)
            self._save_json(
                solutions,
                os.path.join(output_dir, "round3_solutions.json")
            )
        else:
            solutions = state["solutions"]
            print("Skipping round 3 (already completed)")

        # Round 4
        if not _stage_done("round4"):
            reviews = await self.run_round_4_review(
                solutions, research_findings
            )
            reviews = await self.run_round_4_repair_loop(
                reviews, solutions, research_findings
            )
            state["reviews"] = reviews
            self._save_checkpoint(output_dir, "round4", state)
            self._save_json(
                reviews,
                os.path.join(output_dir, "round4_reviews.json")
            )
        else:
            reviews = state["reviews"]
            print("Skipping round 4 (already completed)")

        # Round 5
        if not _stage_done("round5"):
            final_plan = await self.run_round_5_consensus(
                reviews, research_findings
            )
            state["final_plan"] = final_plan
            self._save_checkpoint(output_dir, "round5", state)
            self._save_json(
                final_plan,
                os.path.join(output_dir, "round5_consensus.json")
            )
        else:
            final_plan = state["final_plan"]
            print("Skipping round 5 (already completed)")

        report = self.coordinator.generate_report()
        report_path = os.path.join(output_dir, "validation_report.md")

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

    def _save_checkpoint(self, output_dir: str, round_name: str,
                         state: Dict[str, Any]):
        """Save pipeline state after a completed round."""
        checkpoint = {
            "completed_round": round_name,
            "state": state
        }
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
