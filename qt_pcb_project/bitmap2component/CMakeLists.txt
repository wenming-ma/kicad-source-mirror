# Bitmap2component executable for minimum PCB set

add_executable( bitmap2component
    bitmap2cmp_control.cpp
    bitmap2cmp_frame.cpp
    bitmap2cmp_main.cpp
    bitmap2cmp_panel.cpp
    bitmap2cmp_panel_base.cpp
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