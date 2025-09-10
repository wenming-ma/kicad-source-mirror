#!/usr/bin/env python3
"""
Check if duplicate symbols actually cause problems in the algorithm
"""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
INDEX = ROOT / r"scripts\tu_index.json"

# Load the index
idx_all = json.loads(INDEX.read_text(encoding="utf-8"))["items"]

# Build mappings
per = {}
providers = {}

for it in idx_all:
    abs_path = Path(it["src"])
    per[abs_path] = (set(it["defined"]), set(it["undefined"]))
    
    for s in it["defined"]:
        providers.setdefault(s, set()).add(abs_path)

# Find symbols with most providers
multi_providers = [(sym, len(files)) for sym, files in providers.items() if len(files) > 1]
multi_providers.sort(key=lambda x: x[1], reverse=True)

print("Top symbols with multiple providers:")
for sym, count in multi_providers[:5]:
    print(f"  '{sym[:60]}...' - {count} files")

# Check a specific case: what happens with these widely duplicated symbols?
test_symbol = multi_providers[0][0]  # Most duplicated symbol
print(f"\nAnalyzing symbol: '{test_symbol[:80]}...'")
print(f"Provided by {len(providers[test_symbol])} files")

# Check which files require this symbol
requiring_files = []
for path, (defined, undefined) in per.items():
    if test_symbol in undefined:
        requiring_files.append(path)

print(f"Required by {len(requiring_files)} files")

# The key insight: template/inline symbols are typically NOT in undefined lists
# because they are instantiated locally in each compilation unit

# Let's verify this
template_symbols = [sym for sym in providers.keys() if '?$' in sym or '@@' in sym][:100]
symbols_actually_required = []

for sym in template_symbols:
    required_count = 0
    for path, (defined, undefined) in per.items():
        if sym in undefined:
            required_count += 1
    if required_count > 0:
        symbols_actually_required.append((sym, required_count))

print(f"\nTemplate/inline symbol analysis:")
print(f"Checked {len(template_symbols)} template symbols")
print(f"Actually required as undefined: {len(symbols_actually_required)}")

if symbols_actually_required:
    print("Examples of template symbols that ARE required:")
    for sym, count in symbols_actually_required[:5]:
        print(f"  '{sym[:60]}...' required by {count} files")