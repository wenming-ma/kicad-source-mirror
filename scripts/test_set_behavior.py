#!/usr/bin/env python3
# Test set.add() behavior when adding existing elements

S = {1, 2, 3}
print(f"Initial S: {S}")

# Test 1: Add new element
S.add(4)
print(f"After adding 4: {S}")

# Test 2: Add existing element
initial_len = len(S)
S.add(2)  # 2 already exists
final_len = len(S)
print(f"After adding existing 2: {S}")
print(f"Length changed: {initial_len} -> {final_len}")

# Test 3: Simulating the bug scenario
S = {1, 2, 3}
added_count = 0
for item in [2, 4, 3, 5]:  # Mix of existing and new
    S.add(item)
    added_count += 1  # Bug: counts even when not actually added
    
print(f"\nSimulated bug scenario:")
print(f"Final S: {S}")
print(f"added_count: {added_count} (should be 2, but counted 4)")