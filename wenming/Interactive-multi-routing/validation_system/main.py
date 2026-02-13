"""Main entry point for the validation system."""

import asyncio
import os
import sys
import argparse
from pathlib import Path
from dotenv import load_dotenv

# Load environment variables BEFORE importing config
# (config reads env vars at module level)
load_dotenv()

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent))

from orchestrator import ValidationBattle
from config import (
    KICAD_REPO_PATH,
    DESIGN_DOC_PATH,
    OUTPUT_DIR,
    ANTHROPIC_API_KEY
)


async def async_main():
    """Async main entry point."""
    parser = argparse.ArgumentParser(
        description="KiCad Multi-Line Routing Validation System"
    )
    parser.add_argument(
        "--design-doc",
        type=str,
        default=DESIGN_DOC_PATH,
        help="Path to design document"
    )
    parser.add_argument(
        "--kicad-repo",
        type=str,
        default=KICAD_REPO_PATH,
        help="Path to KiCad source repository"
    )
    parser.add_argument(
        "--output-dir",
        type=str,
        default=OUTPUT_DIR,
        help="Output directory for results"
    )
    parser.add_argument(
        "--round",
        type=int,
        choices=[1, 2, 3, 4, 5],
        help="Run only a specific round (default: run all rounds)"
    )
    parser.add_argument(
        "--resume",
        action="store_true",
        help="Resume from last checkpoint in the output directory"
    )

    args = parser.parse_args()

    if not ANTHROPIC_API_KEY:
        print("Error: ANTHROPIC_API_KEY environment variable not set")
        print("Please set it in your environment or in a .env file")
        sys.exit(1)

    if not os.path.exists(args.design_doc):
        print(f"Error: Design document not found: {args.design_doc}")
        sys.exit(1)

    if not os.path.exists(args.kicad_repo):
        print(f"Error: KiCad repository not found: {args.kicad_repo}")
        sys.exit(1)

    battle = ValidationBattle(args.kicad_repo)

    try:
        report_path = await battle.execute(
            args.design_doc, args.output_dir, resume=args.resume
        )
        print(f"\nSuccess! Validation report: {report_path}")
    except Exception as e:
        print(f"\nError during validation: {str(e)}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    # Suppress spurious RuntimeError/ValueError from ProactorEventLoop
    # transport __del__ on Windows (fires after loop is already closed).
    if sys.platform == "win32":
        from asyncio.proactor_events import _ProactorBasePipeTransport  # type: ignore[attr-defined]
        _orig_del = _ProactorBasePipeTransport.__del__

        def _silent_del(self, _warn=None):
            try:
                _orig_del(self, _warn)
            except (RuntimeError, ValueError):
                pass

        _ProactorBasePipeTransport.__del__ = _silent_del

    asyncio.run(async_main())
