#!/usr/bin/env python3
"""
KiCad Minimum Set File Copier
Copy minimum source files to qt_pcb_project with same folder structure
Handle generated files from build directory properly
"""

import json
import os
import shutil
from pathlib import Path
import sys

# Configuration
KICAD_ROOT = Path("D:/MyProjects/kicad/kicad")
QT_PROJECT_ROOT = KICAD_ROOT / "qt_pcb_project"
MINSET_SOURCES_JSON = KICAD_ROOT / "build" / "minset_sources.json"
MINSET_HEADERS_JSON = KICAD_ROOT / "build" / "minset_headers.json"

def load_minset_files():
    """Load minimum set file lists from JSON files"""
    sources = []
    headers = []
    
    # Load sources
    try:
        with open(MINSET_SOURCES_JSON, 'r', encoding='utf-8') as f:
            data = json.load(f)
        sources = data.get('sources', [])
    except FileNotFoundError:
        print(f"Error: {MINSET_SOURCES_JSON} not found. Run dependency analysis first.")
        sys.exit(1)
    
    # Load headers
    try:
        with open(MINSET_HEADERS_JSON, 'r', encoding='utf-8') as f:
            data = json.load(f)
        headers = data.get('headers', [])
    except FileNotFoundError:
        print(f"Warning: {MINSET_HEADERS_JSON} not found. Continuing with sources only.")
    
    return sources, headers

def normalize_path(file_path):
    """Convert path to use forward slashes and make relative to KICAD_ROOT"""
    path = Path(file_path)
    try:
        # Make relative to KICAD_ROOT
        relative_path = path.relative_to(KICAD_ROOT)
        return relative_path
    except ValueError:
        print(f"Warning: Path {file_path} is not under KICAD_ROOT")
        return None

def categorize_files(file_list):
    """Categorize files into source files, generated files, and thirdparty"""
    source_files = []
    generated_files = []
    thirdparty_files = []
    
    for file_path in file_list:
        rel_path = normalize_path(file_path)
        if rel_path is None:
            continue
            
        path_str = str(rel_path).replace('\\', '/')
        
        if path_str.startswith('build/'):
            generated_files.append(rel_path)
        elif path_str.startswith('thirdparty/'):
            thirdparty_files.append(rel_path)
        else:
            source_files.append(rel_path)
    
    return source_files, generated_files, thirdparty_files

def copy_file_with_structure(src_file, dest_root, is_generated=False):
    """Copy file maintaining directory structure"""
    if is_generated:
        # For generated files, put them in a generated/ subdirectory
        # but maintain their original structure
        dest_file = dest_root / "generated" / src_file.relative_to(Path("build"))
    else:
        dest_file = dest_root / src_file
    
    # Create destination directory
    dest_file.parent.mkdir(parents=True, exist_ok=True)
    
    # Copy file
    src_full = KICAD_ROOT / src_file
    if src_full.exists():
        shutil.copy2(src_full, dest_file)
        print(f"Copied: {src_file} -> {dest_file.relative_to(dest_root)}")
        return True
    else:
        print(f"Warning: Source file not found: {src_full}")
        return False

def copy_cmake_files(source_dirs, dest_root):
    """Copy CMakeLists.txt files from used directories"""
    cmake_dirs = set()
    
    # Collect all directories that contain our source files
    for src_file in source_dirs:
        current_dir = src_file.parent
        while current_dir != Path('.'):
            cmake_dirs.add(current_dir)
            current_dir = current_dir.parent
    
    # Also add root directory
    cmake_dirs.add(Path('.'))
    
    copied_count = 0
    for cmake_dir in cmake_dirs:
        src_cmake = KICAD_ROOT / cmake_dir / "CMakeLists.txt"
        if src_cmake.exists():
            dest_cmake = dest_root / cmake_dir / "CMakeLists.txt"
            dest_cmake.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(src_cmake, dest_cmake)
            print(f"Copied CMakeLists.txt: {cmake_dir}")
            copied_count += 1
    
    return copied_count

