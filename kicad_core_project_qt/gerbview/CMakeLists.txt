# Simplified Gerbview library for minimum set
add_compile_definitions( GERBVIEW )

include_directories(
    dialogs
    widgets
    tools
    navlib
    ../common
    ../include
    )

# Gerbview source files
set( GERBVIEW_SRCS
    # Core gerbview files
    X2_gerber_attributes.cpp
    am_param.cpp
    am_primitive.cpp
    aperture_macro.cpp
    clear_gbr_drawlayers.cpp
    dcode.cpp
    evaluate.cpp
    events_called_functions.cpp
    excellon_read_drill_file.cpp
    export_to_pcbnew.cpp
    files.cpp
    gbr_layout.cpp
    gerber_collectors.cpp
    gerber_draw_item.cpp
    gerber_file_image.cpp
    gerber_file_image_list.cpp
    gerbview_draw_panel_gal.cpp
    gerbview_frame.cpp
    gerbview_painter.cpp
    gerbview_printout.cpp
    job_file_reader.cpp
    menubar.cpp
    readgerb.cpp
    rs274_read_XY_and_IJ_coordinates.cpp
    rs274d.cpp
    rs274x.cpp
    toolbars_gerber.cpp
    )

# Dialog files
set( DIALOGS_SRCS

    )

# Widget files
set( WIDGET_SRCS

    )

# Tool files
set( TOOLS_SRCS
    tools/gerbview_actions.cpp
    tools/gerbview_control.cpp
    tools/gerbview_inspection_tool.cpp
    tools/gerbview_selection.cpp
    tools/gerbview_selection_tool.cpp
    )

# Navigation library
add_subdirectory( navlib )

# Create gerbview library
add_library( gerbview STATIC
    ${GERBVIEW_SRCS}
    ${DIALOGS_SRCS}
    ${WIDGET_SRCS}
    ${TOOLS_SRCS}
    )

target_link_libraries( gerbview
    PUBLIC
        common
        gal
        core
        gerbview_navlib
        nlohmann_json
        ${wxWidgets_LIBRARIES}
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
    )

add_dependencies( gerbview gerbview_navlib )

