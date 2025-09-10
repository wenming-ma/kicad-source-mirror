#!/usr/bin/env python3
"""
Analyze the 233 symbols that are both defined multiple times AND required by other files
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

# Find symbols that are both undefined somewhere AND have multiple providers
both_undefined_and_multi_provided = []
for sym in undefined_symbols:
    if sym in providers and len(providers[sym]) > 1:
        both_undefined_and_multi_provided.append(
            (sym, providers[sym], undefined_symbols[sym])
        )

print(f"Total problematic symbols: {len(both_undefined_and_multi_provided)}\n")

# Categorize these symbols
categories = {
    'Constructors (??0)': [],
    'Destructors (??1)': [],
    'Virtual destructors (??_E)': [],
    'Copy constructors': [],
    'Assignment operators (??4)': [],
    'Other MSVC symbols': [],
    'Regular C functions': [],
    'Other': []
}

for sym, prov_files, req_files in both_undefined_and_multi_provided:
    prov_count = len(prov_files)
    req_count = len(req_files)
    
    if sym.startswith('??0'):
        # Check if it's a copy constructor
        if 'AEBV' in sym or 'AEBU' in sym:  # const reference parameter
            categories['Copy constructors'].append((sym, prov_count, req_count))
        else:
            categories['Constructors (??0)'].append((sym, prov_count, req_count))
    elif sym.startswith('??1'):
        categories['Destructors (??1)'].append((sym, prov_count, req_count))
    elif sym.startswith('??_E'):
        categories['Virtual destructors (??_E)'].append((sym, prov_count, req_count))
    elif sym.startswith('??4'):
        categories['Assignment operators (??4)'].append((sym, prov_count, req_count))
    elif sym.startswith('?'):
        categories['Other MSVC symbols'].append((sym, prov_count, req_count))
    elif not sym.startswith('_'):
        categories['Regular C functions'].append((sym, prov_count, req_count))
    else:
        categories['Other'].append((sym, prov_count, req_count))

# Print analysis
for cat_name, symbols in categories.items():
    if symbols:
        print(f"{cat_name}: {len(symbols)} symbols")
        
        # Sort by provider count
        symbols.sort(key=lambda x: x[1], reverse=True)
        
        # Show examples
        for sym, prov_count, req_count in symbols[:3]:
            # Extract class name if possible
            class_name = ""
            if '@@' in sym:
                parts = sym.split('@@')
                if len(parts) > 1:
                    class_name = parts[0].replace('??0', '').replace('??1', '').replace('??_E', '').replace('??4', '')
            
            display_sym = f"{class_name}" if class_name else sym[:60]
            print(f"  {display_sym}: {prov_count} providers, {req_count} requirers")
        print()

# Analyze specific patterns
print("\n=== Pattern Analysis ===")

# Check if these are from specific classes
class_names = {}
for sym, _, _ in both_undefined_and_multi_provided:
    if '@@' in sym and sym.startswith('??'):
        parts = sym.split('@@')
        if len(parts) > 1:
            # Extract class name
            class_name = parts[0][3:]  # Skip ??0, ??1, etc.
            if class_name:
                class_names[class_name] = class_names.get(class_name, 0) + 1

print("Classes with multiple problematic symbols:")
sorted_classes = sorted(class_names.items(), key=lambda x: x[1], reverse=True)
for class_name, count in sorted_classes[:10]:
    print(f"  {class_name}: {count} symbols")

# Check specific files that provide these symbols
print("\n=== Provider File Analysis ===")
provider_file_counts = {}
for sym, prov_files, _ in both_undefined_and_multi_provided:
    for f in prov_files:
        provider_file_counts[f] = provider_file_counts.get(f, 0) + 1

# Find files that provide many of these problematic symbols
sorted_providers = sorted(provider_file_counts.items(), key=lambda x: x[1], reverse=True)
print(f"Files providing the most problematic symbols:")
for file, count in sorted_providers[:5]:
    file_name = Path(file).name
    print(f"  {file_name}: {count} symbols")