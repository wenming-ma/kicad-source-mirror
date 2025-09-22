#!/usr/bin/env python3
"""
Analyze file differences between Qt and wx versions of KiCad project
"""

import os
import json
from pathlib import Path
from collections import defaultdict

def get_file_list(directory, base_path=""):
    """Recursively get all files in a directory with relative paths"""
    files = set()
    directory = Path(directory)

    if not directory.exists():
        return files

    for item in directory.rglob('*'):
        if item.is_file():
            # Get relative path from the base directory
            rel_path = item.relative_to(directory)
            files.add(str(rel_path))

    return files

def analyze_file_extensions(files):
    """Analyze file extensions and their counts"""
    extensions = defaultdict(int)
    for file in files:
        ext = Path(file).suffix.lower()
        if not ext:
            ext = '[no extension]'
        extensions[ext] += 1
    return dict(extensions)

def analyze_directory_structure(files):
    """Analyze directory structure and file distribution"""
    dirs = defaultdict(int)
    for file in files:
        dir_path = str(Path(file).parent)
        if dir_path == '.':
            dir_path = '[root]'
        dirs[dir_path] += 1
    return dict(dirs)

def main():
    qt_dir = "kicad_core_project_qt"
    wx_dir = "kicad_core_project_wx"

    print("Analyzing Qt and wx version file differences...")
    print(f"Qt directory: {qt_dir}")
    print(f"wx directory: {wx_dir}")

    # Get file lists
    qt_files = get_file_list(qt_dir)
    wx_files = get_file_list(wx_dir)

    # Calculate differences
    shared_files = qt_files & wx_files
    qt_only_files = qt_files - wx_files
    wx_only_files = wx_files - qt_files

    # Analyze file extensions
    qt_extensions = analyze_file_extensions(qt_files)
    wx_extensions = analyze_file_extensions(wx_files)
    shared_extensions = analyze_file_extensions(shared_files)

    # Analyze directory structures
    qt_dirs = analyze_directory_structure(qt_files)
    wx_dirs = analyze_directory_structure(wx_files)

    # Create comprehensive report
    report = {
        "summary": {
            "total_qt_files": len(qt_files),
            "total_wx_files": len(wx_files),
            "shared_files": len(shared_files),
            "qt_only_files": len(qt_only_files),
            "wx_only_files": len(wx_only_files),
            "shared_percentage": round(len(shared_files) / max(len(qt_files), len(wx_files)) * 100, 2) if max(len(qt_files), len(wx_files)) > 0 else 0
        },
        "file_extensions": {
            "qt_extensions": qt_extensions,
            "wx_extensions": wx_extensions,
            "shared_extensions": shared_extensions
        },
        "directory_structure": {
            "qt_directories": qt_dirs,
            "wx_directories": wx_dirs
        },
        "file_lists": {
            "shared_files": sorted(list(shared_files)),
            "qt_only_files": sorted(list(qt_only_files)),
            "wx_only_files": sorted(list(wx_only_files))
        }
    }

    # Print summary to console
    print("\n" + "="*60)
    print("SUMMARY")
    print("="*60)
    print(f"Total Qt files: {report['summary']['total_qt_files']}")
    print(f"Total wx files: {report['summary']['total_wx_files']}")
    print(f"Shared files: {report['summary']['shared_files']}")
    print(f"Qt-only files: {report['summary']['qt_only_files']}")
    print(f"wx-only files: {report['summary']['wx_only_files']}")
    print(f"Shared percentage: {report['summary']['shared_percentage']}%")

    print("\n" + "="*60)
    print("TOP FILE EXTENSIONS")
    print("="*60)
    print("\nQt version:")
    for ext, count in sorted(qt_extensions.items(), key=lambda x: x[1], reverse=True)[:10]:
        print(f"  {ext}: {count}")

    print("\nwx version:")
    for ext, count in sorted(wx_extensions.items(), key=lambda x: x[1], reverse=True)[:10]:
        print(f"  {ext}: {count}")

    print("\n" + "="*60)
    print("TOP DIRECTORIES BY FILE COUNT")
    print("="*60)
    print("\nQt version:")
    for dir_path, count in sorted(qt_dirs.items(), key=lambda x: x[1], reverse=True)[:10]:
        print(f"  {dir_path}: {count}")

    print("\nwx version:")
    for dir_path, count in sorted(wx_dirs.items(), key=lambda x: x[1], reverse=True)[:10]:
        print(f"  {dir_path}: {count}")

    # Save detailed report to JSON file
    with open('qt_wx_file_analysis.json', 'w', encoding='utf-8') as f:
        json.dump(report, f, indent=2, ensure_ascii=False)

    # Create a human-readable text report
    with open('qt_wx_file_analysis.txt', 'w', encoding='utf-8') as f:
        f.write("KiCad Qt vs wx Version File Analysis Report\n")
        f.write("=" * 50 + "\n\n")

        f.write("EXECUTIVE SUMMARY\n")
        f.write("-" * 20 + "\n")
        f.write(f"Total Qt files: {report['summary']['total_qt_files']}\n")
        f.write(f"Total wx files: {report['summary']['total_wx_files']}\n")
        f.write(f"Shared files: {report['summary']['shared_files']}\n")
        f.write(f"Qt-only files: {report['summary']['qt_only_files']}\n")
        f.write(f"wx-only files: {report['summary']['wx_only_files']}\n")
        f.write(f"Shared percentage: {report['summary']['shared_percentage']}%\n\n")

        f.write("FILE EXTENSION ANALYSIS\n")
        f.write("-" * 25 + "\n")
        f.write("Qt version top extensions:\n")
        for ext, count in sorted(qt_extensions.items(), key=lambda x: x[1], reverse=True)[:15]:
            f.write(f"  {ext}: {count}\n")

        f.write("\nwx version top extensions:\n")
        for ext, count in sorted(wx_extensions.items(), key=lambda x: x[1], reverse=True)[:15]:
            f.write(f"  {ext}: {count}\n")

        f.write("\nShared files extensions:\n")
        for ext, count in sorted(shared_extensions.items(), key=lambda x: x[1], reverse=True)[:15]:
            f.write(f"  {ext}: {count}\n")

        f.write("\n\nDIRECTORY STRUCTURE ANALYSIS\n")
        f.write("-" * 30 + "\n")
        f.write("Qt version top directories:\n")
        for dir_path, count in sorted(qt_dirs.items(), key=lambda x: x[1], reverse=True)[:20]:
            f.write(f"  {dir_path}: {count}\n")

        f.write("\nwx version top directories:\n")
        for dir_path, count in sorted(wx_dirs.items(), key=lambda x: x[1], reverse=True)[:20]:
            f.write(f"  {dir_path}: {count}\n")

        f.write("\n\nQT-ONLY FILES (First 50)\n")
        f.write("-" * 25 + "\n")
        for file in sorted(list(qt_only_files))[:50]:
            f.write(f"  {file}\n")
        if len(qt_only_files) > 50:
            f.write(f"  ... and {len(qt_only_files) - 50} more files\n")

        f.write("\n\nWX-ONLY FILES (First 50)\n")
        f.write("-" * 25 + "\n")
        for file in sorted(list(wx_only_files))[:50]:
            f.write(f"  {file}\n")
        if len(wx_only_files) > 50:
            f.write(f"  ... and {len(wx_only_files) - 50} more files\n")

        f.write("\n\nSHARED FILES (First 100)\n")
        f.write("-" * 20 + "\n")
        for file in sorted(list(shared_files))[:100]:
            f.write(f"  {file}\n")
        if len(shared_files) > 100:
            f.write(f"  ... and {len(shared_files) - 100} more files\n")

    print(f"\nDetailed analysis saved to:")
    print(f"  - qt_wx_file_analysis.json (machine-readable)")
    print(f"  - qt_wx_file_analysis.txt (human-readable)")

if __name__ == "__main__":
    main()