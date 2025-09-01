# Minimal PCB Project - Simplified CMake configuration

cmake_minimum_required( VERSION 3.22 FATAL_ERROR )

# Default to CMAKE_BUILD_TYPE = Release unless overridden on command line
if( DEFINED CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Set to either \"Release\" or \"Debug\"" )
else()
    set( CMAKE_BUILD_TYPE Release CACHE STRING "Set to either \"Release\" or \"Debug\"" )
endif()

project( qt_pcb_project )

# Output compile_commands.json for various LSP and other users
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

include( GNUInstallDirs )

# C++ standard
set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

# Windows specific settings
if( WIN32 )
    add_compile_definitions( WIN32_LEAN_AND_MEAN )
endif()

# Find required dependencies for minimal PCB set
find_package( PkgConfig QUIET )

# Use vcpkg installed packages
set( CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows" )
find_package( wxWidgets REQUIRED COMPONENTS base core adv html aui propgrid stc )
find_package( Boost REQUIRED )
find_package( glm REQUIRED )
find_package( protobuf REQUIRED )

# Include directories
include_directories( BEFORE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add subdirectories for minimal PCB functionality
add_subdirectory( thirdparty )
add_subdirectory( libs )
add_subdirectory( common )
add_subdirectory( pcbnew )
add_subdirectory( scripting )
add_subdirectory( bitmap2component )