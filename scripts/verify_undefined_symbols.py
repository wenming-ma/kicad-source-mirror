#!/usr/bin/env python3
"""
Verify which symbols actually appear in undefined lists
"""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
INDEX = ROOT / r"scripts\tu_index.json"

# Load the index
idx_all = json.loads(INDEX.read_text(encoding="utf-8"))["items"]

# Collect all undefined symbols and their requiring files
undefined_symbols = {}
providers = {}

for it in idx_all:
    abs_path = Path(it["src"])
    
    # Track providers
    for s in it["defined"]:
        providers.setdefault(s, set()).add(abs_path)
    
    # Track undefined
    for s in it["undefined"]:
        undefined_symbols.setdefault(s, set()).add(abs_path)

print(f"Total unique undefined symbols: {len(undefined_symbols)}")
print(f"Total unique defined symbols: {len(providers)}")

# Find symbols that are both undefined somewhere AND have multiple providers
both_undefined_and_multi_provided = []
for sym in undefined_symbols:
    if sym in providers and len(providers[sym]) > 1:
        both_undefined_and_multi_provided.append(
            (sym, len(providers[sym]), len(undefined_symbols[sym]))
        )

both_undefined_and_multi_provided.sort(key=lambda x: x[1], reverse=True)

print(f"\nSymbols that are BOTH:")
print(f"  - Required (appear in undefined) AND")
print(f"  - Have multiple providers")
print(f"Count: {len(both_undefined_and_multi_provided)}")

if both_undefined_and_multi_provided:
    print("\nTop 10 examples:")
    for sym, prov_count, req_count in both_undefined_and_multi_provided[:10]:
        display_sym = sym if len(sym) <= 60 else sym[:57] + "..."
        print(f"  '{display_sym}'")
        print(f"    Provided by: {prov_count} files")
        print(f"    Required by: {req_count} files")

# Check what percentage of undefined symbols are external
external_hints = ["std::", "__std", "wx", "boost::", "??_", "__imp_", "operator "]
external_undefined = [s for s in undefined_symbols if any(s.startswith(h) for h in external_hints)]
print(f"\nExternal undefined symbols: {len(external_undefined)} ({len(external_undefined)/len(undefined_symbols)*100:.1f}%)")