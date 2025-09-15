#!/usr/bin/env python3
"""
Update CMakeLists.txt files in kicad_core_project_wx to match KiCad structure
- Keep the same CMakeLists.txt locations as original KiCad
- Include all copied files in the compilation
"""

import os
import re
from pathlib import Path
from typing import List, Set, Dict, Tuple

def get_copied_files_from_log(log_file: Path) -> Dict[str, List[Tuple[str, str]]]:
    """
    Parse the log file and return files grouped by directory.
    Returns dict with directory as key and list of (filename, status) tuples as value.
    """
    copied_files = {}

    with open(log_file, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue

            # Parse status from line
            status = "COPIED"
            if "[SKIPPED" in line:
                status = "SKIPPED"
            elif "[ERROR" in line:
                status = "ERROR"
                continue  # Skip error files

            # Extract file path
            if '\\' in line:
                # Remove status suffix if present
                file_path = line.split('[')[0].strip()
                # Convert to Path object and get relative path
                if 'kicad_core_project_wx' in file_path:
                    parts = file_path.split('kicad_core_project_wx\\')
                    if len(parts) > 1:
                        rel_path = parts[1]
                        # Group by directory
                        if '\\' in rel_path:
                            dir_parts = rel_path.split('\\')
                            if len(dir_parts) >= 2:
                                main_dir = dir_parts[0]
                                sub_path = '/'.join(dir_parts[1:]) if len(dir_parts) > 2 else dir_parts[1]

                                if main_dir not in copied_files:
                                    copied_files[main_dir] = []
                                copied_files[main_dir].append((sub_path, status))

    return copied_files

def collect_files_for_eeschema(project_root: Path) -> Dict[str, List[str]]:
    """Collect all source files for eeschema following KiCad's organization."""
    eeschema_dir = project_root / "eeschema"

    categories = {
        'EESCHEMA_SCH_IO': [],
        'EESCHEMA_DLGS': [],
        'EESCHEMA_SIM_SRCS': [],
        'EESCHEMA_WIDGETS': [],
        'EESCHEMA_ERC_SRCS': [],
        'SYNC_SHEET_PIN_SRCS': [],
        'EESCHEMA_SRCS': []
    }

    # Scan sch_io subdirectory
    sch_io_dir = eeschema_dir / "sch_io"
    if sch_io_dir.exists():
        # Base sch_io files
        for f in sch_io_dir.glob("*.cpp"):
            categories['EESCHEMA_SCH_IO'].append(f"sch_io/{f.name}")

        # Subdirectories
        for subdir in ['kicad_sexpr', 'kicad_legacy', 'database']:
            sub_path = sch_io_dir / subdir
            if sub_path.exists():
                for f in sub_path.glob("*.cpp"):
                    categories['EESCHEMA_SCH_IO'].append(f"sch_io/{subdir}/{f.name}")

    # Scan dialogs subdirectory
    dialogs_dir = eeschema_dir / "dialogs"
    if dialogs_dir.exists():
        for f in dialogs_dir.glob("*.cpp"):
            categories['EESCHEMA_DLGS'].append(f"dialogs/{f.name}")

    # Scan sim subdirectory
    sim_dir = eeschema_dir / "sim"
    if sim_dir.exists():
        for f in sim_dir.glob("*.cpp"):
            categories['EESCHEMA_SIM_SRCS'].append(f"sim/{f.name}")

    # Scan widgets subdirectory
    widgets_dir = eeschema_dir / "widgets"
    if widgets_dir.exists():
        for f in widgets_dir.glob("*.cpp"):
            categories['EESCHEMA_WIDGETS'].append(f"widgets/{f.name}")

    # Scan erc subdirectory
    erc_dir = eeschema_dir / "erc"
    if erc_dir.exists():
        for f in erc_dir.glob("*.cpp"):
            categories['EESCHEMA_ERC_SRCS'].append(f"erc/{f.name}")

    # Scan sync_sheet_pin subdirectory
    sync_dir = eeschema_dir / "sync_sheet_pin"
    if sync_dir.exists():
        for f in sync_dir.glob("*.cpp"):
            categories['SYNC_SHEET_PIN_SRCS'].append(f"sync_sheet_pin/{f.name}")

    # Main eeschema source files (in root of eeschema)
    for f in eeschema_dir.glob("*.cpp"):
        # Skip test files and specific executables
        if not any(skip in f.name for skip in ['_test', 'eeschema.cpp', 'eeschema_main']):
            categories['EESCHEMA_SRCS'].append(f.name)

    return categories

def generate_eeschema_cmake(categories: Dict[str, List[str]]) -> str:
    """Generate CMakeLists.txt content for eeschema."""
    lines = []
    lines.append("# CMakeLists.txt for eeschema - simplified version")
    lines.append("")
    lines.append("include_directories(")
    lines.append("    ${CMAKE_SOURCE_DIR}/common")
    lines.append("    ${CMAKE_SOURCE_DIR}/common/dialogs")
    lines.append("    ${CMAKE_SOURCE_DIR}/libs/sexpr/include")
    lines.append("    ${CMAKE_SOURCE_DIR}/include")
    lines.append("    ${CMAKE_CURRENT_SOURCE_DIR}")
    lines.append("    ./dialogs")
    lines.append("    ./symbol_editor")
    lines.append("    ./tools")
    lines.append("    ./widgets")
    lines.append(")")
    lines.append("")

    # Add each category of source files
    for category, files in categories.items():
        if files:
            lines.append(f"set( {category}")
            for f in sorted(files):
                lines.append(f"    {f}")
            lines.append(")")
            lines.append("")

    # Add the library target
    lines.append("# Build static library")
    lines.append("add_library( eeschema_lib STATIC")
    for category in categories.keys():
        if categories[category]:
            lines.append(f"    ${{{category}}}")
    lines.append(")")
    lines.append("")

    # Add target properties
    lines.append("target_include_directories( eeschema_lib PUBLIC")
    lines.append("    ${CMAKE_CURRENT_SOURCE_DIR}")
    lines.append("    ${CMAKE_SOURCE_DIR}/include")
    lines.append("    ${CMAKE_SOURCE_DIR}/libs/core/include")
    lines.append("    ${CMAKE_SOURCE_DIR}/libs/kimath/include")
    lines.append("    ${CMAKE_SOURCE_DIR}/libs/kiplatform/include")
    lines.append("    ${CMAKE_SOURCE_DIR}/thirdparty")
    lines.append(")")

    return "\n".join(lines)

def generate_bitmap2component_cmake(project_root: Path) -> str:
    """Generate CMakeLists.txt content for bitmap2component."""
    bitmap_dir = project_root / "bitmap2component"

    cpp_files = []
    if bitmap_dir.exists():
        for f in bitmap_dir.glob("*.cpp"):
            cpp_files.append(f.name)

    lines = []
    lines.append("# CMakeLists.txt for bitmap2component - simplified version")
    lines.append("")
    lines.append("set( BITMAP2COMPONENT_SRCS")
    for f in sorted(cpp_files):
        lines.append(f"    {f}")
    lines.append(")")
    lines.append("")

    lines.append("add_library( bitmap2component_lib STATIC")
    lines.append("    ${BITMAP2COMPONENT_SRCS}")
    lines.append(")")
    lines.append("")

    lines.append("target_include_directories( bitmap2component_lib PUBLIC")
    lines.append("    ${CMAKE_CURRENT_SOURCE_DIR}")
    lines.append("    ${CMAKE_SOURCE_DIR}/include")
    lines.append("    ${CMAKE_SOURCE_DIR}/libs/core/include")
    lines.append("    ${CMAKE_SOURCE_DIR}/libs/kimath/include")
    lines.append("    ${CMAKE_SOURCE_DIR}/libs/kiplatform/include")
    lines.append("    ${CMAKE_SOURCE_DIR}/thirdparty")
    lines.append(")")

    return "\n".join(lines)

def main():
    # Setup paths
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    wx_project_root = project_root / "kicad_core_project_wx"
    log_file = wx_project_root / "file_copy_log.txt"

    print(f"Project root: {wx_project_root}")
    print(f"Log file: {log_file}")

    # Parse log file
    copied_files = get_copied_files_from_log(log_file)

    # Update bitmap2component/CMakeLists.txt
    bitmap_cmake = wx_project_root / "bitmap2component" / "CMakeLists.txt"
    if bitmap_cmake.parent.exists():
        print(f"\nUpdating {bitmap_cmake}")
        content = generate_bitmap2component_cmake(wx_project_root)
        with open(bitmap_cmake, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"  Updated with {len([f for f in copied_files.get('bitmap2component', [])])} files")

    # Update eeschema/CMakeLists.txt
    eeschema_cmake = wx_project_root / "eeschema" / "CMakeLists.txt"
    if eeschema_cmake.parent.exists():
        print(f"\nUpdating {eeschema_cmake}")
        categories = collect_files_for_eeschema(wx_project_root)
        content = generate_eeschema_cmake(categories)
        with open(eeschema_cmake, 'w', encoding='utf-8') as f:
            f.write(content)

        # Print summary
        total_files = sum(len(files) for files in categories.values())
        print(f"  Updated with {total_files} files organized in categories:")
        for cat, files in categories.items():
            if files:
                print(f"    {cat}: {len(files)} files")

    print("\nCMakeLists.txt files updated to match KiCad structure!")

if __name__ == "__main__":
    main()