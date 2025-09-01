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

target_link_libraries( bitmap2component
    common
    potrace
    # ${wxWidgets_LIBRARIES}
)