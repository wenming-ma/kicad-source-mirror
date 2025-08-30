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
import os
import time
from pathlib import Path
from typing import Set, List, Dict, Optional
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor, as_completed
from multiprocessing import cpu_count

def normalize_path(path_str: str) -> str:
    """标准化路径分隔符为正斜杠"""
    return path_str.replace('\\', '/')


class TransitiveClosure:
    def __init__(self, deps_json_path: str = "deps.json"):
        self.deps_json_path = Path(deps_json_path)
        self.deps_data = None
        self.processed_cpp = set()
        self.all_dependencies = set()
        self.project_root = None
        self.build_root = None
        self.deps_index = {}  # input_file -> command 的快速查找索引
        
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
        
        # 构建 deps.json 查找索引
        self._build_deps_index()
    
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
    
    def _build_deps_index(self):
        """构建 deps.json 的快速查找索引"""
        print("Building deps.json lookup index...")
        start_time = time.time()
        
        self.deps_index = {}
        
        for tu in self.deps_data.get('translation-units', []):
            for cmd in tu.get('commands', []):
                input_file = cmd.get('input-file', '')
                if input_file:
                    # 标准化路径作为键
                    normalized_key = os.path.normpath(input_file).replace('\\', '/')
                    self.deps_index[normalized_key] = cmd
        
        elapsed = time.time() - start_time
        print(f"Built deps index with {len(self.deps_index)} entries in {elapsed:.3f}s")
    
    def get_cpp_dependencies(self, cpp_file: str) -> Set[str]:
        """获取指定cpp文件的头文件依赖 - 使用索引优化"""        
        # 使用索引快速查找
        cmd = self._find_command_by_file(cpp_file)
        if not cmd:
            return set()
        
        headers = set()
        for dep in cmd.get('file-deps', []):
            if isinstance(dep, dict):
                dep_file = dep.get('file-entry', '')
            else:
                dep_file = dep
            
            if dep_file and self._should_include(dep_file):
                normalized = self._normalize_path(dep_file)
                headers.add(normalized)
        
        return headers
    
    def _find_command_by_file(self, cpp_file: str) -> Optional[Dict]:
        """使用索引快速查找文件对应的命令"""
        # 标准化输入路径
        cpp_file = cpp_file.replace('\\', '/')
        
        # 构建可能的完整路径
        if os.path.isabs(cpp_file):
            # 已经是绝对路径
            full_path = os.path.normpath(cpp_file).replace('\\', '/')
        else:
            # 相对路径，需要加上项目根目录
            full_path = os.path.normpath(f"{self.project_root}/{cpp_file}").replace('\\', '/')
        
        # 在索引中查找
        if full_path in self.deps_index:
            return self.deps_index[full_path]
        
        # 如果没找到，打印调试信息
        print(f"  WARNING: Could not find {cpp_file} in deps.json")
        print(f"    Tried path: {full_path}")
        
        return None
    
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
        """在 deps.json 索引中查找可能对应的 cpp 文件"""
        header_path_obj = Path(header_path)
        
        if header_path_obj.suffix.lower() not in ['.h', '.hpp', '.hxx']:
            return []
        
        base_name = header_path_obj.stem
        found_cpp = []
        
        # 在 deps.json 索引中查找所有可能的 cpp 文件名
        possible_cpp_names = [
            f"{base_name}.cpp",
            f"{base_name}.cc", 
            f"{base_name}.cxx",
            f"{base_name}.c"
        ]
        
        # 搜索常见的 cpp 文件位置模式
        header_dir = header_path_obj.parent
        possible_locations = [
            header_dir,  # 同目录
            Path('common'),
            Path('common') / header_dir.name if 'include' in str(header_path) else None,
            Path('libs/core/src'),
            Path('libs/kimath/src'),
            Path('pcbnew'),
            Path('eeschema'),
        ]
        
        # 在 deps.json 索引中查找
        for location in possible_locations:
            if location is None:
                continue
            for cpp_name in possible_cpp_names:
                # 构建可能的完整路径
                possible_path = location / cpp_name
                normalized_path = str(possible_path).replace('\\', '/')
                
                # 尝试在索引中查找（需要考虑项目根目录前缀）
                full_path = f"{self.project_root}/{normalized_path}".replace('\\', '/')
                full_path_normalized = os.path.normpath(full_path).replace('\\', '/')
                
                if full_path_normalized in self.deps_index:
                    found_cpp.append(normalized_path)
        
        return found_cpp
    
    
    def _process_batch_parallel(self, batch: Set[str]) -> List:
        """并行处理一批cpp文件"""
        # 过滤出未处理的文件
        to_process = [f for f in batch if f not in self.processed_cpp]
        
        if not to_process:
            return []
        
        max_workers = min(len(to_process), cpu_count())
        results = []
        
        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            future_to_file = {
                executor.submit(self._process_single_cpp, cpp_file): cpp_file 
                for cpp_file in to_process
            }
            
            for future in as_completed(future_to_file):
                cpp_file = future_to_file[future]
                try:
                    result = future.result()
                    results.append(result)
                except Exception as e:
                    print(f"Error processing {cpp_file}: {e}")
                    results.append(None)
        
        return results
    
    def _process_single_cpp(self, cpp_file: str):
        """处理单个cpp文件 - 工作线程函数"""
        # 获取头文件依赖
        headers = self.get_cpp_dependencies(cpp_file)
        
        # 为每个头文件寻找cpp实现
        new_cpp_files = []
        for header in headers:
            cpp_impls = self.find_cpp_for_header(header)
            new_cpp_files.extend(cpp_impls)
        
        return cpp_file, headers, new_cpp_files
    
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
            
            # 并行处理当前批次
            batch_results = self._process_batch_parallel(current_batch)
            
            # 合并结果
            new_files_count = 0
            for result in batch_results:
                if result:
                    cpp_file, headers, new_cpp_files = result
                    self.processed_cpp.add(normalize_path(cpp_file))
                    
                    # 添加头文件依赖
                    normalized_headers = {normalize_path(h) for h in headers}
                    self.all_dependencies.update(normalized_headers)
                    
                    # 添加新发现的cpp文件到工作队列
                    for cpp_impl in new_cpp_files:
                        normalized_cpp_impl = normalize_path(cpp_impl)
                        if normalized_cpp_impl not in self.processed_cpp:
                            work_queue.add(normalized_cpp_impl)
                            new_files_count += 1
            
            # 显示当前收集到的唯一文件总数
            total_cpp = len(self.processed_cpp)
            total_headers = len(self.all_dependencies)
            print(f"  Collection: {total_cpp} cpp files, {total_headers} headers")
            
            # 防止无限循环
            if iteration > 50:
                print("WARNING: Maximum iterations reached!")
                break
        
        print(f"\\nFinal collection: {len(self.processed_cpp)} cpp files, {len(self.all_dependencies)} headers")
        
        # 返回所有依赖 (头文件 + cpp文件)
        return self.all_dependencies | self.processed_cpp
    
    def save_results(self, output_file: str = "all_dependencies.txt"):
        """保存结果到文件"""
        all_files = self.all_dependencies | self.processed_cpp
        
        with open(output_file, 'w', encoding='utf-8') as f:
            for file_path in sorted(all_files):
                f.write(f"{file_path}\n")
        
        print(f"\nSaved {len(self.processed_cpp)} cpp files to {output_file}")

