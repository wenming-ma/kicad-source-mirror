#!/usr/bin/env python3
"""
Debug script to check what filenames are actually being loaded
"""

import json
import os
from pathlib import Path

def load_minset_sources(json_path: str):
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

def main():
    minset_json = Path("scripts/minset_sources.json")
    dir_sources = load_minset_sources(str(minset_json))
    
    # Create filename set for common directory
    common_sources = dir_sources['common']
    needed_filenames = {os.path.basename(src) for src in common_sources}
    
    print("Common directory sources count:", len(common_sources))
    print("Needed filenames count:", len(needed_filenames))
    
    # Check if our problematic files are in the needed_filenames
    problem_files = ['pcbexpr_evaluator.cpp', 'pcbexpr_functions.cpp', 'board_design_settings.cpp', 
                    'teardrop_parameters.cpp', 'pns_meander.cpp', 'board.cpp', 'footprint.cpp']
    
    print("\nChecking problematic files:")
    for file in problem_files:
        if file in needed_filenames:
            print(f"  {file}: IN needed_filenames for common")
        else:
            print(f"  {file}: NOT in needed_filenames for common")
    
    # Show some of the actual filenames in common
    print(f"\nFirst 20 filenames in common:")
    sorted_filenames = sorted(list(needed_filenames))
    for i, filename in enumerate(sorted_filenames[:20]):
        print(f"  {i+1}: {filename}")

if __name__ == "__main__":
    main()