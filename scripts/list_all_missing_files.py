#!/usr/bin/env python3
"""
List all files that are in minset_sources.json but not in kicad_core_project_wx directory
"""

import json
import os
from pathlib import Path

def main():
    # Read minset_sources.json
    minset_json_path = "scripts/tem/minset_sources.json"
    with open(minset_json_path, 'r', encoding='utf-8') as f:
        minset_data = json.load(f)

    sources = minset_data["sources"]

    # Base paths
    base_path = Path("C:/Users/wenming_ma/source/repos/kicad-source-mirror")
    wx_project_path = base_path / "kicad_core_project_wx"

    missing_files = []

    for source_file in sources:
        # Convert absolute path to relative path
        source_path = Path(source_file)
        try:
            relative_path = source_path.relative_to(base_path)
        except ValueError:
            # Handle build directory paths
            if "build/x64-Debug" in source_file:
                # Convert build path to generated path
                build_part = source_file.split("build/x64-Debug/")[1]
                relative_path = Path("generated") / build_part
            else:
                print(f"Cannot process path: {source_file}")
                continue

        # Check if file exists in wx project
        wx_file_path = wx_project_path / relative_path

        if not wx_file_path.exists():
            missing_files.append({
                'original': source_file,
                'relative': str(relative_path),
                'target': str(wx_file_path)
            })

    # Print results
    print(f"Total files in minset_sources.json: {len(sources)}")
    print(f"Missing files in kicad_core_project_wx: {len(missing_files)}")
    print("\n完整的缺失文件列表:")
    print("=" * 80)

    # Group by category
    categories = {
        'Generated Files (build/)': [],
        'PCB IO Plugins': [],
        'Common IO Parsers': [],
        'Graphics/GAL': [],
        'Other Common': [],
        'PCB Core': []
    }

    for missing in missing_files:
        rel_path = missing['relative']
        if 'generated/' in rel_path or 'build/' in rel_path:
            categories['Generated Files (build/)'].append(missing)
        elif 'pcbnew/pcb_io/' in rel_path or 'pcbnew\\pcb_io\\' in rel_path:
            categories['PCB IO Plugins'].append(missing)
        elif 'common/io/' in rel_path or 'common\\io\\' in rel_path:
            categories['Common IO Parsers'].append(missing)
        elif 'gal/' in rel_path or 'draw_panel_gal' in rel_path:
            categories['Graphics/GAL'].append(missing)
        elif 'common/' in rel_path or 'common\\' in rel_path:
            categories['Other Common'].append(missing)
        else:
            categories['PCB Core'].append(missing)

    for category, files in categories.items():
        if files:
            print(f"\n{category} ({len(files)} files):")
            print("-" * 50)
            for file_info in sorted(files, key=lambda x: x['relative']):
                print(f"  {file_info['relative']}")
                print(f"    原始路径: {file_info['original']}")
                print(f"    目标路径: {file_info['target']}")
                print()

    # Save to file for easy reference
    output_file = "scripts/missing_files_detailed.txt"
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(f"缺失文件详细列表 (总计 {len(missing_files)} 个文件)\n")
        f.write("=" * 80 + "\n\n")

        for category, files in categories.items():
            if files:
                f.write(f"{category} ({len(files)} files):\n")
                f.write("-" * 50 + "\n")
                for file_info in sorted(files, key=lambda x: x['relative']):
                    f.write(f"  {file_info['relative']}\n")
                    f.write(f"    原始: {file_info['original']}\n")
                    f.write(f"    目标: {file_info['target']}\n\n")
                f.write("\n")

    print(f"\n详细列表已保存到: {output_file}")

if __name__ == "__main__":
    main()