# Minimal PCB Project - Simplified CMake configuration

cmake_minimum_required( VERSION 3.22 FATAL_ERROR )

# Set CMake policies to avoid warnings
cmake_policy(SET CMP0167 NEW)  # Use find_package for Boost instead of FindBoost

# Enable CMAKE_MSVC_DEBUG_INFORMATION_FORMAT to override the cmake pdb flags easily
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
endif (POLICY CMP0141)

# Default to CMAKE_BUILD_TYPE = Release unless overridden on command line
if( DEFINED CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Set to either \"Release\" or \"Debug\"" )
else()
    set( CMAKE_BUILD_TYPE Release CACHE STRING "Set to either \"Release\" or \"Debug\"" )
endif()

project( kicad_core_project_wx LANGUAGES C CXX )

# Configure platform-specific settings including assembly language support
include( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ConfigurePlatform.cmake )

# Auto-detect and configure ccache if available
include( ${CMAKE_CURRENT_SOURCE_DIR}/AutoCCache.cmake )

# Output compile_commands.json for various LSP and other users
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

# MSVC debug information format settings for ccache compatibility
if( MSVC )
    # cmake 3.25 or higher required for MSVC, should be fine
    if( CMAKE_C_COMPILER_LAUNCHER OR CMAKE_CXX_COMPILER_LAUNCHER )
        # Equivalent to /Z7, ensures cmake doesn't fight over the default
        # ccache may encounter external PDB file write errors due to parallelism
        # Using the embedded PDB mode is ok for development use of ccache
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "Embedded")
    else()
        # Equivalent to /Zi, ensures cmake doesn't fight over the default
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "ProgramDatabase")
    endif()
endif()

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
    # In fully standards-compliant mode, M_PI et al. are defined only if
    # _USE_MATH_DEFINES is set.
    add_compile_definitions( _USE_MATH_DEFINES )
    # Hide Windows's min() and max() macros
    add_compile_definitions( NOMINMAX )
    # Add UTF-8 support for fmt library and Unicode
    if( MSVC )
        add_compile_options( /utf-8 /wd4273 )
    endif()
    # Set wxWidgets port for KIPLATFORM
    set( KICAD_WX_PORT "msw" )
endif()

# Enable KICAD API support - DISABLED FOR MINIMAL BUILD
# add_compile_definitions( KICAD_IPC_API )

# Find required dependencies for minimal PCB set
find_package( PkgConfig QUIET )

# Use vcpkg installed packages
# Check if using vcpkg from command line or from local install
if( DEFINED CMAKE_TOOLCHAIN_FILE )
    # Using vcpkg toolchain, paths are handled automatically
    message( STATUS "Using vcpkg from toolchain: ${CMAKE_TOOLCHAIN_FILE}" )
else()
    # Fallback to local vcpkg_installed directory - disabled for minimal build testing
    # set( CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows" )
    message( STATUS "No vcpkg toolchain specified, dependencies must be found via system paths" )
endif()
set( wxWidgets_USE_STATIC ON )
find_package( wxWidgets REQUIRED COMPONENTS base core adv html aui propgrid stc gl net xml richtext webview )

# Boost configuration for vcpkg
set( Boost_USE_STATIC_LIBS ON )
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_RUNTIME OFF )
set( Boost_NO_BOOST_CMAKE OFF )  # Use the new Boost CMake config
if( MSVC )
    # Disallow implicit linking for Boost
    add_compile_definitions( BOOST_ALL_NO_LIB )
    # But allow it for the UUID so that it will link against bcrypt
    add_compile_definitions( BOOST_UUID_FORCE_AUTO_LINK )
endif()
find_package( Boost 1.71.0 REQUIRED CONFIG COMPONENTS headers locale filesystem system thread )
find_package( glm REQUIRED )
find_package( opencascade REQUIRED )
find_package( Fontconfig REQUIRED )
find_package( CURL REQUIRED )
find_package( ZLIB REQUIRED )
find_package( zstd REQUIRED )
find_package( Freetype REQUIRED )
find_package( harfbuzz CONFIG REQUIRED )
find_package( OpenGL REQUIRED )

# Optional libraries for extended functionality
# find_package( ngspice QUIET )  # For circuit simulation in eeschema
# find_package( libgit2 QUIET )  # For git integration
# find_package( protobuf QUIET )  # For IPC API support

# Add CMAKE_MODULE_PATH for finding custom modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set( KICAD_CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

# Cairo and Pixman are required and always available in this build
find_package( Cairo REQUIRED )
find_package( Pixman REQUIRED )

message( STATUS "Cairo found: ${CAIRO_INCLUDE_DIR}" )
message( STATUS "Pixman found: ${PIXMAN_INCLUDE_DIR}" )
add_compile_definitions( HAVE_CAIRO=1 )
add_compile_definitions( HAVE_PIXMAN=1 )

# Find GLEW library, required for OpenGL
find_package( GLEW REQUIRED )
include_directories( SYSTEM ${GLEW_INCLUDE_DIR} )

