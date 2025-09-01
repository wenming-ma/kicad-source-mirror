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

# Windows specific settings
if( WIN32 )
    add_compile_definitions( WIN32_LEAN_AND_MEAN )
endif()

# For now, test basic configuration
# TODO: Add package finding later

# Include directories
include_directories( BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/include )

# Add subdirectories for minimal PCB functionality
add_subdirectory( thirdparty )
add_subdirectory( libs )
add_subdirectory( common )
add_subdirectory( pcbnew )
add_subdirectory( scripting )
add_subdirectory( bitmap2component )