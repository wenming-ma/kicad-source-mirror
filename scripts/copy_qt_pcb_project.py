#!/usr/bin/env python3
"""
Copy minset files to Qt PCB project maintaining KiCad folder structure.
Generated files go to a separate 'generated' folder.
"""

import json
import os
import shutil
from pathlib import Path
from typing import Set, Dict, List

def load_minset_files(build_dir: Path) -> tuple[List[str], List[str]]:
    """Load minset header and source files."""
    headers_file = build_dir / "minset_headers.json"
    sources_file = build_dir / "minset_sources.json"
    
    with open(headers_file, 'r') as f:
        headers_data = json.load(f)
    
    with open(sources_file, 'r') as f:
        sources_data = json.load(f)
    
    return headers_data['headers'], sources_data['sources']

def categorize_files(headers: List[str], sources: List[str], kicad_root: Path) -> Dict:
    """Categorize files into regular and generated."""
    build_path = str(kicad_root / "build")
    
    regular_headers = []
    generated_headers = []
    regular_sources = []
    generated_sources = []
    
    for header in headers:
        if build_path in header:
            generated_headers.append(header)
        else:
            regular_headers.append(header)
    
    for source in sources:
        if build_path in source:
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

def copy_file_with_structure(src_file: str, kicad_root: Path, qt_project_root: Path, is_generated: bool = False):
    """Copy file maintaining folder structure."""
    src_path = Path(src_file)
    
    if not src_path.exists():
        print(f"Warning: Source file not found: {src_file}")
        return
    
    if is_generated:
        # For generated files, remove the 'build' prefix and put in 'generated' folder
        rel_path = get_relative_path(src_file, kicad_root / "build")
        if isinstance(rel_path, Path):
            dst_path = qt_project_root / "generated" / rel_path
        else:
            dst_path = qt_project_root / "generated" / rel_path
    else:
        # For regular files, maintain exact structure
        rel_path = get_relative_path(src_file, kicad_root)
        dst_path = qt_project_root / rel_path
    
    # Create directory if it doesn't exist
    dst_path.parent.mkdir(parents=True, exist_ok=True)
    
    # Copy the file
    shutil.copy2(src_path, dst_path)
    print(f"Copied: {rel_path}")

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

def copy_cmake_files(directories: Set[Path], kicad_root: Path, qt_project_root: Path):
    """Copy CMakeLists.txt files for all directories."""
    for directory in sorted(directories):
        src_cmake = kicad_root / directory / "CMakeLists.txt"
        if src_cmake.exists():
            dst_cmake = qt_project_root / directory / "CMakeLists.txt"
            dst_cmake.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(src_cmake, dst_cmake)
            print(f"Copied CMakeLists.txt: {directory}")

def copy_root_cmake_files(kicad_root: Path, qt_project_root: Path):
    """Copy root level CMake files."""
    root_files = [
        "CMakeLists.txt",
        "CMakeSettings.json",
        "KiCadVersion.cmake",
        "KiCadSentryDefaults.cmake"
    ]
    
    cmake_dir = kicad_root / "cmake"
    if cmake_dir.exists():
        dst_cmake_dir = qt_project_root / "cmake"
        if not dst_cmake_dir.exists():
            shutil.copytree(cmake_dir, dst_cmake_dir)
            print("Copied cmake directory")
    
    for file_name in root_files:
        src_file = kicad_root / file_name
        if src_file.exists():
            dst_file = qt_project_root / file_name
            shutil.copy2(src_file, dst_file)
            print(f"Copied root file: {file_name}")

def main():
    # Setup paths
    script_dir = Path(__file__).parent
    kicad_root = script_dir.parent
    build_dir = kicad_root / "build"
    qt_project_root = kicad_root / "qt_pcb_project"
    
    print(f"KiCad root: {kicad_root}")
    print(f"Qt project root: {qt_project_root}")
    
    # Create Qt project directory
    qt_project_root.mkdir(exist_ok=True)
    
    # Load minset files
    print("\nLoading minset files...")
    headers, sources = load_minset_files(build_dir)
    print(f"Found {len(headers)} headers and {len(sources)} sources")
    
    # Categorize files
    print("\nCategorizing files...")
    categorized = categorize_files(headers, sources, kicad_root)
    print(f"Regular headers: {len(categorized['regular_headers'])}")
    print(f"Generated headers: {len(categorized['generated_headers'])}")
    print(f"Regular sources: {len(categorized['regular_sources'])}")
    print(f"Generated sources: {len(categorized['generated_sources'])}")
    
    # Copy regular files
    print("\n=== Copying regular header files ===")
    for header in categorized['regular_headers']:
        copy_file_with_structure(header, kicad_root, qt_project_root, is_generated=False)
    
    print("\n=== Copying regular source files ===")
    for source in categorized['regular_sources']:
        copy_file_with_structure(source, kicad_root, qt_project_root, is_generated=False)
    
    # Copy generated files
    print("\n=== Copying generated header files ===")
    for header in categorized['generated_headers']:
        copy_file_with_structure(header, kicad_root, qt_project_root, is_generated=True)
    
    print("\n=== Copying generated source files ===")
    for source in categorized['generated_sources']:
        copy_file_with_structure(source, kicad_root, qt_project_root, is_generated=True)
    
    # Collect directories that need CMakeLists.txt
    print("\n=== Collecting CMake directories ===")
    all_files = categorized['regular_headers'] + categorized['regular_sources']
    cmake_dirs = collect_cmake_directories(all_files, kicad_root)
    
    # Copy CMakeLists.txt files
    print(f"\n=== Copying CMakeLists.txt files for {len(cmake_dirs)} directories ===")
    copy_cmake_files(cmake_dirs, kicad_root, qt_project_root)
    
    # Copy root CMake files
    print("\n=== Copying root CMake files ===")
    copy_root_cmake_files(kicad_root, qt_project_root)
    
    # Create summary
    summary = {
        'total_headers': len(headers),
        'total_sources': len(sources),
        'regular_headers': len(categorized['regular_headers']),
        'generated_headers': len(categorized['generated_headers']),
        'regular_sources': len(categorized['regular_sources']),
        'generated_sources': len(categorized['generated_sources']),
        'cmake_directories': len(cmake_dirs)
    }
    
    summary_file = qt_project_root / "copy_summary.json"
    with open(summary_file, 'w') as f:
        json.dump(summary, f, indent=2)
    
    print("\n=== Copy Summary ===")
    print(json.dumps(summary, indent=2))
    print(f"\nSummary saved to: {summary_file}")
    print("\nAll files copied successfully!")

if __name__ == "__main__":
    main()