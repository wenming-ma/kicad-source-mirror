include_directories( BEFORE ${INC_BEFORE} )

include_directories(
    ./
    ../
    ../../include
    ../../3d-viewer
    ../../pcbnew
    ../../libs/kimath/include
    ../../libs/core/include
    ../../thirdparty/clipper2/Clipper2Lib/include
    ../../thirdparty/nlohmann_json
    ../../thirdparty/fmt/include
    ../../generated
    ${INC_AFTER}
)

# Router core source files from minset
set( PCBNEW_PNS_SRCS
    # Core algorithm files
    pns_router.cpp
    pns_line_placer.cpp
    pns_shove.cpp
    pns_node.cpp
    pns_line.cpp
    pns_via.cpp
    pns_item.cpp
    pns_optimizer.cpp
    pns_walkaround.cpp
    
    # Differential pair routing
    pns_diff_pair.cpp
    pns_diff_pair_placer.cpp
    
    # Meander/tuning algorithms
    pns_meander.cpp
    pns_meander_placer.cpp
    pns_meander_placer_base.cpp
    pns_dp_meander_placer.cpp
    pns_meander_skew_placer.cpp
    
    # Dragging algorithms
    pns_dragger.cpp
    pns_component_dragger.cpp
    pns_multi_dragger.cpp
    
    # Data structures and utilities
    pns_topology.cpp
    pns_solid.cpp
    pns_arc.cpp
    pns_hole.cpp
    pns_index.cpp
    pns_itemset.cpp
    pns_algo_base.cpp
    pns_routing_settings.cpp
    pns_sizes_settings.cpp
    pns_utils.cpp
    pns_logger.cpp
    pns_mouse_trail_tracer.cpp
    time_limit.cpp
)

add_library( pnsrouter STATIC ${PCBNEW_PNS_SRCS} )

target_link_libraries( pnsrouter PRIVATE
    common
    kimath
    core
)
