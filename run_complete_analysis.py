#!/usr/bin/env python3
"""
完整的依赖分析流程
自动执行批量分析和Excel报告生成
"""

import os
import sys
import subprocess

def main():
    if len(sys.argv) < 2:
        print("用法: python run_complete_analysis.py <all_dependencies.txt>")
        sys.exit(1)
    
    dependencies_file = sys.argv[1]
    if not os.path.exists(dependencies_file):
        print(f"错误: 文件不存在 {dependencies_file}")
        sys.exit(1)
    
    print("=== KiCad 依赖分析完整流程 ===")
    print(f"输入文件: {dependencies_file}")
    print()
    
    # 步骤1: 批量分析
    print("步骤1: 执行批量依赖分析...")
    try:
        result = subprocess.run([
            sys.executable, 
            'batch_analyze_dependencies.py', 
            dependencies_file
        ], check=True)
        print("✓ 批量分析完成")
    except subprocess.CalledProcessError as e:
        print(f"✗ 批量分析失败: {e}")
        sys.exit(1)
    
    # 步骤2: 生成Excel报告  
    print("\n步骤2: 生成Excel报告...")
    try:
        result = subprocess.run([
            sys.executable,
            'generate_excel_report.py'
        ], check=True)
        print("✓ Excel报告生成完成")
    except subprocess.CalledProcessError as e:
        print(f"✗ Excel报告生成失败: {e}")
        sys.exit(1)
    
    print("\n=== 分析完成 ===")
    print("生成的文件:")
    print("- individual_deps_analysis/ (详细分析结果)")
    print("- dependencies_analysis_report.xlsx (Excel报告)")
    print()
    print("打开Excel文件查看每个CPP文件的依赖分析结果")

if __name__ == "__main__":
    main()