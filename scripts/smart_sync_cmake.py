#!/usr/bin/env python3
"""
Smart sync CMakeLists.txt files from wx to qt project
- Skip identical files
- Only copy different files
- Then restore Qt-specific content from backups
"""

import os
import shutil
import filecmp
from pathlib import Path
import re

def find_backup_file(qt_file):
    """Find the most recent backup file for a given CMakeLists.txt"""
    parent_dir = qt_file.parent
    backups = list(parent_dir.glob('CMakeLists.txt.backup_*'))
    if backups:
        # Sort by modification time and return the most recent
        return sorted(backups, key=lambda x: x.stat().st_mtime)[-1]
    return None

def extract_qt_specific_lines(file_path):
    """Extract Qt-specific lines from a CMakeLists.txt file"""
    if not file_path or not file_path.exists():
        return []

    qt_lines = []
    with open(file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    for i, line in enumerate(lines):
        # Check for Qt-related content
        if any(marker in line for marker in ['Qt6', 'Qt::', 'qt6_', 'qt5_', 'MOC', 'AUTOMOC', 'AUTORCC', 'AUTOUIC',
                                              'CMAKE_AUTOMOC', 'CMAKE_AUTORCC', 'CMAKE_AUTOUIC',
                                              'qt_wrap_cpp', 'qt_add_resources', 'windeployqt']):
            qt_lines.append((i, line))

    return qt_lines

def sync_cmake_files():
    wx_dir = Path('kicad_core_project_wx')
    qt_dir = Path('kicad_core_project_qt')

    if not wx_dir.exists() or not qt_dir.exists():
        print("Error: Both project directories must exist")
        return

    identical_count = 0
    replaced_count = 0

    # Process all CMakeLists.txt files
    for root, dirs, files in os.walk(wx_dir):
        # Skip build directories
        if 'build' in root.split(os.sep):
            continue

        if 'CMakeLists.txt' in files:
            src_file = Path(root) / 'CMakeLists.txt'
            relative_path = src_file.relative_to(wx_dir)
            dst_file = qt_dir / relative_path

            if dst_file.exists():
                # Compare files
                if not filecmp.cmp(src_file, dst_file, shallow=False):
                    print(f"Different: {relative_path}")

                    # Find backup file to extract Qt content
                    backup_file = find_backup_file(dst_file)
                    qt_content = []
                    if backup_file:
                        qt_content = extract_qt_specific_lines(backup_file)
                        if qt_content:
                            print(f"  Found {len(qt_content)} Qt-specific lines in backup")

                    # Copy wx version
                    shutil.copy2(src_file, dst_file)
                    print(f"  Replaced with wx version")
                    replaced_count += 1

                    # TODO: Add Qt content restoration logic here if needed
                    # This would require more sophisticated merging based on the specific file

                else:
                    identical_count += 1
            else:
                # New file, create directory if needed and copy
                dst_file.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(src_file, dst_file)
                print(f"New file: {relative_path}")
                replaced_count += 1

    print(f"\nSummary:")
    print(f"  Identical files (skipped): {identical_count}")
    print(f"  Replaced/new files: {replaced_count}")

    # Now handle special cases for key files
    print("\nNow manually restore Qt content for key files...")
    restore_qt_content()

def restore_qt_content():
    """Restore Qt-specific content for important files"""
    qt_dir = Path('kicad_core_project_qt')

    # 1. Fix main CMakeLists.txt - change project name and add Qt support
    main_cmake = qt_dir / 'CMakeLists.txt'
    if main_cmake.exists():
        with open(main_cmake, 'r', encoding='utf-8') as f:
            content = f.read()

        # Change project name
        content = content.replace('project( kicad_core_project_wx', 'project( kicad_core_project_qt')
        content = content.replace('add_executable( kicad_core_project_wx', 'add_executable( kicad_core_project_qt')
        content = content.replace('target_link_libraries( kicad_core_project_wx', 'target_link_libraries( kicad_core_project_qt')

        # Add Qt6 finding and configuration before wxWidgets
        qt_config = """
# Qt6 configuration
if( Qt6_ROOT AND EXISTS "${Qt6_ROOT}" )
    set( Qt6_DIR "${Qt6_ROOT}/lib/cmake/Qt6" )
    list( APPEND CMAKE_PREFIX_PATH "${Qt6_ROOT}" )
    message( STATUS "Found Qt6 at: ${Qt6_ROOT}" )
elseif( Qt6_ROOT )
    message( WARNING "Qt6_ROOT is set to '${Qt6_ROOT}' but directory does not exist" )
else()
    message( STATUS "Qt6_ROOT not set, trying system paths..." )
endif()

# Find Qt6 - for future transformation
find_package( Qt6 QUIET COMPONENTS Core Widgets Gui OpenGL PrintSupport Xml Svg Network )

# Enable Qt automoc for automatic meta-object compilation
if( Qt6_FOUND )
    set( CMAKE_AUTOMOC ON )
    set( CMAKE_AUTORCC ON )
    set( CMAKE_AUTOUIC ON )
    message( STATUS "Qt6 support enabled" )
endif()

"""
        # Insert Qt configuration before wxWidgets finding
        wx_find_pos = content.find('set( wxWidgets_USE_STATIC ON )')
        if wx_find_pos > 0:
            content = content[:wx_find_pos] + qt_config + content[wx_find_pos:]

        # Add Qt include paths
        qt_includes = """
    # Qt6 include directories
    $<$<BOOL:${Qt6_FOUND}>:${Qt6_ROOT}/include>
    $<$<BOOL:${Qt6_FOUND}>:${Qt6_ROOT}/include/QtCore>
    $<$<BOOL:${Qt6_FOUND}>:${Qt6_ROOT}/include/QtWidgets>
    $<$<BOOL:${Qt6_FOUND}>:${Qt6_ROOT}/include/QtGui>
    $<$<BOOL:${Qt6_FOUND}>:${Qt6_ROOT}/include/QtNetwork>
    $<$<BOOL:${Qt6_FOUND}>:${Qt6_ROOT}/include/QtOpenGL>
    $<$<BOOL:${Qt6_FOUND}>:${Qt6_ROOT}/include/QtPrintSupport>
    $<$<BOOL:${Qt6_FOUND}>:${Qt6_ROOT}/include/QtXml>
    $<$<BOOL:${Qt6_FOUND}>:${Qt6_ROOT}/include/QtSvg>
"""
        # Add Qt includes after wxWidgets includes
        wx_include_pos = content.find('    ${wxWidgets_INCLUDE_DIRS}')
        if wx_include_pos > 0:
            end_of_line = content.find('\n', wx_include_pos)
            content = content[:end_of_line+1] + qt_includes + content[end_of_line+1:]

        # Add Qt libraries to target_link_libraries
        qt_libs = """
    # Qt libraries
    $<$<BOOL:${Qt6_FOUND}>:Qt6::Core>
    $<$<BOOL:${Qt6_FOUND}>:Qt6::Widgets>
    $<$<BOOL:${Qt6_FOUND}>:Qt6::Gui>
    $<$<BOOL:${Qt6_FOUND}>:Qt6::OpenGL>
    $<$<BOOL:${Qt6_FOUND}>:Qt6::PrintSupport>
    $<$<BOOL:${Qt6_FOUND}>:Qt6::Xml>
    $<$<BOOL:${Qt6_FOUND}>:Qt6::Svg>
    $<$<BOOL:${Qt6_FOUND}>:Qt6::Network>

"""
        # Find the end of target_link_libraries and add Qt libs
        target_link_end = content.rfind(')')
        if target_link_end > 0:
            content = content[:target_link_end] + qt_libs + content[target_link_end:]

        # Add windeployqt for Windows
        windeploy = """
# Windows-specific Qt deployment
if( WIN32 AND Qt6_FOUND )
    # Use windeployqt for automatic deployment
    add_custom_command( TARGET kicad_core_project_qt POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E env PATH="${Qt6_ROOT}/bin"
        ${Qt6_ROOT}/bin/windeployqt.exe
        --debug
        --force
        --no-compiler-runtime
        --no-translations
        --no-system-d3d-compiler
        --no-opengl-sw
        --dir $<TARGET_FILE_DIR:kicad_core_project_qt>
        $<TARGET_FILE:kicad_core_project_qt>
        COMMENT "Deploying Qt dependencies..."
    )

    message( STATUS "Qt deployment configured: windeployqt will copy required DLLs after build" )
endif()
"""
        # Add at the end of file
        content = content.rstrip() + '\n\n' + windeploy

        with open(main_cmake, 'w', encoding='utf-8') as f:
            f.write(content)
        print("  Fixed main CMakeLists.txt with Qt support")

    # 2. Fix common/CMakeLists.txt if needed
    common_cmake = qt_dir / 'common' / 'CMakeLists.txt'
    backup_common = common_cmake.parent / 'CMakeLists.txt.backup_20250921_005655'

    if backup_common.exists():
        # Extract MOC headers section from backup
        with open(backup_common, 'r', encoding='utf-8') as f:
            backup_lines = f.readlines()

        moc_section = []
        in_moc = False
        for line in backup_lines:
            if 'MOC_HEADERS' in line:
                in_moc = True
            if in_moc:
                moc_section.append(line)
                if ')' in line and not line.strip().endswith('\\'):
                    break

        if moc_section and common_cmake.exists():
            with open(common_cmake, 'r', encoding='utf-8') as f:
                content = f.read()

            # Add MOC section and Qt configuration if not present
            if 'MOC_HEADERS' not in content:
                qt_common_config = '\n'.join(moc_section) + """
# Generate MOC files for headers with Q_OBJECT
qt6_wrap_cpp( MOC_SOURCES ${MOC_HEADERS} )

# Configure Qt MOC, UIC, and RCC processing
set_target_properties( common PROPERTIES AUTOMOC ON )
set_target_properties( common PROPERTIES AUTORCC ON )
set_target_properties( common PROPERTIES AUTOUIC OFF )

"""
                # Add before target_link_libraries
                target_pos = content.find('target_link_libraries( common')
                if target_pos > 0:
                    content = content[:target_pos] + qt_common_config + '\n' + content[target_pos:]

                # Add MOC_SOURCES to the common library sources
                add_lib_pos = content.find('add_library( common STATIC')
                if add_lib_pos > 0:
                    # Find the closing parenthesis of add_library
                    paren_count = 0
                    start_search = add_lib_pos
                    for i in range(start_search, len(content)):
                        if content[i] == '(':
                            paren_count += 1
                        elif content[i] == ')':
                            paren_count -= 1
                            if paren_count == 0:
                                # Insert MOC_SOURCES before closing parenthesis
                                content = content[:i] + '\n    ${MOC_SOURCES}\n' + content[i:]
                                break

                # Add Qt libraries to linking
                qt_link_libs = """    # Qt libraries
    $<$<BOOL:${Qt6_FOUND}>:Qt6::Core>
    $<$<BOOL:${Qt6_FOUND}>:Qt6::Widgets>
    $<$<BOOL:${Qt6_FOUND}>:Qt6::Gui>
    $<$<BOOL:${Qt6_FOUND}>:Qt6::Network>
"""
                link_end = content.rfind(')', content.find('target_link_libraries( common'))
                if link_end > 0:
                    content = content[:link_end] + qt_link_libs + content[link_end:]

                with open(common_cmake, 'w', encoding='utf-8') as f:
                    f.write(content)
                print("  Fixed common/CMakeLists.txt with Qt MOC support")

if __name__ == "__main__":
    sync_cmake_files()