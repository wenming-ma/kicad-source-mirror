import json
import os

with open('scripts/minset_sources.json', 'r') as f:
    data = json.load(f)
    
# Group files by directory
dirs = {}
base_path = r'C:\Users\wenming_ma\source\repos\kicad-source-mirror'

for source in data['sources']:
    # Extract relative path from absolute path
    rel_path = source.replace(base_path + '\\', '')
    # Get directory
    parts = rel_path.split('\\')
    if len(parts) > 1:
        dir_path = '\\'.join(parts[:-1])
        filename = parts[-1]
        if dir_path not in dirs:
            dirs[dir_path] = []
        dirs[dir_path].append(filename)

# Print directory structure
for dir_path in sorted(dirs.keys()):
    print(f'{dir_path}: {len(dirs[dir_path])} files')
    for file in sorted(dirs[dir_path]):
        print(f'  {file}')
    print()