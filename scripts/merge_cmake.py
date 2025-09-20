#!/usr/bin/env python3
"""
Script to merge wx CMakeLists.txt content into Qt CMakeLists.txt
Preserves Qt-specific configurations while adding wx content
"""

import sys
from pathlib import Path

def extract_file_list(cmake_content, lib_name):
    """Extract source file list from add_library command"""
    files = []
    in_library = False
    paren_count = 0

    for line in cmake_content.split('\n'):
        # Look for add_library command
        if f'add_library( {lib_name} STATIC' in line or f'add_library({lib_name} STATIC' in line:
            in_library = True
            paren_count = line.count('(') - line.count(')')
            continue

        if in_library:
            # Count parentheses to find the end
            paren_count += line.count('(') - line.count(')')

            # Skip empty lines and comments
            stripped = line.strip()
            if stripped and not stripped.startswith('#'):
                # Remove trailing comments
                if '#' in stripped:
                    stripped = stripped.split('#')[0].strip()
                if stripped and not stripped.startswith('${'):
                    files.append(stripped)

            # Check if we've reached the end
            if paren_count <= 0 and ')' in line:
                break

    return files

def merge_common_cmake():
    """Merge common/CMakeLists.txt files"""
    wx_file = Path("kicad_core_project_wx/common/CMakeLists.txt")
    qt_file = Path("kicad_core_project_qt/common/CMakeLists.txt")

    # Read both files
    wx_content = wx_file.read_text(encoding='utf-8')
    qt_content = qt_file.read_text(encoding='utf-8')

    # Extract source files from wx version
    wx_files = []
    in_library = False
    paren_count = 0

    for line in wx_content.split('\n'):
        if 'add_library( common STATIC' in line or 'add_library(common STATIC' in line:
            in_library = True
            paren_count = line.count('(') - line.count(')')
            continue

        if in_library:
            paren_count += line.count('(') - line.count(')')
            stripped = line.strip()

            # Add all non-empty, non-comment lines
            if stripped and not stripped.startswith('#'):
                # Skip variable references and MOC sources
                if not stripped.startswith('${'):
                    wx_files.append(line)  # Keep original indentation

            # Keep comments for context
            elif stripped.startswith('#'):
                wx_files.append(line)

            if paren_count <= 0 and ')' in line:
                break

    # Find the add_library section in Qt file and merge
    qt_lines = qt_content.split('\n')
    new_lines = []
    in_library = False
    library_processed = False
    paren_count = 0

    for i, line in enumerate(qt_lines):
        if 'add_library( common STATIC' in line or 'add_library(common STATIC' in line:
            new_lines.append(line)
            in_library = True
            paren_count = line.count('(') - line.count(')')

            # Add wx source files
            new_lines.extend(wx_files)

            # Add Qt-specific items
            new_lines.append("    ")
            new_lines.append("    # Qt-specific generated files")
            new_lines.append("    ${COMMON_GENERATED_SRCS}")
            new_lines.append("    ")
            new_lines.append("    # MOC generated files")
            new_lines.append("    ${MOC_SOURCES}")
            new_lines.append(")")

            library_processed = True

            # Skip the rest of the original add_library block
            for j in range(i+1, len(qt_lines)):
                paren_count += qt_lines[j].count('(') - qt_lines[j].count(')')
                if paren_count <= 0 and ')' in qt_lines[j]:
                    i = j
                    break
            continue

        if library_processed and i < len(qt_lines) - 1:
            # Skip lines that were part of the library definition
            if in_library:
                paren_count += line.count('(') - line.count(')')
                if paren_count <= 0 and ')' in line:
                    in_library = False
                continue

        new_lines.append(line)

    # Write merged content
    merged_content = '\n'.join(new_lines)
    qt_file.write_text(merged_content, encoding='utf-8')
    print(f"Merged common/CMakeLists.txt")

def merge_pcbnew_cmake():
    """Merge pcbnew/CMakeLists.txt files"""
    wx_file = Path("kicad_core_project_wx/pcbnew/CMakeLists.txt")
    qt_file = Path("kicad_core_project_qt/pcbnew/CMakeLists.txt")

    # Read both files
    wx_content = wx_file.read_text(encoding='utf-8')
    qt_content = qt_file.read_text(encoding='utf-8')

    # Extract source files from wx version
    wx_files = []
    in_library = False
    paren_count = 0

    for line in wx_content.split('\n'):
        if 'add_library( pcbnew STATIC' in line or 'add_library(pcbnew STATIC' in line:
            in_library = True
            paren_count = line.count('(') - line.count(')')
            continue

        if in_library:
            paren_count += line.count('(') - line.count(')')
            stripped = line.strip()

            # Add all non-empty lines (including comments for context)
            if stripped:
                wx_files.append(line)  # Keep original indentation

            if paren_count <= 0 and ')' in line:
                break

    # Find the add_library section in Qt file and replace with wx content
    qt_lines = qt_content.split('\n')
    new_lines = []
    in_library = False
    library_processed = False
    paren_count = 0

    for i, line in enumerate(qt_lines):
        if 'add_library( pcbnew STATIC' in line or 'add_library(pcbnew STATIC' in line:
            new_lines.append(line)
            in_library = True
            paren_count = line.count('(') - line.count(')')

            # Add wx source files
            new_lines.extend(wx_files)

            library_processed = True

            # Skip the rest of the original add_library block
            for j in range(i+1, len(qt_lines)):
                paren_count += qt_lines[j].count('(') - qt_lines[j].count(')')
                if paren_count <= 0 and ')' in qt_lines[j]:
                    i = j
                    break
            continue

        if library_processed and i < len(qt_lines) - 1:
            # Skip lines that were part of the library definition
            if in_library:
                paren_count += line.count('(') - line.count(')')
                if paren_count <= 0 and ')' in line:
                    in_library = False
                continue

        new_lines.append(line)

    # Write merged content
    merged_content = '\n'.join(new_lines)
    qt_file.write_text(merged_content, encoding='utf-8')
    print(f"Merged pcbnew/CMakeLists.txt")

def main():
    # Merge common/CMakeLists.txt
    merge_common_cmake()

    # Merge pcbnew/CMakeLists.txt
    merge_pcbnew_cmake()

    print("\nMerge complete!")
    print("Note: The merged files preserve Qt-specific configurations (MOC, Qt libraries)")
    print("while incorporating all source files from the wx version.")

if __name__ == "__main__":
    main()