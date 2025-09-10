#!/usr/bin/env python3
"""
Verify the real issue with duplicate symbols
"""

# The REAL scenario that causes problems
print("=== The REAL Problem Scenario ===\n")

# Initial state
S = {'file1.cpp', 'file2.cpp'}
provided = {'foo', 'bar'}  # Symbols provided by S
required = {'foo', 'bar', 'baz', 'qux'}  # Symbols needed by S
missing = required - provided  # {'baz', 'qux'}

print(f"S = {S}")
print(f"provided = {provided}")
print(f"required = {required}")
print(f"missing = {missing}\n")

# Providers for missing symbols
providers = {
    'baz': {'file3.cpp', 'file4.cpp'},
    'qux': {'file2.cpp', 'file5.cpp'}  # NOTE: file2.cpp is already in S!
}

print("Providers for missing symbols:")
for sym, files in providers.items():
    print(f"  {sym}: {files}")

print("\n=== Without '- S' ===")
for m in sorted(missing):
    cands = providers.get(m, set())  # No filtering
    if cands:
        chosen = sorted(cands)[0]
        print(f"For '{m}': candidates = {cands}")
        print(f"  Choose: {chosen}")
        if chosen in S:
            print(f"  WARNING: {chosen} already in S! No actual progress!")
        S.add(chosen)

print(f"Final S: {S}")

print("\n=== With '- S' ===")
S = {'file1.cpp', 'file2.cpp'}  # Reset
for m in sorted(missing):
    cands = providers.get(m, set()) - S  # Filter out files already in S
    if cands:
        chosen = sorted(cands)[0]
        print(f"For '{m}': candidates after filtering = {cands}")
        print(f"  Choose: {chosen}")
        S.add(chosen)
    else:
        print(f"For '{m}': No candidates after filtering!")

print(f"Final S: {S}")

print("\n=== WAIT, THIS SHOULDN'T HAPPEN! ===")
print("If 'qux' is in missing, it means it's NOT provided by S")
print("So how can file2.cpp (which is in S) be a provider for 'qux'?")
print("\nThis indicates a BUG in the symbol index or algorithm!")
print("Possible causes:")
print("1. Symbol index is stale/incorrect")
print("2. Symbol comparison has issues (case sensitivity, encoding)")
print("3. External symbol filtering is incorrect")