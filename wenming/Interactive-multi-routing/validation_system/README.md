# KiCad Multi-Line Routing Validation System

A multi-agent validation system using Claude SDK to rigorously validate the KiCad multi-line routing design through adversarial debate and evidence-based analysis.

## Overview

The system orchestrates 7 specialized AI agents to challenge, verify, and refine the design:

| Agent | Role | Model |
|---|---|---|
| Research Agent | Analyzes open-source implementations | Sonnet |
| Architecture Critic | Challenges architectural decisions | Opus |
| Algorithm Critic | Challenges algorithmic correctness | Opus |
| Implementation Critic | Challenges implementation feasibility | Opus |
| Code Verification Agent | Verifies challenges against KiCad source | Opus |
| Solution Synthesizer | Generates improved alternatives | Opus |
| Coordinator Agent | Orchestrates the validation process | Sonnet |

Validation proceeds through 5 rounds:
1. **Challenges** - All critics raise concerns
2. **Verification** - Code verifier investigates each challenge against KiCad source
3. **Solutions** - Synthesizer proposes fixes for valid issues
4. **Review + Repair Loop** - Critics review solutions; rejected ones are revised (max 2 iterations)
5. **Consensus** - Coordinator builds final recommendations

## Directory Structure

```
validation_system/
├── agents/
│   ├── base_agent.py               # Base agent class (API integration, history management)
│   ├── research_agent.py           # Open-source implementation analysis
│   ├── architecture_critic.py      # Architecture challenge agent
│   ├── algorithm_critic.py         # Algorithm correctness challenge agent
│   ├── implementation_critic.py    # Implementation feasibility challenge agent
│   ├── code_verifier.py            # KiCad source code verification
│   ├── solution_synthesizer.py     # Alternative solution generation
│   └── coordinator.py              # Workflow orchestration & reporting
├── config.py                       # Configuration settings
├── orchestrator.py                 # 5-round validation orchestration
├── main.py                         # CLI entry point
├── test_system.py                  # Test suite
├── requirements.txt                # Dependencies
├── .env.example                    # Environment template
└── validation_output/              # Generated reports & JSON
```

## Quick Start

### Prerequisites

- Python 3.8+
- uv package manager
- Anthropic API key (or custom endpoint)

### Installation

```bash
# Install uv (Windows)
powershell -c "irm https://astral.sh/uv/install.ps1 | iex"

# Install uv (macOS/Linux)
curl -LsSf https://astral.sh/uv/install.sh | sh

# Install dependencies
cd wenming/Interactive-multi-routing/validation_system
uv pip install -r requirements.txt

# Configure API key
cp .env.example .env
# Edit .env: set ANTHROPIC_API_KEY=your_key_here
# Optional: set ANTHROPIC_API_BASE=https://your-custom-endpoint.com/api
```

### Verify & Run

```bash
# Verify installation
uv run python test_system.py

# Run full validation
uv run python main.py --design-doc path/to/design_document.md

# Run specific round only
uv run python main.py --round 1

# Custom output directory
uv run python main.py --output-dir ./my_results
```

### Programmatic Usage

```python
from orchestrator import ValidationBattle

battle = ValidationBattle("/path/to/kicad-source-mirror")
report_path = battle.execute(
    design_doc_path="./design_document.md",
    output_dir="./validation_output"
)
```

## Output

The system generates per-round JSON files and a final Markdown report:

| File | Content |
|---|---|
| `round1_challenges.json` | All challenges with IDs, severity, details |
| `round2_verifications.json` | Verification status, evidence, solutions |
| `round3_solutions.json` | Alternative approaches, pros/cons |
| `round4_reviews.json` | Critic reviews with approve/reject/revise verdicts |
| `round5_consensus.json` | Final consensus and recommendations |
| `validation_report.md` | Comprehensive report (executive summary, findings, implementation plan) |

## Architecture Diagrams

### System Overview

