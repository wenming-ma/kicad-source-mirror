#!/usr/bin/env python3
"""
Update CMakeLists.txt files in kicad_core_project_wx
- Simplify newly copied CMakeLists.txt files
- Update existing ones to include new files
"""

import os
import re
from pathlib import Path
from typing import List, Set, Dict

def get_files_in_directory(directory: Path, extensions: List[str]) -> Set[str]:
    """Get all files with specified extensions in a directory."""
    files = set()
    for ext in extensions:
        for file in directory.glob(f"*.{ext}"):
            if file.is_file():
                files.add(file.name)
    return files

def parse_cmake_sources(cmake_content: str) -> Dict[str, List[str]]:
    """Parse source files from CMakeLists.txt content."""
    sources = {'cpp': [], 'h': [], 'c': []}

    # Find set() commands with source files
    set_pattern = r'set\s*\(\s*(\w+)\s+(.*?)\s*\)'
    matches = re.finditer(set_pattern, cmake_content, re.DOTALL)

    for match in matches:
        var_name = match.group(1)
        files_text = match.group(2)

        # Extract file names
        file_pattern = r'[\w/\\]+\.\w+'
        files = re.findall(file_pattern, files_text)

        for file in files:
            file = os.path.basename(file)
            if file.endswith('.cpp'):
                sources['cpp'].append(file)
            elif file.endswith('.h') or file.endswith('.hpp'):
                sources['h'].append(file)
            elif file.endswith('.c'):
                sources['c'].append(file)

    return sources

def generate_simple_cmake(directory: Path, project_name: str) -> str:
    """Generate a simplified CMakeLists.txt for a directory."""
    # Get all source files in the directory
    cpp_files = sorted(get_files_in_directory(directory, ['cpp']))
    c_files = sorted(get_files_in_directory(directory, ['c']))
    h_files = sorted(get_files_in_directory(directory, ['h', 'hpp']))

    cmake_content = []
    cmake_content.append(f"# Simplified CMakeLists.txt for {project_name}")
    cmake_content.append("")

    # Add source files
    if cpp_files or c_files:
        cmake_content.append(f"set({project_name.upper()}_SRCS")
        for file in cpp_files:
            cmake_content.append(f"    {file}")
        for file in c_files:
            cmake_content.append(f"    {file}")
        cmake_content.append(")")
        cmake_content.append("")

    # Add header files
    if h_files:
        cmake_content.append(f"set({project_name.upper()}_HEADERS")
        for file in h_files:
            cmake_content.append(f"    {file}")
        cmake_content.append(")")
        cmake_content.append("")

    # Add library or executable
    if cpp_files or c_files:
        cmake_content.append(f"# Build library")
        cmake_content.append(f"add_library({project_name} STATIC")
        cmake_content.append(f"    ${{{project_name.upper()}_SRCS}}")
        if h_files:
            cmake_content.append(f"    ${{{project_name.upper()}_HEADERS}}")
        cmake_content.append(")")
        cmake_content.append("")

        # Add include directories
        cmake_content.append("target_include_directories({} PUBLIC".format(project_name))
        cmake_content.append("    ${CMAKE_CURRENT_SOURCE_DIR}")
        cmake_content.append("    ${CMAKE_SOURCE_DIR}/include")
        cmake_content.append("    ${CMAKE_SOURCE_DIR}/libs/core/include")
        cmake_content.append("    ${CMAKE_SOURCE_DIR}/libs/kimath/include")
        cmake_content.append("    ${CMAKE_SOURCE_DIR}/libs/kiplatform/include")
        cmake_content.append("    ${CMAKE_SOURCE_DIR}/thirdparty")
        cmake_content.append(")")

    return "\n".join(cmake_content)

def update_cmake_for_directory(dir_path: Path, log_file: Path):
    """Update CMakeLists.txt for a specific directory."""
    cmake_file = dir_path / "CMakeLists.txt"

    if not cmake_file.exists():
        print(f"No CMakeLists.txt in {dir_path}")
        return

    # Read the log file to determine which files are new
    with open(log_file, 'r', encoding='utf-8') as f:
        log_content = f.read()

    # Check if this directory has new files
    dir_name = dir_path.name
    new_files_in_dir = []

    for line in log_content.split('\n'):
        if dir_name in line and "[SKIPPED" not in line and "[ERROR" not in line:
            # Extract filename
            if '\\' in line:
                parts = line.split('\\')
                for i, part in enumerate(parts):
                    if part == dir_name and i < len(parts) - 1:
                        filename = parts[-1]
                        if '.' in filename:
                            new_files_in_dir.append(filename)
                        break

    if new_files_in_dir:
        print(f"\nDirectory {dir_name} has {len(new_files_in_dir)} new files")

        # Generate simplified CMake
        simplified_cmake = generate_simple_cmake(dir_path, dir_name)

        # Backup original
        backup_file = cmake_file.with_suffix('.txt.bak')
        if not backup_file.exists():
            with open(cmake_file, 'r', encoding='utf-8') as f:
                original_content = f.read()
            with open(backup_file, 'w', encoding='utf-8') as f:
                f.write(original_content)

        # Write simplified version
        with open(cmake_file, 'w', encoding='utf-8') as f:
            f.write(simplified_cmake)

        print(f"Updated {cmake_file}")
    else:
        print(f"Directory {dir_name} has no new files, keeping existing CMakeLists.txt")

def main():
    # Setup paths
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    wx_project_root = project_root / "kicad_core_project_wx"
    log_file = wx_project_root / "file_copy_log.txt"

    print(f"Project root: {wx_project_root}")
    print(f"Log file: {log_file}")

    # Key directories to process
    directories_to_process = [
        wx_project_root / "bitmap2component",
        wx_project_root / "eeschema",
        wx_project_root / "eeschema" / "sch_io",
        wx_project_root / "eeschema" / "sch_io" / "kicad_sexpr",
        wx_project_root / "eeschema" / "sch_io" / "kicad_legacy",
        wx_project_root / "eeschema" / "sch_io" / "database",
        wx_project_root / "eeschema" / "dialogs",
        wx_project_root / "eeschema" / "erc",
        wx_project_root / "eeschema" / "netlist_exporters",
        wx_project_root / "eeschema" / "sim",
        wx_project_root / "eeschema" / "symbol_editor",
        wx_project_root / "eeschema" / "sync_sheet_pin",
        wx_project_root / "eeschema" / "tools",
        wx_project_root / "eeschema" / "widgets",
    ]

    # Process each directory
    for directory in directories_to_process:
        if directory.exists():
            update_cmake_for_directory(directory, log_file)

    print("\nCMakeLists.txt update complete!")

if __name__ == "__main__":
    main()