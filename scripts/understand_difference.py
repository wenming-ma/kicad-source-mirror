#!/usr/bin/env python3
"""
Understand why the two algorithms diverge
"""

# Simplified example to show the difference
print("=== Example Scenario ===\n")

# Files and their symbols
files = {
    'seed.cpp': {'defined': {'init'}, 'undefined': {'A', 'B'}},
    'file1.cpp': {'defined': {'A', 'X'}, 'undefined': {'C'}},
    'file2.cpp': {'defined': {'B', 'X'}, 'undefined': {'D'}},  # Also provides X!
    'file3.cpp': {'defined': {'C', 'Y'}, 'undefined': {'E'}},
    'file4.cpp': {'defined': {'D', 'Y'}, 'undefined': {'F'}},  # Also provides Y!
    'file5.cpp': {'defined': {'E'}, 'undefined': set()},
    'file6.cpp': {'defined': {'F'}, 'undefined': set()},
}

# Build providers map
providers = {}
for file, info in files.items():
    for sym in info['defined']:
        providers.setdefault(sym, set()).add(file)

print("Providers:")
for sym, files_set in sorted(providers.items()):
    print(f"  {sym}: {sorted(files_set)}")

def run_original(seed):
    """Algorithm with - S"""
    S = {seed}
    iterations = []
    
    for i in range(5):
        provided = set()
        required = set()
        
        for f in S:
            provided |= files[f]['defined']
            required |= files[f]['undefined']
        
        missing = required - provided
        if not missing:
            break
        
        added = []
        for m in sorted(missing):
            cands = providers.get(m, set()) - S
            if cands:
                chosen = sorted(cands)[0]
                S.add(chosen)
                added.append(f"{m}->{chosen}")
        
        iterations.append({'missing': sorted(missing), 'added': added, 'S': sorted(S)})
    
    return S, iterations

def run_intersection(seed):
    """Algorithm with intersection check"""
    S = {seed}
    iterations = []
    
    for i in range(5):
        provided = set()
        required = set()
        
        for f in S:
            provided |= files[f]['defined']
            required |= files[f]['undefined']
        
        missing = required - provided
        if not missing:
            break
        
        added = []
        for m in sorted(missing):
            providers_for_m = providers.get(m, set())
            # Key difference: skip if ANY file in S provides m
            # But m is in missing, so this should never happen!
            if not providers_for_m.intersection(S):
                if providers_for_m:
                    chosen = sorted(providers_for_m)[0]
                    S.add(chosen)
                    added.append(f"{m}->{chosen}")
        
        iterations.append({'missing': sorted(missing), 'added': added, 'S': sorted(S)})
    
    return S, iterations

print("\n=== Original Algorithm (with - S) ===")
S_orig, iters_orig = run_original('seed.cpp')
for i, it in enumerate(iters_orig):
    print(f"Iter {i+1}: missing={it['missing']}, add={it['added']}")
print(f"Final: {sorted(S_orig)}")

print("\n=== Intersection Algorithm ===")
S_inter, iters_inter = run_intersection('seed.cpp')
for i, it in enumerate(iters_inter):
    print(f"Iter {i+1}: missing={it['missing']}, add={it['added']}")
print(f"Final: {sorted(S_inter)}")

print("\n=== Analysis ===")
if S_orig == S_inter:
    print("Both algorithms produce the same result (as expected)")
    print("The intersection check should never trigger because:")
    print("  missing = required - provided")
    print("  So symbols in 'missing' are NOT provided by S")
else:
    print(f"Different results! Original: {len(S_orig)} files, Intersection: {len(S_inter)} files")
    print(f"Difference: {S_orig - S_inter}")