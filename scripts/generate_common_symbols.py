#!/usr/bin/env python3
"""
生成 common_symbols.txt 文件
基于重复符号分析结果，生成分类的重复符号配置文件
"""

import json
import re
from pathlib import Path
from datetime import datetime

def classify_symbol_detailed(symbol_name):
    """详细分类符号"""

    # 编译器生成的析构函数 (包括标量、向量析构) - 高优先级匹配
    if symbol_name.startswith('??_E'):
        return "compiler_generated_scalar_destructor"
    if symbol_name.startswith('??_G'):
        return "compiler_generated_vector_destructor"
    if symbol_name.startswith('??_H'):
        return "compiler_generated_virtual_destructor"

    # 全局operator new/delete - 高优先级匹配
    if '__empty_global_delete' in symbol_name:
        return "global_operator_delete"
    if '__empty_global_new' in symbol_name:
        return "global_operator_new"
    if symbol_name.startswith('??2@') or symbol_name.startswith('??3@'):
        return "global_operator_new_delete"

    # RTTI相关符号
    if symbol_name.startswith('??_R'):
        if '??_R0' in symbol_name:
            return "rtti_type_descriptor"
        elif '??_R1' in symbol_name:
            return "rtti_base_class_descriptor"
        elif '??_R2' in symbol_name:
            return "rtti_base_class_array"
        elif '??_R3' in symbol_name:
            return "rtti_class_hierarchy_descriptor"
        elif '??_R4' in symbol_name:
            return "rtti_complete_object_locator"
        else:
            return "rtti_symbols"

    # 虚函数表相关
    if symbol_name.startswith('??_7'):
        return "vtable_symbols"
    if symbol_name.startswith('??_6'):
        return "vftable_symbols"

    # 构造函数和析构函数 (用户定义)
    if symbol_name.startswith('??0'):
        return "user_constructor"
    if symbol_name.startswith('??1'):
        return "user_destructor"

    # 赋值操作符
    if symbol_name.startswith('??4'):
        return "assignment_operator"

    # 字符串常量 (编译器生成)
    if symbol_name.startswith('??_C@'):
        return "string_literal_constant"

    # C++ 异常处理相关
    if '_CT??' in symbol_name:
        return "exception_constructor_thunk"
    if 'exception@std@@' in symbol_name:
        return "std_exception_class"

    # STL标准库模板实例化
    if any(std_pattern in symbol_name for std_pattern in [
        'std@@', '@std@@', 'basic_string@', 'basic_ostream@', 'basic_istream@',
        'allocator@', 'vector@', 'map@', 'set@', 'list@', 'deque@',
        '_Allocate@', 'is_constant_evaluated@std@@'
    ]):
        return "stl_template_instantiation"

    # wxWidgets框架符号
    if any(wx_pattern in symbol_name for wx_pattern in [
        'wxString@@', 'wxObject@@', 'wxEvt', 'wxApp@@', 'wxFrame@@', '??_EwxString@@'
    ]):
        return "wxwidgets_framework"

    # boost库符号
    if 'boost' in symbol_name:
        return "boost_library"

    # protobuf相关符号
    if any(pb_pattern in symbol_name for pb_pattern in [
        'protobuf', 'google@@', 'Message@', 'Descriptor@', 'TableStruct_',
        '.pb.', 'Serialize@', 'Deserialize@'
    ]):
        return "protobuf_library"

    # KiCad 特定符号
    if any(kicad_pattern in symbol_name for kicad_pattern in [
        'PCB_', 'SCH_', 'EDA_', 'BOARD_', 'FOOTPRINT_', 'PAD_', 'TRACK_'
    ]):
        return "kicad_specific"

    # 模板函数实例化 (通用)
    if '@@YA' in symbol_name or '@@3' in symbol_name:
        return "template_function_instantiation"

    # 运行时支持函数
    if any(runtime_pattern in symbol_name for runtime_pattern in [
        '_RTC_', '__ImageBase', '__security_', '__guard_'
    ]):
        return "runtime_support"

    # 其他重复符号
    return "other_duplicate"

