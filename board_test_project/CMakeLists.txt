cmake_minimum_required(VERSION 3.16)
project(board_test_project VERSION 1.0.0 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages (disabled for basic test)
# find_package(Boost CONFIG REQUIRED)
# find_package(wxwidgets CONFIG REQUIRED)
# find_package(OpenGL REQUIRED)

# Include directories
include_directories(include)
include_directories(libs/core/include)
include_directories(libs/kimath/include)
include_directories(thirdparty)

# Collect all source files
file(GLOB_RECURSE COMMON_SOURCES "common/*.cpp")
file(GLOB_RECURSE LIBS_SOURCES "libs/*/src/*.cpp")
file(GLOB_RECURSE PCB_SOURCES "pcbnew/*.cpp")

# Add executable
add_executable(board_test
    ${COMMON_SOURCES}
    ${LIBS_SOURCES} 
    ${PCB_SOURCES}
)

# Link libraries (disabled for basic test)
# target_link_libraries(board_test 
#     PRIVATE 
#     Boost::headers
#     wx::core wx::base
#     OpenGL::GL
# )

# Preprocessor definitions
target_compile_definitions(board_test PRIVATE
    UNICODE
    _UNICODE
    WXUSINGDLL
    KICAD_BUILD_VERSION="test"
    HAVE_VISIBILITY=1
)

# Platform specific settings
if(WIN32)
    target_compile_definitions(board_test PRIVATE WIN32 _WINDOWS)
endif()