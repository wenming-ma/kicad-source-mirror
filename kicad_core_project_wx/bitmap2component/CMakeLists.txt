# CMakeLists.txt for bitmap2component - simplified version

set( BITMAP2COMPONENT_SRCS
    bitmap2cmp_control.cpp
    bitmap2cmp_frame.cpp
    bitmap2cmp_main.cpp
    bitmap2cmp_panel.cpp
    bitmap2cmp_panel_base.cpp
    bitmap2cmp_settings.cpp
    bitmap2component.cpp
)

add_library( bitmap2component_lib STATIC
    ${BITMAP2COMPONENT_SRCS}
)

target_include_directories( bitmap2component_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libs/core/include
    ${CMAKE_SOURCE_DIR}/libs/kimath/include
    ${CMAKE_SOURCE_DIR}/libs/kiplatform/include
    ${CMAKE_SOURCE_DIR}/thirdparty
)