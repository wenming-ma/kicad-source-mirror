#!/usr/bin/env python3
"""
符号分布分析工具
分析 tu_index.json 中符号在 defined 和 undefined 字段的分布情况
"""
import json
import sys
from collections import defaultdict, Counter
from typing import Dict, Set, List, Tuple

def load_tu_index(file_path: str) -> Dict:
    """加载 tu_index.json 文件"""
    print(f"正在加载文件: {file_path}")
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    print(f"加载完成，包含 {len(data['items'])} 个翻译单元")
    return data

def analyze_symbol_distribution(tu_data: Dict) -> Dict:
    """分析符号分布"""
    # 符号出现统计
    symbol_defined_in = defaultdict(set)    # 符号 -> 定义它的文件集合
    symbol_undefined_in = defaultdict(set)  # 符号 -> 需要它但未定义的文件集合

    # 文件路径简化映射
    file_short_names = {}

    print("正在分析符号分布...")

    for i, item in enumerate(tu_data['items']):
        if i % 100 == 0:
            print(f"处理进度: {i}/{len(tu_data['items'])}")

        src_file = item['src']
        # 简化文件路径
        short_name = src_file.split('\\')[-1] if '\\' in src_file else src_file.split('/')[-1]
        file_short_names[src_file] = short_name

        # 收集定义的符号
        for symbol in item.get('defined', []):
            symbol_defined_in[symbol].add(src_file)

        # 收集未定义的符号
        for symbol in item.get('undefined', []):
            symbol_undefined_in[symbol].add(src_file)

    print("分析完成")
    return {
        'symbol_defined_in': symbol_defined_in,
        'symbol_undefined_in': symbol_undefined_in,
        'file_short_names': file_short_names
    }

def find_target_symbols(analysis_result: Dict) -> List[str]:
    """查找目标符号进行详细分析"""
    symbol_defined_in = analysis_result['symbol_defined_in']
    symbol_undefined_in = analysis_result['symbol_undefined_in']

    target_symbols = []

    # 查找 wxString 相关符号
    for symbol in symbol_defined_in.keys():
        if 'wxString' in symbol and symbol.startswith('??'):
            target_symbols.append(symbol)
            if len(target_symbols) >= 3:
                break

    # 查找 DIALOG_CONFIGURE_PATHS 构造函数
    dialog_symbol = "??0DIALOG_CONFIGURE_PATHS@@QEAA@PEAVwxWindow@@@Z"
    if dialog_symbol in symbol_defined_in or dialog_symbol in symbol_undefined_in:
        target_symbols.append(dialog_symbol)

    # 查找一些常见的重复符号
    common_patterns = ['??_E', '??0', '??1', '??4']  # 析构、构造、析构、赋值
    for symbol in list(symbol_defined_in.keys())[:1000]:  # 只检查前1000个符号以节省时间
        for pattern in common_patterns:
            if symbol.startswith(pattern) and len(symbol_defined_in[symbol]) > 1:
                target_symbols.append(symbol)
                break
        if len(target_symbols) >= 8:
            break

    return list(set(target_symbols))  # 去重

def analyze_symbol_details(symbol: str, analysis_result: Dict) -> Dict:
    """分析单个符号的详细分布"""
    symbol_defined_in = analysis_result['symbol_defined_in']
    symbol_undefined_in = analysis_result['symbol_undefined_in']
    file_short_names = analysis_result['file_short_names']

    defined_files = symbol_defined_in.get(symbol, set())
    undefined_files = symbol_undefined_in.get(symbol, set())

    # 简化文件名显示
    defined_short = [file_short_names.get(f, f) for f in defined_files]
    undefined_short = [file_short_names.get(f, f) for f in undefined_files]

    return {
        'symbol': symbol,
        'defined_count': len(defined_files),
        'undefined_count': len(undefined_files),
        'defined_files': sorted(defined_short),
        'undefined_files': sorted(undefined_short),
        'cross_reference': len(defined_files & undefined_files),  # 同时在defined和undefined中的文件数
        'total_files': len(defined_files | undefined_files)
    }