def load_analysis_results():
    """加载分析结果"""
    analysis_file = Path("scripts/duplicate_symbols_analysis.json")
    if not analysis_file.exists():
        print(f"错误: 找不到分析结果文件 {analysis_file}")
        return None

    with open(analysis_file, 'r', encoding='utf-8') as f:
        return json.load(f)

def generate_common_symbols_file():
    """生成 common_symbols.txt 文件"""

    # 加载分析结果
    analysis = load_analysis_results()
    if not analysis:
        return

    # 准备分类数据
    categories = {}
    category_stats = {}

    print("正在重新分类符号...")

    # 处理top_10_duplicates（高重复次数符号）
    for item in analysis['top_10_duplicates']:
        symbol = item['symbol']
        count = item['duplicate_count']
        category = classify_symbol_detailed(symbol)

        if category not in categories:
            categories[category] = []
            category_stats[category] = {'count': 0, 'total_occurrences': 0}

        categories[category].append((symbol, count))
        category_stats[category]['count'] += 1
        category_stats[category]['total_occurrences'] += count

    # 处理其他类别中的符号
    for cat_name, symbols in analysis['categories'].items():
        for item in symbols:
            symbol = item['symbol']
            count = item['duplicate_count']

            # 重新分类 - 使用更详细的分类
            category = classify_symbol_detailed(symbol)

            if category not in categories:
                categories[category] = []
                category_stats[category] = {'count': 0, 'total_occurrences': 0}

            # 避免重复添加已经在top_10中的符号
            existing_symbols = set(sym for sym, _ in categories[category])
            if symbol not in existing_symbols:
                categories[category].append((symbol, count))
                category_stats[category]['count'] += 1
                category_stats[category]['total_occurrences'] += count

    # 生成输出文件
    output_file = Path("scripts/common_symbols.txt")
    print(f"正在生成 {output_file}...")

    with open(output_file, 'w', encoding='utf-8') as f:
        # 写入文件头
        f.write("# 编译器生成的重复符号 - 这些符号会在多个编译单元中出现\n")
        f.write("# 格式：符号名称 [tab] 分类说明 [tab] 重复次数\n")
        f.write(f"# 生成时间: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(f"# 总重复符号数: {analysis['summary']['total_duplicate_symbols']}\n")
        f.write(f"# 总重复实例数: {analysis['summary']['total_duplicate_instances']}\n")
        f.write("\n")

        # 按优先级排序类别
        priority_order = [
            "global_operator_delete",
            "global_operator_new",
            "global_operator_new_delete",
            "compiler_generated_scalar_destructor",
            "compiler_generated_vector_destructor",
            "compiler_generated_virtual_destructor",
            "string_literal_constant",
            "rtti_type_descriptor",
            "rtti_base_class_descriptor",
            "rtti_symbols",
            "vtable_symbols",
            "vftable_symbols",
            "std_exception_class",
            "exception_constructor_thunk",
            "stl_template_instantiation",
            "wxwidgets_framework",
            "user_constructor",
            "user_destructor",
            "assignment_operator",
            "template_function_instantiation",
            "protobuf_library",
            "kicad_specific",
            "boost_library",
            "runtime_support",
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
                    # 对于极长的符号名，进行适当截断显示
                    display_symbol = symbol
                    if len(symbol) > 200:
                        display_symbol = symbol[:200] + "..."

                    f.write(f"{symbol}\t{category}\t{count}\n")

    # 输出统计信息
    print("\n分类统计:")
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

    # 输出超高重复次数的符号（重复次数 > 500）
    print("\n超高重复次数符号 (>500次):")
    high_frequency_symbols = []
    for category, symbols in categories.items():
        for symbol, count in symbols:
            if count > 500:
                high_frequency_symbols.append((symbol, count, category))

    high_frequency_symbols.sort(key=lambda x: x[1], reverse=True)
    for symbol, count, category in high_frequency_symbols[:20]:
        display_symbol = symbol[:80] + "..." if len(symbol) > 80 else symbol
        print(f"  {count:4d}x  {category:<30}  {display_symbol}")

    return output_file

if __name__ == "__main__":
    generate_common_symbols_file()