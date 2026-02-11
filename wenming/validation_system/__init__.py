"""KiCad Multi-Line Routing Validation System."""

__version__ = "1.0.0"
__author__ = "KiCad Validation Team"

from .orchestrator import ValidationBattle
from .config import (
    KICAD_REPO_PATH,
    DESIGN_DOC_PATH,
    OUTPUT_DIR
)

__all__ = [
    "ValidationBattle",
    "KICAD_REPO_PATH",
    "DESIGN_DOC_PATH",
    "OUTPUT_DIR"
]
