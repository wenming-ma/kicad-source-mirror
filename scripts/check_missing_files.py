#!/usr/bin/env python3
"""
Check files that are in minset_sources.json but not in kicad_core_project_wx directory
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
            missing_files.append(str(relative_path))

    # Print results
    print(f"Total files in minset_sources.json: {len(sources)}")
    print(f"Missing files in kicad_core_project_wx: {len(missing_files)}")
    print("\nMissing files:")
    print("=" * 50)

    for missing_file in sorted(missing_files):
        print(missing_file)

if __name__ == "__main__":
    main()