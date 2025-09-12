# Kinng library for minimum PCB set

add_library( kinng STATIC
    src/kinng.cpp
)

target_include_directories( kinng PUBLIC
    include
    ${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/include
)

# Find and link nng library (optional for minimal build)
find_package( nng QUIET )
if( nng_FOUND )
    target_link_libraries( kinng PRIVATE nng::nng )
    target_compile_definitions( kinng PRIVATE HAVE_NNG )
else()
    message( STATUS "nng library not found, building without nng support" )
endif()

# For now, comment out wx functionality since it's causing configuration issues
# Will use simplified includes approach instead