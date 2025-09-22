#!/usr/bin/env python3
"""
Script to sync kicad_core_project_wx unique files to kicad_core_project_qt
and update corresponding CMakeLists.txt files
"""

import os
import shutil
import json
from pathlib import Path
from typing import Set, List, Dict, Tuple

def get_all_files(directory: Path, base_dir: Path) -> Set[str]:
    """Get all files in directory relative to base directory"""
    files = set()
    skip_dirs = ['.vs', 'build', '.git', '__pycache__', '.vscode', 'Debug', 'Release', 'x64-Debug', 'x64-Release']

    for root, dirs, filenames in os.walk(directory):
        # Skip unwanted directories
        dirs[:] = [d for d in dirs if not any(skip in d for skip in skip_dirs)]

        # Skip if current directory contains skip patterns
        if any(skip in str(root) for skip in skip_dirs):
            continue

        for filename in filenames:
            file_path = Path(root) / filename
            relative_path = file_path.relative_to(base_dir)
            files.add(str(relative_path).replace('\\', '/'))
    return files

def find_unique_files(wx_dir: Path, qt_dir: Path) -> Tuple[Set[str], Set[str], Set[str]]:
    """Find files unique to wx directory, unique to qt, and common files"""
    wx_files = get_all_files(wx_dir, wx_dir)
    qt_files = get_all_files(qt_dir, qt_dir)

    unique_wx = wx_files - qt_files
    unique_qt = qt_files - wx_files
    common = wx_files & qt_files

    return unique_wx, unique_qt, common

def copy_files_safely(wx_dir: Path, qt_dir: Path) -> List[Dict]:
    """Copy files from wx to qt, skipping existing files"""
    copy_log = []
    skip_dirs = ['.vs', 'build', '.git', '__pycache__', '.vscode', 'Debug', 'Release', 'x64-Debug', 'x64-Release']

    # Get all files from wx directory
    for root, dirs, filenames in os.walk(wx_dir):
        # Skip unwanted directories
        dirs[:] = [d for d in dirs if not any(skip in d for skip in skip_dirs)]

        # Skip if current directory contains skip patterns
        if any(skip in str(root) for skip in skip_dirs):
            continue

        for filename in filenames:
            # Skip log and temporary files
            if filename in ['file_copy_log.txt', 'copy_summary.json', 'link-error.txt'] or filename.endswith('.log'):
                continue

            source_file = Path(root) / filename
            relative_path = source_file.relative_to(wx_dir)
            target_file = qt_dir / relative_path

            # Skip if target file already exists
            if target_file.exists():
                copy_log.append({
                    'file': str(relative_path).replace('\\', '/'),
                    'status': 'skipped',
                    'message': 'Target file already exists'
                })
                print(f"Skipped (exists): {relative_path}")
                continue

            try:
                # Create target directory if it doesn't exist
                target_file.parent.mkdir(parents=True, exist_ok=True)

                # Copy the file
                shutil.copy2(source_file, target_file)

                copy_log.append({
                    'file': str(relative_path).replace('\\', '/'),
                    'status': 'copied',
                    'source': str(source_file),
                    'target': str(target_file)
                })
                print(f"Copied: {relative_path}")

            except Exception as e:
                copy_log.append({
                    'file': str(relative_path).replace('\\', '/'),
                    'status': 'error',
                    'message': str(e)
                })
                print(f"Error copying {relative_path}: {e}")

    return copy_log

def update_cmake_files(wx_dir: Path, qt_dir: Path) -> List[Dict]:
    """Update CMakeLists.txt files from wx to qt directory (preserving Qt configs)"""
    update_log = []

    # Find all CMakeLists.txt files in wx directory
    for cmake_file in wx_dir.rglob("CMakeLists.txt"):
        # Skip build directories
        if any(skip in str(cmake_file) for skip in ['.vs', 'build']):
            continue

        relative_path = cmake_file.relative_to(wx_dir)
        target_cmake = qt_dir / relative_path

        try:
            # Read wx CMakeLists.txt content
            with open(cmake_file, 'r', encoding='utf-8') as f:
                wx_content = f.read()

            # Check if target exists
            if target_cmake.exists():
                # Always preserve existing Qt CMakeLists.txt files
                print(f"Skipped (preserving existing): CMakeLists.txt - {relative_path}")
                update_log.append({
                    'file': str(relative_path),
                    'status': 'skipped',
                    'message': 'Preserving existing Qt CMakeLists.txt'
                })
            else:
                # Target doesn't exist, create it
                target_cmake.parent.mkdir(parents=True, exist_ok=True)
                print(f"Creating CMakeLists.txt: {relative_path}")
                with open(target_cmake, 'w', encoding='utf-8') as f:
                    f.write(wx_content)
                update_log.append({
                    'file': str(relative_path),
                    'status': 'created'
                })

        except Exception as e:
            update_log.append({
                'file': str(relative_path),
                'status': 'error',
                'message': str(e)
            })
            print(f"Error updating {relative_path}: {e}")

    return update_log

def main():
    # Setup paths
    base_dir = Path(__file__).parent.parent
    wx_dir = base_dir / "kicad_core_project_wx"
    qt_dir = base_dir / "kicad_core_project_qt"

    print(f"Source directory: {wx_dir}")
    print(f"Target directory: {qt_dir}")
    print("-" * 80)

    # Proceed with copying automatically
    print("Synchronizing files (skipping existing files)...")

    # Copy files safely
    print("\n" + "=" * 80)
    print("Copying files...")
    copy_log = copy_files_safely(wx_dir, qt_dir)

    # Create output directory and save results
    output_dir = base_dir / "scripts" / "sync_results"
    output_dir.mkdir(exist_ok=True)

    # Save copy log
    with open(output_dir / "copy_log.json", 'w', encoding='utf-8') as f:
        json.dump(copy_log, f, indent=2)

    # Update CMakeLists.txt files
    print("\n" + "=" * 80)
    print("Updating CMakeLists.txt files...")
    cmake_log = update_cmake_files(wx_dir, qt_dir)

    # Save cmake update log
    with open(output_dir / "cmake_update_log.json", 'w', encoding='utf-8') as f:
        json.dump(cmake_log, f, indent=2)

    # Print summary
    print("\n" + "=" * 80)
    print("SUMMARY")
    print("-" * 80)

    copied_count = len([x for x in copy_log if x['status'] == 'copied'])
    error_count = len([x for x in copy_log if x['status'] == 'error'])

    print(f"Files copied successfully: {copied_count}")
    print(f"Files with errors: {error_count}")

    cmake_updated = len([x for x in cmake_log if x['status'] in ['updated', 'created']])
    cmake_errors = len([x for x in cmake_log if x['status'] == 'error'])

    print(f"CMakeLists.txt files updated: {cmake_updated}")
    print(f"CMakeLists.txt errors: {cmake_errors}")

    print(f"\nLogs saved to: {output_dir}")
    print("  - files_to_copy.json: List of all analyzed files")
    print("  - copy_log.json: Detailed copy operation log")
    print("  - cmake_update_log.json: CMakeLists.txt update log")

if __name__ == "__main__":
    main()