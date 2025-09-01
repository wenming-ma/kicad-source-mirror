#!/usr/bin/env python3
"""
Test the exact script logic to find the bug
"""

import json
import os
import re
from pathlib import Path

def load_minset_filenames():
    """Load filenames from minset_sources.json"""
    with open('scripts/minset_sources.json', 'r', encoding='utf-8') as f:
        data = json.load(f)
    return {os.path.basename(src) for src in data['sources']}

def test_line(line, needed_filenames):
    """Test the exact logic from the script on a specific line"""
    print(f"\nTesting line: {line.strip()}")
    
    # Look for .cpp or .cc files in the line
    if re.search(r'\b\w+\.(cpp|cc)\b', line) and not line.strip().startswith('#'):
        # Extract all .cpp/.cc filenames from the line
        cpp_files = re.findall(r'\b(\w+\.(?:cpp|cc))\b', line)
        print(f"  Found cpp files: {cpp_files}")
        
        should_comment = False
        missing_files = []
        
        for cpp_file in cpp_files:
            # Only check filename (ignore path prefixes)
            if cpp_file not in needed_filenames:
                missing_files.append(cpp_file)
                print(f"    {cpp_file}: NOT in minset")
            else:
                print(f"    {cpp_file}: IN minset")
        
        # Comment if NO cpp files in this line are in minset
        found_files = []
        for cpp_file in cpp_files:
            if cpp_file in needed_filenames:
                found_files.append(cpp_file)
        
        # Only comment if NO files are in minset (all files are missing)
        should_comment = len(found_files) == 0
        print(f"  Missing files: {missing_files}")
        print(f"  Should comment: {should_comment} (missing_files={len(missing_files)}, total_files={len(cpp_files)})")
        
        return should_comment
    else:
        print("  No cpp files found or already commented")
        return False

def main():
    # Load minset filenames
    needed_filenames = load_minset_filenames()
    print(f"Loaded {len(needed_filenames)} needed filenames")
    print(f"Sample filenames: {sorted(list(needed_filenames))[:10]}")
    
    # Test problematic lines (uncommented versions)
    test_lines = [
        "    ${CMAKE_SOURCE_DIR}/pcbnew/pcbexpr_evaluator.cpp",
        "    ${CMAKE_SOURCE_DIR}/pcbnew/pcbexpr_functions.cpp",
        "    ${CMAKE_SOURCE_DIR}/pcbnew/board_design_settings.cpp",
        "    ${CMAKE_SOURCE_DIR}/pcbnew/teardrop/teardrop_parameters.cpp     #needed by board_design_settings.cpp",
        "    ${CMAKE_SOURCE_DIR}/pcbnew/router/pns_meander.cpp               #needed by board_design_settings.cpp",
        "    ${CMAKE_SOURCE_DIR}/pcbnew/board.cpp",
        "    ${CMAKE_SOURCE_DIR}/pcbnew/footprint.cpp"
    ]
    
    print("\n" + "="*50)
    print("TESTING SCRIPT LOGIC")
    print("="*50)
    
    for line in test_lines:
        should_comment = test_line(line, needed_filenames)
        if should_comment:
            print(f"  RESULT: Would comment this line (ERROR!)")
        else:
            print(f"  RESULT: Would keep this line (CORRECT!)")

if __name__ == "__main__":
    main()