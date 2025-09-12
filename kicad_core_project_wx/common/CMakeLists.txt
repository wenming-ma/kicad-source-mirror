# Common library for minimum PCB set

# Generated files from kicad_core_project_wx generated directory
set( KICAD_GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../generated" )
set( COMMON_GENERATED_SRCS
    ${KICAD_GENERATED_DIR}/common/drawing_sheet/drawing_sheet_keywords.cpp
    ${KICAD_GENERATED_DIR}/common/lib_table_keywords.cpp
    # Add other generated files as needed
)

add_library( common STATIC
    # Main common files
    advanced_config.cpp
    asset_archive.cpp
    background_jobs_monitor.cpp
    base_screen.cpp
    bin_mod.cpp
    bitmap.cpp
    bitmap_base.cpp
    bitmap_info.cpp
    bitmap_store.cpp
    clipboard.cpp
    commit.cpp
    common.cpp
    config_params.cpp
    confirm.cpp
    design_block_info.cpp
    design_block_info_impl.cpp
    design_block_io.cpp
    design_block_lib_table.cpp
    dialog_shim.cpp
    draw_panel_gal.cpp
    dsnlexer.cpp
    eda_base_frame.cpp
    eda_dde.cpp
    eda_doc.cpp
    eda_draw_frame.cpp
    eda_item.cpp
    eda_pattern_match.cpp
    eda_shape.cpp
    eda_text.cpp
    eda_units.cpp
    embedded_files.cpp
    env_paths.cpp
    env_vars.cpp
    exceptions.cpp
    executable_names.cpp
    file_history.cpp
    filename_resolver.cpp
    footprint_filter.cpp
    footprint_info.cpp
    gal_display_options_common.cpp
    gestfich.cpp
    gr_basic.cpp
    gr_text.cpp
    grid_tricks.cpp
    hotkey_store.cpp
    hotkeys_basic.cpp
    json_conversions.cpp
    json_schema_validator.cpp
    kiface_base.cpp
    kiid.cpp
    kiway.cpp
    kiway_express.cpp
    kiway_holder.cpp
    kiway_player.cpp
    launch_ext.cpp
    layer_id.cpp
    lib_id.cpp
    lib_table_base.cpp
    lib_table_grid_tricks.cpp
    lib_tree_model.cpp
    lib_tree_model_adapter.cpp
    locale_io.cpp
    lset.cpp
    marker_base.cpp
    netclass.cpp
    notifications_manager.cpp
    origin_transforms.cpp
    origin_viewitem.cpp
    paths.cpp
    pgm_base.cpp
    project.cpp
    ptree.cpp
    rc_item.cpp
    refdes_utils.cpp
    reference_image.cpp
    render_settings.cpp
    reporter.cpp
    richio.cpp
    scintilla_tricks.cpp
    search_stack.cpp
    singleton.cpp
    status_popup.cpp
    string_utils.cpp
    stroke_params.cpp
    systemdirsappend.cpp
    template_fieldnames.cpp
    textentry_tricks.cpp
    thread_pool.cpp
    title_block.cpp
    trace_helpers.cpp
    ui_events.cpp
    undo_redo_container.cpp
    validators.cpp
    wildcards_and_files_ext.cpp
    xnode.cpp

    # api subdirectory
    api/api_enums.cpp
    api/api_handler.cpp
    api/api_handler_editor.cpp
    api/api_plugin.cpp
    api/api_plugin_manager.cpp
    api/api_server.cpp
    api/api_utils.cpp
    api/serializable.cpp

    # database subdirectory
    database/database_connection.cpp
    database/database_lib_settings.cpp

    # dialog_about subdirectory
    dialog_about/AboutDialog_main.cpp
    dialog_about/dialog_about.cpp
    dialog_about/dialog_about_base.cpp

    # dialogs subdirectory
    dialogs/dialog_HTML_reporter_base.cpp
    dialogs/dialog_assign_netclass.cpp
    dialogs/dialog_assign_netclass_base.cpp
    dialogs/dialog_book_reporter.cpp
    dialogs/dialog_book_reporter_base.cpp
    dialogs/dialog_color_picker.cpp
    dialogs/dialog_color_picker_base.cpp
    dialogs/dialog_configure_paths.cpp
    dialogs/dialog_configure_paths_base.cpp
    dialogs/dialog_display_html_text_base.cpp
    dialogs/dialog_edit_library_tables.cpp
    dialogs/dialog_global_lib_table_config.cpp
    dialogs/dialog_global_lib_table_config_base.cpp
    dialogs/dialog_hotkey_list.cpp
    dialogs/dialog_import_choose_project.cpp
    dialogs/dialog_import_choose_project_base.cpp
    dialogs/dialog_migrate_settings.cpp
    dialogs/dialog_migrate_settings_base.cpp
    dialogs/dialog_page_settings.cpp
    dialogs/dialog_page_settings_base.cpp
    dialogs/dialog_paste_special.cpp
    dialogs/dialog_paste_special_base.cpp
    dialogs/dialog_plugin_options.cpp
    dialogs/dialog_plugin_options_base.cpp
    dialogs/dialog_text_entry.cpp
    dialogs/dialog_text_entry_base.cpp
    dialogs/dialog_unit_entry.cpp
    dialogs/dialog_unit_entry_base.cpp
    dialogs/eda_list_dialog.cpp
    dialogs/eda_list_dialog_base.cpp
    dialogs/eda_reorderable_list_dialog.cpp
    dialogs/eda_reorderable_list_dialog_base.cpp
    dialogs/eda_view_switcher_base.cpp
    dialogs/git/panel_git_repos.cpp
    dialogs/git/panel_git_repos_base.cpp
    dialogs/hotkey_cycle_popup.cpp
    dialogs/html_message_box.cpp
    dialogs/panel_color_settings.cpp
    dialogs/panel_color_settings_base.cpp
    dialogs/panel_common_settings.cpp
    dialogs/panel_common_settings_base.cpp
    dialogs/panel_embedded_files.cpp
    dialogs/panel_embedded_files_base.cpp
    dialogs/panel_hotkeys_editor.cpp
    dialogs/panel_image_editor.cpp
    dialogs/panel_image_editor_base.cpp
    dialogs/panel_mouse_settings.cpp
    dialogs/panel_mouse_settings_base.cpp
    dialogs/panel_packages_and_updates.cpp
    dialogs/panel_packages_and_updates_base.cpp
    dialogs/panel_plugin_settings.cpp
    dialogs/panel_plugin_settings_base.cpp
    dialogs/panel_setup_netclasses.cpp
    dialogs/panel_setup_netclasses_base.cpp
    dialogs/panel_setup_severities.cpp
    dialogs/panel_text_variables.cpp
    dialogs/panel_text_variables_base.cpp

    # drawing_sheet subdirectory
    drawing_sheet/drawing_sheet_default_description.cpp
    drawing_sheet/drawing_sheet_parser.cpp
    drawing_sheet/ds_data_item.cpp
    drawing_sheet/ds_data_model.cpp
    drawing_sheet/ds_data_model_io.cpp
    drawing_sheet/ds_draw_item.cpp
    drawing_sheet/ds_painter.cpp
    drawing_sheet/ds_proxy_undo_item.cpp
    drawing_sheet/ds_proxy_view_item.cpp

    # font subdirectory
    font/font.cpp
    font/fontconfig.cpp
    font/glyph.cpp
    font/outline_decomposer.cpp
    font/outline_font.cpp
    font/stroke_font.cpp
    font/text_attributes.cpp
    font/version_info.cpp

    # gal subdirectory files moved to separate gal library (built via add_subdirectory)
    # All gal/* files now built in dedicated gal library to avoid duplication

    # http_lib subdirectory
    http_lib/http_lib_connection.cpp
    http_lib/http_lib_settings.cpp

    # import_gfx subdirectory
    import_gfx/dxf_import_plugin.cpp
    import_gfx/graphics_import_mgr.cpp
    import_gfx/graphics_importer.cpp
    import_gfx/graphics_importer_buffer.cpp
    import_gfx/svg_import_plugin.cpp

    # io subdirectory
    io/altium/altium_ascii_parser.cpp
    io/altium/altium_binary_parser.cpp
    io/altium/altium_parser_utils.cpp
    io/altium/altium_props_utils.cpp
    io/cadstar/cadstar_archive_parser.cpp
    io/cadstar/cadstar_parts_lib_parser.cpp
    io/eagle/eagle_parser.cpp
    io/easyeda/easyeda_parser_base.cpp
    io/easyeda/easyeda_parser_structs.cpp
    io/easyedapro/easyedapro_import_utils.cpp
    io/easyedapro/easyedapro_parser.cpp
    io/io_base.cpp
    io/kicad/kicad_io_utils.cpp

    # kicad_curl subdirectory
    kicad_curl/kicad_curl.cpp

    # plotters subdirectory
    plotters/DXF_plotter.cpp
    plotters/HPGL_plotter.cpp
    plotters/PDF_plotter.cpp
    plotters/PS_plotter.cpp
    plotters/SVG_plotter.cpp
    plotters/common_plot_functions.cpp
    plotters/plotter.cpp

    # preview_items subdirectory
    preview_items/anchor_debug.cpp
    preview_items/construction_geom.cpp
    preview_items/item_drawing_utils.cpp
    preview_items/polygon_geom_manager.cpp
    preview_items/polygon_item.cpp
    preview_items/selection_area.cpp
    preview_items/simple_overlay_item.cpp
    preview_items/snap_indicator.cpp

    # project subdirectory
    project/board_project_settings.cpp
    project/net_settings.cpp
    project/project_archiver.cpp
    project/project_file.cpp
    project/project_local_settings.cpp

    # properties subdirectory
    properties/color4d_variant.cpp
    properties/eda_angle_variant.cpp
    properties/pg_cell_renderer.cpp
    properties/pg_editors.cpp
    properties/pg_properties.cpp
    properties/property_mgr.cpp
    properties/std_optional_variants.cpp

    # settings subdirectory
    settings/app_settings.cpp
    settings/aui_settings.cpp
    settings/bom_settings.cpp
    settings/color_settings.cpp
    settings/common_settings.cpp
    settings/grid_settings.cpp
    settings/json_settings.cpp
    settings/kicad_settings.cpp
    settings/layer_settings_utils.cpp
    settings/nested_settings.cpp
    settings/parameters.cpp
    settings/settings_manager.cpp

    # tool subdirectory
    tool/action_manager.cpp
    tool/action_menu.cpp
    tool/action_toolbar.cpp
    tool/actions.cpp
    tool/common_control.cpp
    tool/common_tools.cpp
    tool/conditional_menu.cpp
    tool/construction_manager.cpp
    tool/edit_constraints.cpp
    tool/edit_points.cpp
    tool/editor_conditions.cpp
    tool/embed_tool.cpp
    tool/grid_helper.cpp
    tool/grid_menu.cpp
    tool/library_editor_control.cpp
    tool/picker_tool.cpp
    tool/point_editor_behavior.cpp
    tool/properties_tool.cpp
    tool/selection.cpp
    tool/selection_conditions.cpp
    tool/selection_tool.cpp
    tool/tool_action.cpp
    tool/tool_base.cpp
    tool/tool_dispatcher.cpp
    tool/tool_event.cpp
    tool/tool_interactive.cpp
    tool/tool_manager.cpp
    tool/tool_menu.cpp
    tool/tools_holder.cpp
    tool/zoom_menu.cpp
    tool/zoom_tool.cpp

    # view subdirectory
    view/view.cpp
    view/view_controls.cpp
    view/view_group.cpp
    view/view_item.cpp
    view/view_overlay.cpp
    view/wx_view_controls.cpp
    view/zoom_controller.cpp

    # widgets subdirectory
    widgets/app_progress_dialog.cpp
    widgets/bitmap_button.cpp
    widgets/button_row_panel.cpp
    widgets/color_swatch.cpp
    widgets/filter_combobox.cpp
    widgets/font_choice.cpp
    widgets/footprint_choice.cpp
    widgets/footprint_preview_widget.cpp
    widgets/footprint_select_widget.cpp
    widgets/grid_checkbox.cpp
    widgets/grid_color_swatch_helpers.cpp
    widgets/grid_combobox.cpp
    widgets/grid_icon_text_helpers.cpp
    widgets/grid_text_button_helpers.cpp
    widgets/grid_text_helpers.cpp
    widgets/html_window.cpp
    widgets/kistatusbar.cpp
    widgets/lib_tree.cpp
    widgets/listbox_tricks.cpp
    widgets/mathplot.cpp
    widgets/msgpanel.cpp
    widgets/number_badge.cpp
    widgets/paged_dialog.cpp
    widgets/progress_reporter_base.cpp
    widgets/properties_panel.cpp
    widgets/search_pane.cpp
    widgets/search_pane_base.cpp
    widgets/search_pane_tab.cpp
    widgets/std_bitmap_button.cpp
    widgets/ui_common.cpp
    widgets/unit_binder.cpp
    widgets/widget_hotkey_list.cpp
    widgets/wx_aui_art_providers.cpp
    widgets/wx_aui_utils.cpp
    widgets/wx_busy_indicator.cpp
    widgets/wx_combobox.cpp
    widgets/wx_dataviewctrl.cpp
    widgets/wx_grid.cpp
    widgets/wx_grid_autosizer.cpp
    widgets/wx_html_report_box.cpp
    widgets/wx_html_report_panel.cpp
    widgets/wx_html_report_panel_base.cpp
    widgets/wx_infobar.cpp
    widgets/wx_listbox.cpp
    widgets/wx_panel.cpp
    widgets/wx_progress_reporters.cpp
    widgets/wx_treebook.cpp

    # Generated sources
    ${COMMON_GENERATED_SRCS}
)

# Include directories
target_include_directories( common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../cursors  # For cursor XPM files
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs  # Added for eda_view_switcher_base.h
    ${KICAD_GENERATED_DIR}
    ${KICAD_GENERATED_DIR}/common
)

# Add subdirectories that have their own CMakeLists.txt
add_subdirectory( gal )

# Link libraries
target_link_libraries( common PUBLIC
    gal
    kimath
    kiplatform
    ${wxWidgets_LIBRARIES}
)
