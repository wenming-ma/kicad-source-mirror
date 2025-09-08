#!/usr/bin/env python3
"""
Script to analyze header file usage in qt_pcb_project
"""
import os
import re
import glob

def find_all_headers():
    """Find all header files in the project"""
    headers = []
    for root, dirs, files in os.walk('.'):
        # Skip unwanted directories
        if any(skip in root for skip in ['vcpkg_installed', 'thirdparty', '/build/', 'generated']):
            continue
        for file in files:
            if file.endswith(('.h', '.hpp')):
                path = os.path.join(root, file)
                # Normalize path
                path = path.replace('.\\', '').replace('./', '').replace('\\', '/')
                headers.append(path)
    return set(headers)

def find_included_headers():
    """Find all headers that are included"""
    includes = set()
    for root, dirs, files in os.walk('.'):
        if any(skip in root for skip in ['vcpkg_installed', 'thirdparty', '/build/', 'generated']):
            continue
        for file in files:
            if file.endswith(('.cpp', '.c', '.h', '.hpp')):
                filepath = os.path.join(root, file)
                try:
                    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                        content = f.read()
                        # Find include statements for .h files
                        matches = re.findall(r'^#include\s*[<"]([^<>"]*\.h)[>"]', content, re.MULTILINE)
                        for match in matches:
                            includes.add(match.replace('\\', '/'))
                except Exception as e:
                    continue
    return includes

def normalize_include_paths(includes, headers):
    """Try to match include paths to actual header file paths"""
    matched = set()
    unmatched = set()
    
    for inc in includes:
        found = False
        
        # Direct match
        if inc in headers:
            matched.add(inc)
            found = True
        # Try with include/ prefix (common pattern)
        elif f'include/{inc}' in headers:
            matched.add(f'include/{inc}')
            found = True
        # Try with common/ prefix
        elif f'common/{inc}' in headers:
            matched.add(f'common/{inc}')
            found = True
        # Try to find by basename and partial path match
        else:
            basename = os.path.basename(inc)
            for hf in headers:
                if os.path.basename(hf) == basename:
                    # Check if the include path suffix matches header path suffix
                    inc_parts = inc.split('/')
                    hf_parts = hf.split('/')
                    
                    # If include path is shorter, check if it matches the end of header path
                    if len(inc_parts) <= len(hf_parts):
                        if hf_parts[-len(inc_parts):] == inc_parts:
                            matched.add(hf)
                            found = True
                            break
        
        if not found:
            unmatched.add(inc)
    
    return matched, unmatched

def main():
    print("Analyzing header files in qt_pcb_project...")
    print()
    
    # Find all headers
    all_headers = find_all_headers()
    print(f"Total header files found: {len(all_headers)}")
    
    # Find included headers
    included_raw = find_included_headers()
    print(f"Total unique #include statements: {len(included_raw)}")
    
    # Try to match includes to actual files
    matched_headers, unmatched_includes = normalize_include_paths(included_raw, all_headers)
    print(f"Successfully matched includes: {len(matched_headers)}")
    print(f"Unmatched includes: {len(unmatched_includes)}")
    
    # Find unused headers
    unused_headers = all_headers - matched_headers
    print(f"Potentially unused headers: {len(unused_headers)}")
    print()
    
    # Show some examples
    print("First 30 potentially unused headers:")
    for i, header in enumerate(sorted(unused_headers)):
        if i >= 30:
            break
        print(f"  {header}")
    
    print()
    print("Some unmatched includes (might be system headers or missing files):")
    for i, inc in enumerate(sorted(unmatched_includes)):
        if i >= 20:
            break
        print(f"  {inc}")
    
    # Save results
    with open('unused_headers_final.txt', 'w') as f:
        for header in sorted(unused_headers):
            f.write(f"{header}\n")
    
    with open('matched_headers_final.txt', 'w') as f:
        for header in sorted(matched_headers):
            f.write(f"{header}\n")
    
    print()
    print("Results saved to 'unused_headers_final.txt' and 'matched_headers_final.txt'")

if __name__ == "__main__":
    main()