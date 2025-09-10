#!/usr/bin/env python3
"""
Find if there's a bug in the symbol index where a symbol appears 
both as defined and undefined in the same file
"""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
INDEX = ROOT / r"scripts\tu_index.json"

# Load the index
idx_all = json.loads(INDEX.read_text(encoding="utf-8"))["items"]

print("Checking for index inconsistencies...\n")

# Check 1: Symbol both defined and undefined in same file
inconsistent_files = []
for it in idx_all:
    defined = set(it["defined"])
    undefined = set(it["undefined"])
    overlap = defined & undefined
    if overlap:
        inconsistent_files.append((it["src"], overlap))

if inconsistent_files:
    print(f"ERROR: Found {len(inconsistent_files)} files with symbols both defined and undefined!")
    for src, symbols in inconsistent_files[:3]:
        print(f"  {Path(src).name}: {len(symbols)} overlapping symbols")
        for sym in list(symbols)[:2]:
            print(f"    - {sym[:60]}")
else:
    print("OK: No files have symbols both defined and undefined")

# Check 2: Duplicate entries for same source file
src_counts = {}
for it in idx_all:
    src = it["src"]
    src_counts[src] = src_counts.get(src, 0) + 1

duplicates = [(src, count) for src, count in src_counts.items() if count > 1]
if duplicates:
    print(f"\nERROR: Found {len(duplicates)} source files with duplicate entries!")
    for src, count in duplicates[:3]:
        print(f"  {Path(src).name}: {count} entries")
else:
    print("\nOK: No duplicate entries for source files")

# Check 3: Empty defined/undefined sets
empty_defined = []
empty_undefined = []
for it in idx_all:
    if not it["defined"]:
        empty_defined.append(it["src"])
    if not it["undefined"]:
        empty_undefined.append(it["src"])

print(f"\n Files with no defined symbols: {len(empty_defined)}")
print(f"Files with no undefined symbols: {len(empty_undefined)}")

print("\n=== Summary ===")
print(f"Total files in index: {len(idx_all)}")
print(f"Files with issues: {len(inconsistent_files) + len(duplicates)}")