#!/usr/bin/env python3
"""
Update common/CMakeLists.txt to include all files from minimum set.
"""

import json
from pathlib import Path
from typing import List, Set

def load_minset_files(scripts_dir: Path) -> List[str]:
    """Load minset source files."""
    sources_file = scripts_dir / "tem" / "minset_sources.json"
    
    with open(sources_file, 'r') as f:
        sources_data = json.load(f)
    
    return sources_data['sources']

def get_common_files(sources: List[str], kicad_root: Path) -> List[str]:
    """Get all source files in common folder from minimum set."""
    common_files = []
    
    for src in sources:
        # Skip build directory files (generated files)
        if 'build' in src:
            continue
            
        path = Path(src)
        try:
            rel_path = path.relative_to(kicad_root)
            parts = str(rel_path).replace('\\', '/').split('/')
            
            # Check if file is in common folder
            if len(parts) > 1 and parts[0] == 'common':
                # Get relative path from common folder
                rel_from_common = '/'.join(parts[1:])
                if rel_from_common.endswith(('.cpp', '.cc', '.c')):
                    common_files.append(rel_from_common)
                    
        except:
            pass
    
    return common_files

def get_existing_files_from_cmake(cmake_file: Path) -> Set[str]:
    """Extract existing source files from CMakeLists.txt."""
    files = set()
    
    if not cmake_file.exists():
        return files
    
    with open(cmake_file, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    in_add_library = False
    for line in lines:
        # Check for add_library start
        if 'add_library' in line and 'common' in line:
            in_add_library = True
            continue
        
        # Check for end of add_library
        if in_add_library and line.strip() == ')':
            break
        
        # Extract source files
        if in_add_library:
            # Remove comments
            if '#' in line:
                comment_pos = line.index('#')
                # Check if it's a commented out file
                if '.cpp' in line or '.cc' in line or '.c' in line:
                    # Skip commented out files
                    continue
                line = line[:comment_pos]
            
            line = line.strip()
            
            # Skip empty lines and CMake variables
            if not line or line.startswith('$'):
                continue
            
            # Add file if it's a source file
            if line.endswith(('.cpp', '.cc', '.c')):
                files.add(line)
    
    return files

def generate_new_cmake(project_root: Path, common_files: List[str]):
    """Generate new common/CMakeLists.txt with all minimum set files."""
    
    # Organize files by subdirectory
    main_files = []
    subdir_files = {}
    
    for file in common_files:
        if '/' in file:
            # File in subdirectory
            parts = file.split('/')
            subdir = parts[0]
            
            if subdir not in subdir_files:
                subdir_files[subdir] = []
            subdir_files[subdir].append(file)
        else:
            # File in main common directory
            main_files.append(file)
    
    # Generate CMakeLists.txt content
    content = """# Common library for minimum PCB set

# Generated files from kicad_core_project_wx generated directory
set( KICAD_GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../generated" )
set( COMMON_GENERATED_SRCS
    ${KICAD_GENERATED_DIR}/common/drawing_sheet/drawing_sheet_keywords.cpp
    ${KICAD_GENERATED_DIR}/common/lib_table_keywords.cpp
    # Add other generated files as needed
)

add_library( common STATIC
    # Main common files
"""
    
    # Add main files
    for file in sorted(main_files):
        content += f"    {file}\n"
    
    # Add subdirectory files
    for subdir in sorted(subdir_files.keys()):
        content += f"\n    # {subdir} subdirectory\n"
        for file in sorted(subdir_files[subdir]):
            content += f"    {file}\n"
    
    # Add generated sources
    content += """
    # Generated sources
    ${COMMON_GENERATED_SRCS}
)

# Include directories
target_include_directories( common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${KICAD_GENERATED_DIR}
    ${KICAD_GENERATED_DIR}/common
)

# Add subdirectories that have their own CMakeLists.txt
add_subdirectory( gal )

# Link libraries
target_link_libraries( common PUBLIC
    kimath
    kiplatform
    ${wxWidgets_LIBRARIES}
)
"""
    
    # Write the file
    cmake_file = project_root / "common" / "CMakeLists.txt"
    
    # Backup existing file
    backup_file = cmake_file.with_suffix('.txt.bak')
    if cmake_file.exists():
        with open(cmake_file, 'r', encoding='utf-8') as f:
            backup_content = f.read()
        with open(backup_file, 'w', encoding='utf-8') as f:
            f.write(backup_content)
        print(f"Backed up existing file to: {backup_file}")
    
    with open(cmake_file, 'w', encoding='utf-8') as f:
        f.write(content)
    
    print(f"Generated: {cmake_file}")
    print(f"  Included {len(main_files)} main files")
    print(f"  Included files from {len(subdir_files)} subdirectories")
    print(f"  Total: {len(common_files)} source files")

def main():
    # Setup paths
    script_dir = Path(__file__).parent
    kicad_root = script_dir.parent
    project_root = kicad_root / "kicad_core_project_wx"
    
    print(f"KiCad root: {kicad_root}")
    print(f"Project root: {project_root}")
    
    # Load minset files
    print("\nLoading minset files...")
    sources = load_minset_files(script_dir)
    print(f"Found {len(sources)} sources")
    
    # Get common folder files
    print("\nExtracting common folder files...")
    common_files = get_common_files(sources, kicad_root)
    print(f"Found {len(common_files)} source files in common folder")
    
    # Show sample of files
    print("\nSample of files to include:")
    for file in sorted(common_files)[:10]:
        print(f"  - {file}")
    if len(common_files) > 10:
        print(f"  ... and {len(common_files) - 10} more")
    
    # Generate new CMakeLists.txt
    print("\n=== Generating new common/CMakeLists.txt ===")
    generate_new_cmake(project_root, common_files)
    
    print("\nDone! Please review the generated CMakeLists.txt")
    print("The original file has been backed up with .bak extension")

if __name__ == "__main__":
    main()