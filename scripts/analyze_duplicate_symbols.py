#!/usr/bin/env python3
"""
符号重复分析工具
分析 tu_index.json 文件，找出在多个源文件中定义的相同符号
"""

import json
import sys
from collections import defaultdict, Counter
from pathlib import Path
import os

def load_tu_index(filepath):
    """加载 tu_index.json 文件"""
    print(f"正在加载 {filepath}...")
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            data = json.load(f)
        print(f"成功加载，包含 {len(data.get('items', []))} 个编译单元")
        return data
    except Exception as e:
        print(f"加载文件失败: {e}")
        return None

def analyze_symbol_duplicates(tu_data):
    """分析符号重复情况"""
    print("开始分析符号重复...")

    # 符号到源文件的映射
    symbol_to_sources = defaultdict(list)

    # 统计信息
    total_items = len(tu_data.get('items', []))
    processed = 0

    for item in tu_data.get('items', []):
        src_file = item.get('src', '')
        defined_symbols = item.get('defined', [])

        # 提取源文件名（不包含路径）
        src_name = os.path.basename(src_file)

        for symbol in defined_symbols:
            symbol_to_sources[symbol].append({
                'src_file': src_file,
                'src_name': src_name
            })

        processed += 1
        if processed % 100 == 0:
            print(f"已处理 {processed}/{total_items} 个编译单元...")

    print(f"分析完成，共处理 {processed} 个编译单元")
    return symbol_to_sources

def find_duplicate_symbols(symbol_to_sources):
    """找出重复符号"""
    print("查找重复符号...")

    duplicate_symbols = {}
    for symbol, sources in symbol_to_sources.items():
        if len(sources) > 1:
            duplicate_symbols[symbol] = sources

    print(f"找到 {len(duplicate_symbols)} 个重复符号")
    return duplicate_symbols

def analyze_duplicate_patterns(duplicate_symbols):
    """分析重复符号的模式"""
    print("分析重复符号模式...")

    # 按重复次数统计
    duplicate_counts = Counter()
    for symbol, sources in duplicate_symbols.items():
        duplicate_counts[len(sources)] += 1

    # 符号类型模式分析
    symbol_patterns = {
        'constructor': 0,        # ??0
        'destructor': 0,         # ??1
        'assignment': 0,         # ??4
        'vtable': 0,            # ??_7
        'vftable': 0,           # ??_6
        'typeinfo': 0,          # ??_R
        'template': 0,          # ?func@@YA
        'global_delete': 0,     # __empty_global_delete
        'global_new': 0,        # __empty_global_new
        'protobuf': 0,          # protobuf相关
        'other': 0
    }

    for symbol in duplicate_symbols.keys():
        if '??0' in symbol:
            symbol_patterns['constructor'] += 1
        elif '??1' in symbol:
            symbol_patterns['destructor'] += 1
        elif '??4' in symbol:
            symbol_patterns['assignment'] += 1
        elif '??_7' in symbol:
            symbol_patterns['vtable'] += 1
        elif '??_6' in symbol:
            symbol_patterns['vftable'] += 1
        elif '??_R' in symbol:
            symbol_patterns['typeinfo'] += 1
        elif '__empty_global_delete' in symbol:
            symbol_patterns['global_delete'] += 1
        elif '__empty_global_new' in symbol:
            symbol_patterns['global_new'] += 1
        elif 'protobuf' in symbol or '.pb.' in symbol:
            symbol_patterns['protobuf'] += 1
        elif symbol.startswith('?') and '@@YA' in symbol:
            symbol_patterns['template'] += 1
        else:
            symbol_patterns['other'] += 1

    return duplicate_counts, symbol_patterns

def get_top_duplicates(duplicate_symbols, n=10):
    """获取重复次数最多的前N个符号"""
    sorted_duplicates = sorted(
        duplicate_symbols.items(),
        key=lambda x: len(x[1]),
        reverse=True
    )
    return sorted_duplicates[:n]

def simplify_symbol_name(symbol):
    """简化符号名称以便阅读"""
    # 移除复杂的模板参数
    if '@@' in symbol:
        parts = symbol.split('@@')
        if len(parts) > 1:
            return parts[0] + '@@...'
    return symbol

def print_analysis_report(duplicate_symbols, duplicate_counts, symbol_patterns, top_duplicates):
    """打印分析报告"""
    print("\n" + "="*80)
    print("符号重复分析报告")
    print("="*80)

    # 总体统计
    print(f"\n[总体统计]:")
    print(f"  - 总重复符号数量: {len(duplicate_symbols)}")
    total_duplicate_instances = sum(len(sources) for sources in duplicate_symbols.values())
    print(f"  - 重复实例总数: {total_duplicate_instances}")

    # 重复次数分布
    print(f"\n[重复次数分布]:")
    for count in sorted(duplicate_counts.keys(), reverse=True):
        print(f"  - 重复 {count} 次的符号: {duplicate_counts[count]} 个")

    # 符号类型模式
    print(f"\n[符号类型模式]:")
    for pattern, count in sorted(symbol_patterns.items(), key=lambda x: x[1], reverse=True):
        if count > 0:
            print(f"  - {pattern}: {count} 个")

    # 前10个重复最多的符号
    print(f"\n[重复次数最多的前10个符号]:")
    for i, (symbol, sources) in enumerate(top_duplicates, 1):
        simplified_symbol = simplify_symbol_name(symbol)
        print(f"\n{i:2d}. {simplified_symbol}")
        print(f"    重复次数: {len(sources)}")
        print(f"    定义文件:")

        # 按文件名分组显示
        source_names = [src['src_name'] for src in sources]
        unique_names = list(set(source_names))
        unique_names.sort()

        for name in unique_names[:5]:  # 只显示前5个文件名
            count = source_names.count(name)
            print(f"      - {name} ({count}次)")

        if len(unique_names) > 5:
            print(f"      - ... 还有 {len(unique_names) - 5} 个文件")

