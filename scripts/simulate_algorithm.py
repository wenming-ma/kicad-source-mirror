#!/usr/bin/env python3
"""
Simulate the minimum set algorithm with and without '- S' operation
"""

# Simplified dependency graph
# Format: file -> (provides, requires)
deps = {
    'A': ({'foo', 'template_x'}, {'bar'}),
    'B': ({'bar', 'template_x'}, {'baz'}),  # template_x also in A
    'C': ({'baz', 'template_x'}, {'qux'}),  # template_x also in A,B
    'D': ({'qux'}, set()),
    'E': ({'extra'}, {'foo'}),  # requires foo from A
}

# Build providers map
providers = {}
for file, (provides, _) in deps.items():
    for sym in provides:
        providers.setdefault(sym, set()).add(file)

print("Symbol providers:")
for sym, files in sorted(providers.items()):
    print(f"  {sym}: {sorted(files)}")

def simulate_with_minus_S(seed):
    """Correct algorithm with '- S'"""
    S = {seed}
    iterations = []
    
    for i in range(10):
        provided = set()
        required = set()
        
        for f in S:
            p, r = deps[f]
            provided |= p
            required |= r
        
        missing = required - provided
        if not missing:
            break
            
        added = []
        for m in sorted(missing):
            cands = providers.get(m, set()) - S  # KEY: minus S
            if cands:
                chosen = sorted(cands)[0]
                S.add(chosen)
                added.append((m, chosen))
        
        iterations.append({'S': sorted(S), 'missing': sorted(missing), 'added': added})
    
    return S, iterations

def simulate_without_minus_S(seed):
    """Buggy algorithm without '- S'"""
    S = {seed}
    iterations = []
    
    for i in range(10):
        provided = set()
        required = set()
        
        for f in S:
            p, r = deps[f]
            provided |= p
            required |= r
        
        missing = required - provided
        if not missing:
            break
            
        added = []
        for m in sorted(missing):
            cands = providers.get(m, set())  # BUG: no minus S
            if cands:
                chosen = sorted(cands)[0]
                old_size = len(S)
                S.add(chosen)
                if len(S) > old_size:
                    added.append((m, chosen))
                else:
                    added.append((m, f"{chosen} (already in S!)"))
        
        iterations.append({'S': sorted(S), 'missing': sorted(missing), 'added': added})
    
    return S, iterations

print("\n=== With '- S' (CORRECT) ===")
result, iters = simulate_with_minus_S('E')
for i, it in enumerate(iters):
    print(f"Iteration {i+1}: S={it['S']}, missing={it['missing']}")
    for sym, file in it['added']:
        print(f"  Added {file} for symbol '{sym}'")
print(f"Final: {sorted(result)}")

print("\n=== Without '- S' (BUGGY) ===")
result, iters = simulate_without_minus_S('E')
for i, it in enumerate(iters):
    print(f"Iteration {i+1}: S={it['S']}, missing={it['missing']}")
    for sym, file in it['added']:
        print(f"  Added {file} for symbol '{sym}'")
print(f"Final: {sorted(result)}")

# Now test with a case where duplicate providers matter
print("\n=== Starting from 'C' (template_x conflict) ===")
print("With '- S':", sorted(simulate_with_minus_S('C')[0]))
print("Without '- S':", sorted(simulate_without_minus_S('C')[0]))