def main():
    parser = argparse.ArgumentParser(description='Compute transitive closure of file dependencies')
    parser.add_argument('--input-file', '-f', help='Text file with seed files (one per line)')
    parser.add_argument('files', nargs='*', help='Seed files as command line arguments')
    parser.add_argument('--output', '-o', default='all_dependencies.txt', help='Output file')
    parser.add_argument('--deps-json', default='deps.json', help='Path to deps.json file')
    
    args = parser.parse_args()
    
    # 确定种子文件
    if args.input_file:
        print(f"Reading seed files from {args.input_file}")
        with open(args.input_file, 'r', encoding='utf-8') as f:
            seed_files = [line.strip() for line in f if line.strip()]
    elif args.files:
        seed_files = args.files
    else:
        print("ERROR: Must provide either --input-file or files as arguments!")
        return 1
    
    if not seed_files:
        print("ERROR: No seed files provided!")
        return 1
    
    try:
        # 创建传递闭包分析器
        analyzer = TransitiveClosure(args.deps_json)
        analyzer.load_deps_data()
        
        # 标准化种子文件路径并计算传递闭包
        normalized_seed_files = [normalize_path(f) for f in seed_files]
        all_deps = analyzer.compute_transitive_closure(normalized_seed_files)
        
        # 保存结果
        analyzer.save_results(args.output)
        
        return 0
    
    except Exception as e:
        print(f"ERROR: {e}")
        return 1

if __name__ == '__main__':
    sys.exit(main())