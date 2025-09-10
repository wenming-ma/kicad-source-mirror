#!/usr/bin/env python3
"""
Debug why the intersection check doesn't work as expected
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

# Load index
idx_all = json.loads(INDEX.read_text(encoding="utf-8"))["items"]

# Build mappings
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
S = set()
for seed in seeds:
    for abs_path in per.keys():
        if abs_path.match(f"*/{seed}") or abs_path.match(f"*\\{seed}"):
            S.add(abs_path)
            break
        if len(seed.parts) <= len(abs_path.parts):
            if abs_path.parts[-len(seed.parts):] == seed.parts:
                S.add(abs_path)
                break

print(f"Initial seed files: {len(S)}")

# Run first iteration to debug
provided = set()
required = set()

for s in S:
    d, u = per.get(s, (set(), set()))
    provided |= d
    required |= u

missing = {m for m in (required - provided) if not is_external(m)}

print(f"\nFirst iteration analysis:")
print(f"  Provided symbols: {len(provided)}")
print(f"  Required symbols: {len(required)}")
print(f"  Missing symbols: {len(missing)}")

# Check for the anomaly: symbols in missing that have providers in S
anomalies = []
for m in missing:
    providers_for_m = providers.get(m, set())
    intersection = providers_for_m.intersection(S)
    if intersection:
        anomalies.append((m, providers_for_m, intersection))

if anomalies:
    print(f"\n!!! FOUND {len(anomalies)} ANOMALIES !!!")
    print("Symbols in 'missing' that have providers in S:")
    for sym, all_providers, in_S in anomalies[:5]:
        print(f"\n  Symbol: {sym[:80]}")
        print(f"    All providers: {len(all_providers)} files")
        print(f"    Providers in S: {len(in_S)} files")
        for f in list(in_S)[:2]:
            print(f"      - {Path(f).name}")
else:
    print("\nNo anomalies found (as expected)")
    print("All symbols in 'missing' have no providers in S")

# Double-check: are these symbols really defined by files in S?
if anomalies:
    print("\n=== Double-checking anomalies ===")
    for sym, _, in_S in anomalies[:2]:
        print(f"\nSymbol: {sym[:60]}")
        for file in list(in_S)[:1]:
            defined, undefined = per[file]
            if sym in defined:
                print(f"  CONFIRMED: {Path(file).name} defines this symbol")
            else:
                print(f"  ERROR: {Path(file).name} does NOT define this symbol!")
                print(f"  This indicates index corruption!")