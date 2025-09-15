#!/usr/bin/env python3
"""
Copy minset files to kicad_core_project_wx maintaining KiCad folder structure.
Generated files go to a separate 'generated' folder.
Creates a detailed copy log with status for each file.
"""

import json
import os
import shutil
from pathlib import Path
from typing import Dict, List, Set, Tuple
from datetime import datetime


def load_minset_files(scripts_dir: Path) -> tuple[List[str], List[str]]:
    """Load minset header and source files."""
    headers_file = scripts_dir / "tem" / "minset_headers.json"
    sources_file = scripts_dir / "tem" / "minset_sources.json"

    with open(headers_file, 'r') as f:
        headers_data = json.load(f)

    with open(sources_file, 'r') as f:
        sources_data = json.load(f)

    return headers_data['headers'], sources_data['sources']

def categorize_files(headers: List[str], sources: List[str], kicad_root: Path) -> Dict:
    """Categorize files into regular and generated."""
    # Original build path in the JSON files
    original_build_path = str(kicad_root / "build" / "x64-Debug")

    regular_headers = []
    generated_headers = []
    regular_sources = []
    generated_sources = []

    for header in headers:
        if original_build_path in header:
            generated_headers.append(header)
        else:
            regular_headers.append(header)

    for source in sources:
        if original_build_path in source:
            generated_sources.append(source)
        else:
            regular_sources.append(source)

    return {
        'regular_headers': regular_headers,
        'generated_headers': generated_headers,
        'regular_sources': regular_sources,
        'generated_sources': generated_sources
    }

def get_relative_path(file_path: str, kicad_root: Path) -> Path:
    """Get relative path from kicad root."""
    file_path = Path(file_path)
    try:
        return file_path.relative_to(kicad_root)
    except ValueError:
        # File is outside kicad root
        return file_path.name

def copy_file_with_structure(src_file: str, kicad_root: Path, qt_project_root: Path, is_generated: bool = False, actual_build_path: Path = None) -> Tuple[str, str]:
    """Copy file maintaining folder structure. Returns (file_path, status)."""
    # Map the original build path to actual build path if needed
    original_build_path = str(kicad_root / "build" / "x64-Debug")
    if actual_build_path and original_build_path in src_file:
        # Replace the original build path with actual build path
        src_file = src_file.replace(original_build_path, str(actual_build_path))

    src_path = Path(src_file)

    if not src_path.exists():
        print(f"Warning: Source file not found: {src_file}")
        return (src_file, "NOT_FOUND")

    if is_generated:
        # For generated files, extract relative path from build directory
        if actual_build_path and str(actual_build_path) in src_file:
            # Get relative path from actual build directory
            rel_path = get_relative_path(src_file, actual_build_path)
        else:
            # Fallback to original logic
            rel_path = get_relative_path(src_file, kicad_root / "build" / "x64-Debug")

        if isinstance(rel_path, Path):
            dst_path = qt_project_root / "generated" / rel_path
        else:
            dst_path = qt_project_root / "generated" / rel_path
    else:
        # For regular files, maintain exact structure
        rel_path = get_relative_path(src_file, kicad_root)
        dst_path = qt_project_root / rel_path

    # Skip if file already exists
    if dst_path.exists():
        print(f"Skipped (already exists): {rel_path}")
        return (str(dst_path), "SKIPPED_EXISTS")

    # Create directory if it doesn't exist
    dst_path.parent.mkdir(parents=True, exist_ok=True)

    # Copy the file
    shutil.copy2(src_path, dst_path)
    print(f"Copied: {rel_path}")
    return (str(dst_path), "COPIED")

def collect_cmake_directories(files: List[str], kicad_root: Path) -> Set[Path]:
    """Collect all directories that need CMakeLists.txt."""
    directories = set()

    for file_path in files:
        file_path = Path(file_path)
        try:
            rel_path = file_path.relative_to(kicad_root)
            # Add all parent directories
            current = rel_path.parent
            while current != Path('.'):
                directories.add(current)
                current = current.parent
        except ValueError:
            continue

    return directories

def copy_cmake_files(directories: Set[Path], kicad_root: Path, qt_project_root: Path) -> List[Tuple[str, str]]:
    """Copy CMakeLists.txt files for all directories. Returns list of (file_path, status)."""
    cmake_copy_log = []
    for directory in sorted(directories):
        src_cmake = kicad_root / directory / "CMakeLists.txt"
        dst_cmake = qt_project_root / directory / "CMakeLists.txt"

        if src_cmake.exists():
            if not dst_cmake.exists():
                dst_cmake.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(src_cmake, dst_cmake)
                print(f"Copied CMakeLists.txt: {directory}")
                cmake_copy_log.append((str(dst_cmake), "COPIED"))
            else:
                print(f"Skipped CMakeLists.txt (already exists): {directory}")
                cmake_copy_log.append((str(dst_cmake), "SKIPPED_EXISTS"))
    return cmake_copy_log

def copy_root_cmake_files(kicad_root: Path, qt_project_root: Path):
    """Copy root level CMake files - DISABLED as requested."""
    print("Skipping root CMake files as requested (CMakeLists.txt, CMakeSettings.json)")

    # Still copy cmake directory if it doesn't exist
    cmake_dir = kicad_root / "cmake"
    dst_cmake_dir = qt_project_root / "cmake"
    if cmake_dir.exists() and not dst_cmake_dir.exists():
        shutil.copytree(cmake_dir, dst_cmake_dir)
        print("Copied cmake directory")
    elif dst_cmake_dir.exists():
        print("Skipped cmake directory (already exists)")

