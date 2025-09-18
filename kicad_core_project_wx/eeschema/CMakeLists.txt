# CMakeLists.txt for eeschema - simplified version

include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/common/dialogs
    ${CMAKE_SOURCE_DIR}/libs/sexpr/include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/generated/eeschema
    ${CMAKE_CURRENT_SOURCE_DIR}
    ./dialogs
    ./symbol_editor
    ./tools
    ./widgets
)

set( EESCHEMA_SCH_IO
    sch_io/kicad_sexpr/sch_io_kicad_sexpr.cpp
    sch_io/kicad_sexpr/sch_io_kicad_sexpr_common.cpp
    sch_io/kicad_sexpr/sch_io_kicad_sexpr_lib_cache.cpp
    sch_io/kicad_sexpr/sch_io_kicad_sexpr_parser.cpp
    sch_io/sch_io.cpp
    sch_io/sch_io_lib_cache.cpp
)

set( EESCHEMA_ERC_SRCS
    erc/erc.cpp
    erc/erc_item.cpp
    erc/erc_sch_pin_context.cpp
    erc/erc_settings.cpp
)

set( SYNC_SHEET_PIN_SRCS
    sync_sheet_pin/sheet_synchronization_agent.cpp
    sync_sheet_pin/sheet_synchronization_item.cpp
    sync_sheet_pin/sheet_synchronization_model.cpp
)

set( SYMBOL_EDITOR_SRCS
    symbol_editor/lib_logger.cpp
)

set( TOOLS_SRCS
    tools/sch_actions.cpp
    tools/sch_navigate_tool.cpp
)

set( EESCHEMA_SRCS
    autoplace_fields.cpp
    connection_graph.cpp
    junction_helpers.cpp
    lib_symbol.cpp
    pin_layout_cache.cpp
    pin_type.cpp
    project_sch.cpp
    sch_bitmap.cpp
    sch_bus_entry.cpp
    sch_collectors.cpp
    sch_connection.cpp
    sch_field.cpp
    sch_item.cpp
    sch_junction.cpp
    sch_label.cpp
    sch_line.cpp
    sch_marker.cpp
    sch_no_connect.cpp
    sch_pin.cpp
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
    sch_view.cpp
    schematic.cpp
    schematic_settings.cpp
    schematic_undo_redo.cpp
    symb_transforms_utils.cpp
    symbol.cpp
    symbol_async_loader.cpp
    symbol_checker.cpp
    symbol_lib_table.cpp
    symbol_library.cpp
    symbol_library_manager.cpp

    # Generated files
    ${CMAKE_SOURCE_DIR}/generated/eeschema/schematic_keywords.cpp
)

# Build static library
add_library( eeschema_lib STATIC
    ${EESCHEMA_SCH_IO}
    ${EESCHEMA_ERC_SRCS}
    ${SYNC_SHEET_PIN_SRCS}
    ${SYMBOL_EDITOR_SRCS}
    ${TOOLS_SRCS}
    ${EESCHEMA_SRCS}
)

target_include_directories( eeschema_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/generated/eeschema
    ${CMAKE_SOURCE_DIR}/libs/core/include
    ${CMAKE_SOURCE_DIR}/libs/kimath/include
    ${CMAKE_SOURCE_DIR}/libs/kiplatform/include
    ${CMAKE_SOURCE_DIR}/thirdparty
)
