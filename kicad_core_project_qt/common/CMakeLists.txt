# Common library for minimum PCB set

# Create minimal kicommon library first (needed by gal)
add_library( kicommon STATIC
    # Minimal files needed by gal and other components
    build_version.cpp
    string_utils.cpp
    exceptions.cpp
    richio.cpp
    config_params.cpp
    common.cpp
    eda_units.cpp
)

target_link_libraries( kicommon
    kimath
    kiplatform
    nlohmann_json
    ${wxWidgets_LIBRARIES}
)

target_include_directories( kicommon PUBLIC
    .
    ${CMAKE_BINARY_DIR}
    ../include
)

# Add subdirectories
add_subdirectory( gal )

# Generated files from kicad_core_project_wx generated directory
set( KICAD_GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../generated" )
set( COMMON_GENERATED_SRCS
    # ${KICAD_GENERATED_DIR}/api/cpp/api/common/envelope.pb.cc  # DISABLED FOR MINIMAL BUILD
    ${KICAD_GENERATED_DIR}/common/drc_rules_keywords.cpp
    ${KICAD_GENERATED_DIR}/common/embedded_files_keywords.cpp
    ${KICAD_GENERATED_DIR}/common/lib_table_keywords.cpp
    ${KICAD_GENERATED_DIR}/common/netlist_keywords.cpp
    ${KICAD_GENERATED_DIR}/common/pcb_keywords.cpp
    ${KICAD_GENERATED_DIR}/common/pcb_plot_params_keywords.cpp
    ${KICAD_GENERATED_DIR}/common/stroke_params_keywords.cpp
    ${KICAD_GENERATED_DIR}/common/drawing_sheet/drawing_sheet_keywords.cpp
)

add_library( common STATIC
    # Core common files
    advanced_config.cpp
    asset_archive.cpp
    # UI component - commented out for minimal build
    # background_jobs_monitor.cpp
    base_screen.cpp
    bin_mod.cpp
    bitmap.cpp
    bitmap_base.cpp
    bitmap_info.cpp
    bitmap_store.cpp
    build_version.cpp
    callback_gal.cpp
    common.cpp
    config_params.cpp
    confirm.cpp
    design_block_info.cpp
    design_block_info_impl.cpp
    design_block_io.cpp
    design_block_lib_table.cpp

    # New common files
    board_printout.cpp
    clipboard.cpp
    eda_draw_frame.cpp
    gal_display_options_common.cpp
    gbr_metadata.cpp
    lib_tree_model.cpp
    lib_tree_model_adapter.cpp
    origin_viewitem.cpp
    printout.cpp

    dpi_scaling.cpp
    dpi_scaling_common.cpp
    dsnlexer.cpp
    eda_base_frame.cpp
    eda_dde.cpp
    eda_doc.cpp
    eda_item.cpp
    eda_pattern_match.cpp
    eda_shape.cpp
    eda_text.cpp
    eda_units.cpp
    embedded_files.cpp
    env_paths.cpp
    env_vars.cpp
    exceptions.cpp
    file_history.cpp
    gestfich.cpp
    gr_basic.cpp
    gr_text.cpp
    grid_tricks.cpp
    hash_eda.cpp
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
    locale_io.cpp
    lset.cpp
    marker_base.cpp
    markup_parser.cpp
    netclass.cpp
    # UI component - commented out for minimal build
    # notifications_manager.cpp
    origin_transforms.cpp
    page_info.cpp
    paths.cpp
    pgm_base.cpp
    project.cpp
    rc_item.cpp
    refdes_utils.cpp
    reference_image.cpp
    render_settings.cpp
    reporter.cpp
    richio.cpp
    scintilla_tricks.cpp
    search_stack.cpp
    singleton.cpp
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
    wx_filename.cpp
    
    # API files - DISABLED FOR MINIMAL BUILD
    # api/api_enums.cpp
    # api/api_handler.cpp
    # api/api_plugin.cpp
    # api/api_plugin_manager.cpp
    # api/api_server.cpp
    # api/api_utils.cpp
    api/serializable.cpp  # Re-enabled for base class interface compatibility
    
    # Database files
    database/database_lib_settings.cpp
    
    
    # Font files
    font/font.cpp
    font/fontconfig.cpp
    font/glyph.cpp
    newstroke_font.cpp
    font/outline_decomposer.cpp
    font/outline_font.cpp
    font/stroke_font.cpp
    font/text_attributes.cpp
    font/version_info.cpp
    
    # GAL files
    gal/color4d.cpp
    gal/gal_display_options.cpp
    gal/graphics_abstraction_layer.cpp
    gal/painter.cpp
    # UI component - commented out for minimal build (OpenGL UI context)
    # gal/opengl/gl_context_mgr.cpp
    
    # Import/Export GFX files
    import_gfx/dxf_import_plugin.cpp
    import_gfx/graphics_import_mgr.cpp
    import_gfx/graphics_importer.cpp
    import_gfx/graphics_importer_buffer.cpp
    import_gfx/svg_import_plugin.cpp

    # IO files
    io/io_base.cpp
    io/kicad/kicad_io_utils.cpp

    # Job files (only headers copied, no .cpp files)
    
    # Kicad curl
    kicad_curl/kicad_curl.cpp
    
    # Libeval compiler
    libeval_compiler/libeval_compiler.cpp

    # Plotters
    plotters/DXF_plotter.cpp
    plotters/GERBER_plotter.cpp
    plotters/HPGL_plotter.cpp
    plotters/PDF_plotter.cpp
    plotters/PS_plotter.cpp
    plotters/SVG_plotter.cpp
    plotters/common_plot_functions.cpp
    plotters/plotter.cpp

    # Preview items
    preview_items/anchor_debug.cpp
    preview_items/construction_geom.cpp
    preview_items/item_drawing_utils.cpp
    preview_items/preview_utils.cpp
    preview_items/ruler_item.cpp
    preview_items/selection_area.cpp
    preview_items/simple_overlay_item.cpp
    preview_items/snap_indicator.cpp

    # Project files
    project/board_project_settings.cpp
    project/net_settings.cpp
    project/project_archiver.cpp
    project/project_file.cpp
    project/project_local_settings.cpp
    
    # Properties
    properties/property_mgr.cpp
    
    # Settings files
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
    
    # Tool files
    tool/action_manager.cpp
    tool/action_menu.cpp
    tool/action_toolbar.cpp
    tool/actions.cpp
    tool/common_control.cpp
    tool/common_tools.cpp
    tool/conditional_menu.cpp
    tool/construction_manager.cpp
    tool/editor_conditions.cpp
    tool/grid_helper.cpp
    tool/grid_menu.cpp
    tool/selection.cpp
    tool/selection_conditions.cpp
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

    # View files
    view/view.cpp
    view/view_controls.cpp
    view/view_group.cpp
    view/view_item.cpp
    view/view_overlay.cpp
    # UI component - commented out for minimal build
    # view/wx_view_controls.cpp
    view/zoom_controller.cpp
    

    
    # Generated files
    ${COMMON_GENERATED_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../generated/common/template_fieldnames_keywords.cpp
)

target_include_directories( common PUBLIC
    .
    ./dialogs
    ../include
    # ../generated/api/cpp  # DISABLED FOR MINIMAL BUILD
    # ../generated/api/cpp/api  # DISABLED FOR MINIMAL BUILD
    ../scripting
    ../thirdparty/magic_enum
    ../thirdparty/nanosvg
    ${wxWidgets_INCLUDE_DIRS}
)

target_compile_definitions( common PUBLIC
    ${wxWidgets_DEFINITIONS}
    WXSTATIC
    wxUSE_DLL=0
    KICOMMON_DLL
    GAL_DLL  # Define GAL_DLL to export GAL symbols instead of importing them
)

# Qt MOC support for future transformation
if( Qt6_FOUND )
    # Headers that will need MOC processing when migrated to Qt
    set( MOC_HEADERS
        ../include/rc_item.h
        ../include/widgets/wx_splash.h
        ../include/widgets/std_bitmap_button.h
        ../include/widgets/bitmap_button.h
        ../include/widgets/html_window.h
    )

    # Generate MOC files for headers with Q_OBJECT
    qt6_wrap_cpp( MOC_SOURCES ${MOC_HEADERS} )

    # Add MOC sources to common library
    target_sources( common PRIVATE ${MOC_SOURCES} )

    # Configure Qt MOC, UIC, and RCC processing
    set_target_properties( common PROPERTIES AUTOMOC ON )
    set_target_properties( common PROPERTIES AUTORCC ON )
    set_target_properties( common PROPERTIES AUTOUIC OFF )
endif()

target_link_libraries( common PUBLIC
    core
    kimath
    kiplatform
    sexpr
    nlohmann_json_schema_validator
    ${wxWidgets_LIBRARIES}
    ${Boost_LIBRARIES}
    # Third-party libraries (local)
    fmt
    libcontext
    clipper2
    delaunator
    othermath
    potrace
    # Third-party libraries from vcpkg
    Fontconfig::Fontconfig
    CURL::libcurl
    zstd::libzstd
    Freetype::Freetype
    harfbuzz::harfbuzz
    # protobuf::libprotobuf

    # Qt libraries (for future transformation)
    $<$<BOOL:${Qt6_FOUND}>:Qt6::Core>
    $<$<BOOL:${Qt6_FOUND}>:Qt6::Widgets>
    $<$<BOOL:${Qt6_FOUND}>:Qt6::Gui>
    $<$<BOOL:${Qt6_FOUND}>:Qt6::Network>
)
