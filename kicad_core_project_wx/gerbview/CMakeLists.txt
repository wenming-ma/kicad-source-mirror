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
    dialogs/dialog_draw_layers_settings.cpp
    dialogs/dialog_draw_layers_settings_base.cpp
    dialogs/dialog_layers_select_to_pcb.cpp
    dialogs/dialog_layers_select_to_pcb_base.cpp
    dialogs/dialog_print_gerbview.cpp
    dialogs/dialog_select_one_pcb_layer.cpp
    )

# Widget files
set( WIDGET_SRCS
    widgets/dcode_selection_box.cpp
    widgets/gbr_layer_box_selector.cpp
    widgets/gerbview_layer_widget.cpp
    widgets/layer_widget.cpp
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
add_library( gerbview_kiface_objects OBJECT
    ${GERBVIEW_SRCS}
    ${DIALOGS_SRCS}
    ${WIDGET_SRCS}
    ${TOOLS_SRCS}
    )

target_link_libraries( gerbview_kiface_objects
    PUBLIC
        common
        gal
        core
    )

target_link_libraries( gerbview_kiface_objects PUBLIC gerbview_navlib)
add_dependencies( gerbview_kiface_objects gerbview_navlib )

# Main gerbview kiface module
add_library( gerbview_kiface MODULE )

set_target_properties( gerbview_kiface PROPERTIES
    OUTPUT_NAME     gerbview
    PREFIX          ${KIFACE_PREFIX}
    SUFFIX          ${KIFACE_SUFFIX}
    )

target_link_libraries( gerbview_kiface
    PRIVATE
        nlohmann_json
        gal
        common
        core
        gerbview_kiface_objects
        ${wxWidgets_LIBRARIES}
    )

set_source_files_properties( gerbview.cpp PROPERTIES
    COMPILE_DEFINITIONS     "BUILD_KIWAY_DLL;COMPILING_DLL"
    )