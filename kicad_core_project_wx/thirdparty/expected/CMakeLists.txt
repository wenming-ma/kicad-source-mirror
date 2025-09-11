add_library( expected INTERFACE )

target_include_directories( expected INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include )

target_sources( expected INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tl/expected.hpp
    )

# Create alias to match the expected target name
add_library( tl::expected ALIAS expected )