def print_symbol_analysis(symbol_details: Dict):
    """打印符号分析结果"""
    print(f"\n{'='*80}")
    print(f"符号: {symbol_details['symbol']}")
    print(f"{'='*80}")

    print(f"定义次数: {symbol_details['defined_count']} 个文件")
    if symbol_details['defined_files']:
        print("定义文件:")
        for f in symbol_details['defined_files'][:10]:  # 最多显示10个
            print(f"  - {f}")
        if len(symbol_details['defined_files']) > 10:
            print(f"  ... 还有 {len(symbol_details['defined_files']) - 10} 个文件")

    print(f"\n引用次数: {symbol_details['undefined_count']} 个文件")
    if symbol_details['undefined_files']:
        print("引用文件:")
        for f in symbol_details['undefined_files'][:10]:  # 最多显示10个
            print(f"  - {f}")
        if len(symbol_details['undefined_files']) > 10:
            print(f"  ... 还有 {len(symbol_details['undefined_files']) - 10} 个文件")

    print(f"\n交叉引用: {symbol_details['cross_reference']} 个文件")
    print(f"总影响文件数: {symbol_details['total_files']}")

def generate_distribution_summary(analysis_result: Dict) -> Dict:
    """生成分布总结"""
    symbol_defined_in = analysis_result['symbol_defined_in']
    symbol_undefined_in = analysis_result['symbol_undefined_in']

    # 统计各种分布模式
    only_defined = 0      # 只在defined中出现
    only_undefined = 0    # 只在undefined中出现
    both_fields = 0       # 同时在两个字段中出现

    multiple_definitions = 0  # 在多个文件中定义
    high_usage = 0           # 被多个文件引用

    all_symbols = set(symbol_defined_in.keys()) | set(symbol_undefined_in.keys())

    for symbol in all_symbols:
        defined_count = len(symbol_defined_in.get(symbol, set()))
        undefined_count = len(symbol_undefined_in.get(symbol, set()))

        if defined_count > 0 and undefined_count == 0:
            only_defined += 1
        elif defined_count == 0 and undefined_count > 0:
            only_undefined += 1
        elif defined_count > 0 and undefined_count > 0:
            both_fields += 1

        if defined_count > 1:
            multiple_definitions += 1

        if undefined_count > 5:
            high_usage += 1

    return {
        'total_symbols': len(all_symbols),
        'only_defined': only_defined,
        'only_undefined': only_undefined,
        'both_fields': both_fields,
        'multiple_definitions': multiple_definitions,
        'high_usage': high_usage
    }

def main():
    if len(sys.argv) < 2:
        tu_index_path = "scripts/tu_index.json"
    else:
        tu_index_path = sys.argv[1]

    try:
        # 1. 加载数据
        tu_data = load_tu_index(tu_index_path)

        # 2. 分析符号分布
        analysis_result = analyze_symbol_distribution(tu_data)

        # 3. 生成总体分布摘要
        print("\n" + "="*80)
        print("符号分布总体摘要")
        print("="*80)
        summary = generate_distribution_summary(analysis_result)

        print(f"总符号数量: {summary['total_symbols']:,}")
        print(f"只在defined中的符号: {summary['only_defined']:,} ({summary['only_defined']/summary['total_symbols']*100:.1f}%)")
        print(f"只在undefined中的符号: {summary['only_undefined']:,} ({summary['only_undefined']/summary['total_symbols']*100:.1f}%)")
        print(f"同时在两个字段的符号: {summary['both_fields']:,} ({summary['both_fields']/summary['total_symbols']*100:.1f}%)")
        print(f"多重定义符号: {summary['multiple_definitions']:,}")
        print(f"高使用率符号 (>5个文件引用): {summary['high_usage']:,}")

        # 4. 查找目标符号
        print("\n正在查找目标符号进行详细分析...")
        target_symbols = find_target_symbols(analysis_result)
        print(f"找到 {len(target_symbols)} 个目标符号")

        # 5. 详细分析目标符号
        for symbol in target_symbols:
            details = analyze_symbol_details(symbol, analysis_result)
            print_symbol_analysis(details)

        # 6. 特殊分析：查找 DIALOG_CONFIGURE_PATHS 相关符号
        print(f"\n{'='*80}")
        print("DIALOG_CONFIGURE_PATHS 相关符号分析")
        print(f"{'='*80}")

        dialog_symbols = []
        for symbol in analysis_result['symbol_defined_in'].keys():
            if 'DIALOG_CONFIGURE_PATHS' in symbol:
                dialog_symbols.append(symbol)

        print(f"找到 {len(dialog_symbols)} 个 DIALOG_CONFIGURE_PATHS 相关符号:")
        for symbol in dialog_symbols:
            details = analyze_symbol_details(symbol, analysis_result)
            print(f"\n符号: {symbol}")
            print(f"  定义: {details['defined_count']} 文件, 引用: {details['undefined_count']} 文件")

    except Exception as e:
        print(f"错误: {e}")
        return 1

    return 0

if __name__ == "__main__":
    sys.exit(main())