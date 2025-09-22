#!/usr/bin/env python3
"""
Copy all CMakeLists.txt files from wx to qt project
"""

import os
import shutil
from pathlib import Path

def copy_all_cmake_files():
    wx_dir = Path('kicad_core_project_wx')
    qt_dir = Path('kicad_core_project_qt')

    copied_count = 0

    # Find and copy all CMakeLists.txt files
    for root, dirs, files in os.walk(wx_dir):
        # Skip build directories
        if 'build' in root.split(os.sep):
            continue

        if 'CMakeLists.txt' in files:
            src_file = Path(root) / 'CMakeLists.txt'
            relative_path = src_file.relative_to(wx_dir)
            dst_file = qt_dir / relative_path

            # Create directory if needed
            dst_file.parent.mkdir(parents=True, exist_ok=True)

            # Copy the file
            shutil.copy2(src_file, dst_file)
            print(f"Copied: {relative_path}")
            copied_count += 1

    print(f"\nTotal files copied: {copied_count}")

if __name__ == "__main__":
    copy_all_cmake_files()