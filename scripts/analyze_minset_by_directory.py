#!/usr/bin/env python3
"""
分析minset_sources.json，按目录结构分组源文件
"""

import json
import os
from pathlib import Path
from collections import defaultdict

def analyze_sources_by_directory(json_file):
    """读取minset_sources.json并按目录分组"""
    
    # 读取JSON文件
    with open(json_file, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    sources = data.get('sources', [])
    print(f"总计源文件数: {len(sources)}")
    
    # 定义基础路径
    base_path = r"C:\Users\wenming_ma\source\repos\kicad-source-mirror"
    
    # 按目录分组
    directory_groups = {
        'bitmap2component/': [],
        'common/': [],
        'libs/': [],
        'pcbnew/': [],
        'scripting/': [],
        'thirdparty/': [],
        'build/': []
    }
    
    other_files = []
    
    for source in sources:
        # 获取相对于基础路径的路径
        if source.startswith(base_path):
            rel_path = source[len(base_path):].lstrip('\\').replace('\\', '/')
        else:
            rel_path = source.replace('\\', '/')
        
        # 按目录分类
        categorized = False
        for dir_name in directory_groups.keys():
            if rel_path.startswith(dir_name):
                # 转换为相对于该目录的路径
                relative_to_dir = rel_path[len(dir_name):]
                directory_groups[dir_name].append(relative_to_dir)
                categorized = True
                break
        
        if not categorized:
            other_files.append(rel_path)
    
    # 生成报告
    print("\n" + "="*80)
    print("KiCad PCB Minimum Compilation Set - Directory Analysis")
    print("="*80)
    
    total_files = 0
    for dir_name, files in directory_groups.items():
        if files:
            print(f"\n[{dir_name}] ({len(files)} files)")
            print("-" * 60)
            
            # 按文件扩展名分组
            cpp_files = [f for f in files if f.endswith(('.cpp', '.cc'))]
            other_ext = [f for f in files if not f.endswith(('.cpp', '.cc'))]
            
            if cpp_files:
                print("C++ Source Files:")
                for file in sorted(cpp_files):
                    print(f"  - {file}")
            
            if other_ext:
                print("Other Files:")
                for file in sorted(other_ext):
                    print(f"  - {file}")
            
            total_files += len(files)
    
    # 显示未分类的文件
    if other_files:
        print(f"\n[Other Uncategorized Files] ({len(other_files)} files)")
        print("-" * 60)
        for file in sorted(other_files):
            print(f"  - {file}")
        total_files += len(other_files)
    
    print(f"\nTotal analyzed files: {total_files}")
    
    # 生成汇总统计
    print("\n" + "="*80)
    print("Directory Statistics Summary")
    print("="*80)
    for dir_name, files in directory_groups.items():
        if files:
            cpp_count = len([f for f in files if f.endswith(('.cpp', '.cc'))])
            print(f"{dir_name:20} {len(files):>3} files ({cpp_count} C++ source files)")
    
    return directory_groups

if __name__ == "__main__":
    # 查找minset_sources.json文件
    possible_paths = [
        "build/x64-Debug/minset_sources.json",
        "build/minset_sources.json",
        "scripts/minset_sources.json"
    ]
    
    json_file = None
    for path in possible_paths:
        if os.path.exists(path):
            json_file = path
            break
    
    if json_file:
        print(f"Using file: {json_file}")
        analyze_sources_by_directory(json_file)
    else:
        print("Error: Cannot find minset_sources.json file")
        print("Please ensure dependency analysis scripts have been run to generate this file")