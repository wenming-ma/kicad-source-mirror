# Quick Start Guide

## Installation Steps

### 1. Install uv (Fast Python Package Installer)

```bash
# On macOS and Linux
curl -LsSf https://astral.sh/uv/install.sh | sh

# On Windows
powershell -c "irm https://astral.sh/uv/install.ps1 | iex"
```

### 2. Install Python Dependencies

```bash
cd wenming/validation_system
uv pip install -r requirements.txt
```

This will install:
- anthropic (Claude SDK)
- requests (HTTP library)
- python-dotenv (Environment variable management)
- gitpython (Git repository access)

### 3. Set Up API Key

Create a `.env` file in the `validation_system` directory:

```bash
cp .env.example .env
```

Edit `.env` and add your Anthropic API key:

```
ANTHROPIC_API_KEY=sk-ant-api03-...
```

**For Official Anthropic API:**
1. Go to https://console.anthropic.com/
2. Sign in or create an account
3. Navigate to API Keys
4. Create a new key

**For Custom API Endpoints:**

If you're using a third-party Claude API service, also set the base URL:

```
ANTHROPIC_API_KEY=your_api_key_here
ANTHROPIC_API_BASE=https://your-custom-endpoint.com/api
```

See `CUSTOM_ENDPOINT_GUIDE.md` for detailed configuration instructions.

### 4. Verify Installation

Run the test script:

```bash
uv run python test_system.py
```

You should see:
```
============================================================
Validation System Test Suite
============================================================
Testing imports...
[PASS] All imports successful

Testing agent initialization...
  [OK] research_agent initialized
  [OK] arch_critic initialized
  [OK] algo_critic initialized
  [OK] impl_critic initialized
  [OK] solution_synth initialized
  [OK] coordinator initialized
[PASS] All agents initialized successfully

Testing configuration...
  KICAD_REPO_PATH: C:\Users\...\kicad-source-mirror-master
  DESIGN_DOC_PATH: ./wenming/Interactive-multi-routing/...
  OUTPUT_DIR: ./validation_output
  [OK] ANTHROPIC_API_KEY is set
[PASS] Configuration loaded

Testing orchestrator...
  [OK] ValidationBattle initialized with 7 agents
  [OK] Coordinator status: Round 1, 0 issues
[PASS] Orchestrator working

============================================================
Test Results
============================================================
Imports: [PASS]
Agent Initialization: [PASS]
Configuration: [PASS]
Orchestrator: [PASS]

============================================================
All tests passed! System is ready.

Next steps:
1. Set ANTHROPIC_API_KEY in .env file
2. Prepare design document
3. Run: python main.py --design-doc path/to/document.md
============================================================
```

## Running Your First Validation

### Prepare Design Document

Ensure you have a design document in Markdown format. For example:
- `./wenming/Interactive-multi-routing/KiCad多线同步布线技术设计文档.md`

### Run Validation

```bash
uv run python main.py --design-doc path/to/your/design_document.md
```

### Monitor Progress

The system will print progress as it runs through 5 rounds:

```
============================================================
KiCad Multi-Line Routing Validation Battle
============================================================

=== Round 1: Initial Challenges ===

Research agent analyzing open-source implementations...
Architecture critic reviewing design...
Algorithm critic reviewing design...
Implementation critic reviewing design...

Round 1 complete: 15 challenges identified

=== Round 2: Verification ===

Verifying challenge 1/15: ARCH-001
Verifying challenge 2/15: ARCH-002
...

Round 2 complete: 15 verifications completed

=== Round 3: Solution Generation ===

Generating solutions for 10 valid issues...
Generating solution 1/10
...

Round 3 complete: 10 solutions generated

=== Round 4: Solution Review ===

Reviewing solution 1/10
...

Round 4 complete: 10 solutions reviewed

=== Round 5: Consensus ===

Round 5 complete: Consensus reached

============================================================
Validation complete!
Report saved to: ./validation_output/validation_report.md
============================================================
```

### Review Results

Check the output directory for:

1. **validation_report.md** - Main report with findings and recommendations
2. **round1_challenges.json** - All challenges raised
3. **round2_verifications.json** - Verification results
4. **round3_solutions.json** - Solution proposals
5. **round4_reviews.json** - Solution reviews
6. **round5_consensus.json** - Final consensus

## Cost Estimation

Each validation run makes approximately:
- 1 research call (Sonnet)
- 3 critic calls per round (Opus) × 2 rounds = 6 calls
- N verification calls (Opus) where N = number of challenges
- M solution calls (Opus) where M = number of valid issues
- M × 3 review calls (Opus)
- 1 consensus call (Sonnet)

**Estimated cost**: $5-20 per validation depending on:
- Design document size
- Number of issues found
- Complexity of discussions

## Troubleshooting

### ModuleNotFoundError: No module named 'anthropic'

**Solution**: Install dependencies using uv
```bash
uv pip install -r requirements.txt
```

### Error: ANTHROPIC_API_KEY environment variable not set

**Solution**: Create `.env` file with your API key
```bash
echo "ANTHROPIC_API_KEY=your_key_here" > .env
```

### Error: Design document not found

**Solution**: Provide correct path
```bash
uv run python main.py --design-doc /full/path/to/document.md
```

### Unicode encoding errors on Windows

The system has been updated to use ASCII-safe characters. If you still see encoding errors, set:
```bash
set PYTHONIOENCODING=utf-8
```

## Advanced Usage

### Run Specific Round Only

```bash
uv run python main.py --round 1  # Only run Round 1 (Challenges)
uv run python main.py --round 2  # Only run Round 2 (Verification)
```

### Custom Output Directory

```bash
uv run python main.py --output-dir ./my_validation_results
```

### Custom KiCad Repository Path

```bash
uv run python main.py --kicad-repo /path/to/kicad-source-mirror
```

## Next Steps

After successful validation:

1. Review the validation report
2. Address critical issues identified
3. Implement recommended solutions
4. Re-run validation to verify fixes
5. Proceed with implementation

## Support

For issues or questions:
- Check the README.md for detailed documentation
- Review the code in `agents/` directory
- Examine the orchestrator logic in `orchestrator.py`
