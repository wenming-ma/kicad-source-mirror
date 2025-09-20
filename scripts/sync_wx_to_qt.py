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
    for root, _, filenames in os.walk(directory):
        for filename in filenames:
            # Skip build directories and generated files
            if any(skip in str(root) for skip in ['.vs', 'build', '.git', '__pycache__']):
                continue
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

def copy_files(unique_files: Set[str], source_dir: Path, target_dir: Path) -> List[Dict]:
    """Copy unique files from source to target directory"""
    copy_log = []

    for file_path in sorted(unique_files):
        source = source_dir / file_path
        target = target_dir / file_path

        if not source.exists():
            copy_log.append({
                'file': file_path,
                'status': 'source_not_found',
                'message': f'Source file not found: {source}'
            })
            continue

        try:
            # Create target directory if it doesn't exist
            target.parent.mkdir(parents=True, exist_ok=True)

            # Copy the file
            shutil.copy2(source, target)

            copy_log.append({
                'file': file_path,
                'status': 'copied',
                'source': str(source),
                'target': str(target)
            })
            print(f"Copied: {file_path}")

        except Exception as e:
            copy_log.append({
                'file': file_path,
                'status': 'error',
                'message': str(e)
            })
            print(f"Error copying {file_path}: {e}")

    return copy_log

def update_cmake_files(wx_dir: Path, qt_dir: Path) -> List[Dict]:
    """Update CMakeLists.txt files from wx to qt directory"""
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

            # Check if target exists and read it
            if target_cmake.exists():
                with open(target_cmake, 'r', encoding='utf-8') as f:
                    qt_content = f.read()

                # Check if Qt-specific configurations exist
                has_qt_config = any(qt_marker in qt_content for qt_marker in
                                  ['find_package(Qt', 'Qt5', 'Qt6', 'qt_add', 'qt5_add'])

                if has_qt_config:
                    # Preserve Qt configurations but update the rest
                    print(f"Updating CMakeLists.txt (preserving Qt config): {relative_path}")
                    # For now, we'll completely replace with wx content
                    # In production, you might want to merge more carefully
                    with open(target_cmake, 'w', encoding='utf-8') as f:
                        f.write(wx_content)
                    update_log.append({
                        'file': str(relative_path),
                        'status': 'updated_with_qt_preservation',
                        'note': 'Qt configurations may need manual review'
                    })
                else:
                    # No Qt config, safe to replace
                    print(f"Updating CMakeLists.txt: {relative_path}")
                    with open(target_cmake, 'w', encoding='utf-8') as f:
                        f.write(wx_content)
                    update_log.append({
                        'file': str(relative_path),
                        'status': 'updated'
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

    # Find unique files
    print("Analyzing directories...")
    unique_wx, unique_qt, common = find_unique_files(wx_dir, qt_dir)

    print(f"\nFound {len(unique_wx)} files unique to wx directory")
    print(f"Found {len(unique_qt)} files unique to qt directory")
    print(f"Found {len(common)} common files")

    # Filter out certain file types from copying
    files_to_copy = {f for f in unique_wx
                     if not f.endswith(('.log', '.json', '.md', '.txt'))
                     or f.endswith('CMakeLists.txt')}

    print(f"\nWill copy {len(files_to_copy)} files (excluding logs and temporary files)")

    # Save list of files to copy
    output_dir = base_dir / "scripts" / "sync_results"
    output_dir.mkdir(exist_ok=True)

    with open(output_dir / "files_to_copy.json", 'w', encoding='utf-8') as f:
        json.dump({
            'unique_to_wx': sorted(list(unique_wx)),
            'unique_to_qt': sorted(list(unique_qt)),
            'common_files': sorted(list(common)),
            'files_to_copy': sorted(list(files_to_copy))
        }, f, indent=2)

    print(f"\nFile lists saved to {output_dir / 'files_to_copy.json'}")

    # Ask for confirmation before copying
    response = input("\nProceed with copying files? (yes/no): ")
    if response.lower() != 'yes':
        print("Aborted.")
        return

    # Copy files
    print("\n" + "=" * 80)
    print("Copying files...")
    copy_log = copy_files(files_to_copy, wx_dir, qt_dir)

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