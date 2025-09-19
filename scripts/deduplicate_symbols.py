#!/usr/bin/env python3
"""
Deduplicate unused_symbols.txt file
Each line is treated as a unique object
"""

import os

def deduplicate_symbols():
    input_file = "unused_symbols.txt"
    output_file = "unused_symbols_dedup.txt"

    # Read all lines
    with open(input_file, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    # Remove duplicates while preserving order
    seen = set()
    unique_lines = []

    for line in lines:
        line = line.strip()
        if line and line not in seen:
            seen.add(line)
            unique_lines.append(line)

    # Write deduplicated lines
    with open(output_file, 'w', encoding='utf-8') as f:
        for line in unique_lines:
            f.write(line + '\n')

    # Print statistics
    print(f"Original lines: {len(lines)}")
    print(f"Unique lines: {len(unique_lines)}")
    print(f"Duplicates removed: {len(lines) - len(unique_lines)}")
    print(f"Output saved to: {output_file}")

    # Also update the original file with deduplicated content
    with open(input_file, 'w', encoding='utf-8') as f:
        for line in unique_lines:
            f.write(line + '\n')
    print(f"Original file updated: {input_file}")

if __name__ == "__main__":
    deduplicate_symbols()
