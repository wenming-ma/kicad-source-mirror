#!/usr/bin/env python3
"""
Fix CMakeLists.txt files in qt_pcb_project to only reference existing source files
"""

import os
import re
from pathlib import Path

QT_PROJECT_ROOT = Path("D:/MyProjects/kicad/kicad/qt_pcb_project")

def get_existing_sources(directory):
    """Get list of existing .cpp/.cc/.c files in directory"""
    existing = []
    if directory.exists():
        for ext in ['*.cpp', '*.cc', '*.c']:
            existing.extend(directory.glob(f"**/{ext}"))
    return [f.relative_to(directory) for f in existing]

def fix_cmake_file(cmake_file, directory):
    """Fix a CMakeLists.txt file to only reference existing sources"""
    if not cmake_file.exists():
        return False
    
    print(f"Fixing: {cmake_file.relative_to(QT_PROJECT_ROOT)}")
    
    with open(cmake_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Get existing source files
    existing_sources = get_existing_sources(directory)
    existing_names = [f.name for f in existing_sources]
    
    print(f"  Found {len(existing_sources)} source files: {existing_names}")
    
    # Find add_library statements and fix them
    def fix_add_library(match):
        target_name = match.group(1)
        library_type = match.group(2)
        sources_section = match.group(3)
        
        # Extract source file names from the sources section
        source_lines = []
        for line in sources_section.split('\n'):
            line = line.strip()
            if line and not line.startswith('#'):
                # Extract filename
                filename = line.replace('"', '').replace('${CMAKE_CURRENT_SOURCE_DIR}/', '')
                if filename.endswith('.cpp') or filename.endswith('.cc') or filename.endswith('.c'):
                    # Check if this file exists
                    file_path = Path(filename)
                    if file_path.name in existing_names or file_path in existing_sources:
                        source_lines.append(f"    {filename}")
        
        if source_lines:
            sources_text = '\n'.join(source_lines)
            return f"add_library( {target_name} {library_type}\n{sources_text}\n)"
        else:
            # No sources found, comment out the library
            return f"# add_library( {target_name} {library_type} ) # No sources available"
    
    # Replace add_library statements
    pattern = r'add_library\s*\(\s*(\w+)\s+(STATIC|SHARED|MODULE)\s*\n(.*?)\n\s*\)'
    content = re.sub(pattern, fix_add_library, content, flags=re.DOTALL)
    
    # Write back
    with open(cmake_file, 'w', encoding='utf-8') as f:
        f.write(content)
    
    return True

def main():
    """Main execution function"""
    print("=== Fixing CMakeLists.txt files ===")
    
    # Fix subdirectory CMakeLists.txt files
    subdirs = ['libs/core', 'libs/kimath', 'libs/kiplatform', 'libs/sexpr', 
               'thirdparty/clipper2', 'common']
    
    for subdir in subdirs:
        directory = QT_PROJECT_ROOT / subdir
        cmake_file = directory / "CMakeLists.txt"
        
        if cmake_file.exists():
            fix_cmake_file(cmake_file, directory)
        else:
            print(f"Skipping: {cmake_file} (not found)")
    
    print("\n=== CMakeLists.txt fixing completed ===")

if __name__ == "__main__":
    main()