```mermaid
graph TB
    subgraph "KiCad Multi-Line Routing Validation Battle System"
        Entry[main.py] --> Orchestrator[ValidationBattle]
        Orchestrator --> Round1[Round 1: Challenges]
        Orchestrator --> Round2[Round 2: Verification]
        Orchestrator --> Round3[Round 3: Solutions]
        Orchestrator --> Round4[Round 4: Review + Repair Loop]
        Orchestrator --> Round5[Round 5: Consensus]
        Round5 --> Report[validation_report.md]
    end
```

### Agent Class Hierarchy

```mermaid
classDiagram
    class BaseAgent {
        +string name
        +string role
        +string system_prompt
        +list conversation_history
        +process(message) dict
        +parse_response(text) dict
        +reset_conversation()
    }
    BaseAgent <|-- ResearchAgent
    BaseAgent <|-- ArchitectureCriticAgent
    BaseAgent <|-- AlgorithmCriticAgent
    BaseAgent <|-- ImplementationCriticAgent
    BaseAgent <|-- CodeVerificationAgent
    BaseAgent <|-- SolutionSynthesizerAgent
    BaseAgent <|-- CoordinatorAgent
```

### Agent Interaction Flow

```mermaid
graph LR
    R[Research] -->|Findings| C[Coordinator]
    AC[ArchCritic] -->|Challenges| C
    AlC[AlgoCritic] -->|Challenges| C
    IC[ImplCritic] -->|Challenges| C
    C -->|Issues| V[CodeVerifier]
    V -->|Evidence| C
    C -->|Valid Issues| S[Synthesizer]
    S -->|Solutions| C
    C -->|Solutions| AC & AlC & IC
    AC & AlC & IC -->|Reviews| C
    C -->|Feedback| S
    S -->|Revised| C
```

## Validation Findings Summary

Based on 5 rounds of research across 8 repositories and 15+ documents:

**Verdict: Proceed with implementation. No blocking issues found.**

### Key Conclusions

1. **No open-source EDA tool implements general interactive multi-line routing for N>2 traces.** This is genuinely novel work.
2. **The Leader-Follower approach is the only viable architecture for N>2.** The diff-pair gateway approach doesn't scale (2^N combinations), concurrent planning is too slow, sequential backtracking is unreliable.
3. **Corner spacing is a solved problem geometrically.** Clipper2's join algorithms (DoMiter, DoRound, DoSquare) handle all first-phase corner styles.
4. **Obstacle avoidance requires the "fat trace" abstraction.** Route the bundle as a single wide entity, then decompose into individual traces.
5. **Real-time performance is achievable via head/tail split.** Only recalculate the volatile head on each mouse move. R-tree provides O(log N) collision queries.
6. **KiCad's existing infrastructure covers ~70% of the implementation.** Router framework, collision detection, spatial indexing, walkaround, shove, and corner generation are all reusable.

### Repositories Analyzed

| Repository | Key Finding |
|---|---|
| KiCad PNS Router | THE reference. `pns_diff_pair_placer.cpp` is the only open-source multi-trace (2-line) implementation |
| Clipper2 (1.4k stars) | Essential for follower generation. 4 join types directly solve corner spacing. Already a KiCad dependency |
| horizon-eda (900 stars) | Uses KiCad's router v6.0.4. Confirms diff pair limited to 2 traces |
| freerouting (800 stars) | Does NOT support multi-line routing |
| CGAL (4.8k stars) | Exact polygon offset via Minkowski sum. Too heavyweight for interactive routing |
| LibrePCB (2.1k stars) | No multi-line routing. Single-trace only |

### Validated First-Phase Features

- **Corner Styles:** MITERED_45, Miter/Chamfer, Rounded/Fillet - all validated with concrete implementation paths
- **Obstacle Avoidance:** STRICT, WALKAROUND (default), PUSH_SHOVE, HIGHLIGHT_ONLY - all validated
- **Spacing:** Uniform user-set spacing (Plan B) - validated with DRC integration
- **Performance:** Incremental computation (head/tail split) + R-tree spatial indexing - target <16ms/frame

