#!/usr/bin/env python3
"""
Simulate the real bug scenario where algorithm fails without '- S'
"""

# More complex dependency graph with multiple providers
deps = {
    'seed': ({'init'}, {'sym1', 'sym2'}),
    'A': ({'sym1', 'common_template'}, {'sym3'}),
    'B': ({'sym1', 'sym2', 'common_template'}, {'sym4'}),  # Also provides sym1!
    'C': ({'sym3', 'common_template'}, {'sym5'}),
    'D': ({'sym4'}, {'sym6'}),
    'E': ({'sym5', 'sym6'}, set()),
}

providers = {}
for file, (provides, _) in deps.items():
    for sym in provides:
        providers.setdefault(sym, set()).add(file)

print("Providers map (note sym1 has multiple providers):")
for sym, files in sorted(providers.items()):
    print(f"  {sym}: {sorted(files)}")

def run_algorithm(with_minus_S=True):
    S = {'seed'}
    iteration = 0
    trace = []
    
    while iteration < 10:
        iteration += 1
        provided = set()
        required = set()
        
        for f in S:
            p, r = deps.get(f, (set(), set()))
            provided |= p
            required |= r
        
        missing = required - provided
        if not missing:
            trace.append(f"Iteration {iteration}: Converged! S={sorted(S)}")
            break
        
        trace.append(f"Iteration {iteration}: S={sorted(S)}")
        trace.append(f"  Provided: {sorted(provided)}")
        trace.append(f"  Required: {sorted(required)}")
        trace.append(f"  Missing: {sorted(missing)}")
        
        added_count = 0
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
                
                if new_len > old_len:
                    trace.append(f"  -> Add {chosen} for '{m}'")
                else:
                    trace.append(f"  -> Try to add {chosen} for '{m}' (ALREADY IN S!)")
                
                added_count += 1  # BUG: counts even if not actually added!
        
        if added_count == 0:
            trace.append(f"  No providers found, stopping")
            break
    
    return S, trace

print("\n=== WITH '- S' (Correct) ===")
final_S, trace = run_algorithm(with_minus_S=True)
for line in trace:
    print(line)
print(f"Final set size: {len(final_S)}")

print("\n=== WITHOUT '- S' (Buggy) ===")
final_S, trace = run_algorithm(with_minus_S=False)
for line in trace:
    print(line)
print(f"Final set size: {len(final_S)}")

# The key insight: 
print("\n=== KEY INSIGHT ===")
print("Without '- S', when sym1 is needed:")
print("  - Both A and B provide sym1")
print("  - If A is chosen first and later B is also in S")
print("  - The algorithm might try to add A again for some symbol")
print("  - This creates false progress signals")