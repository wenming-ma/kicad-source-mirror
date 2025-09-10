#!/usr/bin/env python3
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
INDEX = ROOT / r"scripts\tu_index.json"

# Load the index
idx_all = json.loads(INDEX.read_text(encoding="utf-8"))["items"]

# Build providers mapping
providers = {}
symbol_types = {
    'template': [],
    'inline': [],
    'weak': [],
    'normal': []
}

for it in idx_all:
    abs_path = Path(it["src"])
    for s in it["defined"]:
        providers.setdefault(s, set()).add(abs_path)

# Categorize symbols with multiple providers
for sym, files in providers.items():
    if len(files) > 1:
        # MSVC template/inline patterns
        if '?$' in sym or '@@' in sym:  # Template instantiation
            symbol_types['template'].append((sym, len(files)))
        elif '__inline' in sym or 'inline@' in sym:  # Inline functions
            symbol_types['inline'].append((sym, len(files)))
        elif '??_' in sym:  # Weak symbols (vtables, RTTI)
            symbol_types['weak'].append((sym, len(files)))
        else:
            symbol_types['normal'].append((sym, len(files)))

print("Symbol duplication analysis:")
print(f"Templates: {len(symbol_types['template'])} symbols")
print(f"Inline functions: {len(symbol_types['inline'])} symbols")
print(f"Weak symbols: {len(symbol_types['weak'])} symbols")
print(f"Normal symbols: {len(symbol_types['normal'])} symbols")

# Show examples of normal duplicated symbols
print("\nExamples of normal (non-template) duplicated symbols:")
for sym, count in symbol_types['normal'][:10]:
    print(f"  '{sym}' - {count} files")