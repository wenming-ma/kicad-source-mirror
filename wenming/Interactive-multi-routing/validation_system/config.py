"""Configuration for the validation system."""

import os
from pathlib import Path

# API Configuration
ANTHROPIC_API_KEY = os.environ.get("ANTHROPIC_API_KEY")
ANTHROPIC_API_BASE = os.environ.get("ANTHROPIC_API_BASE", None)  # Custom API endpoint (optional)

# Model Configuration
DEFAULT_MODEL = "claude-opus-4-6"
FAST_MODEL = "claude-sonnet-4-5"  # For less critical agents

# Path Configuration
KICAD_REPO_PATH = os.environ.get(
    "KICAD_REPO_PATH",
    str(Path(__file__).parent.parent.absolute())
)

# Design Document Path
_DESIGN_DOC_DIR = Path(__file__).parent.parent.absolute()
_DESIGN_DOC_NAME = "KiCad多线同步布线技术设计文档.md"
DESIGN_DOC_PATH = os.environ.get(
    "DESIGN_DOC_PATH",
    str(_DESIGN_DOC_DIR / _DESIGN_DOC_NAME)
)

# First-Phase Scope Definition
# All agents reference this constant to stay aligned on what is in scope.
FIRST_PHASE_SCOPE = """
FIRST-PHASE SCOPE (implement now):
- Corner styles: 45-degree diagonal (MITERED_45), miter/chamfer (MITERED_45 + miter ratio), \
rounded/fillet (ROUNDED_45/ROUNDED_90)
- Obstacle avoidance: ALL 4 modes -- STRICT, WALKAROUND (default), PUSH_SHOVE, HIGHLIGHT_ONLY
- Spacing: Uniform user-set spacing (Plan B), adjustable via hotkey/scroll wheel
- Performance: incremental computation, R-tree spatial indexing
- Single layer, N traces, 45/90 degree angle constraints

DEFERRED (do NOT focus on these):
- Corner styles: 90-degree (MITERED_90), any-angle
- Spacing: per-net-pair DRC spacing matrix (Plan A)
- Performance: deferred precise computation, parallel computation (N > 8)
- Layer switching, length matching, signal integrity, differential pair coupling
"""

# Output Configuration
OUTPUT_DIR = "./validation_output"

# Agent Configuration
AGENT_CONFIG = {
    "research_agent": {
        "model": FAST_MODEL  # Use faster model for research
    },
    "arch_critic": {
        "model": DEFAULT_MODEL
    },
    "algo_critic": {
        "model": DEFAULT_MODEL
    },
    "impl_critic": {
        "model": DEFAULT_MODEL
    },
    "code_verifier": {
        "model": DEFAULT_MODEL
    },
    "solution_synth": {
        "model": DEFAULT_MODEL
    },
    "coordinator": {
        "model": DEFAULT_MODEL
    }
}

# Validation Configuration
MAX_ROUNDS = 5
MAX_CHALLENGES_PER_AGENT = 20
MAX_SOLUTIONS_PER_ISSUE = 3

# Logging Configuration
LOG_LEVEL = "INFO"
LOG_FILE = os.path.join(OUTPUT_DIR, "validation.log")