def analyze_symbol_categories(duplicate_symbols):
    """分析重复符号的具体类别"""
    categories = {
        'global_operators': [],      # 全局操作符
        'template_instantiations': [],  # 模板实例化
        'inline_functions': [],      # 内联函数
        'constructor_destructor': [], # 构造析构函数
        'vtable_related': [],        # 虚表相关
        'protobuf_generated': [],    # protobuf生成代码
        'runtime_support': [],       # 运行时支持
        'unknown': []               # 未知类别
    }

    for symbol, sources in duplicate_symbols.items():
        if '__empty_global_' in symbol or '?__empty_global_' in symbol:
            categories['global_operators'].append((symbol, sources))
        elif 'protobuf' in symbol or '.pb.' in symbol or 'TableStruct_' in symbol:
            categories['protobuf_generated'].append((symbol, sources))
        elif '??0' in symbol or '??1' in symbol:  # 构造/析构函数
            categories['constructor_destructor'].append((symbol, sources))
        elif '??_7' in symbol or '??_6' in symbol or '??_R' in symbol:  # 虚表相关
            categories['vtable_related'].append((symbol, sources))
        elif '_RTC_' in symbol or '__ImageBase' in symbol:
            categories['runtime_support'].append((symbol, sources))
        elif '@@YA' in symbol or '@@3' in symbol:  # 可能的模板或全局变量
            categories['template_instantiations'].append((symbol, sources))
        else:
            categories['unknown'].append((symbol, sources))

    return categories

def print_category_analysis(categories):
    """打印类别分析结果"""
    print(f"\n[符号类别详细分析]:")

    category_names = {
        'global_operators': '全局操作符 (new/delete)',
        'template_instantiations': '模板实例化',
        'inline_functions': '内联函数',
        'constructor_destructor': '构造/析构函数',
        'vtable_related': '虚表相关',
        'protobuf_generated': 'Protobuf生成代码',
        'runtime_support': '运行时支持',
        'unknown': '未知类别'
    }

    for category, symbols in categories.items():
        if symbols:
            print(f"\n  - {category_names[category]} ({len(symbols)} 个符号):")

            # 显示该类别中重复次数最多的前3个符号
            sorted_symbols = sorted(symbols, key=lambda x: len(x[1]), reverse=True)
            for i, (symbol, sources) in enumerate(sorted_symbols[:3]):
                simplified = simplify_symbol_name(symbol)
                print(f"    {i+1}. {simplified} (重复{len(sources)}次)")

def main():
    """主函数"""
    # 获取 tu_index.json 文件路径
    script_dir = Path(__file__).parent
    tu_index_path = script_dir / 'tu_index.json'

    if not tu_index_path.exists():
        print(f"错误: 找不到文件 {tu_index_path}")
        sys.exit(1)

    # 加载数据
    tu_data = load_tu_index(tu_index_path)
    if not tu_data:
        sys.exit(1)

    # 分析符号重复
    symbol_to_sources = analyze_symbol_duplicates(tu_data)
    duplicate_symbols = find_duplicate_symbols(symbol_to_sources)

    if not duplicate_symbols:
        print("未发现重复符号")
        return

    # 分析模式
    duplicate_counts, symbol_patterns = analyze_duplicate_patterns(duplicate_symbols)
    top_duplicates = get_top_duplicates(duplicate_symbols, 10)

    # 分析类别
    categories = analyze_symbol_categories(duplicate_symbols)

    # 打印报告
    print_analysis_report(duplicate_symbols, duplicate_counts, symbol_patterns, top_duplicates)
    print_category_analysis(categories)

    # 保存详细结果到文件
    output_file = script_dir / 'duplicate_symbols_analysis.json'
    print(f"\n[保存详细分析结果到]: {output_file}")

    analysis_result = {
        'summary': {
            'total_duplicate_symbols': len(duplicate_symbols),
            'total_duplicate_instances': sum(len(sources) for sources in duplicate_symbols.values()),
            'duplicate_count_distribution': dict(duplicate_counts),
            'symbol_type_patterns': symbol_patterns
        },
        'top_10_duplicates': [
            {
                'symbol': symbol,
                'duplicate_count': len(sources),
                'source_files': [src['src_file'] for src in sources]
            }
            for symbol, sources in top_duplicates
        ],
        'categories': {
            category: [
                {
                    'symbol': symbol,
                    'duplicate_count': len(sources),
                    'source_files': [src['src_file'] for src in sources]
                }
                for symbol, sources in symbols
            ]
            for category, symbols in categories.items()
        }
    }

    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(analysis_result, f, indent=2, ensure_ascii=False)

    print("\n[分析完成]!")

if __name__ == '__main__':
    main()