# Include directories
include_directories( BEFORE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/common/gal/opengl  # For bitmap_font_img.c
    ${CMAKE_CURRENT_SOURCE_DIR}  # For cursor XPM files (cursors/cursor-*.xpm) - using local cursors directory
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/common
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/eeschema
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/include
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/include/gal/shaders  # For GLSL shader headers
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/api/cpp  # Re-enabled for API headers
    
    # Core libraries include paths (for headers like math/*, kiplatform/*, core/*)
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/kimath/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/kiplatform/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/sexpr/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/kinng/include
    
    ${CMAKE_CURRENT_SOURCE_DIR}/pcbnew
    ${CMAKE_CURRENT_SOURCE_DIR}/eeschema
    ${CMAKE_CURRENT_SOURCE_DIR}/bitmap2component
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/dynamic_bitset
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/picosha2
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/thread-pool
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/rtree
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fmt/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/magic_enum/magic_enum
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/expected/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/nlohmann_json
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/json_schema_validator
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/potrace/src
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/clipper2/Clipper2Lib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/compoundfilereader
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/delaunator
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/dxflib_qcad
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/nanosvg/src
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/other_math
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/pegtl/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/tinyspline_lib
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/nanodbc
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/markdown2html
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libcontext
    ${wxWidgets_INCLUDE_DIRS}
    ${CMAKE_PREFIX_PATH}/include/opencascade
    ${Boost_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
    ${HarfBuzz_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIR}
)

# Add wxWidgets compile definitions globally
add_compile_definitions( ${wxWidgets_DEFINITIONS} )

# Disable optional features for minimum build
add_compile_definitions(
    KICAD_USE_3DCONNEXION=0  # Disable 3D mouse support
    KICAD_SPICE=0  # Disable spice simulation for minimal build
    # KICAD_USE_SENTRY=0  # Disable sentry metrics - DO NOT DEFINE to avoid #ifdef evaluation
    GLM_FORCE_CTOR_INIT  # Force GLM constructor initialization
    # Fix DLL import/export issues for static builds
    KICOMMON_DLL=1  # Define as DLL to use APIEXPORT instead of APIIMPORT for static builds
    COMPILING_DLL=1  # Ensure KIFACE_API uses APIEXPORT
)

# Include version header creation
include( ${KICAD_CMAKE_MODULE_PATH}/CreateGitVersionHeader.cmake )
include( ${KICAD_CMAKE_MODULE_PATH}/KiCadVersion.cmake )

# Generate version header file.
add_custom_target(
    version_header ALL
    COMMAND ${CMAKE_COMMAND}
        -DKICAD_VERSION_EXTRA=${KICAD_VERSION_EXTRA}
        -DOUTPUT_FILE=${CMAKE_BINARY_DIR}/kicad_build_version.h
        -DTEXT_OUTPUT_FILE=${CMAKE_BINARY_DIR}/kicad_build_version.txt
        -DSRC_PATH=${PROJECT_SOURCE_DIR}
        -DKICAD_CMAKE_MODULE_PATH=${KICAD_CMAKE_MODULE_PATH}
        -P ${KICAD_CMAKE_MODULE_PATH}/BuildSteps/WriteVersionHeader.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    BYPRODUCTS ${CMAKE_BINARY_DIR}/kicad_build_version.h
    DEPENDS ${KICAD_CMAKE_MODULE_PATH}/BuildSteps/WriteVersionHeader.cmake
    COMMENT "Generating version string header"
    )

# Add subdirectories for minimal PCB functionality
add_subdirectory( thirdparty )
add_subdirectory( libs )
add_subdirectory( common )
add_subdirectory( eeschema )
add_subdirectory( pcbnew )
add_subdirectory( test )


# Create simple main executable for minimal build testing
add_executable( kicad_core_project_wx
    main.cpp
 "config.h")

target_link_libraries( kicad_core_project_wx
    # Core KiCad libraries - commented out for minimal build
    common
    pcbnew
    eeschema_lib
    
    # Libraries from libs directory
    kimath
    kiplatform
    core
    sexpr
    
    # Thirdparty libraries from local build
    clipper2
    delaunator
    fmt
    nlohmann_json_schema_validator
    libcontext
    othermath
    potrace
    dxflib_qcad
    tinyspline_lib
    markdown_lib  
    nanodbc
    compoundfilereader
    TDxNavlib  
    rtree
    nlohmann_json
    
    # Third-party libraries from vcpkg
    ${wxWidgets_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CAIRO_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${CURL_LIBRARIES}
    ${ZLIB_LIBRARIES}
    $<IF:$<TARGET_EXISTS:zstd::libzstd_shared>,zstd::libzstd_shared,zstd::libzstd_static>
    harfbuzz::harfbuzz
    Fontconfig::Fontconfig
    
    # Windows-specific libraries
    $<$<PLATFORM_ID:Windows>:ws2_32>
    $<$<PLATFORM_ID:Windows>:winmm>
    $<$<PLATFORM_ID:Windows>:shlwapi>
    $<$<PLATFORM_ID:Windows>:bcrypt>
    $<$<PLATFORM_ID:Windows>:odbc32>
)
