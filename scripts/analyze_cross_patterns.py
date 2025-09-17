#!/usr/bin/env python3
"""
符号交叉分析工具
详细分析符号在 defined/undefined 字段中的交叉模式和技术原因
"""
import json
import sys
from collections import defaultdict, Counter
from typing import Dict, Set, List, Tuple

def load_tu_index(file_path: str) -> Dict:
    """加载 tu_index.json 文件"""
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    return data

def analyze_cross_reference_patterns(tu_data: Dict) -> Dict:
    """分析交叉引用模式"""
    symbol_defined_in = defaultdict(set)
    symbol_undefined_in = defaultdict(set)
    file_to_symbols = defaultdict(lambda: {'defined': set(), 'undefined': set()})

    for item in tu_data['items']:
        src_file = item['src']

        for symbol in item.get('defined', []):
            symbol_defined_in[symbol].add(src_file)
            file_to_symbols[src_file]['defined'].add(symbol)

        for symbol in item.get('undefined', []):
            symbol_undefined_in[symbol].add(src_file)
            file_to_symbols[src_file]['undefined'].add(symbol)

    return {
        'symbol_defined_in': symbol_defined_in,
        'symbol_undefined_in': symbol_undefined_in,
        'file_to_symbols': file_to_symbols
    }

def find_cross_reference_symbols(analysis_result: Dict) -> List[Tuple[str, Dict]]:
    """查找同时在defined和undefined中出现的符号"""
    symbol_defined_in = analysis_result['symbol_defined_in']
    symbol_undefined_in = analysis_result['symbol_undefined_in']

    cross_symbols = []

    for symbol in symbol_defined_in.keys():
        if symbol in symbol_undefined_in:
            defined_files = symbol_defined_in[symbol]
            undefined_files = symbol_undefined_in[symbol]

            # 检查是否有文件同时定义和引用同一符号（这通常不正常）
            overlap_files = defined_files & undefined_files

            cross_info = {
                'symbol': symbol,
                'defined_files': defined_files,
                'undefined_files': undefined_files,
                'overlap_files': overlap_files,
                'total_defined': len(defined_files),
                'total_undefined': len(undefined_files),
                'total_overlap': len(overlap_files)
            }
            cross_symbols.append((symbol, cross_info))

    # 按影响文件数排序
    cross_symbols.sort(key=lambda x: x[1]['total_defined'] + x[1]['total_undefined'], reverse=True)
    return cross_symbols

def analyze_symbol_type(symbol: str) -> Dict:
    """分析符号类型"""
    symbol_type = "unknown"
    description = ""

    if symbol.startswith('??0'):
        symbol_type = "constructor"
        description = "构造函数"
    elif symbol.startswith('??1'):
        symbol_type = "destructor"
        description = "析构函数"
    elif symbol.startswith('??4'):
        symbol_type = "assignment"
        description = "赋值操作符"
    elif symbol.startswith('??_7'):
        symbol_type = "vtable"
        description = "虚函数表"
    elif symbol.startswith('??_G'):
        symbol_type = "scalar_deleting_destructor"
        description = "标量删除析构函数"
    elif symbol.startswith('??_E'):
        symbol_type = "vector_deleting_destructor"
        description = "向量删除析构函数"
    elif symbol.startswith('??_R'):
        symbol_type = "rtti"
        description = "运行时类型信息"
    elif symbol.startswith('?'):
        symbol_type = "function_or_variable"
        description = "函数或变量"
    elif symbol.startswith('_'):
        symbol_type = "c_symbol"
        description = "C符号或外部符号"
    elif 'vtable' in symbol.lower():
        symbol_type = "vtable"
        description = "虚函数表"
    elif 'typeinfo' in symbol.lower():
        symbol_type = "typeinfo"
        description = "类型信息"

    return {
        'type': symbol_type,
        'description': description
    }

