#!/usr/bin/env python3
"""
刷新qt_pcb_project中的.h和.cpp文件
从KiCad源码重新复制所有.h和.cpp文件到qt_pcb_project目录，覆盖现有文件
"""

import os
import shutil
import glob
from pathlib import Path

def find_source_file(kicad_root, filename, current_file_path):
    """在KiCad源码中查找指定文件名的文件"""
    # 只在主要目录中搜索，避免全盘搜索
    search_dirs = [
        "include",
        "common", 
        "libs",
        "pcbnew",
        "thirdparty"
    ]
    
    for search_dir in search_dirs:
        dir_path = kicad_root / search_dir
        if not dir_path.exists():
            continue
            
        pattern = f"**/{filename}"
        matches = list(dir_path.glob(pattern))
        if matches:
            # 返回第一个找到的匹配文件（不是qt_pcb_project下的）
            for match in matches:
                if 'qt_pcb_project' not in str(match) and match != current_file_path:
                    return match
    
    return None

def refresh_files():
    """刷新qt_pcb_project中的所有.h和.cpp文件"""
    
    # 当前工作目录就是KiCad源码根目录
    kicad_root = Path.cwd()
    qt_project_root = kicad_root / "qt_pcb_project"
    
    if not qt_project_root.exists():
        print(f"错误: qt_pcb_project目录不存在: {qt_project_root}")
        return
    
    print(f"KiCad源码根目录: {kicad_root}")
    print(f"Qt项目目录: {qt_project_root}")
    print()
    
    # 统计信息
    refreshed_count = 0
    not_found_count = 0
    error_count = 0
    
    # 遍历qt_pcb_project中的所有.h和.cpp文件
    for ext in ['*.h', '*.cpp']:
        for file_path in qt_project_root.rglob(ext):
            # 跳过build目录
            if 'build' in file_path.parts:
                continue
                
            filename = file_path.name
            
            # 计算相对路径用于显示
            rel_path = file_path.relative_to(qt_project_root)
            
            # 在KiCad源码中查找对应文件
            source_file = find_source_file(kicad_root, filename, file_path)
            
            if source_file is None:
                print(f"[SKIP] 跳过文件: {filename} (未找到源文件或为同一文件)")
                not_found_count += 1
                continue
            
            try:
                # 复制文件，覆盖现有文件
                shutil.copy2(source_file, file_path)
                print(f"[OK] 已刷新: {rel_path} <- {source_file.relative_to(kicad_root)}")
                refreshed_count += 1
            except Exception as e:
                print(f"[ERROR] 复制失败: {rel_path} - {e}")
                error_count += 1
    
    # 打印统计结果
    print()
    print("=" * 50)
    print("刷新完成统计:")
    print(f"[OK] 成功刷新: {refreshed_count} 个文件")
    print(f"[SKIP] 跳过文件: {not_found_count} 个文件")
    print(f"[ERROR] 复制失败: {error_count} 个文件")
    print("=" * 50)

if __name__ == "__main__":
    refresh_files()