def write_copy_log(log_file: Path, copy_results: List[Tuple[str, str]]):
    """Write detailed copy log to file."""
    with open(log_file, 'w', encoding='utf-8') as f:
        f.write(f"# File Copy Log - {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(f"# Status: COPIED (new file copied), SKIPPED_EXISTS (file already exists), NOT_FOUND (source not found)\n\n")

        # Group by status
        copied = [(p, s) for p, s in copy_results if s == "COPIED"]
        skipped = [(p, s) for p, s in copy_results if s == "SKIPPED_EXISTS"]
        not_found = [(p, s) for p, s in copy_results if s == "NOT_FOUND"]

        f.write(f"## Summary\n")
        f.write(f"- Total files: {len(copy_results)}\n")
        f.write(f"- Copied: {len(copied)}\n")
        f.write(f"- Skipped (exists): {len(skipped)}\n")
        f.write(f"- Not found: {len(not_found)}\n\n")

        f.write(f"## All Files (one per line)\n\n")
        for file_path, status in copy_results:
            if status == "SKIPPED_EXISTS":
                f.write(f"{file_path} [SKIPPED: already exists]\n")
            elif status == "NOT_FOUND":
                f.write(f"{file_path} [ERROR: source not found]\n")
            else:
                f.write(f"{file_path}\n")

def main():
    # Setup paths
    script_dir = Path(__file__).parent
    kicad_root = script_dir.parent
    qt_project_root = kicad_root / "kicad_core_project_wx"

    # Actual build directory path (mapped from original)
    actual_build_path = Path(r"Y:\wenming\kicad\build\x64-Debug")

    print(f"KiCad root: {kicad_root}")
    print(f"KiCad core project root: {qt_project_root}")

    # Create KiCad core project directory
    qt_project_root.mkdir(exist_ok=True)

    # Load minset files from scripts directory
    print("\nLoading minset files...")
    headers, sources = load_minset_files(script_dir)
    print(f"Found {len(headers)} headers and {len(sources)} sources")

    # Categorize files
    print("\nCategorizing files...")
    categorized = categorize_files(headers, sources, kicad_root)
    print(f"Regular headers: {len(categorized['regular_headers'])}")
    print(f"Generated headers: {len(categorized['generated_headers'])}")
    print(f"Regular sources: {len(categorized['regular_sources'])}")
    print(f"Generated sources: {len(categorized['generated_sources'])}")

    # Track all copy operations
    all_copy_results = []

    # Copy regular files
    print("\n=== Copying regular header files ===")
    for header in categorized['regular_headers']:
        result = copy_file_with_structure(header, kicad_root, qt_project_root, is_generated=False, actual_build_path=actual_build_path)
        all_copy_results.append(result)

    print("\n=== Copying regular source files ===")
    for source in categorized['regular_sources']:
        result = copy_file_with_structure(source, kicad_root, qt_project_root, is_generated=False, actual_build_path=actual_build_path)
        all_copy_results.append(result)

    # Copy generated files
    print("\n=== Copying generated header files ===")
    print(f"Using actual build path: {actual_build_path}")
    for header in categorized['generated_headers']:
        result = copy_file_with_structure(header, kicad_root, qt_project_root, is_generated=True, actual_build_path=actual_build_path)
        all_copy_results.append(result)

    print("\n=== Copying generated source files ===")
    for source in categorized['generated_sources']:
        result = copy_file_with_structure(source, kicad_root, qt_project_root, is_generated=True, actual_build_path=actual_build_path)
        all_copy_results.append(result)

    # Collect directories that need CMakeLists.txt
    print("\n=== Collecting CMake directories ===")
    all_files = categorized['regular_headers'] + categorized['regular_sources']
    cmake_dirs = collect_cmake_directories(all_files, kicad_root)

    # Copy CMakeLists.txt files
    print(f"\n=== Copying CMakeLists.txt files for {len(cmake_dirs)} directories ===")
    cmake_results = copy_cmake_files(cmake_dirs, kicad_root, qt_project_root)
    all_copy_results.extend(cmake_results)

    # Copy root CMake files
    print("\n=== Copying root CMake files ===")
    copy_root_cmake_files(kicad_root, qt_project_root)

    # Write detailed copy log
    log_file = qt_project_root / "file_copy_log.txt"
    write_copy_log(log_file, all_copy_results)

    # Create summary
    copied_count = len([r for r in all_copy_results if r[1] == "COPIED"])
    skipped_count = len([r for r in all_copy_results if r[1] == "SKIPPED_EXISTS"])
    not_found_count = len([r for r in all_copy_results if r[1] == "NOT_FOUND"])

    summary = {
        'total_headers': len(headers),
        'total_sources': len(sources),
        'regular_headers': len(categorized['regular_headers']),
        'generated_headers': len(categorized['generated_headers']),
        'regular_sources': len(categorized['regular_sources']),
        'generated_sources': len(categorized['generated_sources']),
        'cmake_directories': len(cmake_dirs),
        'files_copied': copied_count,
        'files_skipped': skipped_count,
        'files_not_found': not_found_count,
        'total_files_processed': len(all_copy_results)
    }

    summary_file = qt_project_root / "copy_summary.json"
    with open(summary_file, 'w') as f:
        json.dump(summary, f, indent=2)

    print("\n=== Copy Summary ===")
    print(json.dumps(summary, indent=2))
    print(f"\nSummary saved to: {summary_file}")
    print(f"Detailed file log saved to: {log_file}")
    print("\nAll files processed!")

if __name__ == "__main__":
    main()