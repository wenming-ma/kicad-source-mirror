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

# Include ccache configuration
include( ${CMAKE_CURRENT_SOURCE_DIR}/AutoCCache.cmake )

include( GNUInstallDirs )

# C++ standard
set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

# Windows specific settings
if( WIN32 )
    add_compile_definitions( WIN32_LEAN_AND_MEAN )
    add_compile_definitions( WXSTATIC )
    add_compile_definitions( wxUSE_DLL=0 )
    add_compile_definitions( __WXMSW__ )
    # Add UTF-8 support for fmt library and Unicode
    if( MSVC )
        add_compile_options( /utf-8 )
    endif()
endif()

# Enable KICAD API support - DISABLED FOR MINIMAL BUILD
# add_compile_definitions( KICAD_IPC_API )

# Find required dependencies for minimal PCB set
find_package( PkgConfig QUIET )

# Use vcpkg installed packages
set( CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows" )
set( wxWidgets_USE_STATIC ON )
find_package( wxWidgets REQUIRED COMPONENTS base core adv html aui propgrid stc )
find_package( Boost REQUIRED )
find_package( glm REQUIRED )
# find_package( protobuf REQUIRED )
find_package( opencascade REQUIRED )
# find_package( fmt REQUIRED )  # Using local fmt from thirdparty

# Include directories
include_directories( BEFORE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/common
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/api/cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pcbnew
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/dynamic_bitset
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/picosha2
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/thread-pool
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/rtree
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fmt/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/magic_enum/magic_enum
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/expected/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows/include/opencascade
)

# Add subdirectories for minimal PCB functionality
add_subdirectory( thirdparty )
add_subdirectory( libs )
add_subdirectory( common )
add_subdirectory( pcbnew )
add_subdirectory( bitmap2component )