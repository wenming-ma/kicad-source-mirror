#!/usr/bin/env python3
"""
Debug script to check why files are being incorrectly commented
"""

import json
import os
import re
from pathlib import Path

def main():
    # Load minset sources
    with open('scripts/minset_sources.json', 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    # Create filename set
    filenames = {os.path.basename(src) for src in data['sources']}
    
    # Check specific lines from common/CMakeLists.txt that were incorrectly commented
    problematic_lines = [
        "    # ${CMAKE_SOURCE_DIR}/pcbnew/pcbexpr_evaluator.cpp  # NOT in minset: pcbexpr_evaluator.cpp",
        "    # ${CMAKE_SOURCE_DIR}/pcbnew/pcbexpr_functions.cpp  # NOT in minset: pcbexpr_functions.cpp", 
        "    # ${CMAKE_SOURCE_DIR}/pcbnew/board_design_settings.cpp  # NOT in minset: board_design_settings.cpp",
        "    # ${CMAKE_SOURCE_DIR}/pcbnew/teardrop/teardrop_parameters.cpp     #needed by board_design_settings.cpp  # NOT in minset: teardrop_parameters.cpp, board_design_settings.cpp",
        "    # ${CMAKE_SOURCE_DIR}/pcbnew/router/pns_meander.cpp               #needed by board_design_settings.cpp  # NOT in minset: pns_meander.cpp, board_design_settings.cpp",
        "    # ${CMAKE_SOURCE_DIR}/pcbnew/board.cpp  # NOT in minset: board.cpp",
        "    # ${CMAKE_SOURCE_DIR}/pcbnew/footprint.cpp  # NOT in minset: footprint.cpp"
    ]
    
    print("Checking problematic lines:")
    for line in problematic_lines:
        # Extract cpp files from the line (remove comment part first)
        original_line = line.split('  # NOT in minset:')[0].replace('#', '').strip()
        cpp_files = re.findall(r'\b(\w+\.(?:cpp|cc))\b', original_line)
        
        print(f"\nLine: {line}")
        print(f"  Extracted cpp files: {cpp_files}")
        
        for cpp_file in cpp_files:
            if cpp_file in filenames:
                print(f"  {cpp_file}: IN minset (SHOULD NOT BE COMMENTED!)")
            else:
                print(f"  {cpp_file}: NOT in minset (OK to comment)")

if __name__ == "__main__":
    main()