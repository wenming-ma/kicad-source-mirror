#!/usr/bin/env python3
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / r"build"
SRCJ = BUILD / "minset_sources.json"
HDRJ = BUILD / "minset_headers.json"
UNRS = BUILD / "unresolved_symbols.json"
OUT = ROOT / "DEPENDENCIES_MINSET.md"


def main():
    # 读取分析结果
    try:
        srcs = json.loads(SRCJ.read_text(encoding="utf-8"))["sources"]
    except FileNotFoundError:
        print(f"错误: 找不到 {SRCJ}")
        return
    
    try:
        hdrs = json.loads(HDRJ.read_text(encoding="utf-8"))["headers"]
    except FileNotFoundError:
        print(f"错误: 找不到 {HDRJ}")
        return
        
    try:
        unres = json.loads(UNRS.read_text(encoding="utf-8")).get("unresolved_symbols", [])
    except FileNotFoundError:
        print(f"警告: 找不到 {UNRS}，假设无未解析符号")
        unres = []

    # 生成Markdown报告
    md = []
    md.append("# KiCad PCB最小可编译依赖集合分析报告")
    md.append("")
    md.append("**基于BOARD和FOOTPRINT核心数据的编译期依赖分析结果**")
    md.append("")
    md.append("## 📊 统计摘要")
    md.append("")
    md.append(f"- **最小源文件集合**: {len(srcs)} 个 .cpp/.cc 文件")
    md.append(f"- **最小头文件集合**: {len(hdrs)} 个 .h/.hpp 文件")
    md.append(f"- **未解析符号**: {len(unres)} 个 (需要外部库)")
    md.append("")
    
    md.append("## 🔧 最小源文件集合 (.cpp/.cc)")
    md.append("")
    md.append("以下源文件构成了BOARD/FOOTPRINT功能的最小编译单元：")
    md.append("")
    for i, s in enumerate(sorted(srcs), 1):
        # 转换为相对路径显示
        try:
            rel_path = Path(s).relative_to(ROOT)
            md.append(f"{i:3d}. `{rel_path}`")
        except ValueError:
            md.append(f"{i:3d}. `{s}`")
    md.append("")
    
    md.append("## 📁 最小头文件闭包 (.h/.hpp)")
    md.append("")
    md.append("以下头文件是上述源文件的完整依赖闭包：")
    md.append("")
    
    # 按目录分组显示头文件
    header_groups = {}
    for h in sorted(hdrs):
        try:
            rel_path = Path(h).relative_to(ROOT)
            parent = str(rel_path.parent)
            if parent == ".":
                parent = "根目录"
        except ValueError:
            rel_path = Path(h)
            parent = "外部路径"
        
        if parent not in header_groups:
            header_groups[parent] = []
        header_groups[parent].append(str(rel_path))
    
    for group, headers in sorted(header_groups.items()):
        md.append(f"### {group}/")
        md.append("")
        for h in sorted(headers):
            md.append(f"- `{h}`")
        md.append("")
    
    md.append("## ⚠️ 未解析符号 (外部依赖)")
    md.append("")
    if unres:
        md.append("以下符号需要通过外部库或vcpkg安装包提供：")
        md.append("")
        
        # 按符号前缀分组
        symbol_groups = {}
        for sym in unres[:500]:  # 限制显示数量
            if sym.startswith("std::"):
                group = "C++标准库"
            elif sym.startswith(("boost::", "__boost")):
                group = "Boost库"
            elif sym.startswith(("Qt", "Q")):
                group = "Qt框架"
            elif sym.startswith(("wx", "WX", "WXDLLIMPEXP")):
                group = "wxWidgets"
            elif sym.startswith(("__imp_", "??_")):
                group = "Windows/MSVC运行时"
            elif any(sym.startswith(p) for p in ["SSL_", "EVP_", "CRYPTO_"]):
                group = "OpenSSL"
            elif sym.startswith(("sqlite3_", "SQLITE_")):
                group = "SQLite"
            elif sym.startswith("cairo_"):
                group = "Cairo图形库"
            else:
                group = "其他"
            
            if group not in symbol_groups:
                symbol_groups[group] = []
            symbol_groups[group].append(sym)
        
        for group, symbols in sorted(symbol_groups.items()):
            md.append(f"### {group}")
            md.append("")
            for sym in sorted(symbols)[:50]:  # 每组最多显示50个
                md.append(f"- `{sym}`")
            if len(symbols) > 50:
                md.append(f"- ... 还有 {len(symbols) - 50} 个符号")
            md.append("")
        
        if len(unres) > 500:
            md.append(f"**注意**: 总共有 {len(unres)} 个未解析符号，此处仅显示前500个")
            md.append("")
    else:
        md.append("✅ 没有未解析符号！所有依赖都已在最小集合中解决。")
        md.append("")
    
    md.append("## 🚀 使用建议")
    md.append("")
    md.append("### 1. 源文件复制")
    md.append("```bash")
    md.append("# 将最小源文件集合复制到目标项目")
    md.append("# 建议保持相对目录结构")
    md.append("```")
    md.append("")
    md.append("### 2. 头文件复制")
    md.append("```bash")
    md.append("# 将头文件闭包复制到目标项目")
    md.append("# 注意保持include路径的一致性")
    md.append("```")
    md.append("")
    md.append("### 3. 外部依赖安装")
    md.append("通过vcpkg安装以下包来解决外部符号依赖：")
    md.append("- wxwidgets (如果使用Qt替代，可能不需要)")
    md.append("- boost")
    md.append("- openssl")
    md.append("- sqlite3")
    md.append("- cairo")
    md.append("- 其他根据未解析符号确定的包")
    md.append("")
    
    md.append("## ⚠️ 重要提醒")
    md.append("")
    md.append("1. **编译验证**: 此分析基于静态依赖，复制后需要验证编译通过")
    md.append("2. **功能测试**: 最小集合可能缺少运行时动态加载的组件")
    md.append("3. **增量添加**: 如果功能不完整，可以以此为基础逐步添加缺失部分")
    md.append("4. **版本兼容**: 确保外部库版本与KiCad编译环境兼容")
    md.append("")
    
    md.append("---")
    md.append(f"**报告生成时间**: {Path(__file__).stat().st_mtime}")
    md.append("**工具**: KiCad依赖分析工具链 (clang-scan-deps + 符号分析)")
    md.append("**目标**: KiCad Qt移植项目最小依赖集合")
    
    # 写入文件
    OUT.write_text("\n".join(md), encoding="utf-8")
    print(f"生成报告: {OUT}")
    print(f"统计: {len(srcs)} 源文件, {len(hdrs)} 头文件, {len(unres)} 未解析符号")


if __name__ == "__main__":
    main()