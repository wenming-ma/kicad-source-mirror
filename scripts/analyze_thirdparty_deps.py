#!/usr/bin/env python3
"""
Analyze third-party dependencies for the minimum compilation set.
Reads minset_headers.json and deps_full.json to extract third-party library dependencies.
"""

import json
import os
from pathlib import Path
from collections import defaultdict

def load_json_file(filepath):
    """Load JSON file and return data."""
    with open(filepath, 'r', encoding='utf-8') as f:
        return json.load(f)

def analyze_thirdparty_deps():
    """Analyze third-party dependencies for the minimum compilation set."""
    
    # Define paths
    script_dir = Path(__file__).parent
    kicad_root = script_dir.parent
    build_dir = kicad_root / "build"
    
    # Load input files
    print("Loading minset_sources.json...")
    minset_sources_data = load_json_file(build_dir / "minset_sources.json")
    minset_sources = set(minset_sources_data["sources"])
    
    # Use source files for analysis since deps_full.json contains source file dependencies
    all_minset_files = minset_sources
    
    print(f"Found {len(all_minset_files)} files in minimum set")
    
    print("Loading deps_full.json...")
    deps_full_data = load_json_file(build_dir / "deps_full.json")
    
    # Extract translation units from deps_full
    translation_units = deps_full_data.get("translation-units", [])
    
    # Third-party library patterns (from vcpkg.json)
    thirdparty_patterns = {
        'boost': ['boost/', 'boost\\'],
        'wxwidgets': ['wx/', 'wx\\', 'wxWidgets'],
        'glew': ['GL/glew', 'GL\\glew'],
        'glm': ['glm/', 'glm\\'],
        'cairo': ['cairo/', 'cairo\\', 'cairo.h'],
        'curl': ['curl/', 'curl\\'],
        'harfbuzz': ['harfbuzz/', 'harfbuzz\\', 'hb.h', 'hb-'],
        'opencascade': ['opencascade/', 'opencascade\\', 'Standard_', 'TopTools', 'TopoDS', 'BRep', 'Geom'],
        'opengl': ['GL/', 'GL\\', 'gl.h', 'glu.h'],
        'python': ['Python.h', 'python3', 'pybind11'],
        'openssl': ['openssl/', 'openssl\\'],
        'sqlite3': ['sqlite3.h'],
        'icu': ['unicode/', 'unicode\\', 'icu/'],
        'ngspice': ['ngspice/', 'ngspice\\'],
        'libgit2': ['git2.h', 'git2/', 'git2\\'],
        'nng': ['nng/', 'nng\\', 'nng.h'],
        'protobuf': ['google/protobuf', 'google\\protobuf'],
        'zstd': ['zstd.h', 'zstd/', 'zstd\\'],
        'gettext': ['libintl.h', 'gettext.h']
    }
    
    # Collect third-party dependencies
    thirdparty_deps = defaultdict(set)
    files_using_thirdparty = defaultdict(set)
    
    print("Analyzing dependencies...")
    analyzed_count = 0
    
    for file_path in all_minset_files:
        # Normalize path for comparison
        normalized_path = str(Path(file_path).resolve())
        
        # Find this file in translation units
        file_entry = None
        for tu in translation_units:
            if tu.get("commands"):
                for cmd in tu["commands"]:
                    # Check if the file appears in file-deps (the first one is usually the source file)
                    file_deps = cmd.get("file-deps", [])
                    if file_deps:
                        # Check if the first file-dep matches our file
                        first_dep = str(Path(file_deps[0]).resolve())
                        if first_dep == normalized_path:
                            file_entry = cmd
                            break
                if file_entry:
                    break
        
        if not file_entry:
            continue
            
        analyzed_count += 1
        
        # Analyze file dependencies
        file_deps = file_entry.get("file-deps", [])
        for dep in file_deps:
            dep_lower = dep.lower().replace('\\', '/')
            
            # Check if it's a third-party header
            for lib_name, patterns in thirdparty_patterns.items():
                for pattern in patterns:
                    pattern_lower = pattern.lower().replace('\\', '/')
                    if pattern_lower in dep_lower:
                        thirdparty_deps[lib_name].add(dep)
                        files_using_thirdparty[lib_name].add(file_path)
                        break
    
    print(f"Analyzed {analyzed_count} files from minimum set")
    
    # Prepare results
    results = {
        "summary": {
            "total_files_analyzed": len(all_minset_files),
            "third_party_libraries_used": len(thirdparty_deps),
            "libraries": sorted(thirdparty_deps.keys())
        },
        "dependencies": {}
    }
    
    # Add detailed dependency information
    for lib_name in sorted(thirdparty_deps.keys()):
        results["dependencies"][lib_name] = {
            "headers": sorted(list(thirdparty_deps[lib_name])),
            "headers_count": len(thirdparty_deps[lib_name]),
            "files_using": sorted(list(files_using_thirdparty[lib_name])),
            "files_count": len(files_using_thirdparty[lib_name])
        }
    
    # Save results
    output_file = build_dir / "minset_thirdparty_deps.json"
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(results, f, indent=2, ensure_ascii=False)
    
    print(f"\nAnalysis complete! Results saved to: {output_file}")
    
    # Print summary
    print("\n=== Third-party Dependencies Summary ===")
    print(f"Total files analyzed: {results['summary']['total_files_analyzed']}")
    print(f"Third-party libraries used: {results['summary']['third_party_libraries_used']}")
    print("\nLibraries found:")
    for lib_name in results['summary']['libraries']:
        dep_info = results['dependencies'][lib_name]
        print(f"  - {lib_name}: {dep_info['headers_count']} headers, used by {dep_info['files_count']} files")
    
    # Determine minimal vcpkg dependencies
    vcpkg_deps = []
    vcpkg_mapping = {
        'boost': [
            'boost-algorithm', 'boost-bimap', 'boost-filesystem', 'boost-functional',
            'boost-iterator', 'boost-locale', 'boost-optional', 'boost-property-tree',
            'boost-ptr-container', 'boost-random', 'boost-range', 'boost-test', 'boost-uuid'
        ],
        'wxwidgets': ['wxwidgets'],
        'glew': ['glew'],
        'glm': ['glm'],
        'cairo': ['cairo'],
        'curl': ['curl'],
        'harfbuzz': ['harfbuzz'],
        'opencascade': ['opencascade'],
        'opengl': ['opengl'],
        'python': ['python3'],
        'openssl': ['openssl'],
        'sqlite3': ['sqlite3'],
        'icu': ['icu'],
        'ngspice': ['ngspice'],
        'libgit2': ['libgit2'],
        'nng': ['nng'],
        'protobuf': ['protobuf'],
        'zstd': ['zstd'],
        'gettext': ['gettext']
    }
    
    for lib in results['summary']['libraries']:
        if lib in vcpkg_mapping:
            vcpkg_deps.extend(vcpkg_mapping[lib])
    
    # Remove duplicates and sort
    vcpkg_deps = sorted(list(set(vcpkg_deps)))
    
    # Add vcpkg dependencies to results
    results["vcpkg_dependencies"] = vcpkg_deps
    
    # Save updated results
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(results, f, indent=2, ensure_ascii=False)
    
    print(f"\n=== Minimal vcpkg dependencies ({len(vcpkg_deps)} packages) ===")
    for dep in vcpkg_deps:
        print(f"  - {dep}")
    
    return results

if __name__ == "__main__":
    analyze_thirdparty_deps()