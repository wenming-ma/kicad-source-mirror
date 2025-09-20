#!/usr/bin/env python3
"""
Compare and sync CMakeLists.txt files from wx to qt project.
Only backup and replace files that are different.
"""

import os
import shutil
import filecmp
from pathlib import Path
from datetime import datetime

def find_cmake_files(project_dir):
    """Find all CMakeLists.txt files in a project directory"""
    cmake_files = []
    for root, dirs, files in os.walk(project_dir):
        # Skip build directories
        if 'build' in root.split(os.sep):
            continue
        if 'CMakeLists.txt' in files:
            relative_path = os.path.relpath(os.path.join(root, 'CMakeLists.txt'), project_dir)
            cmake_files.append(relative_path)
    return sorted(cmake_files)

def backup_file(file_path):
    """Create a backup of a file with timestamp"""
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    backup_path = f"{file_path}.backup_{timestamp}"
    shutil.copy2(file_path, backup_path)
    return backup_path

def extract_qt_content(file_path):
    """Extract Qt-specific content from a CMakeLists.txt file"""
    if not os.path.exists(file_path):
        return []

    qt_sections = []
    with open(file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    in_qt_section = False
    current_section = []

    for line in lines:
        # Check for Qt-related content
        if any(qt_marker in line.lower() for qt_marker in ['qt6', 'qt::', 'moc_', 'automoc', 'autorcc', 'autouic', 'qt6_wrap']):
            in_qt_section = True
            current_section.append(line)
        elif in_qt_section:
            # Continue collecting lines that might be part of Qt configuration
            if line.strip() and not line.strip().startswith('#'):
                current_section.append(line)
            else:
                # End of Qt section
                if current_section:
                    qt_sections.append(''.join(current_section))
                current_section = []
                in_qt_section = False

    # Don't forget the last section
    if current_section:
        qt_sections.append(''.join(current_section))

    return qt_sections

def merge_qt_content(wx_file, qt_sections):
    """Merge Qt-specific content into wx CMakeLists.txt"""
    with open(wx_file, 'r', encoding='utf-8') as f:
        content = f.read()

    # Add Qt sections at appropriate places
    # For now, we'll append them at the end, but this could be made smarter
    if qt_sections:
        content += "\n# Qt-specific configuration (preserved from original)\n"
        for section in qt_sections:
            content += section
            if not section.endswith('\n'):
                content += '\n'

    return content

def main():
    wx_dir = Path('kicad_core_project_wx')
    qt_dir = Path('kicad_core_project_qt')

    if not wx_dir.exists() or not qt_dir.exists():
        print("Error: Both project directories must exist")
        return

    # Find all CMakeLists.txt files
    wx_cmake_files = find_cmake_files(wx_dir)
    qt_cmake_files = find_cmake_files(qt_dir)

    print(f"Found {len(wx_cmake_files)} CMakeLists.txt files in wx project")
    print(f"Found {len(qt_cmake_files)} CMakeLists.txt files in qt project")
    print()

    # Track statistics
    identical_count = 0
    replaced_count = 0
    new_count = 0

    # Process each wx CMakeLists.txt file
    for cmake_file in wx_cmake_files:
        wx_file = wx_dir / cmake_file
        qt_file = qt_dir / cmake_file

        if qt_file.exists():
            # Compare files
            if not filecmp.cmp(wx_file, qt_file, shallow=False):
                print(f"Different: {cmake_file}")

                # Extract Qt-specific content before replacing
                qt_sections = extract_qt_content(qt_file)

                # Backup the Qt version
                backup_path = backup_file(qt_file)
                print(f"  Backed up to: {os.path.basename(backup_path)}")

                # Copy wx version to qt
                shutil.copy2(wx_file, qt_file)

                # If there was Qt-specific content, merge it back
                if qt_sections:
                    merged_content = merge_qt_content(qt_file, qt_sections)
                    with open(qt_file, 'w', encoding='utf-8') as f:
                        f.write(merged_content)
                    print(f"  Replaced with wx version and preserved Qt content")
                else:
                    print(f"  Replaced with wx version")

                replaced_count += 1
            else:
                # Files are identical
                identical_count += 1
        else:
            # Qt file doesn't exist, copy from wx
            print(f"New file: {cmake_file}")
            qt_file.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(wx_file, qt_file)
            print(f"  Copied from wx project")
            new_count += 1

    # Check for Qt-only CMakeLists.txt files
    qt_only_files = set(qt_cmake_files) - set(wx_cmake_files)
    if qt_only_files:
        print("\nQt-only CMakeLists.txt files (not modified):")
        for cmake_file in qt_only_files:
            print(f"  {cmake_file}")

    print(f"\nSummary:")
    print(f"  Identical files: {identical_count}")
    print(f"  Replaced files: {replaced_count}")
    print(f"  New files: {new_count}")
    print(f"  Qt-only files: {len(qt_only_files)}")

if __name__ == "__main__":
    main()