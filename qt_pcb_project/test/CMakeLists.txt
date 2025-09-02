# CMakeLists.txt for PCB Parser Test
cmake_minimum_required(VERSION 3.16)


# Test executable for PCB parsing
add_executable(test_pcb_parser
    test_pcb_parser.cpp
)

# Include directories
target_include_directories(test_pcb_parser PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/pcbnew
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/libs
    ${CMAKE_SOURCE_DIR}/generated
    ${CMAKE_BINARY_DIR}/generated
)

# Link required libraries
target_link_libraries(test_pcb_parser
    pcbnew
    common
    kimath
    sexpr
    ${wxWidgets_LIBRARIES}
    ${Boost_LIBRARIES}
)

# Compiler flags
target_compile_definitions(test_pcb_parser PRIVATE
    WXUSINGDLL
    __WXMSW__
    _UNICODE
    UNICODE
)

# Copy test PCB file to build directory for easier testing
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/complex_hierarchy.kicad_pcb
    ${CMAKE_CURRENT_BINARY_DIR}/complex_hierarchy.kicad_pcb
    COPYONLY
)

# Set output directory
set_target_properties(test_pcb_parser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Add to test targets
add_custom_target(run_pcb_test
    COMMAND test_pcb_parser
    DEPENDS test_pcb_parser
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running PCB parser test"
)
