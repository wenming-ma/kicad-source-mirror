#!/usr/bin/env python3
"""
Replace all references to ui_common.h with tepui_common.h
"""

import os
from pathlib import Path

files_to_update = [
    "kicad_core_project_qt/common/eda_draw_frame.cpp",
    "kicad_core_project_qt/eeschema/connection_graph.cpp",
    "kicad_core_project_qt/eeschema/eeschema_settings.cpp",
    "kicad_core_project_qt/common/lib_tree_model_adapter.cpp",
    "kicad_core_project_qt/eeschema/sch_marker.cpp",
    "kicad_core_project_qt/eeschema/erc/erc_settings.h",
    "kicad_core_project_qt/pcbnew/pcb_marker.cpp",
    "kicad_core_project_qt/include/eda_base_frame.h",
    "kicad_core_project_qt/include/board_design_settings.h",
    "kicad_core_project_qt/common/view/wx_view_controls.cpp",
    "kicad_core_project_qt/common/tool/conditional_menu.cpp",
    "kicad_core_project_qt/common/tool/action_menu.cpp",
    "kicad_core_project_qt/common/scintilla_tricks.cpp",
    "kicad_core_project_qt/common/rc_item.cpp",
    "kicad_core_project_qt/common/notifications_manager.cpp"
]

for file_path in files_to_update:
    file_path = Path(file_path)
    if file_path.exists():
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()

        # Replace ui_common.h with tepui_common.h
        new_content = content.replace('ui_common.h', 'tepui_common.h')

        if new_content != content:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(new_content)
            print(f"Updated: {file_path}")
        else:
            print(f"No changes: {file_path}")
    else:
        print(f"File not found: {file_path}")

print("\nDone!")