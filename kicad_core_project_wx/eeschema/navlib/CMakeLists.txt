# Simplified navlib for minimum set - 3DxWare support disabled

add_library(eeschema_navlib STATIC
    nl_schematic_plugin.cpp
    nl_schematic_plugin_impl.cpp
)

# eeschema_navlib depends on make_lexer outputs in common
add_dependencies( eeschema_navlib common )

target_include_directories(eeschema_navlib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(eeschema_navlib
    kiplatform # Needs kiplatform to check versioning
)

# Note: 3DxWare support is disabled in minimum set
# To enable 3D mouse support, uncomment the following and ensure 3DxWare SDK is available:
# find_package(TDxWare_SDK 4.0 REQUIRED COMPONENTS 3DxWare::Navlib)
# target_compile_definitions(eeschema_navlib PRIVATE
#     $<TARGET_PROPERTY:3DxWare::Navlib,INTERFACE_COMPILE_DEFINITIONS>
# )
# target_link_libraries(eeschema_navlib
#     3DxWare::Navlib
# )