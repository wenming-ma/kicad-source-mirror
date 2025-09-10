#!/usr/bin/env python3
import json
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
INDEX = ROOT / r"scripts\tu_index.json"
MINSR = ROOT / r"scripts\tem\minset_sources.json"
OUT = ROOT / r"scripts\tem\minset_headers.json"


def is_project_header(header_path, project_root):
    """Check if header is part of the KiCad project (not system or third-party)"""
    header = Path(header_path).resolve()
    project = project_root.resolve()

    try:
        # Check if header is within project directory
        header.relative_to(project)

        # Filter out vcpkg_installed directory
        if "vcpkg_installed" in header.parts:
            return False

        return True
    except ValueError:
        # Header is outside project directory - it's a system/external header
        return False


def main():
    idx = {
        it["src"]: it for it in json.loads(INDEX.read_text(encoding="utf-8"))["items"]
    }
    srcs = json.loads(MINSR.read_text(encoding="utf-8"))["sources"]
    all_headers = set()
    filtered_headers = set()

    for s in srcs:
        it = idx.get(s)
        if it:
            for h in it.get("headers", []):
                all_headers.add(h)
                if is_project_header(h, ROOT):
                    filtered_headers.add(h)

    # Ensure output directory exists
    OUT.parent.mkdir(parents=True, exist_ok=True)
    
    OUT.write_text(
        json.dumps({"headers": sorted(filtered_headers)}, indent=2), encoding="utf-8"
    )
    print(f"Total headers found: {len(all_headers)}")
    print(f"Project headers after filtering: {len(filtered_headers)}")
    print(
        f"Filtered out: {len(all_headers) - len(filtered_headers)} system/third-party headers"
    )
    print(f"Wrote {OUT} with {len(filtered_headers)} headers")


if __name__ == "__main__":
    main()
