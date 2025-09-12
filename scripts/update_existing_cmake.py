#!/usr/bin/env python3
"""
Update existing CMakeLists.txt files to include newly copied files from minimum set.
"""

import json
from pathlib import Path
from typing import Dict, List, Set

def load_minset_files(scripts_dir: Path) -> tuple[List[str], List[str]]:
    """Load minset header and source files."""
    headers_file = scripts_dir / "tem" / "minset_headers.json"
    sources_file = scripts_dir / "tem" / "minset_sources.json"
    
    with open(headers_file, 'r') as f:
        headers_data = json.load(f)
    
    with open(sources_file, 'r') as f:
        sources_data = json.load(f)
    
    return headers_data['headers'], sources_data['sources']

def categorize_files_by_folder(sources: List[str], kicad_root: Path) -> Dict[str, List[str]]:
    """Categorize source files by their parent folder."""
    categorized = {}
    
    for src in sources:
        # Skip build directory files (generated files)
        if 'build' in src:
            continue
            
        path = Path(src)
        try:
            rel_path = path.relative_to(kicad_root)
            parts = str(rel_path).replace('\\', '/').split('/')
            
            # Get top-level folder
            if len(parts) > 0:
                top_folder = parts[0]
                
                # Skip bitmap2component and eeschema as they were already handled
                if top_folder in ['bitmap2component', 'eeschema']:
                    continue
                
                if top_folder not in categorized:
                    categorized[top_folder] = []
                
                # Store relative path from the top folder
                if len(parts) > 1:
                    rel_from_folder = '/'.join(parts[1:])
                    categorized[top_folder].append(rel_from_folder)
                    
        except:
            pass
    
    return categorized

def get_existing_sources_from_cmake(cmake_file: Path) -> Set[str]:
    """Extract existing source files from CMakeLists.txt."""
    sources = set()
    
    if not cmake_file.exists():
        return sources
    
    with open(cmake_file, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    in_sources_block = False
    for line in lines:
        line = line.strip()
        
        # Check for source variable definitions
        if 'SOURCES' in line and ('set(' in line or 'SET(' in line):
            in_sources_block = True
            continue
        
        # Check for end of block
        if in_sources_block and ')' in line:
            in_sources_block = False
            continue
        
        # Extract source files
        if in_sources_block:
            # Remove comments
            if '#' in line:
                line = line[:line.index('#')]
            
            line = line.strip()
            
            # Skip empty lines and CMake commands
            if not line or line.startswith('$'):
                continue
            
            # Clean up the file path
            if line.endswith('.cpp') or line.endswith('.cc') or line.endswith('.c'):
                sources.add(line)
    
    return sources

def update_common_cmake(project_root: Path, files: List[str]):
    """Update common/CMakeLists.txt to include new files."""
    cmake_file = project_root / "common" / "CMakeLists.txt"
    
    # Get existing sources
    existing_sources = get_existing_sources_from_cmake(cmake_file)
    
    # Find new sources to add
    new_sources = []
    for file in files:
        if file.endswith(('.cpp', '.cc', '.c')):
            if file not in existing_sources:
                new_sources.append(file)
    
    if not new_sources:
        print(f"No new source files to add to common/CMakeLists.txt")
        return
    
    print(f"Found {len(new_sources)} new source files to add to common/CMakeLists.txt:")
    for src in sorted(new_sources)[:10]:  # Show first 10
        print(f"  - {src}")
    if len(new_sources) > 10:
        print(f"  ... and {len(new_sources) - 10} more")
    
    # Read the CMakeLists.txt file
    with open(cmake_file, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    # Find where to insert new sources
    insert_index = -1
    in_sources_block = False
    sources_end_index = -1
    
    for i, line in enumerate(lines):
        if 'COMMON_SRCS' in line and 'set(' in line.lower():
            in_sources_block = True
            continue
        
        if in_sources_block:
            if ')' in line:
                sources_end_index = i
                insert_index = i  # Insert before the closing parenthesis
                break
    
    if insert_index == -1:
        print(f"ERROR: Could not find COMMON_SRCS block in {cmake_file}")
        return
    
    # Insert new sources
    new_lines = []
    for src in sorted(new_sources):
        new_lines.append(f"    {src}\n")
    
    # Add comment before new sources
    lines.insert(insert_index, "\n    # Newly added files from minimum set\n")
    insert_index += 1
    
    # Insert all new source files
    for new_line in new_lines:
        lines.insert(insert_index, new_line)
        insert_index += 1
    
    # Write back
    with open(cmake_file, 'w', encoding='utf-8') as f:
        f.writelines(lines)
    
    print(f"✓ Updated {cmake_file}")
    print(f"  Added {len(new_sources)} new source files")

def update_folder_cmake(project_root: Path, folder: str, files: List[str]):
    """Update CMakeLists.txt for a specific folder."""
    cmake_file = project_root / folder / "CMakeLists.txt"
    
    if not cmake_file.exists():
        print(f"Skipping {folder}: CMakeLists.txt not found")
        return
    
    # Filter for source files only
    source_files = [f for f in files if f.endswith(('.cpp', '.cc', '.c'))]
    
    if not source_files:
        print(f"No source files to add to {folder}/CMakeLists.txt")
        return
    
    # Get existing sources
    existing_sources = get_existing_sources_from_cmake(cmake_file)
    
    # Find new sources to add
    new_sources = []
    for file in source_files:
        if file not in existing_sources:
            new_sources.append(file)
    
    if not new_sources:
        print(f"No new source files to add to {folder}/CMakeLists.txt")
        return
    
    print(f"Found {len(new_sources)} new source files for {folder}:")
    for src in sorted(new_sources)[:5]:  # Show first 5
        print(f"  - {src}")
    if len(new_sources) > 5:
        print(f"  ... and {len(new_sources) - 5} more")
    
    # For now, just report what needs to be added
    # Each folder might have different structure in CMakeLists.txt
    print(f"⚠ Please manually add these files to {cmake_file}")

def main():
    # Setup paths
    script_dir = Path(__file__).parent
    kicad_root = script_dir.parent
    project_root = kicad_root / "kicad_core_project_wx"
    
    print(f"KiCad root: {kicad_root}")
    print(f"Project root: {project_root}")
    
    # Load minset files
    print("\nLoading minset files...")
    headers, sources = load_minset_files(script_dir)
    print(f"Found {len(headers)} headers and {len(sources)} sources")
    
    # Categorize files by folder
    print("\nCategorizing files by folder...")
    categorized = categorize_files_by_folder(sources, kicad_root)
    
    # Focus on key folders that existed before
    key_folders = ['common', 'libs', 'pcbnew', 'thirdparty']
    
    for folder in key_folders:
        if folder in categorized:
            print(f"\n=== Updating {folder}/CMakeLists.txt ===")
            if folder == 'common':
                update_common_cmake(project_root, categorized[folder])
            else:
                update_folder_cmake(project_root, folder, categorized[folder])
    
    # Report other folders with files
    other_folders = set(categorized.keys()) - set(key_folders)
    if other_folders:
        print("\n=== Other folders with files ===")
        for folder in sorted(other_folders):
            print(f"  {folder}: {len(categorized[folder])} files")
    
    print("\n✓ Analysis complete!")
    print("Please review the changes and compile to test.")

if __name__ == "__main__":
    main()