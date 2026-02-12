"""Coordinator agent for orchestrating the validation battle."""

from typing import Dict, Any, List
from .base_agent import BaseAgent
from config import FIRST_PHASE_SCOPE


class CoordinatorAgent(BaseAgent):
    """Agent specialized in coordinating the validation process."""

    def __init__(self):
        system_prompt = (
            """You are a Coordinator Agent managing the validation battle.

## First-Phase Scope

"""
            + FIRST_PHASE_SCOPE
            + """

Ensure the validation focuses on first-phase features. When summarizing or generating
reports, clearly separate first-phase issues from deferred-feature issues.

Your task is to orchestrate the discussion:
1. Manage discussion flow through rounds
2. Track all issues and resolutions
3. Prevent circular debates
4. Ensure comprehensive coverage
5. Synthesize final recommendations

## Using Research Findings

You will receive research findings from the Research Agent. Use them to:
- Ensure all major findings from research are addressed in the validation
- Check that proposed solutions reference real implementations found in research
- Include research summary in the final report
- Flag any research insights that were not addressed by critics or synthesizer

Workflow:
- Research Phase: 5 rounds of deep exploration (repos + documents)
- Round 1: Collect all challenges (informed by research)
- Round 2: Verify each challenge (cross-referenced with research)
- Round 3: Generate solutions (based on research evidence)
- Round 4: Review solutions
- Round 5: Build consensus

For final reports, provide:
{
  "report_type": "final",
  "executive_summary": "High-level overview",
  "research_summary": "Key findings from research phase",
  "key_findings": ["Finding 1", "Finding 2"],
  "recommendations": ["Recommendation 1", "Recommendation 2"],
  "implementation_plan": "Detailed plan grounded in research evidence",
  "risks": ["Risk 1", "Risk 2"],
  "next_steps": ["Step 1", "Step 2"]
}

Be decisive and clear. Keep discussions focused.

## File I/O

Your message will contain file paths instead of inline data. Use the Read tool to
load the information you need:

**For build_consensus (Round 5):**
Read ALL previous round files from `output_dir`:
- `{output_dir}/round1_challenges.json` -- initial challenges from critics
- `{output_dir}/round2_verifications.json` -- verified challenges
- `{output_dir}/round3_solutions.json` -- generated solutions
- `{output_dir}/round4_reviews.json` -- solution reviews and repair results
- `{output_dir}/research/all_findings.json` -- consolidated research findings
- The tracked issues list is provided inline in the message."""
        )

        super().__init__("coordinator", "Coordinator", system_prompt)
        self.issues: Dict[str, Dict] = {}
        self.errors: List[Dict] = []
        self.current_round = 1
        self.research_findings: List[Dict] = []

    def set_research_findings(self, findings: List[Dict]):
        """Store research findings for use in report generation."""
        self.research_findings = findings

    def track_issue(self, issue: Dict):
        """Track an issue through the workflow."""
        issue_id = issue.get('id', f"ISSUE-{len(self.issues)}")
        self.issues[issue_id] = issue

    def track_error(self, error: Dict):
        """Track an agent error for inclusion in the final report."""
        self.errors.append(error)

    def advance_round(self):
        """Move to next discussion round."""
        self.current_round += 1

    def get_status(self) -> Dict[str, Any]:
        """Get current validation status."""
        total = len(self.issues)
        resolved = sum(1 for i in self.issues.values() if i.get('status') == 'resolved')
        return {
            "round": self.current_round,
            "total_issues": total,
            "resolved_issues": resolved,
            "pending_issues": total - resolved,
            "issues": self.issues
        }

    def generate_report(self) -> str:
        """Generate final validation report."""
        status = self.get_status()
        arch = [i for i in self.issues.values() if i.get('id', '').startswith('ARCH')]
        algo = [i for i in self.issues.values() if i.get('id', '').startswith('ALGO')]
        impl = [i for i in self.issues.values() if i.get('id', '').startswith('IMPL')]

        report = f"""# KiCad Multi-Line Routing Validation Report

## Executive Summary

Validation completed after {self.current_round} rounds.
- Total issues: {status['total_issues']}
- Resolved: {status['resolved_issues']}
- Pending: {status['pending_issues']}

## Architecture Issues: {len(arch)}

"""
        for issue in arch:
            report += f"- **{issue.get('id')}**: {issue.get('challenge', 'N/A')}\n"

        report += f"\n## Algorithm Issues: {len(algo)}\n\n"
        for issue in algo:
            report += f"- **{issue.get('id')}**: {issue.get('challenge', 'N/A')}\n"

        report += f"\n## Implementation Issues: {len(impl)}\n\n"
        for issue in impl:
            report += f"- **{issue.get('id')}**: {issue.get('challenge', 'N/A')}\n"

        if self.errors:
            report += f"\n## Agent Errors: {len(self.errors)}\n\n"
            for err in self.errors:
                agent = err.get('agent', 'unknown')
                err_type = err.get('error_type', 'Unknown')
                err_msg = err.get('error_message', 'No details')
                report += f"- **{agent}** ({err_type}): {err_msg}\n"

        return report
