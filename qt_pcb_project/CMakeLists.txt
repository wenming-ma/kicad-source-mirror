#
#  This program source code file is part of KICAD, a free EDA CAD application.
#
#  Copyright (C) 2007-2021 Kicad Developers, see AUTHORS.txt for contributors.
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, you may find one here:
#  http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
#  or you may search the http://www.gnu.org website for the version 2 license,
#  or you may write to the Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
#

cmake_minimum_required( VERSION 3.22 FATAL_ERROR )

# Generate DEPFILES without transforming relative paths
cmake_policy( SET CMP0116 OLD )

# Needed to allow the doxygen-python process that depends on swig generation running twice to occur
# The current structure requires the add_custom_command to get called twice, the NEW behavior deduplicates it
cmake_policy( SET CMP0113 OLD )

#  Enable CMAKE_MSVC_DEBUG_INFORMATION_FORMAT to override the cmake pdb flags easily
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
endif (POLICY CMP0141)

# Default to CMAKE_BUILD_TYPE = Release unless overridden on command line
if( DEFINED CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Set to either \"Release\" or \"Debug\"" )
else()
    set( CMAKE_BUILD_TYPE Release CACHE STRING "Set to either \"Release\" or \"Debug\"" )
endif()

project( qt_pcb_project )

# Auto-detect and configure ccache if available
include( ${CMAKE_CURRENT_SOURCE_DIR}/AutoCCache.cmake OPTIONAL )

# Create a default build type for our QA that doesn't include `NDEBUG`
set(CMAKE_CXX_FLAGS_QABUILD "-Os -g1 -ggdb1")

include( GNUInstallDirs )
include( CMakeDependentOption )
include( CheckCXXCompilerFlag )

# Output compile_commands.json for various LSP and other users
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

# Path to KiCad's CMake modules.
set( KICAD_CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" )
set( CMAKE_MODULE_PATH       "${KICAD_CMAKE_MODULE_PATH}" )

include( ConfigurePlatform OPTIONAL )

if( WIN32 )
    # Without WIN32_LEAN_AND_MEAN defined including windows.h pulls in legacy
    # winsocks.h which conflicts with winsocks2.h used by modern libraries such
    # as curl.
    # There is no harm in defining this globally, at worst some additional
    # headers have to be included explicitly.
    add_compile_definitions( WIN32_LEAN_AND_MEAN )
endif()

if( APPLE )
    # Apple, in their infinite wisdom, has decided to mark OpenGL as deprecated.
    # Luckily we can silence warnings about its deprecation.
    add_compile_definitions( GL_SILENCE_DEPRECATION=1 )
endif()

# Create a flag to identify the non-Apple unix systems
if( UNIX AND NOT APPLE )
    set( UNIX_NOT_APPLE ON )
else()
    set( UNIX_NOT_APPLE OFF )
endif()

if( APPLE OR WIN32 )
    set( APPLE_OR_WIN32 ON )
else()
    set( APPLE_OR_WIN32 OFF )
endif()

#
# KiCad build options for minimal PCB set
#

# Basic build options
option( KICAD_BUILD_QA_TESTS "Build KiCad quality assurance unit tests" OFF )
option( KICAD_WIN32_DPI_AWARE "Set DPI awareness to system DPI on Windows" ON )

# Configure build settings
set( CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "" FORCE )

# Find packages required by minimum set
find_package( PkgConfig REQUIRED )

# Find required dependencies for minimal PCB set
find_package( wxWidgets REQUIRED COMPONENTS base core adv html aui propgrid stc )
find_package( Boost REQUIRED COMPONENTS system filesystem program_options locale thread )
find_package( OpenCascade REQUIRED )
find_package( glm REQUIRED )
find_package( protobuf REQUIRED )

# Include directories
include_directories( BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/include )

# Add subdirectories for minimal PCB functionality
add_subdirectory( thirdparty )
add_subdirectory( libs )
add_subdirectory( common )
add_subdirectory( pcbnew )
add_subdirectory( scripting )
add_subdirectory( bitmap2component )
add_subdirectory( kicad )