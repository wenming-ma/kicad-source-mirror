#!/usr/bin/env python3
"""
快速生成 common_symbols.txt 文件
专注于最重要的重复符号（重复次数 > 50）
"""

import json
import re
from pathlib import Path
from datetime import datetime

def classify_symbol_detailed(symbol_name):
    """详细分类符号"""

    # 编译器生成的析构函数
    if symbol_name.startswith('??_E'):
        return "compiler_generated_scalar_destructor"
    if symbol_name.startswith('??_G'):
        return "compiler_generated_vector_destructor"
    if symbol_name.startswith('??_H'):
        return "compiler_generated_virtual_destructor"

    # 全局operator new/delete
    if '__empty_global_delete' in symbol_name:
        return "global_operator_delete"
    if '__empty_global_new' in symbol_name:
        return "global_operator_new"
    if symbol_name.startswith('??2@') or symbol_name.startswith('??3@'):
        return "global_operator_new_delete"

    # RTTI相关符号
    if symbol_name.startswith('??_R'):
        return "rtti_symbols"

    # 虚函数表相关
    if symbol_name.startswith('??_7'):
        return "vtable_symbols"

    # 字符串常量
    if symbol_name.startswith('??_C@'):
        return "string_literal_constant"

    # STL标准库
    if any(std_pattern in symbol_name for std_pattern in [
        'std@@', '@std@@', 'basic_string@', 'allocator@', 'is_constant_evaluated@std@@'
    ]):
        return "stl_template_instantiation"

    # wxWidgets框架
    if any(wx_pattern in symbol_name for wx_pattern in [
        'wxString@@', 'wxObject@@', '??_EwxString@@'
    ]):
        return "wxwidgets_framework"

    # 构造析构函数
    if symbol_name.startswith('??0'):
        return "user_constructor"
    if symbol_name.startswith('??1'):
        return "user_destructor"

    # C++异常处理
    if 'exception@std@@' in symbol_name or '_CT??' in symbol_name:
        return "std_exception_class"

    # protobuf相关
    if any(pb_pattern in symbol_name for pb_pattern in [
        'protobuf', 'google@@', 'TableStruct_'
    ]):
        return "protobuf_library"

    return "other_duplicate"

def generate_common_symbols_fast():
    """快速生成 common_symbols.txt 文件"""

    # 加载分析结果
    analysis_file = Path("scripts/duplicate_symbols_analysis.json")
    if not analysis_file.exists():
        print(f"错误: 找不到分析结果文件 {analysis_file}")
        return

    with open(analysis_file, 'r', encoding='utf-8') as f:
        analysis = json.load(f)

    # 只处理top_10和高重复次数的符号
    categories = {}
    category_stats = {}

    print("正在处理高重复次数符号...")

    # 处理 top_10_duplicates
    processed_symbols = set()
    for item in analysis['top_10_duplicates']:
        symbol = item['symbol']
        count = item['duplicate_count']

        if count >= 50:  # 只处理重复次数 >= 50 的符号
            category = classify_symbol_detailed(symbol)

            if category not in categories:
                categories[category] = []
                category_stats[category] = {'count': 0, 'total_occurrences': 0}

            categories[category].append((symbol, count))
            category_stats[category]['count'] += 1
            category_stats[category]['total_occurrences'] += count
            processed_symbols.add(symbol)

    # 处理其他类别中的高重复符号
    for cat_name, symbols in analysis['categories'].items():
        for item in symbols:
            symbol = item['symbol']
            count = item['duplicate_count']

            if count >= 50 and symbol not in processed_symbols:  # 避免重复处理
                category = classify_symbol_detailed(symbol)

                if category not in categories:
                    categories[category] = []
                    category_stats[category] = {'count': 0, 'total_occurrences': 0}

                categories[category].append((symbol, count))
                category_stats[category]['count'] += 1
                category_stats[category]['total_occurrences'] += count
                processed_symbols.add(symbol)

    # 生成输出文件
    output_file = Path("scripts/common_symbols.txt")
    print(f"正在生成 {output_file}...")

    with open(output_file, 'w', encoding='utf-8') as f:
        # 文件头
        f.write("# 编译器生成的重复符号 - 这些符号会在多个编译单元中出现\n")
        f.write("# 格式：符号名称 [tab] 分类说明 [tab] 重复次数\n")
        f.write(f"# 生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(f"# 总重复符号数: {analysis['summary']['total_duplicate_symbols']}\n")
        f.write(f"# 此文件只包含重复次数 >= 50 的符号\n")
        f.write("\n")

        # 按优先级排序类别
        priority_order = [
            "global_operator_delete",
            "global_operator_new",
            "global_operator_new_delete",
            "compiler_generated_scalar_destructor",
            "string_literal_constant",
            "rtti_symbols",
            "vtable_symbols",
            "std_exception_class",
            "stl_template_instantiation",
            "wxwidgets_framework",
            "user_constructor",
            "user_destructor",
            "protobuf_library",
            "other_duplicate"
        ]

        # 按优先级顺序输出
        for category in priority_order:
            if category in categories and categories[category]:
                stats = category_stats[category]
                f.write(f"\n# === {category} === ({stats['count']} 个符号, 平均重复 {stats['total_occurrences']/stats['count']:.1f} 次)\n")

                # 按重复次数降序排列
                symbols_in_category = sorted(categories[category], key=lambda x: x[1], reverse=True)

                for symbol, count in symbols_in_category:
                    f.write(f"{symbol}\t{category}\t{count}\n")

    # 输出统计信息
    print("\n高重复符号分类统计 (重复次数 >= 50):")
    print(f"{'类别':<35} {'符号数':<10} {'总出现次数':<12} {'平均重复次数':<12}")
    print("-" * 75)

    total_symbols = 0
    total_occurrences = 0

    for category in priority_order:
        if category in category_stats:
            stats = category_stats[category]
            count = stats['count']
            occurrences = stats['total_occurrences']
            avg = occurrences / count if count > 0 else 0

            print(f"{category:<35} {count:<10} {occurrences:<12} {avg:<12.1f}")
            total_symbols += count
            total_occurrences += occurrences

    print("-" * 75)
    print(f"{'总计':<35} {total_symbols:<10} {total_occurrences:<12} {total_occurrences/total_symbols:<12.1f}")

    print(f"\n配置文件已生成: {output_file}")
    print(f"包含 {total_symbols} 个高重复符号 (重复次数 >= 50)")

    return output_file

if __name__ == "__main__":
    generate_common_symbols_fast()