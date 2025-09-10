#!/usr/bin/env python3
import json
from pathlib import Path
from collections import Counter

ROOT = Path(__file__).resolve().parents[1]
INDEX = ROOT / r"scripts\tu_index.json"

# Load the index
idx_all = json.loads(INDEX.read_text(encoding="utf-8"))["items"]

# Count symbol occurrences across files
symbol_files = {}
for it in idx_all:
    src = it["src"]
    for sym in it["defined"]:
        symbol_files.setdefault(sym, []).append(src)

# Find symbols defined in multiple files
duplicates = {sym: files for sym, files in symbol_files.items() if len(files) > 1}

print(f"Total symbols: {len(symbol_files)}")
print(f"Symbols with duplicates: {len(duplicates)}")
print(f"Percentage of duplicates: {len(duplicates)/len(symbol_files)*100:.1f}%")

# Categorize duplicates by pattern
categories = {
    'MSVC Template/Inline': [],
    'MSVC RTTI/VTable': [],
    'MSVC Static Init': [],
    'Standard Library': [],
    'Main Function': [],
    'Regular Functions': [],
    'Other': []
}

for sym, files in duplicates.items():
    count = len(files)
    
    # MSVC template instantiations
    if '?$' in sym or '@@QEAA' in sym or '@@UEAA' in sym or '@@QEBA' in sym:
        categories['MSVC Template/Inline'].append((sym, count))
    # MSVC RTTI/VTable
    elif sym.startswith('??_7') or sym.startswith('??_R'):
        categories['MSVC RTTI/VTable'].append((sym, count))
    # MSVC static initialization
    elif '?$TSS' in sym or '??__E' in sym or '??__F' in sym:
        categories['MSVC Static Init'].append((sym, count))
    # Standard library functions
    elif sym in ['printf', 'fprintf', 'sprintf', 'malloc', 'free', 'memcpy', 'strlen', 
                 '__local_stdio_printf_options', '_vfprintf_l', '_vsnprintf_l']:
        categories['Standard Library'].append((sym, count))
    # Main function
    elif sym == 'main':
        categories['Main Function'].append((sym, count))
    # Regular C/C++ functions
    elif not sym.startswith('?') and not sym.startswith('_'):
        categories['Regular Functions'].append((sym, count))
    else:
        categories['Other'].append((sym, count))

# Print statistics
print("\n=== Duplicate Symbol Categories ===")
for cat, symbols in categories.items():
    if symbols:
        total = len(symbols)
        max_dup = max(s[1] for s in symbols) if symbols else 0
        avg_dup = sum(s[1] for s in symbols) / len(symbols) if symbols else 0
        print(f"\n{cat}: {total} symbols")
        print(f"  Max duplicates: {max_dup} files")
        print(f"  Avg duplicates: {avg_dup:.1f} files")
        
        # Show top 3 examples
        sorted_syms = sorted(symbols, key=lambda x: x[1], reverse=True)[:3]
        for sym, count in sorted_syms:
            # Truncate long symbol names
            display_sym = sym if len(sym) <= 80 else sym[:77] + "..."
            print(f"  Example: '{display_sym}' ({count} files)")

# Find the most duplicated symbols
print("\n=== Top 10 Most Duplicated Symbols ===")
sorted_dups = sorted(duplicates.items(), key=lambda x: len(x[1]), reverse=True)[:10]
for sym, files in sorted_dups:
    display_sym = sym if len(sym) <= 60 else sym[:57] + "..."
    print(f"{len(files):3d} files: {display_sym}")

# Analyze file distribution
print("\n=== File Distribution of Duplicates ===")
dup_counts = Counter(len(files) for files in duplicates.values())
for count in sorted(dup_counts.keys())[:10]:
    num_symbols = dup_counts[count]
    print(f"  {num_symbols:6d} symbols appear in exactly {count} files")