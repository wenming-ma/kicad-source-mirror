#!/usr/bin/env python3
"""
Optimize CMakeLists.txt for newly added folders (bitmap2component and eeschema).
Only compile files in the minimum set.
"""

import json
from pathlib import Path
from typing import Dict, List

def load_minset_files(scripts_dir: Path) -> tuple[List[str], List[str]]:
    """Load minset header and source files."""
    headers_file = scripts_dir / "tem" / "minset_headers.json"
    sources_file = scripts_dir / "tem" / "minset_sources.json"
    
    with open(headers_file, 'r') as f:
        headers_data = json.load(f)
    
    with open(sources_file, 'r') as f:
        sources_data = json.load(f)
    
    return headers_data['headers'], sources_data['sources']

def get_files_for_directory(sources: List[str], kicad_root: Path, target_dir: str) -> List[str]:
    """Get all source files for a specific directory from the minset."""
    files = []
    
    for src in sources:
        # Skip build directory files
        if 'build' in src:
            continue
            
        path = Path(src)
        try:
            rel_path = path.relative_to(kicad_root)
            # Check if this file belongs to the target directory
            if str(rel_path).replace('\\', '/').startswith(target_dir):
                # Get relative path from target directory
                rel_from_target = rel_path.relative_to(target_dir)
                files.append(str(rel_from_target).replace('\\', '/'))
        except:
            pass
    
    return files

def generate_bitmap2component_cmake(project_root: Path, files: List[str]):
    """Generate CMakeLists.txt for bitmap2component."""
    cmake_content = """# CMakeLists.txt for bitmap2component
# Optimized to compile only minimum set files

set(BITMAP2COMPONENT_SOURCES
"""
    
    # Add only cpp/cc files from the minimum set
    for file in sorted(files):
        if file.endswith(('.cpp', '.cc', '.c')):
            cmake_content += f"    {file}\n"
    
    cmake_content += """)

# Create bitmap2component library
add_library(bitmap2component STATIC ${BITMAP2COMPONENT_SOURCES})

target_include_directories(bitmap2component PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(bitmap2component PUBLIC
    common
    ${wxWidgets_LIBRARIES}
)
"""
    
    cmake_file = project_root / "bitmap2component" / "CMakeLists.txt"
    with open(cmake_file, 'w') as f:
        f.write(cmake_content)
    print(f"Generated: {cmake_file}")
    print(f"  Included {len([f for f in files if f.endswith(('.cpp', '.cc', '.c'))])} source files")

def generate_eeschema_cmake(project_root: Path, files: List[str]):
    """Generate CMakeLists.txt for eeschema."""
    # Organize files by subdirectory
    main_sources = []
    subdirs = {}
    
    for file in files:
        if '/' in file:
            # File in subdirectory
            parts = file.split('/')
            subdir = parts[0]
            subfile = '/'.join(parts[1:])
            
            if subdir not in subdirs:
                subdirs[subdir] = []
            subdirs[subdir].append(subfile)
        else:
            # File in main eeschema directory
            if file.endswith(('.cpp', '.cc', '.c')):
                main_sources.append(file)
    
    cmake_content = """# CMakeLists.txt for eeschema
# Optimized to compile only minimum set files

# Main eeschema sources
set(EESCHEMA_SOURCES
"""
    
    # Add main source files
    for file in sorted(main_sources):
        cmake_content += f"    {file}\n"
    
    # Add subdirectory sources
    for subdir in sorted(subdirs.keys()):
        cmake_content += f"\n    # {subdir} sources\n"
        for file in sorted(subdirs[subdir]):
            if file.endswith(('.cpp', '.cc', '.c')):
                cmake_content += f"    {subdir}/{file}\n"
    
    cmake_content += """)

# Create eeschema library
add_library(eeschema_lib STATIC ${EESCHEMA_SOURCES})

target_include_directories(eeschema_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/import_gfx
    ${CMAKE_CURRENT_SOURCE_DIR}/netlist_exporters
    ${CMAKE_CURRENT_SOURCE_DIR}/printing
    ${CMAKE_CURRENT_SOURCE_DIR}/sch_io
    ${CMAKE_CURRENT_SOURCE_DIR}/sim
    ${CMAKE_CURRENT_SOURCE_DIR}/symbol_editor
    ${CMAKE_CURRENT_SOURCE_DIR}/sync_sheet_pin
    ${CMAKE_CURRENT_SOURCE_DIR}/tools
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
)

target_link_libraries(eeschema_lib PUBLIC
    common
    kimath
    ${wxWidgets_LIBRARIES}
)

# Add subdirectories if they have their own CMakeLists.txt
# (Only if needed for special compilation requirements)
"""
    
    cmake_file = project_root / "eeschema" / "CMakeLists.txt"
    with open(cmake_file, 'w') as f:
        f.write(cmake_content)
    
    total_sources = len(main_sources) + sum(len([f for f in files if f.endswith(('.cpp', '.cc', '.c'))]) for files in subdirs.values())
    print(f"Generated: {cmake_file}")
    print(f"  Included {len(main_sources)} main source files")
    print(f"  Included files from {len(subdirs)} subdirectories")
    print(f"  Total: {total_sources} source files")

def update_root_cmake(project_root: Path):
    """Update root CMakeLists.txt to include new directories."""
    cmake_file = project_root / "CMakeLists.txt"
    
    # Read existing content
    with open(cmake_file, 'r') as f:
        content = f.read()
    
    # Check if subdirectories are already added
    if 'add_subdirectory(bitmap2component)' not in content:
        # Find where to add subdirectories (after other add_subdirectory calls)
        lines = content.split('\n')
        insert_index = -1
        
        for i, line in enumerate(lines):
            if 'add_subdirectory(' in line:
                insert_index = i + 1
        
        if insert_index == -1:
            # No existing subdirectories, add before the end
            insert_index = len(lines) - 1
        
        # Insert new subdirectories
        lines.insert(insert_index, 'add_subdirectory(bitmap2component)')
        lines.insert(insert_index + 1, 'add_subdirectory(eeschema)')
        
        # Write back
        with open(cmake_file, 'w') as f:
            f.write('\n'.join(lines))
        
        print(f"Updated: {cmake_file}")
        print("  Added bitmap2component and eeschema subdirectories")
    else:
        print(f"Root CMakeLists.txt already includes the new directories")

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
    
    # Get files for bitmap2component
    print("\n=== Optimizing bitmap2component/CMakeLists.txt ===")
    bitmap2component_files = get_files_for_directory(sources, kicad_root, "bitmap2component")
    if bitmap2component_files:
        generate_bitmap2component_cmake(project_root, bitmap2component_files)
    else:
        print("No files found for bitmap2component")
    
    # Get files for eeschema
    print("\n=== Optimizing eeschema/CMakeLists.txt ===")
    eeschema_files = get_files_for_directory(sources, kicad_root, "eeschema")
    if eeschema_files:
        generate_eeschema_cmake(project_root, eeschema_files)
    else:
        print("No files found for eeschema")
    
    # Update root CMakeLists.txt
    print("\n=== Updating root CMakeLists.txt ===")
    update_root_cmake(project_root)
    
    print("\n✅ CMakeLists.txt optimization complete!")
    print("Only minimum set files will be compiled.")

if __name__ == "__main__":
    main()