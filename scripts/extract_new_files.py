import re

# Read the file copy log
with open('../kicad_core_project_wx/file_copy_log.txt', 'r', encoding='utf-8') as f:
    lines = f.readlines()

new_files = []
for line in lines:
    if 'SKIPPED: already exists' not in line and ('\\' in line or '/' in line):
        # Extract file path
        if 'kicad_core_project_wx' in line:
            # Get relative path from kicad_core_project_wx
            path = line.strip()
            if path.endswith(('.cpp', '.h')):
                # Extract relative path
                parts = path.split('kicad_core_project_wx')
                if len(parts) > 1:
                    rel_path = parts[1].replace('\\', '/')
                    if rel_path.startswith('/'):
                        rel_path = rel_path[1:]
                    new_files.append(rel_path)

# Group files by directory
files_by_dir = {}
for file in new_files:
    if '/' in file:
        dir_path = '/'.join(file.split('/')[:-1])
        filename = file.split('/')[-1]
        if dir_path not in files_by_dir:
            files_by_dir[dir_path] = {'cpp': [], 'h': []}
        if file.endswith('.cpp'):
            files_by_dir[dir_path]['cpp'].append(filename)
        else:
            files_by_dir[dir_path]['h'].append(filename)

# Output organized list
print(f"Total new files: {len(new_files)}")
print("\nOrganized by directory:")
for dir_path in sorted(files_by_dir.keys()):
    print(f"\n{dir_path}:")
    if files_by_dir[dir_path]['cpp']:
        print(f"  CPP files: {', '.join(sorted(files_by_dir[dir_path]['cpp']))}")
    if files_by_dir[dir_path]['h']:
        print(f"  Header files: {', '.join(sorted(files_by_dir[dir_path]['h']))}")

# Save to file
with open('scripts/new_files_organized.txt', 'w', encoding='utf-8') as f:
    for dir_path in sorted(files_by_dir.keys()):
        f.write(f"\n# {dir_path}\n")
        for cpp_file in sorted(files_by_dir[dir_path]['cpp']):
            f.write(f"{dir_path}/{cpp_file}\n")
