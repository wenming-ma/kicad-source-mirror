#!/usr/bin/env python3
"""
Create a focused summary of meaningful file differences between Qt and wx versions
Excludes build artifacts, IDE files, and third-party dependencies
"""

import json
from pathlib import Path

def is_source_file(file_path):
    """Check if file is a meaningful source file"""
    path = Path(file_path)

    # Exclude certain directories
    exclude_dirs = {
        '.vs', 'build', 'vcpkg_installed', '.cache', 'CMakeFiles',
        'out', 'generated', '__pycache__', '.git'
    }

    for part in path.parts:
        if part in exclude_dirs:
            return False

    # Include source and configuration files
    source_extensions = {
        '.cpp', '.cc', '.cxx', '.c',  # C++ source
        '.h', '.hpp', '.hxx',         # Headers
        '.cmake', '.txt',             # CMake files
        '.py',                        # Python
        '.json',                      # Configuration
        '.md',                        # Documentation
        '.xml', '.html'               # Other config
    }

    # Exclude certain file types
    exclude_extensions = {
        '.pyc', '.pdb', '.dll', '.lib', '.exe', '.obj', '.o',  # Binaries
        '.db', '.vsidx', '.ipch', '.wsuo', '.bin',             # IDE files
        '.backup', '.log'                                      # Temp files
    }

    if path.suffix.lower() in exclude_extensions:
        return False

    # Include if it has a source extension or no extension (could be important config)
    return path.suffix.lower() in source_extensions or path.suffix == ''

def main():
    # Load the full analysis
    with open('qt_wx_file_analysis.json', 'r', encoding='utf-8') as f:
        data = json.load(f)

    # Filter to meaningful source files only
    all_qt_files = set(data['file_lists']['qt_only_files'])
    all_wx_files = set(data['file_lists']['wx_only_files'])
    all_shared_files = set(data['file_lists']['shared_files'])

    qt_source_files = {f for f in all_qt_files if is_source_file(f)}
    wx_source_files = {f for f in all_wx_files if is_source_file(f)}
    shared_source_files = {f for f in all_shared_files if is_source_file(f)}

    # Create focused summary
    with open('qt_wx_source_differences_summary.txt', 'w', encoding='utf-8') as f:
        f.write("KiCad Qt vs wx Source Code Differences Summary\n")
        f.write("=" * 50 + "\n\n")

        f.write("FOCUSED ANALYSIS (Source Files Only)\n")
        f.write("-" * 40 + "\n")
        f.write(f"Qt-only source files: {len(qt_source_files)}\n")
        f.write(f"wx-only source files: {len(wx_source_files)}\n")
        f.write(f"Shared source files: {len(shared_source_files)}\n")
        f.write(f"Total source files in Qt: {len(qt_source_files) + len(shared_source_files)}\n")
        f.write(f"Total source files in wx: {len(wx_source_files) + len(shared_source_files)}\n\n")

        f.write("QT-ONLY SOURCE FILES\n")
        f.write("-" * 25 + "\n")
        for file in sorted(qt_source_files):
            f.write(f"  {file}\n")

        f.write(f"\nWX-ONLY SOURCE FILES\n")
        f.write("-" * 25 + "\n")
        for file in sorted(wx_source_files):
            f.write(f"  {file}\n")

        # Analyze file types in differences
        qt_extensions = {}
        wx_extensions = {}

        for file in qt_source_files:
            ext = Path(file).suffix.lower() or '[no extension]'
            qt_extensions[ext] = qt_extensions.get(ext, 0) + 1

        for file in wx_source_files:
            ext = Path(file).suffix.lower() or '[no extension]'
            wx_extensions[ext] = wx_extensions.get(ext, 0) + 1

        f.write(f"\nFILE TYPE BREAKDOWN\n")
        f.write("-" * 20 + "\n")
        f.write("Qt-only by extension:\n")
        for ext, count in sorted(qt_extensions.items(), key=lambda x: x[1], reverse=True):
            f.write(f"  {ext}: {count}\n")

        f.write("\nwx-only by extension:\n")
        for ext, count in sorted(wx_extensions.items(), key=lambda x: x[1], reverse=True):
            f.write(f"  {ext}: {count}\n")

    print("Focused source code differences summary created:")
    print("  - qt_wx_source_differences_summary.txt")
    print(f"\nKey findings:")
    print(f"  - Qt version has {len(qt_source_files)} unique source files")
    print(f"  - wx version has {len(wx_source_files)} unique source files")
    print(f"  - {len(shared_source_files)} source files are shared between versions")

if __name__ == "__main__":
    main()