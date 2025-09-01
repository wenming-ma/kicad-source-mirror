#!/usr/bin/env python3
"""
Script to update CMakeLists.txt files in qt_pcb_project to only compile
the source files specified in minset_sources.json
"""

import json
import os
import re
import sys
from pathlib import Path
from typing import Dict, List, Set

def load_minset_sources(json_path: str) -> Dict[str, List[str]]:
    """Load and categorize sources from minset_sources.json by directory"""
    
    with open(json_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    # Base path to normalize against
    base_path = Path(r"C:\Users\wenming_ma\source\repos\kicad-source-mirror")
    
    # Directory mapping
    dir_sources = {
        'bitmap2component': [],
        'common': [],
        'libs': [],
        'pcbnew': [],
        'scripting': [],
        'thirdparty': [],
        'build': []
    }
    
    for source_file in data['sources']:
        source_path = Path(source_file)
        
        # Make relative to base path
        try:
            rel_path = source_path.relative_to(base_path)
            
            # Categorize by first directory component
            first_dir = rel_path.parts[0]
            
            if first_dir in dir_sources:
                # Store relative path from the directory
                if first_dir == 'build':
                    # For build files, store the full relative path
                    dir_sources[first_dir].append(str(rel_path))
                else:
                    # For source files, store path relative to the directory
                    rel_from_dir = Path(*rel_path.parts[1:])
                    dir_sources[first_dir].append(str(rel_from_dir))
                    
        except ValueError:
            print(f"Warning: Path {source_file} is not relative to base path")
            continue
    
    return dir_sources

def comment_out_unused_sources(cmake_file_path: str, needed_sources: List[str], directory: str, all_minset_filenames: Set[str]):
    """Comment out source files in CMakeLists.txt that are not in needed_sources"""
    
    if not os.path.exists(cmake_file_path):
        print(f"Warning: CMakeLists.txt not found at {cmake_file_path}")
        return
    
    # Create a set with just filenames for matching (ignore path prefixes)
    needed_filenames = {os.path.basename(src) for src in needed_sources}
    
    # For cross-directory references, also check against all minset filenames
    combined_filenames = needed_filenames | all_minset_filenames
    
    print(f"\nProcessing {directory}/CMakeLists.txt")
    print(f"  Needed sources count: {len(needed_sources)}")
    print(f"  Needed filenames: {sorted(list(needed_filenames))[:10]}...")  # Show first 10 for debug
    
    with open(cmake_file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    lines = content.splitlines()
    modified_lines = []
    modifications_made = 0
    
    for line in lines:
        original_line = line
        
        # Look for .cpp or .cc files in the line
        if re.search(r'\b\w+\.(cpp|cc)\b', line) and not line.strip().startswith('#'):
            # Extract all .cpp/.cc filenames from the line
            cpp_files = re.findall(r'\b(\w+\.(?:cpp|cc))\b', line)
            
            should_comment = False
            missing_files = []
            
            for cpp_file in cpp_files:
                # Check against combined filenames (includes cross-directory refs)
                if cpp_file not in combined_filenames:
                    missing_files.append(cpp_file)
            
            # Comment if NO cpp files in this line are in minset
            found_files = []
            for cpp_file in cpp_files:
                if cpp_file in combined_filenames:
                    found_files.append(cpp_file)
            
            # Only comment if NO files are in minset (all files are missing)
            should_comment = len(found_files) == 0
            
            if should_comment and not line.strip().startswith('#'):
                # Comment out the line with explanation
                indentation = len(line) - len(line.lstrip())
                commented_line = ' ' * indentation + '# ' + line.lstrip() + f'  # NOT in minset: {", ".join(missing_files)}'
                modified_lines.append(commented_line)
                modifications_made += 1
                print(f"    Commented out: {line.strip()} -> Missing: {missing_files}")
            else:
                modified_lines.append(line)
        else:
            modified_lines.append(line)
    
    if modifications_made > 0:
        # Write back the modified content
        with open(cmake_file_path, 'w', encoding='utf-8') as f:
            f.write('\n'.join(modified_lines))
        print(f"  Modified {modifications_made} lines")
    else:
        print(f"  No modifications needed")

def main():
    """Main function to update all CMakeLists.txt files"""
    
    # Paths
    script_dir = Path(__file__).parent
    base_dir = script_dir.parent
    qt_project_dir = base_dir / "qt_pcb_project"
    minset_json = script_dir / "minset_sources.json"
    
    print(f"Script directory: {script_dir}")
    print(f"Base directory: {base_dir}")
    print(f"Qt project directory: {qt_project_dir}")
    print(f"Minset JSON: {minset_json}")
    
    # Check if files exist
    if not minset_json.exists():
        print(f"Error: {minset_json} not found!")
        sys.exit(1)
    
    if not qt_project_dir.exists():
        print(f"Error: {qt_project_dir} not found!")
        sys.exit(1)
    
    # Load source files
    print("Loading minset sources...")
    dir_sources = load_minset_sources(str(minset_json))
    
    # Create a global set of all minset filenames for cross-directory references
    all_minset_filenames = set()
    for directory, sources in dir_sources.items():
        for src in sources:
            all_minset_filenames.add(os.path.basename(src))
    
    print(f"Total unique filenames in minset: {len(all_minset_filenames)}")
    
    # Print summary
    print("\nSource files by directory:")
    for directory, sources in dir_sources.items():
        if sources:
            print(f"  {directory}: {len(sources)} files")
    
    # Update CMakeLists.txt files
    directories_to_process = [
        ('bitmap2component', dir_sources['bitmap2component']),
        ('common', dir_sources['common']),
        ('libs', dir_sources['libs']),
        ('pcbnew', dir_sources['pcbnew']),
        ('scripting', dir_sources['scripting']),
        ('thirdparty', dir_sources['thirdparty'])
    ]
    
    for directory, needed_sources in directories_to_process:
        cmake_path = qt_project_dir / directory / "CMakeLists.txt"
        
        if needed_sources:
            comment_out_unused_sources(str(cmake_path), needed_sources, directory, all_minset_filenames)
        else:
            print(f"\nSkipping {directory} - no sources needed")
    
    print("\n=== CMakeLists.txt Update Complete ===")

if __name__ == "__main__":
    main()