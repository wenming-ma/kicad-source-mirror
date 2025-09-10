#!/usr/bin/env python3
"""
Test the actual difference between with and without '- S'
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

def run_algorithm(with_minus_S=True, max_iter=20):
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
    
    print(f"{'With' if with_minus_S else 'Without'} '- S': Initial seeds: {len(S)}")
    
    iteration = 0
    changed = True
    problematic_symbols = []
    
    while changed and iteration < max_iter:
        iteration += 1
        changed = False
        provided = set()
        required = set()
        
        for s in S:
            d, u = per.get(s, (set(), set()))
            provided |= d
            required |= u
        
        missing = {m for m in (required - provided) if not is_external(m)}
        
        print(f"  Iteration {iteration}: |S|={len(S)}, missing={len(missing)}")
        
        added_this_round = 0
        for m in sorted(missing):
            if with_minus_S:
                cands = providers.get(m, set()) - S
            else:
                cands = providers.get(m, set())
            
            if cands:
                chosen = sorted(cands)[0]
                old_len = len(S)
                S.add(chosen)
                new_len = len(S)
                
                if new_len == old_len and not with_minus_S:
                    # This is the problematic case!
                    problematic_symbols.append(m)
                
                added_this_round += 1
                changed = True
    
    print(f"  Final: {len(S)} files")
    
    if problematic_symbols:
        print(f"  Problematic symbols (tried to add existing file): {len(problematic_symbols)}")
        for sym in problematic_symbols[:3]:
            print(f"    - {sym[:80]}")
    
    return S

print("Running comparison...")
S_with = run_algorithm(with_minus_S=True)
print()
S_without = run_algorithm(with_minus_S=False)

print(f"\nDifference: {len(S_with)} vs {len(S_without)} files")
print(f"Missing files without '- S': {len(S_with - S_without)}")