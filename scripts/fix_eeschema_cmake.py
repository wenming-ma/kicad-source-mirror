#!/usr/bin/env python3
"""
Fix eeschema/CMakeLists.txt to properly handle subdirectories and organize files.
"""

import json
from pathlib import Path
from typing import List, Dict

def load_minset_files(scripts_dir: Path) -> List[str]:
    """Load minset source files."""
    sources_file = scripts_dir / "tem" / "minset_sources.json"
    
    with open(sources_file, 'r') as f:
        sources_data = json.load(f)
    
    return sources_data['sources']

def get_eeschema_files(sources: List[str], kicad_root: Path) -> Dict[str, List[str]]:
    """Get all eeschema source files organized by directory."""
    files_by_dir = {
        'main': [],
        'navlib': [],
        'other_subdirs': {}
    }
    
    for src in sources:
        if 'build' in src:
            continue
            
        if 'eeschema' not in src:
            continue
            
        path = Path(src)
        try:
            rel_path = path.relative_to(kicad_root / "eeschema")
            rel_str = str(rel_path).replace('\\', '/')
            
            # Check if it's a source file
            if not rel_str.endswith(('.cpp', '.cc', '.c')):
                continue
            
            # Categorize by directory
            if '/' not in rel_str:
                # Main directory file
                files_by_dir['main'].append(rel_str)
            elif rel_str.startswith('navlib/'):
                # navlib subdirectory (has its own CMakeLists.txt)
                files_by_dir['navlib'].append(rel_str[7:])  # Remove 'navlib/' prefix
            else:
                # Other subdirectories (compiled as part of main library)
                parts = rel_str.split('/')
                subdir = parts[0]
                if subdir not in files_by_dir['other_subdirs']:
                    files_by_dir['other_subdirs'][subdir] = []
                files_by_dir['other_subdirs'][subdir].append(rel_str)
                    
        except:
            pass
    
    return files_by_dir

def generate_eeschema_cmake(project_root: Path, files_by_dir: Dict[str, List[str]]):
    """Generate optimized eeschema/CMakeLists.txt."""
    
    cmake_content = """# CMakeLists.txt for eeschema
# Optimized to compile only minimum set files

# Add subdirectory that has its own CMakeLists.txt
add_subdirectory(navlib)

# Main eeschema sources
set(EESCHEMA_SOURCES
    # Main directory files
"""
    
    # Add main directory files
    for file in sorted(files_by_dir['main']):
        cmake_content += f"    {file}\n"
    
    # Add other subdirectory files (not navlib)
    for subdir in sorted(files_by_dir['other_subdirs'].keys()):
        cmake_content += f"\n    # {subdir} subdirectory\n"
        for file in sorted(files_by_dir['other_subdirs'][subdir]):
            cmake_content += f"    {file}\n"
    
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
    ${CMAKE_CURRENT_SOURCE_DIR}/erc
    ${CMAKE_CURRENT_SOURCE_DIR}/api
)

target_link_libraries(eeschema_lib PUBLIC
    common
    kimath
    eeschema_navlib  # Link navlib library
    ${wxWidgets_LIBRARIES}
)

# Add compile definitions if needed
target_compile_definitions(eeschema_lib PUBLIC
    EESCHEMA
)
"""
    
    # Write the file
    cmake_file = project_root / "eeschema" / "CMakeLists.txt"
    
    # Backup existing file
    backup_file = cmake_file.with_suffix('.txt.bak')
    if cmake_file.exists():
        with open(cmake_file, 'r', encoding='utf-8') as f:
            backup_content = f.read()
        with open(backup_file, 'w', encoding='utf-8') as f:
            f.write(backup_content)
        print(f"Backed up existing file to: {backup_file}")
    
    with open(cmake_file, 'w', encoding='utf-8') as f:
        f.write(cmake_content)
    
    # Statistics
    total_main = len(files_by_dir['main'])
    total_other = sum(len(files) for files in files_by_dir['other_subdirs'].values())
    total_navlib = len(files_by_dir['navlib'])
    
    print(f"Generated: {cmake_file}")
    print(f"  Main directory: {total_main} files")
    print(f"  Other subdirectories: {total_other} files in {len(files_by_dir['other_subdirs'])} directories")
    print(f"  navlib subdirectory: {total_navlib} files (separate CMakeLists.txt)")
    print(f"  Total: {total_main + total_other} files in main library")

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
    
    # Get eeschema files organized by directory
    print("\nAnalyzing eeschema files...")
    files_by_dir = get_eeschema_files(sources, kicad_root)
    
    # Show what we found
    print(f"\nFound eeschema files:")
    print(f"  Main directory: {len(files_by_dir['main'])} files")
    print(f"  navlib: {len(files_by_dir['navlib'])} files")
    print(f"  Other subdirectories: {len(files_by_dir['other_subdirs'])} directories")
    
    # Generate optimized CMakeLists.txt
    print("\n=== Generating optimized eeschema/CMakeLists.txt ===")
    generate_eeschema_cmake(project_root, files_by_dir)
    
    print("\nDone! The eeschema/CMakeLists.txt has been updated to:")
    print("  - Add navlib as a subdirectory")
    print("  - Link with eeschema_navlib library")
    print("  - Include all minimum set files")

if __name__ == "__main__":
    main()