def explain_cross_reference_reason(symbol_info: Dict) -> str:
    """解释交叉引用的技术原因"""
    symbol = symbol_info['symbol']
    symbol_analysis = analyze_symbol_type(symbol)

    reasons = []

    # 多重定义的原因
    if symbol_info['total_defined'] > 1:
        if symbol_analysis['type'] == 'constructor':
            reasons.append("构造函数可能在多个编译单元中实例化（模板或内联函数）")
        elif symbol_analysis['type'] == 'destructor':
            reasons.append("析构函数可能在多个编译单元中实例化")
        elif symbol_analysis['type'] == 'vtable':
            reasons.append("虚函数表可能在多个编译单元中生成（weak symbols）")
        elif symbol_analysis['type'] == 'rtti':
            reasons.append("RTTI信息可能在多个编译单元中生成")
        elif 'template' in symbol or '?' in symbol:
            reasons.append("模板实例化导致在多个编译单元中定义")
        else:
            reasons.append("可能是内联函数或模板导致的多重定义")

    # 交叉引用的原因
    if symbol_info['total_undefined'] > 0:
        if symbol_analysis['type'] in ['constructor', 'destructor']:
            reasons.append("其他编译单元需要调用此构造/析构函数")
        elif symbol_analysis['type'] == 'function_or_variable':
            reasons.append("其他编译单元需要此函数或变量")
        elif symbol_analysis['type'] == 'vtable':
            reasons.append("其他编译单元的虚函数调用需要此虚函数表")
        else:
            reasons.append("其他编译单元依赖此符号")

    # 同文件定义和引用的原因
    if symbol_info['total_overlap'] > 0:
        reasons.append("WARNING: 同一文件既定义又引用同一符号，可能是分析错误或特殊情况")

    return "; ".join(reasons) if reasons else "未知原因"

def get_file_basename(file_path: str) -> str:
    """获取文件基本名称"""
    return file_path.split('\\')[-1] if '\\' in file_path else file_path.split('/')[-1]

def print_cross_reference_analysis(cross_symbols: List[Tuple[str, Dict]], limit: int = 10):
    """打印交叉引用分析结果"""
    print(f"\n{'='*100}")
    print(f"符号交叉引用分析 (显示前 {limit} 个)")
    print(f"{'='*100}")

    for i, (symbol, info) in enumerate(cross_symbols[:limit]):
        print(f"\n{'-'*80}")
        print(f"#{i+1} 符号: {symbol}")
        print(f"{'-'*80}")

        symbol_analysis = analyze_symbol_type(symbol)
        print(f"符号类型: {symbol_analysis['description']} ({symbol_analysis['type']})")

        print(f"定义文件数: {info['total_defined']}")
        if info['defined_files']:
            defined_names = [get_file_basename(f) for f in sorted(info['defined_files'])]
            print(f"定义文件: {', '.join(defined_names[:5])}")
            if len(defined_names) > 5:
                print(f"          ... 还有 {len(defined_names) - 5} 个文件")

        print(f"引用文件数: {info['total_undefined']}")
        if info['undefined_files']:
            undefined_names = [get_file_basename(f) for f in sorted(info['undefined_files'])]
            print(f"引用文件: {', '.join(undefined_names[:5])}")
            if len(undefined_names) > 5:
                print(f"          ... 还有 {len(undefined_names) - 5} 个文件")

        if info['total_overlap'] > 0:
            overlap_names = [get_file_basename(f) for f in sorted(info['overlap_files'])]
            print(f"重叠文件: {', '.join(overlap_names)} (同时定义和引用)")

        # 技术解释
        reason = explain_cross_reference_reason(info)
        print(f"技术原因: {reason}")

