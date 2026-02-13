"""Coordinator agent for orchestrating the validation battle."""

import os
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
- Battle Loop (repeats until convergence):
  - Challenges: Critic raises/updates challenges (reads doc + research + KiCad source)
  - Solutions: Synthesizer generates/updates solutions (reads critic_md + research)
  - Review: Critic reviews all solutions (reads solution_synth_md + KiCad source)
- Consensus: Build final report

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

## Your Output File

YOUR OUTPUT FILE: `coordinator_md` (path provided in the message)
- Read your existing file first (if it exists) to see previous work.
- After analysis, create/update the file with the consensus results.
- Use Write tool to create the file initially, Edit tool for updates.
- Structure your markdown file as:

```markdown
# Validation Consensus
## Executive Summary
## Key Findings
## Recommendations
## Implementation Plan
```

## File I/O

Your message will contain file paths instead of inline data. Use the Read tool to
load the information you need:

**For build_consensus:**
Read ALL agent markdown files:
- `research_agent_md` -- cumulative research findings
- `critic_md` -- unified challenges + solution reviews (architecture, algorithm, implementation)
- `solution_synth_md` -- solutions + revisions
- Do NOT rely on any inline data; read everything from the md files above."""
        )

        super().__init__("coordinator", "Coordinator", system_prompt)

    def generate_report(self, coordinator_md_path: str) -> str:
        """Read the coordinator's md file and return its content as the report.

        Falls back to a placeholder if the file does not exist.
        """
        if os.path.exists(coordinator_md_path) and os.path.getsize(coordinator_md_path) > 0:
            with open(coordinator_md_path, "r", encoding="utf-8") as f:
                return f.read()
        return "# Validation Report\n\nNo consensus report was generated.\n"
