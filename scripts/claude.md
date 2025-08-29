# KiCad PCB 最小依赖集合分析工具

## 🎯 项目目标
**基于编译期依赖分析，从BOARD和FOOTPRINT核心数据相关种子文件开始，通过依赖闭包分析自动计算出最小可编译源文件集合**

改变策略：不再通过试错编译收集文件，而是使用科学的依赖分析方法，精确计算出最小依赖集合。

## 📊 分析策略概述
- **分析方法**: 编译期依赖分析 (clang-scan-deps + 符号分析)
- **种子文件**: KiCad BOARD和FOOTPRINT相关核心数据文件
- **输出结果**: 最小可编译源文件集合 + 最小头文件集合
- **工具链**: LLVM/Clang分析工具 + Python处理脚本

## 🛠️ 工具链组成

### 核心分析工具
1. **clang-scan-deps** - LLVM编译期依赖扫描工具
2. **llvm-nm** - 符号表分析工具  
3. **MSVC cl.exe** - 编译器(用于符号索引构建)
4. **Python 3.10+** - 脚本运行环境

### 分析脚本流水线
```
seeds.txt → 10_scan_src_deps.py → 20_build_tu_index.py → 30_resolve_minset.py → 40_emit_min_headers.py → 50_report_minset.py
```

## 📁 文件说明

### 输入文件
- **seeds.txt** - BOARD/FOOTPRINT相关种子源文件列表
- **compile_commands.json** - CMake生成的编译数据库

### 分析脚本
- **10_scan_src_deps.py** - 头文件闭包分析 (使用clang-scan-deps)
- **20_build_tu_index.py** - TU符号索引构建 (编译.obj + llvm-nm)
- **30_resolve_minset.py** - 最小源文件集合计算 (符号闭包算法)
- **40_emit_min_headers.py** - 最小头文件集合计算
- **50_report_minset.py** - 汇总报告生成

### 输出文件
- **deps_full.json** - clang-scan-deps完整输出
- **deps_src.json** - 种子文件头依赖闭包
- **tu_index.json** - 编译单元符号索引
- **minset_sources.json** - 最小源文件集合
- **minset_headers.json** - 最小头文件集合
- **unresolved_symbols.json** - 未解析符号(外部依赖)
- **DEPENDENCIES_MINSET.md** - 人类可读汇总报告

## 🔧 算法核心

### 1. 头文件闭包分析
使用clang-scan-deps分析种子文件的完整头文件依赖树，包括:
- 直接#include的头文件
- 间接依赖的头文件
- 生成头文件(如*.pb.h)

### 2. 符号索引构建
对每个编译单元(TU)：
- 编译到.obj文件(不链接)
- 使用llvm-nm提取定义的符号
- 提取未定义的符号
- 构建全局符号索引表

### 3. 符号闭包算法
```
S = seeds                      # 初始种子集合
while True:
    provided = ∪defined(s) for s in S    # 已提供符号
    required = ∪undefined(s) for s in S  # 需要的符号
    missing = required - provided         # 缺失符号
    missing = filter_external(missing)   # 过滤外部库符号
    if not missing: break               # 收敛
    candidates = find_providers(missing) # 找到提供者
    S = S ∪ candidates                   # 加入候选者
```

### 4. 外部依赖过滤
自动识别并过滤以下符号模式：
- `std::*` - 标准库
- `boost::*` - Boost库
- `Qt*`, `Q*` - Qt库
- Windows API符号
- 其他已知第三方库符号

## 🚀 使用方法

### 环境准备
确保在"x64 Native Tools Command Prompt for VS"中执行，环境变量包含：
- LLVM工具链(clang-scan-deps, llvm-nm)
- MSVC编译器(cl.exe)
- Python 3.10+

### 执行步骤
```powershell
# 1. 确保编译数据库存在
ls build\x64-Debug\compile_commands.json

# 2. 检查种子文件
type scripts\seeds.txt

# 3. 手动执行依赖扫描(如遇问题)
clang-scan-deps -compilation-database=build\x64-Debug\compile_commands.json -format=experimental-full > build\x64-Debug\deps_full.json

# 4. 执行完整分析流水线
python scripts\10_scan_src_deps.py
python scripts\20_build_tu_index.py  
python scripts\30_resolve_minset.py
python scripts\40_emit_min_headers.py
python scripts\50_report_minset.py

# 5. 查看结果
type DEPENDENCIES_MINSET.md
```

### 一键执行
```powershell
.\scripts\run_all.ps1
```

## 📊 预期输出规模
基于BOARD/FOOTPRINT核心的预估：
- **最小源文件集合**: 50-200个.cpp文件
- **最小头文件集合**: 200-800个.h文件
- **分析处理时间**: 5-15分钟
- **未解析外部符号**: 通常100-500个(需vcpkg提供)

## 🔍 故障排除

### clang-scan-deps问题
- **超时**: 减少并发(-j 1)或分批处理
- **汇编文件错误**: 正常，会被自动过滤
- **资源文件错误**: 正常，不影响C++依赖分析

### 符号解析问题
- **未收敛**: 检查外部库符号过滤规则
- **循环依赖**: 算法自动处理
- **缺失符号**: 通常为生成代码或外部库

### 路径问题
- 使用绝对路径
- Windows路径使用反斜杠
- 注意路径中的空格

## 🎯 应用场景
1. **Qt移植项目** - 确定需要移植的最小KiCad代码集合
2. **模块化重构** - 理解代码依赖关系
3. **性能优化** - 减少编译时间和二进制大小
4. **依赖审计** - 分析第三方库使用情况

## ⚠️ 注意事项
- 此分析基于编译期静态依赖，不包括运行时动态加载
- 结果需要配合实际测试验证功能完整性
- 外部库依赖需要单独安装(通过vcpkg)
- 生成的集合是"可编译"的最小集合，不保证运行时功能完整

---
**Created by Claude Code for KiCad Qt Migration Project**