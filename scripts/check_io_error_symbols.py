#!/usr/bin/env python3
"""
Check why IO_ERROR and similar classes have symbols in multiple files
"""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
INDEX = ROOT / r"scripts\tu_index.json"

# Load the index
idx_all = json.loads(INDEX.read_text(encoding="utf-8"))["items"]

# Focus on IO_ERROR symbols
io_error_symbols = [
    '??0IO_ERROR@@QEAA@AEBV0@@Z',  # Copy constructor
    '??1IO_ERROR@@UEAA@XZ',         # Destructor
    '??_EIO_ERROR@@UEAAPEAXI@Z'     # Virtual destructor
]

print("Analyzing IO_ERROR symbols across files:\n")

for sym in io_error_symbols:
    print(f"Symbol: {sym}")
    
    # Decode MSVC symbol
    if '??0' in sym:
        print("  Type: Copy Constructor")
    elif '??1' in sym:
        print("  Type: Destructor")
    elif '??_E' in sym:
        print("  Type: Virtual Destructor (scalar deleting)")
    
    providers = []
    requirers = []
    
    for it in idx_all:
        src = Path(it["src"])
        if sym in it["defined"]:
            providers.append(src)
        if sym in it["undefined"]:
            requirers.append(src)
    
    print(f"  Provided by {len(providers)} files")
    print(f"  Required by {len(requirers)} files")
    
    if providers:
        # Group by directory to see pattern
        dirs = {}
        for p in providers:
            dir_name = p.parent.name
            dirs[dir_name] = dirs.get(dir_name, 0) + 1
        
        print("  Provider directories:")
        for dir_name, count in sorted(dirs.items(), key=lambda x: x[1], reverse=True)[:5]:
            print(f"    {dir_name}: {count} files")
    
    print()

# Check if these are from PCH or template instantiation
print("\n=== Checking specific provider files ===")

# Find files that provide IO_ERROR symbols
io_error_providers = set()
for it in idx_all:
    for sym in io_error_symbols:
        if sym in it["defined"]:
            io_error_providers.add(Path(it["src"]))
            break

# Sample a few files
sample_files = sorted(io_error_providers)[:3]
print(f"Sample files providing IO_ERROR symbols:")
for f in sample_files:
    print(f"  {f.name}")

# Check if these might be from a header file included everywhere
print("\n=== Hypothesis: Inline/header definitions ===")
print("These symbols might be defined in a header file that's included")
print("in many compilation units. In MSVC, this can happen when:")
print("1. Virtual functions are defined inline in class declaration")
print("2. Compiler generates them implicitly (Rule of Five)")
print("3. They're in a PCH (precompiled header)")
print("4. They're explicitly instantiated in multiple places")