def create_root_cmake(dest_root, source_files, generated_files, thirdparty_files):
    """Create simplified root CMakeLists.txt for the Qt project"""
    cmake_content = '''# Qt PCB Project - Minimum KiCad Set
cmake_minimum_required(VERSION 3.20)
project(QtPcbProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/generated)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CMAKE_SOURCE_DIR}/libs)

# Find packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
find_package(Boost REQUIRED)
find_package(PkgConfig REQUIRED)

# Collect all source files
file(GLOB_RECURSE COMMON_SOURCES
    "common/*.cpp"
    "libs/*.cpp"
    "pcbnew/*.cpp"
    "scripting/*.cpp"
    "generated/*.cpp"
    "generated/*.cc"
)

file(GLOB_RECURSE THIRDPARTY_SOURCES
    "thirdparty/*.cpp"
    "thirdparty/*.cc"
)

# Create library
add_library(kicad_pcb_core
    ${COMMON_SOURCES}
    ${THIRDPARTY_SOURCES}
)

# Link libraries
target_link_libraries(kicad_pcb_core
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    ${Boost_LIBRARIES}
)

# Set compile definitions
target_compile_definitions(kicad_pcb_core PRIVATE
    KICAD_BUILD_VERSION="Qt-Port"
    CMAKE_CXX_COMPILER_ID="MSVC"
    BOOST_ALL_NO_LIB
)

# Create test executable
add_executable(test_pcb_core
    test/main.cpp
)

target_link_libraries(test_pcb_core kicad_pcb_core)
'''
    
    cmake_file = dest_root / "CMakeLists.txt"
    with open(cmake_file, 'w', encoding='utf-8') as f:
        f.write(cmake_content)
    
    print(f"Created root CMakeLists.txt: {cmake_file}")

def create_test_main(dest_root):
    """Create a simple test main.cpp"""
    test_dir = dest_root / "test"
    test_dir.mkdir(exist_ok=True)
    
    test_content = '''#include <iostream>
#include <QApplication>

// Test include for KiCad core
// #include "pcbnew/board.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    
    std::cout << "Qt PCB Project - Minimum KiCad Set Test" << std::endl;
    std::cout << "This is a test executable for the copied minimum set." << std::endl;
    
    // TODO: Add actual KiCad functionality tests here
    
    return 0;
}
'''
    
    test_file = test_dir / "main.cpp"
    with open(test_file, 'w', encoding='utf-8') as f:
        f.write(test_content)
    
    print(f"Created test main: {test_file}")

def main():
    """Main execution function"""
    print("=== KiCad Minimum Set File Copier ===")
    print(f"Source: {KICAD_ROOT}")
    print(f"Destination: {QT_PROJECT_ROOT}")
    
    # Load minimum set files
    sources, headers = load_minset_files()
    all_files = sources + headers
    print(f"Found {len(sources)} source files and {len(headers)} header files")
    print(f"Total files: {len(all_files)}")
    
    # Categorize files
    source_files, generated_files, thirdparty_files = categorize_files(all_files)
    print(f"Source files: {len(source_files)}")
    print(f"Generated files: {len(generated_files)}")
    print(f"Thirdparty files: {len(thirdparty_files)}")
    
    # Create destination directory
    QT_PROJECT_ROOT.mkdir(parents=True, exist_ok=True)
    
    # Copy files
    copied_count = 0
    
    # Copy source files
    print("\n--- Copying source files ---")
    for src_file in source_files:
        if copy_file_with_structure(src_file, QT_PROJECT_ROOT):
            copied_count += 1
    
    # Copy generated files
    print("\n--- Copying generated files ---")
    for gen_file in generated_files:
        if copy_file_with_structure(gen_file, QT_PROJECT_ROOT, is_generated=True):
            copied_count += 1
    
    # Copy thirdparty files
    print("\n--- Copying thirdparty files ---")
    for tp_file in thirdparty_files:
        if copy_file_with_structure(tp_file, QT_PROJECT_ROOT):
            copied_count += 1
    
    # Copy CMakeLists.txt files
    print("\n--- Copying CMakeLists.txt files ---")
    all_source_files = source_files + thirdparty_files
    cmake_count = copy_cmake_files(all_source_files, QT_PROJECT_ROOT)
    
    # Create root CMakeLists.txt for Qt project
    print("\n--- Creating Qt project CMakeLists.txt ---")
    create_root_cmake(QT_PROJECT_ROOT, source_files, generated_files, thirdparty_files)
    
    # Create test main
    print("\n--- Creating test files ---")
    create_test_main(QT_PROJECT_ROOT)
    
    # Summary
    print(f"\n=== Copy Summary ===")
    print(f"Files copied: {copied_count}")
    print(f"CMakeLists.txt files copied: {cmake_count}")
    print(f"Destination: {QT_PROJECT_ROOT}")
    print("\nNext steps:")
    print("1. Review copied files and directory structure")
    print("2. Install required dependencies (Qt6, Boost, etc.)")
    print("3. Configure and build the Qt project")
    print("4. Test compilation with: cd qt_pcb_project && mkdir build && cd build && cmake .. && make")

if __name__ == "__main__":
    main()