# Bitmap2component executable for minimum PCB set

add_executable( bitmap2component
    # UI component - commented out for minimal build (depends on frame)
    # bitmap2cmp_control.cpp
    # UI component - commented out for minimal build
    # bitmap2cmp_frame.cpp
    # UI component - commented out for minimal build (application entry point)
    # bitmap2cmp_main.cpp
    # UI component - commented out for minimal build
    # bitmap2cmp_panel.cpp
    # UI component - commented out for minimal build (wxFormBuilder generated)
    # bitmap2cmp_panel_base.cpp
    bitmap2cmp_settings.cpp
    bitmap2component.cpp
)

target_include_directories( bitmap2component PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/kimath/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
)

target_link_libraries( bitmap2component
    common
    potrace
    # ${wxWidgets_LIBRARIES}
)