# Simplified Gerbview Navigation Library for minimum set
add_library(gerbview_navlib STATIC
    "nl_gerbview_plugin.cpp"
    "nl_gerbview_plugin_impl.cpp"
)

# gerbview_navlib depends on make_lexer outputs in common
add_dependencies( gerbview_navlib common )

# Include directories
target_include_directories(gerbview_navlib PRIVATE
    ../../common
    ../../include
    ../../thirdparty/3dxware_sdk/inc
    )

target_link_libraries(gerbview_navlib
    kiplatform
)