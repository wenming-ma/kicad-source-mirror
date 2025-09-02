#!/usr/bin/env python3
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPTS_DIR = ROOT / "scripts"
SEEDS_FILE = SCRIPTS_DIR / "seeds.txt"
OUTPUT_JSON = SCRIPTS_DIR / "seeds_sources.json"

def convert_seeds_to_json():
    """Convert seeds.txt to JSON format compatible with 50_analyze_individual_dependencies.py"""
    
    if not SEEDS_FILE.exists():
        print(f"Error: seeds.txt not found at {SEEDS_FILE}")
        return False
    
    print(f"Reading seeds from: {SEEDS_FILE}")
    
    # Read seeds.txt and convert relative paths to absolute paths
    sources = []
    with open(SEEDS_FILE, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if line and not line.startswith('#'):
                # Convert relative path to absolute path
                rel_path = Path(line)
                abs_path = (ROOT / rel_path).resolve()
                sources.append(str(abs_path))
    
    print(f"Found {len(sources)} seed files")
    
    # Create JSON structure matching minset_sources.json format
    json_data = {
        "sources": sources
    }
    
    # Write to output file
    with open(OUTPUT_JSON, 'w', encoding='utf-8') as f:
        json.dump(json_data, f, indent=2)
    
    print(f"Converted seeds to JSON format: {OUTPUT_JSON}")
    print("Sample files:")
    for i, source in enumerate(sources[:5]):
        print(f"  {i+1}. {source}")
    if len(sources) > 5:
        print(f"  ... and {len(sources) - 5} more")
    
    return True

def main():
    """Main function"""
    print("Converting seeds.txt to JSON format for dependency analysis...")
    
    success = convert_seeds_to_json()
    
    if success:
        print(f"\nSuccess! Now you can run:")
        print(f"python scripts/50_analyze_individual_dependencies.py")
        print(f"But first modify the script to use '{OUTPUT_JSON.name}' instead of 'minset_sources.json'")
        return 0
    else:
        print("Conversion failed.")
        return 1

if __name__ == "__main__":
    exit(main())