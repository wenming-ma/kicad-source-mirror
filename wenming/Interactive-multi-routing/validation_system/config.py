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
DESIGN_DOC_PATH = os.environ.get(
    "DESIGN_DOC_PATH",
    "./wenming/Interactive-multi-routing/KiCad多线同步布线技术设计文档.md"
)

# Output Configuration
OUTPUT_DIR = "./validation_output"

# Agent Configuration
AGENT_CONFIG = {
    "research": {
        "model": FAST_MODEL,  # Use faster model for research
        "max_tokens": 4096
    },
    "arch_critic": {
        "model": DEFAULT_MODEL,
        "max_tokens": 4096
    },
    "algo_critic": {
        "model": DEFAULT_MODEL,
        "max_tokens": 4096
    },
    "impl_critic": {
        "model": DEFAULT_MODEL,
        "max_tokens": 4096
    },
    "verifier": {
        "model": DEFAULT_MODEL,
        "max_tokens": 4096
    },
    "synthesizer": {
        "model": DEFAULT_MODEL,
        "max_tokens": 4096
    },
    "coordinator": {
        "model": FAST_MODEL,  # Use faster model for coordination
        "max_tokens": 4096
    }
}

# Validation Configuration
MAX_ROUNDS = 5
MAX_CHALLENGES_PER_AGENT = 20
MAX_SOLUTIONS_PER_ISSUE = 3

# Logging Configuration
LOG_LEVEL = "INFO"
LOG_FILE = os.path.join(OUTPUT_DIR, "validation.log")
