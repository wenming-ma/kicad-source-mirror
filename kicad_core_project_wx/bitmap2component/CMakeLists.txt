# CMakeLists.txt for bitmap2component
# Optimized to compile only minimum set files

set(BITMAP2COMPONENT_SOURCES
    bitmap2cmp_control.cpp
    bitmap2cmp_frame.cpp
    bitmap2cmp_main.cpp
    bitmap2cmp_panel.cpp
    bitmap2cmp_panel_base.cpp
    bitmap2cmp_settings.cpp
    bitmap2component.cpp
)

# Create bitmap2component library
add_library(bitmap2component STATIC ${BITMAP2COMPONENT_SOURCES})

target_include_directories(bitmap2component PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(bitmap2component PUBLIC
    common
    ${wxWidgets_LIBRARIES}
)
