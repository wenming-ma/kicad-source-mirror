# KiCad Multi-Line Routing Validation System

A multi-agent validation system using Claude SDK to rigorously validate and refine the KiCad multi-line routing design through adversarial debate and evidence-based analysis.

## Overview

This system orchestrates 7 specialized AI agents that work together to:
- Challenge every technical aspect of the design
- Verify claims against actual KiCad source code
- Propose improved solutions
- Build consensus through structured debate
- Generate comprehensive validation reports

## Architecture

### Agents

1. **Research Agent**: Finds and analyzes open-source implementations
2. **Architecture Critic**: Challenges architectural decisions
3. **Algorithm Critic**: Challenges algorithmic correctness
4. **Implementation Critic**: Challenges implementation feasibility
5. **Code Verification Agent**: Verifies challenges against actual code
6. **Solution Synthesizer**: Generates improved alternatives
7. **Coordinator Agent**: Orchestrates the validation process

### Workflow

The validation proceeds through 5 rounds:

1. **Round 1 - Challenges**: All critics raise concerns
2. **Round 2 - Verification**: Code verifier investigates each challenge
3. **Round 3 - Solutions**: Synthesizer proposes fixes
4. **Round 4 - Review**: Critics review proposed solutions
5. **Round 5 - Consensus**: Coordinator builds final recommendations

## Installation

### Prerequisites

- Python 3.8 or higher
- uv (fast Python package installer)
- Anthropic API key
- KiCad source repository

### Setup

1. Install uv if not already installed:
```bash
# On macOS and Linux
curl -LsSf https://astral.sh/uv/install.sh | sh

# On Windows
powershell -c "irm https://astral.sh/uv/install.ps1 | iex"
```

2. Navigate to the validation system:
```bash
cd /path/to/kicad-source-mirror/wenming/validation_system
```

3. Install dependencies using uv:
```bash
uv pip install -r requirements.txt
```

4. Configure environment variables:
```bash
cp .env.example .env
# Edit .env and add your ANTHROPIC_API_KEY
```

**For Custom API Endpoints:**

If you're using a third-party Claude API service (e.g., a proxy or relay service), you can configure a custom endpoint:

```bash
# In .env file:
ANTHROPIC_API_KEY=your_api_key_here
ANTHROPIC_API_BASE=https://your-custom-endpoint.com/api
```

The system will automatically use the custom endpoint if `ANTHROPIC_API_BASE` is set. Leave it commented out or unset to use the official Anthropic API.

## Usage

### Basic Usage

Run the full validation:

```bash
python main.py --design-doc path/to/design_document.md
```

### Advanced Options

```bash
python main.py \
  --design-doc ./wenming/Interactive-multi-routing/KiCad多线同步布线技术设计文档.md \
  --kicad-repo /path/to/kicad-source-mirror \
  --output-dir ./validation_output
```

### Run Specific Round

To run only a specific round (useful for debugging):

```bash
python main.py --round 1  # Run only Round 1 (Challenges)
```

## Output

The system generates:

1. **Round outputs** (JSON):
   - `round1_challenges.json` - All challenges raised
   - `round2_verifications.json` - Verification results
   - `round3_solutions.json` - Solution proposals
   - `round4_reviews.json` - Solution reviews
   - `round5_consensus.json` - Final consensus

2. **Final report** (Markdown):
   - `validation_report.md` - Comprehensive validation report

## Configuration

Edit `config.py` to customize:

- Model selection (Opus vs Sonnet for different agents)
- Token limits
- Output paths
- Validation parameters

## Cost Considerations

This system makes multiple API calls to Claude. To manage costs:

1. Use Sonnet for less critical agents (research, coordination)
2. Use Opus for critical analysis (critics, verification)
3. Implement caching for repeated code reads
4. Run specific rounds instead of full validation during development

Estimated cost for full validation: $5-20 depending on design document size and number of issues found.

## Example Workflow

```python
from orchestrator import ValidationBattle

# Initialize
battle = ValidationBattle("/path/to/kicad-source-mirror")

# Execute validation
report_path = battle.execute(
    design_doc_path="./design_document.md",
    output_dir="./validation_output"
)

print(f"Report: {report_path}")
```

## Extending the System

### Adding New Agents

1. Create new agent class inheriting from `BaseAgent`
2. Define specialized system prompt
3. Implement `parse_response()` method
4. Add to `orchestrator.py`

### Adding New Rounds

1. Add round method to `ValidationBattle` class
2. Update `execute()` method to include new round
3. Update coordinator to track new round

## Troubleshooting

### API Key Issues

```
Error: ANTHROPIC_API_KEY environment variable not set
```

Solution: Set the environment variable or create a `.env` file:
```bash
export ANTHROPIC_API_KEY=your_key_here
```

### Design Document Not Found

```
Error: Design document not found
```

Solution: Provide correct path to design document:
```bash
python main.py --design-doc /correct/path/to/document.md
```

### KiCad Repository Not Found

```
Error: KiCad repository not found
```

Solution: Provide correct path to KiCad source:
```bash
python main.py --kicad-repo /correct/path/to/kicad-source-mirror
```

## Development

### Running Tests

```bash
uv run pytest tests/
```

### Code Style

```bash
uv run black validation_system/
uv run flake8 validation_system/
```

## License

This validation system is provided as-is for validating KiCad designs. See KiCad license for source code references.

## Contact

For issues or questions, please refer to the KiCad development documentation.
