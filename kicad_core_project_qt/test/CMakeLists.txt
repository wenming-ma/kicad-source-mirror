# CMakeLists.txt for PCB Parser Test
cmake_minimum_required(VERSION 3.16)


# Test executable for PCB parsing
add_executable(test_pcb_parser
    test_pcb_parser.cpp
)

# Test executable for Schematic parsing
add_executable(test_sch_parser
    test_sch_parser.cpp
)

# Include directories for PCB parser
target_include_directories(test_pcb_parser PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/pcbnew
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/libs
    ${CMAKE_SOURCE_DIR}/generated
    ${CMAKE_BINARY_DIR}/generated
)

# Include directories for Schematic parser
target_include_directories(test_sch_parser PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/eeschema
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/libs
    ${CMAKE_SOURCE_DIR}/generated
    ${CMAKE_BINARY_DIR}/generated
)

# Link required libraries for PCB parser
target_link_libraries(test_pcb_parser
    pcbnew
    common
    kimath
    sexpr
    ${wxWidgets_LIBRARIES}
    ${Boost_LIBRARIES}
)

# Link required libraries for Schematic parser
target_link_libraries(test_sch_parser
    eeschema_lib
    common
    kimath
    sexpr
    ${wxWidgets_LIBRARIES}
    ${Boost_LIBRARIES}
)

# Compiler flags for PCB parser
target_compile_definitions(test_pcb_parser PRIVATE
    WXUSINGDLL
    __WXMSW__
    _UNICODE
    UNICODE
)

# Compiler flags for Schematic parser
target_compile_definitions(test_sch_parser PRIVATE
    WXUSINGDLL
    __WXMSW__
    _UNICODE
    UNICODE
)

# Copy test files to build directory for easier testing
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/complex_hierarchy.kicad_pcb
    ${CMAKE_CURRENT_BINARY_DIR}/complex_hierarchy.kicad_pcb
    COPYONLY
)


# Copy the entire complex_hierarchy project folder to build directory
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/complex_hierarchy)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/complex_hierarchy
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

# Set output directories
set_target_properties(test_pcb_parser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_target_properties(test_sch_parser PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Add to test targets
add_custom_target(run_pcb_test
    COMMAND test_pcb_parser
    DEPENDS test_pcb_parser
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running PCB parser test"
)

add_custom_target(run_sch_test
    COMMAND test_sch_parser
    DEPENDS test_sch_parser
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running Schematic parser test"
)

# Combined test target
add_custom_target(run_all_tests
    DEPENDS run_pcb_test run_sch_test
    COMMENT "Running all parser tests"
)
