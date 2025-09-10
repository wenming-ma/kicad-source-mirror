#!/usr/bin/env python3
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
INDEX = ROOT / r"scripts\tu_index.json"

# Load the index
idx_all = json.loads(INDEX.read_text(encoding="utf-8"))["items"]

# Build providers mapping
providers = {}
for it in idx_all:
    abs_path = Path(it["src"])
    for s in it["defined"]:
        providers.setdefault(s, set()).add(abs_path)

# Check for symbols provided by multiple files
multi_providers = {}
for sym, files in providers.items():
    if len(files) > 1:
        multi_providers[sym] = files

print(f"Total symbols: {len(providers)}")
print(f"Symbols with multiple providers: {len(multi_providers)}")

# Show some examples
count = 0
for sym, files in sorted(multi_providers.items())[:10]:
    print(f"\nSymbol '{sym}' is provided by {len(files)} files:")
    for f in sorted(files)[:3]:
        print(f"  - {f}")
    if len(files) > 3:
        print(f"  ... and {len(files)-3} more files")
    count += 1