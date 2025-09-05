# Minimal PCB Project - Simplified CMake configuration

cmake_minimum_required( VERSION 3.22 FATAL_ERROR )

# Set CMake policies to avoid warnings
cmake_policy(SET CMP0167 NEW)  # Use find_package for Boost instead of FindBoost

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
        # /Zc:__cplusplus: define this macro according to C++ standard version - required for Qt
        add_compile_options( /Zc:__cplusplus )
        # /Zc:inline: enforces C++11 requirement that inline functions must have a definition in the same unit
        add_compile_options( /Zc:inline )
        # /permissive-: This puts MSVC into compliance mode, required for Qt
        add_compile_options( /permissive- )
    endif()
    # Disable verbose include output (showIncludes)
    string(REPLACE "/showIncludes" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/showIncludes" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    string(REPLACE "/showIncludes" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    string(REPLACE "/showIncludes" "" CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")
    set(CMAKE_CXX_DEPENDS_USE_COMPILER FALSE)
endif()

# Set Qt root directory - Check for Qt 6.9.1 installation
if( EXISTS "C:/Qt/6.9.1/msvc2022_64" )
    set( Qt6_ROOT "C:/Qt/6.9.1/msvc2022_64" )
else()
    message( STATUS "Qt6 6.9.1 not found at C:/Qt/6.9.1/msvc2022_64, trying vcpkg..." )
endif()

if( Qt6_ROOT )
    set( Qt6_DIR "${Qt6_ROOT}/lib/cmake/Qt6" )
    list( APPEND CMAKE_PREFIX_PATH "${Qt6_ROOT}" )
    message( STATUS "Found Qt6 at: ${Qt6_ROOT}" )
endif()

# Use vcpkg installed packages
list( APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows" )

# Enable KICAD API support - DISABLED FOR MINIMAL BUILD
# add_compile_definitions( KICAD_IPC_API )

# Find required dependencies for minimal PCB set
find_package( PkgConfig QUIET )

# Find wxWidgets - keeping for compatibility during transformation
set( wxWidgets_USE_STATIC ON )
find_package( wxWidgets REQUIRED COMPONENTS base core adv html aui propgrid stc )

# Find Qt6 components - try different approaches
find_package( Qt6 QUIET COMPONENTS Core Widgets Gui OpenGL PrintSupport Xml Svg Network )
if( NOT Qt6_FOUND )
    message( STATUS "Qt6 not found through standard search, trying CONFIG mode..." )
    find_package( Qt6 REQUIRED CONFIG COMPONENTS Core Widgets Gui OpenGL PrintSupport Xml Svg Network )
endif()
find_package( Boost REQUIRED )
find_package( glm REQUIRED )
# find_package( protobuf REQUIRED )
find_package( opencascade REQUIRED )
# find_package( fmt REQUIRED )  # Using local fmt from thirdparty
find_package( Fontconfig REQUIRED )
find_package( CURL REQUIRED )
find_package( zstd REQUIRED )
find_package( Freetype REQUIRED )
find_package( harfbuzz CONFIG REQUIRED )

# Include directories
include_directories( BEFORE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/common
    # ${CMAKE_CURRENT_SOURCE_DIR}/generated/api/cpp  # DISABLED FOR MINIMAL BUILD
    ${CMAKE_CURRENT_SOURCE_DIR}/pcbnew
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/dynamic_bitset
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/picosha2
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/thread-pool
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/rtree
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fmt/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/magic_enum/magic_enum
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/expected/include
    ${wxWidgets_INCLUDE_DIRS}
    ${Qt6Core_INCLUDE_DIRS}
    ${Qt6Widgets_INCLUDE_DIRS}
    ${Qt6Gui_INCLUDE_DIRS}
    ${Qt6OpenGL_INCLUDE_DIRS}
    ${Qt6PrintSupport_INCLUDE_DIRS}
    ${Qt6Xml_INCLUDE_DIRS}
    ${Qt6Svg_INCLUDE_DIRS}
    ${Qt6Network_INCLUDE_DIRS}
    ${CMAKE_PREFIX_PATH}/include/opencascade
)

# Add wxWidgets compile definitions globally
add_compile_definitions( ${wxWidgets_DEFINITIONS} )

# Enable Qt automoc for automatic meta-object compilation
set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )

# Add subdirectories - only common for now
add_subdirectory( thirdparty )
add_subdirectory( libs )
add_subdirectory( common )
# add_subdirectory( pcbnew )  # Disabled until common is fully working
# add_subdirectory( test )     # Disabled until common is fully working

# Create simple main executable for minimal build testing
add_executable( qt_pcb_project
    main.cpp
)

target_link_libraries( qt_pcb_project
    # Core libraries
    common
    # pcbnew  # Disabled until common is fully working
    
    # wxWidgets libraries (for compatibility)
    ${wxWidgets_LIBRARIES}
    
    # Qt libraries
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::OpenGL
    Qt6::PrintSupport
    Qt6::Xml
    Qt6::Svg
    Qt6::Network
)