### Risk Matrix

| Risk | Severity | Mitigation |
|---|---|---|
| Follower-obstacle collision after leader walkaround | Medium | Expand walkaround hull by bundle width; fallback to per-trace walkaround |
| Miter spikes at acute angles violating DRC | High | Implement miter limit with bevel fallback from day one |
| Performance degradation at N>8 traces | Medium | Profile early; use coarser collision detection for preview |
| Inner arc radius below minimum | Medium | Validate at routing start; increase leader radius or warn user |

### Implementation Plan (12 person-days)

| Phase | Duration | Scope |
|---|---|---|
| 1. Skeleton & Straight Lines | 2 days | `MULTI_LINE_PLACER` class, `PNS_MODE_ROUTE_MULTI`, straight-line routing, STRICT mode |
| 2. Corner Styles | 4 days | MITERED_45 + Clipper2 DoMiter, Miter/Chamfer, ROUNDED_45 concentric arcs, miter limit |
| 3. Obstacle Avoidance | 3 days | HIGHLIGHT_ONLY, WALKAROUND (fat-trace hull), PUSH_SHOVE |
| 4. Spacing & Polish | 2 days | Dynamic spacing adjustment, DRC validation, head/tail split, R-tree optimization |
| 5. Testing | 1 day | Spacing accuracy, corner styles, obstacle modes, performance profiling |

### Deferred Features (NOT in first phase)

MITERED_90, any-angle routing, per-net-pair DRC spacing matrix, deferred precise computation, parallel computation for N>8, layer switching, length matching, signal integrity, differential pair coupling.

## Key Functions to Reuse

| Function | Source | Purpose |
|---|---|---|
| `BuildInitialTrace()` | `direction_45.cpp` | Generate leader path with corner styles |
| `makeGapVector()` | `pns_diff_pair.cpp` | Perpendicular offset with integer rounding |
| `WALKAROUND::Route()` | `pns_walkaround.cpp` | Leader obstacle avoidance |
| `SHOVE::ShoveObstacleLine()` | `pns_shove.cpp` | Push-and-shove for bundle |
| `ConvexHull()` | `pns_utils.cpp` | Octagonal hull for walkaround |
| `INDEX::Query()` | `pns_index.h` | R-tree collision detection |
| `DoMiter()/DoRound()/DoSquare()` | Clipper2 `clipper.offset.cpp` | Corner join algorithms for followers |

## Configuration

Edit `config.py` to customize model selection (Opus vs Sonnet per agent), token limits, output paths, and validation parameters.

**Custom API Endpoint:** Set `ANTHROPIC_API_BASE` in `.env` to use a third-party Claude API proxy. Leave unset for official Anthropic API.

**Estimated cost per validation:** $5-20 depending on design document size and number of issues found.

## Troubleshooting

| Error | Solution |
|---|---|
| `ANTHROPIC_API_KEY not set` | Create `.env` file with `ANTHROPIC_API_KEY=your_key_here` |
| `ModuleNotFoundError` | Run `uv pip install -r requirements.txt` |
| `Design document not found` | Provide correct absolute path via `--design-doc` |
| `KiCad repository not found` | Provide correct path via `--kicad-repo` |
| Unicode errors (Windows) | Set `PYTHONIOENCODING=utf-8` or use PowerShell |

## Extending the System

- **New agents:** Inherit from `BaseAgent`, define system prompt, implement `parse_response()`, add to `orchestrator.py`
- **New rounds:** Add round method to `ValidationBattle`, update `execute()`, update coordinator tracking

## Dependencies

- `anthropic` >= 0.40.0 (Claude SDK)
- `requests` >= 2.31.0
- `python-dotenv` >= 1.0.0
- `gitpython` >= 3.1.40

## License

Provided as-is for validating KiCad designs. See KiCad license for source code references.
