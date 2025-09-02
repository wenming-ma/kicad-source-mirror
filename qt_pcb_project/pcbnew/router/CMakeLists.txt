include_directories( BEFORE ${INC_BEFORE} )

include_directories(
    ./
    ../
    ../../include
    ../../3d-viewer
    ../../pcbnew
    ${INC_AFTER}
)

# Router files from minset (only pns_meander.cpp was copied)
set( PCBNEW_PNS_SRCS
    pns_meander.cpp
)

add_library( pnsrouter STATIC ${PCBNEW_PNS_SRCS} )

target_link_libraries( pnsrouter PRIVATE
    common
)
