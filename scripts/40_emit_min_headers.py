#!/usr/bin/env python3
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / r"build"
INDEX = BUILD / "tu_index.json"
MINSR = BUILD / "minset_sources.json"
OUT = BUILD / "minset_headers.json"


def main():
    idx = {
        it["src"]: it for it in json.loads(INDEX.read_text(encoding="utf-8"))["items"]
    }
    srcs = json.loads(MINSR.read_text(encoding="utf-8"))["sources"]
    headers = set()
    for s in srcs:
        it = idx.get(s)
        if it:
            for h in it.get("headers", []):
                headers.add(h)
    OUT.write_text(json.dumps({"headers": sorted(headers)}, indent=2), encoding="utf-8")
    print(f"Wrote {OUT} with {len(headers)} headers")


if __name__ == "__main__":
    main()
