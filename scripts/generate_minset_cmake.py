#!/usr/bin/env python3
"""
Generate optimized CMakeLists.txt files for minimum compilation set.
"""

import json
import os
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

def categorize_source_files(sources: List[str], kicad_root: Path) -> Dict[str, List[str]]:
    """Categorize source files by directory."""
    categorized = {}
    original_build_path = str(kicad_root / "build" / "x64-Debug")
    actual_build_path = Path(r"Y:\wenming\kicad\build\x64-Debug")
    
    for src in sources:
        # Map build paths
        if original_build_path in src:
            # This is a generated file, skip for now
            continue
            
        path = Path(src)
        try:
            rel_path = path.relative_to(kicad_root)
            parent_dir = str(rel_path.parent).replace('\\', '/')
            
            if parent_dir not in categorized:
                categorized[parent_dir] = []
            
            categorized[parent_dir].append(str(rel_path.name))
        except:
            pass
    
    return categorized

def generate_root_cmake(project_root: Path, categorized_sources: Dict[str, List[str]]):
    """Generate root CMakeLists.txt."""
    cmake_content = """cmake_minimum_required(VERSION 3.21)
project(kicad_core_wx)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(wxWidgets 3.2 REQUIRED COMPONENTS core base gl aui html net adv propgrid xml stc richtext)

# Include wxWidgets
include(${wxWidgets_USE_FILE})

# Set include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/eeschema
    ${CMAKE_CURRENT_SOURCE_DIR}/pcbnew
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/core
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/kimath/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/kiplatform/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/sexpr/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/kinng/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/include
)

# Collect all source files from minimum set
set(MINSET_SOURCES
"""
    
    # Add all source files
    for dir_path, files in sorted(categorized_sources.items()):
        for file in sorted(files):
            if file.endswith(('.cpp', '.cc', '.c')):
                cmake_content += f"    {dir_path}/{file}\n"
    
    # Add generated files
    cmake_content += """
    # Generated files
    generated/api/cpp/api/common/commands/editor_commands.pb.cc
    generated/api/cpp/api/common/envelope.pb.cc
    generated/api/cpp/api/common/types/base_types.pb.cc
    generated/api/cpp/api/common/types/enums.pb.cc
    generated/common/drawing_sheet/drawing_sheet_keywords.cpp
    generated/common/gal/glsl_kicad_frag.cpp
    generated/common/gal/glsl_kicad_vert.cpp
    generated/common/gal/glsl_smaa_base.cpp
    generated/common/gal/glsl_smaa_pass_1_frag_luma.cpp
    generated/common/gal/glsl_smaa_pass_1_vert.cpp
    generated/common/gal/glsl_smaa_pass_2_frag.cpp
    generated/common/gal/glsl_smaa_pass_2_vert.cpp
    generated/common/gal/glsl_smaa_pass_3_frag.cpp
    generated/common/gal/glsl_smaa_pass_3_vert.cpp
    generated/common/lib_table_keywords.cpp
    generated/eeschema/dialogs/dialog_bom_cfg_keywords.cpp
    generated/eeschema/schematic_keywords.cpp
)

# Create static library with all sources
add_library(kicad_core_lib STATIC ${MINSET_SOURCES})

# Link libraries
target_link_libraries(kicad_core_lib PUBLIC
    ${wxWidgets_LIBRARIES}
)

# Add compile definitions
target_compile_definitions(kicad_core_lib PUBLIC
    HAVE_STDINT_H
    _USE_MATH_DEFINES
)

# Platform specific settings
if(WIN32)
    target_compile_definitions(kicad_core_lib PUBLIC
        WIN32
        _WINDOWS
        NOMINMAX
    )
endif()

# Create executable (optional, for testing)
add_executable(kicad_core_test main.cpp)
target_link_libraries(kicad_core_test kicad_core_lib)
"""
    
    cmake_file = project_root / "CMakeLists.txt"
    with open(cmake_file, 'w') as f:
        f.write(cmake_content)
    print(f"Generated: {cmake_file}")

def generate_subdirectory_cmake(project_root: Path, dir_path: str, files: List[str]):
    """Generate CMakeLists.txt for a subdirectory."""
    cmake_content = f"""# CMakeLists.txt for {dir_path}
# This file contains only the minimum set files

set(LOCAL_SOURCES
"""
    
    for file in sorted(files):
        if file.endswith(('.cpp', '.cc', '.c')):
            cmake_content += f"    {file}\n"
    
    cmake_content += """
)

# Add sources to parent scope
set(MINSET_SOURCES ${MINSET_SOURCES} ${LOCAL_SOURCES} PARENT_SCOPE)
"""
    
    cmake_file = project_root / dir_path / "CMakeLists.txt"
    cmake_file.parent.mkdir(parents=True, exist_ok=True)
    
    with open(cmake_file, 'w') as f:
        f.write(cmake_content)
    print(f"Generated: {cmake_file}")

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
    
    # Categorize source files
    print("\nCategorizing source files...")
    categorized_sources = categorize_source_files(sources, kicad_root)
    
    # Generate root CMakeLists.txt
    print("\nGenerating root CMakeLists.txt...")
    generate_root_cmake(project_root, categorized_sources)
    
    # Generate subdirectory CMakeLists.txt files (optional)
    # print("\nGenerating subdirectory CMakeLists.txt files...")
    # for dir_path, files in categorized_sources.items():
    #     if files:  # Only create if there are files
    #         generate_subdirectory_cmake(project_root, dir_path, files)
    
    print("\n=== Summary ===")
    print(f"Total directories with sources: {len(categorized_sources)}")
    total_sources = sum(len(files) for files in categorized_sources.values())
    print(f"Total source files: {total_sources}")
    print("\nCMakeLists.txt files have been generated!")
    print("The files are optimized to compile only the minimum set.")

if __name__ == "__main__":
    main()