def analyze_specific_symbol(symbol_name: str, analysis_result: Dict):
    """分析特定符号"""
    symbol_defined_in = analysis_result['symbol_defined_in']
    symbol_undefined_in = analysis_result['symbol_undefined_in']

    print(f"\n{'='*100}")
    print(f"特定符号分析: {symbol_name}")
    print(f"{'='*100}")

    if symbol_name not in symbol_defined_in and symbol_name not in symbol_undefined_in:
        print("符号未找到")
        return

    defined_files = symbol_defined_in.get(symbol_name, set())
    undefined_files = symbol_undefined_in.get(symbol_name, set())

    symbol_analysis = analyze_symbol_type(symbol_name)
    print(f"符号类型: {symbol_analysis['description']} ({symbol_analysis['type']})")

    print(f"\n定义情况: {len(defined_files)} 个文件")
    for f in sorted(defined_files):
        print(f"  - {get_file_basename(f)}")

    print(f"\n引用情况: {len(undefined_files)} 个文件")
    for f in sorted(undefined_files):
        print(f"  - {get_file_basename(f)}")

    # 分析合理性
    print(f"\n分布合理性分析:")
    if len(defined_files) == 1 and len(undefined_files) > 0:
        print("NORMAL: 正常模式 - 单一定义，多处引用")
    elif len(defined_files) > 1 and len(undefined_files) > 0:
        if symbol_analysis['type'] in ['constructor', 'destructor', 'vtable', 'rtti']:
            print("ACCEPTABLE: 可接受 - C++模板/内联导致的多重定义")
        else:
            print("WARNING: 需要注意 - 多重定义可能有问题")
    elif len(defined_files) > 0 and len(undefined_files) == 0:
        print("ISOLATED: 孤立定义 - 定义但无引用，可能是未使用符号")
    elif len(defined_files) == 0 and len(undefined_files) > 0:
        print("MISSING: 缺失定义 - 引用但无定义，可能是外部库符号")

def main():
    if len(sys.argv) < 2:
        tu_index_path = "scripts/tu_index.json"
    else:
        tu_index_path = sys.argv[1]

    # 特定符号分析列表
    specific_symbols = [
        "??_EwxString@@QEAAPEAXI@Z",  # wxString vector deleting destructor
        "??0DIALOG_CONFIGURE_PATHS@@QEAA@PEAVwxWindow@@@Z",  # DIALOG_CONFIGURE_PATHS constructor
    ]

    try:
        print("正在加载符号索引...")
        tu_data = load_tu_index(tu_index_path)
        print(f"加载完成，包含 {len(tu_data['items'])} 个翻译单元")

        print("正在分析交叉引用模式...")
        analysis_result = analyze_cross_reference_patterns(tu_data)

        print("正在查找交叉引用符号...")
        cross_symbols = find_cross_reference_symbols(analysis_result)
        print(f"找到 {len(cross_symbols)} 个交叉引用符号")

        # 打印交叉引用分析
        print_cross_reference_analysis(cross_symbols, limit=15)

        # 分析特定符号
        for symbol in specific_symbols:
            analyze_specific_symbol(symbol, analysis_result)

        # 总结分析
        print(f"\n{'='*100}")
        print("总结分析")
        print(f"{'='*100}")

        total_symbols = len(analysis_result['symbol_defined_in']) + len(analysis_result['symbol_undefined_in'])
        cross_count = len(cross_symbols)

        print(f"总符号数: {total_symbols:,}")
        print(f"交叉引用符号数: {cross_count:,} ({cross_count/total_symbols*100:.1f}%)")

        # 按类型统计
        type_stats = defaultdict(int)
        for symbol, info in cross_symbols:
            symbol_analysis = analyze_symbol_type(symbol)
            type_stats[symbol_analysis['type']] += 1

        print(f"\n交叉引用符号类型分布:")
        for symbol_type, count in sorted(type_stats.items(), key=lambda x: x[1], reverse=True):
            print(f"  {symbol_type}: {count}")

    except Exception as e:
        print(f"错误: {e}")
        import traceback
        traceback.print_exc()
        return 1

    return 0

if __name__ == "__main__":
    sys.exit(main())