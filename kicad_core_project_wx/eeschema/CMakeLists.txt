# CMakeLists.txt for eeschema
# Optimized to compile only minimum set files

# Add subdirectory that has its own CMakeLists.txt
add_subdirectory(navlib)

# Main eeschema sources
set(EESCHEMA_SOURCES
    # Main directory files
    annotate.cpp
    autoplace_fields.cpp
    bom_plugins.cpp
    bus-wire-junction.cpp
    connection_graph.cpp
    cross-probing.cpp
    design_block_tree_model_adapter.cpp
    design_block_utils.cpp
    eeschema_config.cpp
    eeschema_helpers.cpp
    eeschema_settings.cpp
    fields_data_model.cpp
    fields_grid_table.cpp
    files-io.cpp
    generate_alias_info.cpp
    gfx_import_utils.cpp
    junction_helpers.cpp
    lib_symbol.cpp
    menubar.cpp
    net_navigator.cpp
    picksymbol.cpp
    pin_layout_cache.cpp
    pin_numbers.cpp
    pin_type.cpp
    project_rescue.cpp
    project_sch.cpp
    sch_base_frame.cpp
    sch_bitmap.cpp
    sch_bus_entry.cpp
    sch_collectors.cpp
    sch_commit.cpp
    sch_connection.cpp
    sch_draw_panel.cpp
    sch_edit_frame.cpp
    sch_field.cpp
    sch_item.cpp
    sch_junction.cpp
    sch_label.cpp
    sch_line.cpp
    sch_marker.cpp
    sch_no_connect.cpp
    sch_painter.cpp
    sch_pin.cpp
    sch_plotter.cpp
    sch_preview_panel.cpp
    sch_reference_list.cpp
    sch_render_settings.cpp
    sch_rule_area.cpp
    sch_screen.cpp
    sch_shape.cpp
    sch_sheet.cpp
    sch_sheet_path.cpp
    sch_sheet_pin.cpp
    sch_symbol.cpp
    sch_table.cpp
    sch_tablecell.cpp
    sch_text.cpp
    sch_textbox.cpp
    sch_validators.cpp
    sch_view.cpp
    schematic.cpp
    schematic_settings.cpp
    schematic_undo_redo.cpp
    sheet.cpp
    symb_transforms_utils.cpp
    symbol.cpp
    symbol_async_loader.cpp
    symbol_checker.cpp
    symbol_lib_table.cpp
    symbol_library.cpp
    symbol_library_manager.cpp
    symbol_tree_model_adapter.cpp
    symbol_tree_synchronizing_adapter.cpp
    symbol_viewer_frame.cpp
    toolbars_sch_editor.cpp
    toolbars_symbol_viewer.cpp

    # api subdirectory
    api/api_handler_sch.cpp
    api/api_sch_utils.cpp

    # dialogs subdirectory
    dialogs/dialog_annotate.cpp
    dialogs/dialog_annotate_base.cpp
    dialogs/dialog_bom.cpp
    dialogs/dialog_bom_base.cpp
    dialogs/dialog_change_symbols.cpp
    dialogs/dialog_change_symbols_base.cpp
    dialogs/dialog_database_lib_settings.cpp
    dialogs/dialog_database_lib_settings_base.cpp
    dialogs/dialog_design_block_properties.cpp
    dialogs/dialog_design_block_properties_base.cpp
    dialogs/dialog_edit_symbols_libid.cpp
    dialogs/dialog_edit_symbols_libid_base.cpp
    dialogs/dialog_eeschema_page_settings.cpp
    dialogs/dialog_erc.cpp
    dialogs/dialog_erc_base.cpp
    dialogs/dialog_export_netlist.cpp
    dialogs/dialog_export_netlist_base.cpp
    dialogs/dialog_field_properties.cpp
    dialogs/dialog_field_properties_base.cpp
    dialogs/dialog_global_edit_text_and_graphics.cpp
    dialogs/dialog_global_edit_text_and_graphics_base.cpp
    dialogs/dialog_global_sym_lib_table_config.cpp
    dialogs/dialog_image_properties.cpp
    dialogs/dialog_image_properties_base.cpp
    dialogs/dialog_increment_annotations_base.cpp
    dialogs/dialog_junction_props.cpp
    dialogs/dialog_junction_props_base.cpp
    dialogs/dialog_label_properties.cpp
    dialogs/dialog_label_properties_base.cpp
    dialogs/dialog_lib_edit_pin_table.cpp
    dialogs/dialog_lib_edit_pin_table_base.cpp
    dialogs/dialog_lib_new_symbol.cpp
    dialogs/dialog_lib_new_symbol_base.cpp
    dialogs/dialog_lib_symbol_properties.cpp
    dialogs/dialog_lib_symbol_properties_base.cpp
    dialogs/dialog_line_properties.cpp
    dialogs/dialog_line_properties_base.cpp
    dialogs/dialog_migrate_buses.cpp
    dialogs/dialog_migrate_buses_base.cpp
    dialogs/dialog_pin_properties.cpp
    dialogs/dialog_pin_properties_base.cpp
    dialogs/dialog_plot_schematic.cpp
    dialogs/dialog_plot_schematic_base.cpp
    dialogs/dialog_rescue_each.cpp
    dialogs/dialog_rescue_each_base.cpp
    dialogs/dialog_sch_import_settings.cpp
    dialogs/dialog_sch_import_settings_base.cpp
    dialogs/dialog_schematic_find.cpp
    dialogs/dialog_schematic_find_base.cpp
    dialogs/dialog_schematic_setup.cpp
    dialogs/dialog_shape_properties.cpp
    dialogs/dialog_shape_properties_base.cpp
    dialogs/dialog_sheet_pin_properties.cpp
    dialogs/dialog_sheet_pin_properties_base.cpp
    dialogs/dialog_sheet_properties.cpp
    dialogs/dialog_sheet_properties_base.cpp
    dialogs/dialog_sim_command.cpp
    dialogs/dialog_sim_command_base.cpp
    dialogs/dialog_sim_format_value.cpp
    dialogs/dialog_sim_format_value_base.cpp
    dialogs/dialog_sim_model.cpp
    dialogs/dialog_sim_model_base.cpp
    dialogs/dialog_symbol_chooser.cpp
    dialogs/dialog_symbol_fields_table.cpp
    dialogs/dialog_symbol_fields_table_base.cpp
    dialogs/dialog_symbol_properties.cpp
    dialogs/dialog_symbol_properties_base.cpp
    dialogs/dialog_symbol_remap.cpp
    dialogs/dialog_symbol_remap_base.cpp
    dialogs/dialog_table_properties.cpp
    dialogs/dialog_table_properties_base.cpp
    dialogs/dialog_tablecell_properties.cpp
    dialogs/dialog_tablecell_properties_base.cpp
    dialogs/dialog_text_properties.cpp
    dialogs/dialog_text_properties_base.cpp
    dialogs/dialog_update_from_pcb.cpp
    dialogs/dialog_update_from_pcb_base.cpp
    dialogs/dialog_update_symbol_fields.cpp
    dialogs/dialog_update_symbol_fields_base.cpp
    dialogs/dialog_user_defined_signals.cpp
    dialogs/dialog_user_defined_signals_base.cpp
    dialogs/dialog_wire_bus_properties.cpp
    dialogs/dialog_wire_bus_properties_base.cpp
    dialogs/panel_bom_presets.cpp
    dialogs/panel_bom_presets_base.cpp
    dialogs/panel_eeschema_color_settings.cpp
    dialogs/panel_setup_buses.cpp
    dialogs/panel_setup_buses_base.cpp
    dialogs/panel_setup_formatting.cpp
    dialogs/panel_setup_formatting_base.cpp
    dialogs/panel_setup_pinmap.cpp
    dialogs/panel_setup_pinmap_base.cpp
    dialogs/panel_sym_lib_table.cpp
    dialogs/panel_sym_lib_table_base.cpp
    dialogs/panel_template_fieldnames.cpp
    dialogs/panel_template_fieldnames_base.cpp

    # erc subdirectory
    erc/erc.cpp
    erc/erc_item.cpp
    erc/erc_report.cpp
    erc/erc_sch_pin_context.cpp
    erc/erc_settings.cpp

    # import_gfx subdirectory
    import_gfx/dialog_import_gfx_sch.cpp
    import_gfx/dialog_import_gfx_sch_base.cpp
    import_gfx/graphics_importer_lib_symbol.cpp
    import_gfx/graphics_importer_sch.cpp

    # netlist_exporters subdirectory
    netlist_exporters/netlist_exporter_allegro.cpp
    netlist_exporters/netlist_exporter_base.cpp
    netlist_exporters/netlist_exporter_cadstar.cpp
    netlist_exporters/netlist_exporter_kicad.cpp
    netlist_exporters/netlist_exporter_orcadpcb2.cpp
    netlist_exporters/netlist_exporter_pads.cpp
    netlist_exporters/netlist_exporter_spice.cpp
    netlist_exporters/netlist_exporter_spice_model.cpp
    netlist_exporters/netlist_exporter_xml.cpp
    netlist_exporters/netlist_generator.cpp

    # printing subdirectory
    printing/dialog_print.cpp
    printing/dialog_print_base.cpp
    printing/sch_printout.cpp

    # sch_io subdirectory
    sch_io/altium/altium_parser_sch.cpp
    sch_io/altium/sch_io_altium.cpp
    sch_io/cadstar/cadstar_sch_archive_loader.cpp
    sch_io/cadstar/cadstar_sch_archive_parser.cpp
    sch_io/cadstar/sch_io_cadstar_archive.cpp
    sch_io/database/sch_io_database.cpp
    sch_io/eagle/sch_io_eagle.cpp
    sch_io/easyeda/sch_easyeda_parser.cpp
    sch_io/easyeda/sch_io_easyeda.cpp
    sch_io/easyedapro/sch_easyedapro_parser.cpp
    sch_io/easyedapro/sch_io_easyedapro.cpp
    sch_io/http_lib/sch_io_http_lib.cpp
    sch_io/kicad_legacy/sch_io_kicad_legacy.cpp
    sch_io/kicad_legacy/sch_io_kicad_legacy_helpers.cpp
    sch_io/kicad_legacy/sch_io_kicad_legacy_lib_cache.cpp
    sch_io/kicad_sexpr/sch_io_kicad_sexpr.cpp
    sch_io/kicad_sexpr/sch_io_kicad_sexpr_common.cpp
    sch_io/kicad_sexpr/sch_io_kicad_sexpr_lib_cache.cpp
    sch_io/kicad_sexpr/sch_io_kicad_sexpr_parser.cpp
    sch_io/ltspice/ltspice_schematic.cpp
    sch_io/ltspice/sch_io_ltspice.cpp
    sch_io/ltspice/sch_io_ltspice_parser.cpp
    sch_io/sch_io.cpp
    sch_io/sch_io_lib_cache.cpp
    sch_io/sch_io_mgr.cpp

    # sim subdirectory
    sim/kibis/ibis_parser.cpp
    sim/kibis/kibis.cpp
    sim/legacy_workbook.cpp
    sim/ngspice.cpp
    sim/sim_lib_mgr.cpp
    sim/sim_library.cpp
    sim/sim_library_ibis.cpp
    sim/sim_library_spice.cpp
    sim/sim_model.cpp
    sim/sim_model_behavioral.cpp
    sim/sim_model_ibis.cpp
    sim/sim_model_ideal.cpp
    sim/sim_model_l_mutual.cpp
    sim/sim_model_ngspice.cpp
    sim/sim_model_ngspice_data_b3soi.cpp
    sim/sim_model_ngspice_data_b4soi.cpp
    sim/sim_model_ngspice_data_bjt.cpp
    sim/sim_model_ngspice_data_bsim1.cpp
    sim/sim_model_ngspice_data_bsim2.cpp
    sim/sim_model_ngspice_data_bsim3.cpp
    sim/sim_model_ngspice_data_bsim4.cpp
    sim/sim_model_ngspice_data_diode.cpp
    sim/sim_model_ngspice_data_hfet.cpp
    sim/sim_model_ngspice_data_hicum2.cpp
    sim/sim_model_ngspice_data_hsim.cpp
    sim/sim_model_ngspice_data_jfet.cpp
    sim/sim_model_ngspice_data_mes.cpp
    sim/sim_model_ngspice_data_mos.cpp
    sim/sim_model_ngspice_data_mos6.cpp
    sim/sim_model_ngspice_data_mos9.cpp
    sim/sim_model_ngspice_data_vbic.cpp
    sim/sim_model_r_pot.cpp
    sim/sim_model_raw_spice.cpp
    sim/sim_model_serializer.cpp
    sim/sim_model_source.cpp
    sim/sim_model_spice.cpp
    sim/sim_model_spice_fallback.cpp
    sim/sim_model_subckt.cpp
    sim/sim_model_switch.cpp
    sim/sim_model_tline.cpp
    sim/sim_model_xspice.cpp
    sim/sim_plot_colors.cpp
    sim/sim_plot_tab.cpp
    sim/sim_property.cpp
    sim/sim_tab.cpp
    sim/sim_value.cpp
    sim/simulator_frame.cpp
    sim/simulator_frame_ui.cpp
    sim/simulator_frame_ui_base.cpp
    sim/spice_circuit_model.cpp
    sim/spice_generator.cpp
    sim/spice_library_parser.cpp
    sim/spice_model_parser.cpp
    sim/spice_settings.cpp
    sim/spice_simulator.cpp
    sim/spice_value.cpp
    sim/toolbars_simulator_frame.cpp

    # symbol_editor subdirectory
    symbol_editor/lib_logger.cpp
    symbol_editor/lib_symbol_library_manager.cpp
    symbol_editor/menubar_symbol_editor.cpp
    symbol_editor/symbol_edit_frame.cpp
    symbol_editor/symbol_editor.cpp
    symbol_editor/symbol_editor_import_export.cpp
    symbol_editor/symbol_editor_plotter.cpp
    symbol_editor/symbol_editor_undo_redo.cpp
    symbol_editor/toolbars_symbol_editor.cpp

    # sync_sheet_pin subdirectory
    sync_sheet_pin/dialog_sync_sheet_pins.cpp
    sync_sheet_pin/dialog_sync_sheet_pins_base.cpp
    sync_sheet_pin/panel_sync_sheet_pins.cpp
    sync_sheet_pin/panel_sync_sheet_pins_base.cpp
    sync_sheet_pin/sheet_synchronization_agent.cpp
    sync_sheet_pin/sheet_synchronization_item.cpp
    sync_sheet_pin/sheet_synchronization_model.cpp
    sync_sheet_pin/sheet_synchronization_notifier.cpp

    # tools subdirectory
    tools/assign_footprints.cpp
    tools/backannotate.cpp
    tools/ee_grid_helper.cpp
    tools/rule_area_create_helper.cpp
    tools/sch_actions.cpp
    tools/sch_design_block_control.cpp
    tools/sch_drawing_tools.cpp
    tools/sch_edit_table_tool.cpp
    tools/sch_edit_tool.cpp
    tools/sch_editor_conditions.cpp
    tools/sch_editor_control.cpp
    tools/sch_find_replace_tool.cpp
    tools/sch_inspection_tool.cpp
    tools/sch_line_wire_bus_tool.cpp
    tools/sch_move_tool.cpp
    tools/sch_navigate_tool.cpp
    tools/sch_point_editor.cpp
    tools/sch_selection.cpp
    tools/sch_selection_tool.cpp
    tools/sch_tool_utils.cpp
    tools/simulator_control.cpp
    tools/symbol_editor_control.cpp
    tools/symbol_editor_drawing_tools.cpp
    tools/symbol_editor_edit_tool.cpp
    tools/symbol_editor_move_tool.cpp
    tools/symbol_editor_pin_tool.cpp

    # widgets subdirectory
    widgets/design_block_pane.cpp
    widgets/design_block_preview_widget.cpp
    widgets/hierarchy_pane.cpp
    widgets/panel_design_block_chooser.cpp
    widgets/panel_sch_selection_filter.cpp
    widgets/panel_sch_selection_filter_base.cpp
    widgets/panel_symbol_chooser.cpp
    widgets/pin_shape_combobox.cpp
    widgets/pin_type_combobox.cpp
    widgets/sch_properties_panel.cpp
    widgets/sch_search_pane.cpp
    widgets/search_handlers.cpp
    widgets/symbol_diff_widget.cpp
    widgets/symbol_filter_combobox.cpp
    widgets/symbol_preview_widget.cpp
    widgets/symbol_tree_pane.cpp
    widgets/tuner_slider.cpp
    widgets/tuner_slider_base.cpp
)

# Create eeschema library
add_library(eeschema_lib STATIC ${EESCHEMA_SOURCES})

target_include_directories(eeschema_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/import_gfx
    ${CMAKE_CURRENT_SOURCE_DIR}/netlist_exporters
    ${CMAKE_CURRENT_SOURCE_DIR}/printing
    ${CMAKE_CURRENT_SOURCE_DIR}/sch_io
    ${CMAKE_CURRENT_SOURCE_DIR}/sim
    ${CMAKE_CURRENT_SOURCE_DIR}/symbol_editor
    ${CMAKE_CURRENT_SOURCE_DIR}/sync_sheet_pin
    ${CMAKE_CURRENT_SOURCE_DIR}/tools
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/erc
    ${CMAKE_CURRENT_SOURCE_DIR}/api
)

target_link_libraries(eeschema_lib PUBLIC
    common
    kimath
    eeschema_navlib  # Link navlib library
    ${wxWidgets_LIBRARIES}
)

# Add compile definitions if needed
target_compile_definitions(eeschema_lib PUBLIC
    EESCHEMA
)
