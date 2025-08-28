#!/usr/bin/env python3
"""
传递闭包依赖分析工具

用法:
  python transitive_closure.py file1.cpp file2.cpp ...
  python transitive_closure.py --input-file seeds.txt
  
输出: all_dependencies.txt (包含所有发现的依赖文件)
"""

import json
import sys
import argparse
from pathlib import Path
from typing import Set, List

class TransitiveClosure:
    def __init__(self, deps_json_path: str = "deps.json"):
        self.deps_json_path = Path(deps_json_path)
        self.deps_data = None
        self.processed_cpp = set()
        self.all_dependencies = set()
        self.project_root = None
        self.build_root = None
        
    def load_deps_data(self):
        """加载 deps.json 数据"""
        if not self.deps_json_path.exists():
            raise FileNotFoundError(f"deps.json not found at {self.deps_json_path}")
        
        print(f"Loading dependency data from {self.deps_json_path}...")
        with open(self.deps_json_path, 'r', encoding='utf-8') as f:
            self.deps_data = json.load(f)
        
        print(f"Loaded {len(self.deps_data.get('translation-units', []))} translation units")
        
        # 动态推断项目根目录
        self._infer_project_paths()
    
    def _infer_project_paths(self):
        """从deps.json数据中动态推断项目根目录"""
        # 分析input-file路径来推断项目根目录
        sample_paths = []
        for tu in self.deps_data.get('translation-units', [])[:10]:  # 只检查前10个
            for cmd in tu.get('commands', []):
                input_file = cmd.get('input-file', '')
                if input_file and 'kicad' in input_file.lower():
                    sample_paths.append(input_file)
                    if len(sample_paths) >= 5:  # 收集5个样本就够了
                        break
            if len(sample_paths) >= 5:
                break
        
        if not sample_paths:
            raise ValueError("Cannot find any KiCad source files in deps.json")
        
        # 寻找共同的路径前缀
        import os
        common_prefix = os.path.commonpath(sample_paths)
        
        # 项目根目录应该是包含'kicad'的最深层目录
        path_parts = common_prefix.replace('\\', '/').split('/')
        kicad_index = -1
        for i, part in enumerate(path_parts):
            if 'kicad' in part.lower():
                kicad_index = i
        
        if kicad_index >= 0:
            # 项目根目录是kicad目录
            self.project_root = '/'.join(path_parts[:kicad_index + 1])
            # 构建目录通常在项目根目录下
            if '/build/' in common_prefix:
                build_parts = common_prefix.replace('\\', '/').split('/build/')
                self.build_root = build_parts[0] + '/build'
            else:
                self.build_root = self.project_root + '/build'
        else:
            # 使用公共前缀作为项目根目录
            self.project_root = common_prefix.replace('\\', '/')
            self.build_root = self.project_root + '/build'
        
        print(f"Detected project root: {self.project_root}")
        print(f"Detected build root: {self.build_root}")
    
    def get_cpp_dependencies(self, cpp_file: str) -> Set[str]:
        """获取指定cpp文件的头文件依赖"""
        print(f"  Analyzing: {cpp_file}")
        
        # 在deps.json中查找这个cpp文件
        for tu in self.deps_data.get('translation-units', []):
            for cmd in tu.get('commands', []):
                input_file = cmd.get('input-file', '')
                
                # Debug: 显示包含目标文件名的路径
                if cpp_file.split('/')[-1] in input_file:
                    print(f"    DEBUG: Found similar: {input_file}")
                
                # 匹配文件路径 (支持相对路径和绝对路径)
                if self._path_matches(cpp_file, input_file):
                    print(f"    Found in deps.json: {input_file}")
                    
                    headers = set()
                    for dep in cmd.get('file-deps', []):
                        if isinstance(dep, dict):
                            dep_file = dep.get('file-entry', '')
                        else:
                            dep_file = dep
                        
                        if dep_file and self._should_include(dep_file):
                            normalized = self._normalize_path(dep_file)
                            headers.add(normalized)
                    
                    print(f"    Found {len(headers)} header dependencies")
                    return headers
        
        print(f"    WARNING: {cpp_file} not found in deps.json")
        return set()
    
    def _path_matches(self, cpp_file: str, input_file: str) -> bool:
        """检查路径是否匹配"""
        # 将cpp_file转换为绝对路径
        if not cpp_file.startswith(('/', '\\')):
            # 相对路径，添加项目根目录
            expected_absolute = f"{self.project_root}/{cpp_file}"
        else:
            expected_absolute = cpp_file
            
        # 标准化两个路径进行比较
        import os
        expected_normalized = os.path.normpath(expected_absolute).replace('\\', '/')
        input_normalized = os.path.normpath(input_file).replace('\\', '/')
        
        # 严格匹配：只有完全匹配才认为是同一个文件  
        return input_normalized == expected_normalized
    
    def _should_include(self, file_path: str) -> bool:
        """判断是否应该包含这个依赖"""
        file_path_lower = file_path.lower()
        
        # 跳过系统和外部依赖
        skip_patterns = [
            'vcpkg_installed', 'visualstudio', 'vc\\tools', 'vc/tools',
            'windows kits', 'windowskits', 'program files',
            'microsoft visual studio'
        ]
        
        for pattern in skip_patterns:
            if pattern in file_path_lower:
                return False
        
        # 只包含项目根目录下的文件
        project_root_lower = self.project_root.lower()
        file_path_normalized = file_path.replace('\\', '/').lower()
        
        return file_path_normalized.startswith(project_root_lower)
    
    def _normalize_path(self, path: str) -> str:
        """标准化文件路径"""
        import os.path
        
        # 先标准化路径，解析所有的 ".." 和 "." 
        path = os.path.normpath(path)
        path = path.replace('\\', '/')
        
        # 动态移除项目根目录前缀
        project_root_normalized = self.project_root.replace('\\', '/') + '/'
        build_root_normalized = self.build_root.replace('\\', '/') + '/'
        
        prefixes = [
            project_root_normalized,
            build_root_normalized,
        ]
        
        path_to_check = path
        for prefix in prefixes:
            if path.lower().startswith(prefix.lower()):
                path = path[len(prefix):]
                break
        
        return path
    
    def find_cpp_for_header(self, header_path: str) -> List[str]:
        """为头文件寻找对应的cpp实现"""
        header_path = Path(header_path)
        
        if header_path.suffix.lower() not in ['.h', '.hpp', '.hxx']:
            return []
        
        base_name = header_path.stem
        source_extensions = ['.cpp', '.cc', '.cxx', '.c']
        
        # 搜索候选位置
        search_locations = [
            header_path.parent,  # 同目录
            Path('common'),      # common目录
            Path('libs/core/src'),
            Path('libs/kimath/src'), 
            Path('pcbnew'),
            Path('eeschema'),
            header_path.parent.parent / 'src' if 'include' in str(header_path) else None,
        ]
        
        # 移除None值
        search_locations = [loc for loc in search_locations if loc is not None]
        
        found_cpp = []
        for location in search_locations:
            for ext in source_extensions:
                candidate = location / f"{base_name}{ext}"
                # 检查文件是否实际存在
                if candidate.exists() or (Path('.') / candidate).exists():
                    found_cpp.append(str(candidate))
        
        return found_cpp
    
    def compute_transitive_closure(self, seed_files: List[str]) -> Set[str]:
        """计算传递闭包"""
        print(f"\\nStarting transitive closure computation...")
        print(f"Seed files: {seed_files}")
        
        # 初始化工作队列
        work_queue = set(seed_files)
        iteration = 0
        
        while work_queue:
            iteration += 1
            print(f"\\n=== Iteration {iteration} ===")
            print(f"Processing {len(work_queue)} cpp files...")
            
            # 处理当前批次
            current_batch = work_queue.copy()
            work_queue.clear()
            
            for cpp_file in current_batch:
                if cpp_file in self.processed_cpp:
                    continue
                
                self.processed_cpp.add(cpp_file)
                
                # 获取头文件依赖
                headers = self.get_cpp_dependencies(cpp_file)
                self.all_dependencies.update(headers)
                
                # 为每个头文件寻找cpp实现
                new_cpp_count = 0
                for header in headers:
                    cpp_impls = self.find_cpp_for_header(header)
                    for cpp_impl in cpp_impls:
                        if cpp_impl not in self.processed_cpp:
                            work_queue.add(cpp_impl)
                            new_cpp_count += 1
                            print(f"    {header} -> {cpp_impl}")
                
                if new_cpp_count > 0:
                    print(f"  Discovered {new_cpp_count} new cpp files")
            
            print(f"Next iteration will process {len(work_queue)} files")
            
            # 防止无限循环
            if iteration > 50:
                print("WARNING: Maximum iterations reached!")
                break
        
        print(f"\\nTransitive closure completed!")
        print(f"Processed {len(self.processed_cpp)} cpp files")
        print(f"Found {len(self.all_dependencies)} total dependencies")
        
        # 返回所有依赖 (头文件 + cpp文件)
        return self.all_dependencies | self.processed_cpp
    
    def save_results(self, output_file: str = "all_dependencies.txt"):
        """保存结果到文件"""
        all_files = self.all_dependencies | self.processed_cpp
        
        with open(output_file, 'w', encoding='utf-8') as f:
            for file_path in sorted(all_files):
                f.write(f"{file_path}\n")
        
        print(f"\nResults saved to {output_file}")
        print(f"Total files: {len(all_files)}")
        print(f"  - Header files: {len(self.all_dependencies)}")
        print(f"  - Source files: {len(self.processed_cpp)}")

def main():
    parser = argparse.ArgumentParser(description='Compute transitive closure of file dependencies')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--input-file', '-f', help='Text file with seed files (one per line)')
    group.add_argument('files', nargs='*', help='Seed files as command line arguments')
    parser.add_argument('--output', '-o', default='all_dependencies.txt', help='Output file')
    parser.add_argument('--deps-json', default='deps.json', help='Path to deps.json file')
    
    args = parser.parse_args()
    
    # 确定种子文件
    if args.input_file:
        print(f"Reading seed files from {args.input_file}")
        with open(args.input_file, 'r', encoding='utf-8') as f:
            seed_files = [line.strip() for line in f if line.strip()]
    else:
        seed_files = args.files
    
    if not seed_files:
        print("ERROR: No seed files provided!")
        return 1
    
    try:
        # 创建传递闭包分析器
        analyzer = TransitiveClosure(args.deps_json)
        analyzer.load_deps_data()
        
        # 计算传递闭包
        all_deps = analyzer.compute_transitive_closure(seed_files)
        
        # 保存结果
        analyzer.save_results(args.output)
        
        return 0
    
    except Exception as e:
        print(f"ERROR: {e}")
        return 1

if __name__ == '__main__':
    sys.exit(main())