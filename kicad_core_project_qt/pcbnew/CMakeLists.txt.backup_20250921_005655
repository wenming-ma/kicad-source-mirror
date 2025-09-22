# PCBnew library for minimum PCB set

add_compile_definitions( PCBNEW )

# Add router subdirectory first
add_subdirectory(router)

add_dependencies( pnsrouter common )

add_library( pcbnew STATIC
    # Core PCB files
    board.cpp
    board_connected_item.cpp
    board_design_settings.cpp
    board_item.cpp
    collectors.cpp
    component_class_manager.cpp
    convert_shape_list_to_polygon.cpp
    footprint.cpp
    generators_mgr.cpp
    netinfo_item.cpp
    netinfo_list.cpp
    pad.cpp
    pad_utils.cpp
    padstack.cpp
    pcb_dimension.cpp
    pcb_field.cpp
    pcb_generator.cpp
    pcb_group.cpp
    pcb_marker.cpp
    pcb_plot_params.cpp
    pcb_reference_image.cpp
    pcb_shape.cpp
    pcb_table.cpp
    pcb_tablecell.cpp
    pcb_target.cpp
    pcb_text.cpp
    pcb_textbox.cpp
    pcb_track.cpp
    pcbexpr_evaluator.cpp
    pcbexpr_functions.cpp
    zone.cpp
    zone_settings.cpp
    
    # API files
    api/api_pcb_enums.cpp
    api/api_pcb_utils.cpp
    
    # Board stackup
    board_stackup_manager/board_stackup.cpp
    
    # Connectivity files
    connectivity/connectivity_algo.cpp
    connectivity/connectivity_data.cpp
    connectivity/connectivity_items.cpp
    connectivity/from_to_cache.cpp
    
    # DRC files
    drc/drc_cache_generator.cpp
    drc/drc_engine.cpp
    drc/drc_item.cpp
    drc/drc_rule.cpp
    drc/drc_rule_condition.cpp
    drc/drc_rule_parser.cpp
    drc/drc_test_provider.cpp
    
    # Netlist reader
    netlist_reader/pcb_netlist.cpp
    
    # PCB IO files
    pcb_io/pcb_io.cpp
    pcb_io/kicad_sexpr/pcb_io_kicad_sexpr.cpp
    pcb_io/kicad_sexpr/pcb_io_kicad_sexpr_parser.cpp
    
    # Ratsnest
    ratsnest/ratsnest_data.cpp
    
    # Teardrop
    teardrop/teardrop_parameters.cpp
    
    # Tools
    tools/pcb_actions.cpp
)

target_include_directories( pcbnew PUBLIC
    .
    ../include
    ../thirdparty/magic_enum
)

target_link_libraries( pcbnew PUBLIC
    common
    kimath
    kiplatform
    delaunator
    other_math
    pnsrouter
    ${wxWidgets_LIBRARIES}
    ${Boost_LIBRARIES}
)