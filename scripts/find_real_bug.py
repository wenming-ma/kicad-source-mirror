#!/usr/bin/env python3
"""
Find the real bug: why does intersection check give different results?
"""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
INDEX = ROOT / r"scripts\tu_index.json"
SEEDS = ROOT / r"scripts\seeds.txt"

def is_external(sym):
    EXTERNAL_HINTS = ["std::", "__std", "__cxx", "operator new", "operator delete",
                      "??_U", "??_V", "??2", "??3", "__imp_", "??_", "_CRT_",
                      "__acrt_", "__vcrt_", "__security_", "wx", "boost::", "Qt"]
    return any(sym.startswith(p) for p in EXTERNAL_HINTS)

# Load and build index
idx_all = json.loads(INDEX.read_text(encoding="utf-8"))["items"]
per = {}
providers = {}

for it in idx_all:
    abs_path = Path(it["src"])
    per[abs_path] = (set(it["defined"]), set(it["undefined"]))
    for s in it["defined"]:
        providers.setdefault(s, set()).add(abs_path)

# Load seeds
seeds = []
for s in SEEDS.read_text(encoding="utf-8").splitlines():
    if s.strip() and not s.strip().startswith("#"):
        seeds.append(Path(s.strip()))

# Find matching seeds
S_original = set()
S_intersection = set()
for seed in seeds:
    for abs_path in per.keys():
        if abs_path.match(f"*/{seed}") or abs_path.match(f"*\\{seed}"):
            S_original.add(abs_path)
            S_intersection.add(abs_path)
            break
        if len(seed.parts) <= len(abs_path.parts):
            if abs_path.parts[-len(seed.parts):] == seed.parts:
                S_original.add(abs_path)
                S_intersection.add(abs_path)
                break

print("Comparing algorithms on first iteration...\n")

# Compute missing for both
provided = set()
required = set()
for s in S_original:
    d, u = per.get(s, (set(), set()))
    provided |= d
    required |= u
missing = {m for m in (required - provided) if not is_external(m)}

print(f"Initial missing symbols: {len(missing)}")

# Process with original algorithm
added_original = []
for m in sorted(missing):
    cands = providers.get(m, set()) - S_original
    if cands:
        chosen = sorted(cands)[0]
        added_original.append((m, chosen))

# Process with intersection algorithm
added_intersection = []
symbols_skipped = []
for m in sorted(missing):
    providers_for_m = providers.get(m, set())
    if not providers_for_m.intersection(S_intersection):
        if providers_for_m:
            chosen = sorted(providers_for_m)[0]
            added_intersection.append((m, chosen))
    else:
        # This should never happen!
        symbols_skipped.append((m, providers_for_m.intersection(S_intersection)))

print(f"\nOriginal algorithm would add: {len(added_original)} files")
print(f"Intersection algorithm would add: {len(added_intersection)} files")
print(f"Symbols skipped by intersection: {len(symbols_skipped)}")

if symbols_skipped:
    print("\n!!! BUG FOUND !!!")
    print("Symbols that are in 'missing' but have providers in S:")
    for sym, in_S in symbols_skipped[:5]:
        print(f"  {sym}: providers in S = {len(in_S)}")

# Find differences
set_original = {f for _, f in added_original}
set_intersection = {f for _, f in added_intersection}
diff = set_original - set_intersection

if diff:
    print(f"\nFiles added by original but not intersection: {len(diff)}")
    for f in list(diff)[:5]:
        print(f"  {Path(f).name}")
else:
    print("\nBoth algorithms add the same files (as expected)")