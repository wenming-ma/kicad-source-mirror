# KiCad PCB依赖分析与最小集合提取指导

## 🎯 当前任务目标
**使用编译期依赖分析工具，从BOARD和FOOTPRINT核心数据相关的种子文件开始，通过依赖闭包分析找到最小可编译集合**

改变策略：不再通过试错编译来收集文件，而是使用科学的依赖分析方法，精确计算出最小依赖集合。

## 📊 新策略概述
- **分析方法**: 编译期依赖分析 (clang-scan-deps + 符号分析)
- **种子文件**: KiCad BOARD和FOOTPRINT相关的核心数据文件
- **输出结果**: 最小可编译源文件集合 + 最小头文件集合
- **工具链**: LLVM/Clang分析工具 + Python处理脚本

## 🛠️ 第三方依赖管理策略

### KiCad的第三方依赖有两种类型：

#### 1. **thirdparty/目录依赖** (内置第三方库)
- **位置**: `thirdparty/` 目录中
- **特点**: KiCad项目内置，与源码一起分发
- **示例**: clipper2, magic_enum, fmt, nlohmann_json等
- **处理方式**: 从KiCad源码复制到qt_pcb_project/thirdparty/目录

#### 2. **vcpkg依赖** (外部包管理器)
- **位置**: 通过vcpkg包管理器安装
- **特点**: 大型第三方库，通过包管理器管理版本
- **示例**: wxWidgets, boost, cairo, glew, opencascade等
- **处理方式**: 告知用户需要安装，用户会负责安装

### 🎯 依赖分析目标
**通过精确的依赖分析，一次性找到完整的最小依赖集合，避免试错编译的低效过程**

## 🛠️ 依赖分析工具链配置

### 第0步: 工具准备 (Windows)
在x64 Native Tools Command Prompt for VS中执行：

#### 必需工具
- **LLVM for Windows**: `winget install LLVM.LLVM -s winget`
  - 提供 clang-scan-deps, llvm-nm 等分析工具
- **Python 3.10+**: `winget install Python.Python.3`
- **CMake**: `winget install Kitware.CMake` (VS用户通常已有)
- **Ninja**: `winget install Ninja-build.Ninja` (VS用户通常已有)

#### 可选工具
- **ripgrep**: `winget install BurntSushi.ripgrep`
- **jq**: `winget install jqlang.jq`

### 第1步: 生成编译数据库 (compile_commands.json)
```powershell
# 在KiCad源码根目录，生成包含编译命令的数据库
cmake -S . -B build\x64-Debug `
  -G Ninja `
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DCMAKE_BUILD_TYPE=Debug `
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON `
  -DSWIG_EXECUTABLE=C:/swig/swigwin-4.3.1/swig.exe `
  -DKICAD_BUILD_QA_TESTS=OFF `
  -DKICAD_WIN32_DPI_AWARE=ON
```

### 第2步: 创建分析脚本目录结构
```
kicad-source-mirror\
├─ build\x64-Debug\compile_commands.json
├─ scripts\
│   ├─ seeds.txt                    # BOARD/FOOTPRINT种子文件列表
│   ├─ 10_scan_src_deps.py         # 头文件闭包分析
│   ├─ 20_build_tu_index.py        # TU符号索引构建
│   ├─ 30_resolve_minset.py        # 最小源文件集合计算
│   ├─ 40_emit_min_headers.py      # 最小头文件集合计算
│   ├─ 50_report_minset.py         # 汇总报告生成
│   └─ run_all.ps1                 # 一键执行脚本
```

### 第3步: 确定种子文件 (seeds.txt)
包含BOARD和FOOTPRINT核心数据相关的.cpp文件：
```
pcbnew/board.cpp
pcbnew/footprint.cpp  
pcbnew/pad.cpp
pcbnew/zone.cpp
pcbnew/track.cpp
pcbnew/board_item.cpp
pcbnew/pcb_shape.cpp
```

### 第4步: 依赖分析流水线
1. **头文件闭包分析** (10_scan_src_deps.py)
   - 使用clang-scan-deps分析种子文件的所有头文件依赖
   - 生成 deps_src.json

2. **符号索引构建** (20_build_tu_index.py)  
   - 编译每个TU到.obj，提取定义/未定义符号
   - 生成 tu_index.json

3. **最小集合计算** (30_resolve_minset.py)
   - 从种子文件开始，递归解析未定义符号
   - 计算最小可编译源文件集合
   - 生成 minset_sources.json

4. **头文件集合计算** (40_emit_min_headers.py)
   - 为最小源文件集合计算对应的头文件闭包
   - 生成 minset_headers.json

5. **汇总报告** (50_report_minset.py)
   - 生成可读的Markdown报告
   - 输出 DEPENDENCIES_MINSET.md

## 🔧 分析脚本实现

### 脚本核心算法
- **符号闭包算法**: 从种子文件的未定义符号开始，递归查找提供这些符号的源文件
- **外部库过滤**: 识别并过滤std::, boost::, Qt等外部库符号  
- **循环依赖处理**: 自动处理源文件间的循环依赖关系
- **增量收敛**: 迭代添加源文件直到符号闭包收敛

### 输出文件说明
1. **deps_src.json**: 种子文件的完整头文件依赖树
2. **tu_index.json**: 每个编译单元的符号定义/未定义索引  
3. **minset_sources.json**: 最小可编译源文件集合
4. **minset_headers.json**: 对应的最小头文件集合
5. **unresolved_symbols.json**: 需要外部库提供的符号列表
6. **DEPENDENCIES_MINSET.md**: 人类可读的汇总报告

## 📋 执行检查清单

### 工具环境验证
- [ ] LLVM工具链正确安装 (clang-scan-deps, llvm-nm可用)
- [ ] Python 3.10+环境就绪
- [ ] VS Native Tools Command Prompt环境激活
- [ ] vcpkg工具链文件路径正确

### 数据生成验证  
- [ ] compile_commands.json成功生成
- [ ] 种子文件路径在数据库中存在
- [ ] 各阶段输出文件完整生成
- [ ] 未解析符号数量在合理范围内

### 结果质量验证
- [ ] 最小集合包含所有种子文件
- [ ] 源文件数量在预期范围(通常几十到几百个)
- [ ] 头文件闭包包含核心依赖
- [ ] 外部依赖符号被正确识别和过滤

## 🚀 执行命令序列

### 一键执行 (推荐)
```powershell
# 在x64 Native Tools Command Prompt中执行
cd scripts
.\run_all.ps1
```

### 分步执行 (调试用)
```powershell
python scripts\10_scan_src_deps.py
python scripts\20_build_tu_index.py  
python scripts\30_resolve_minset.py
python scripts\40_emit_min_headers.py
python scripts\50_report_minset.py
```

## 📊 预期输出规模
基于BOARD/FOOTPRINT核心数据文件的预估：
- **最小源文件集合**: 50-200个.cpp文件
- **最小头文件集合**: 200-800个.h文件  
- **未解析外部符号**: 100-500个符号
- **分析处理时间**: 5-15分钟

## ⚠️ 重要提醒

1. **工具链依赖** - 必须在正确的VS开发环境中执行
2. **路径处理** - Windows路径使用反斜杠，脚本已适配
3. **符号过滤** - 外部库符号过滤规则可能需要微调
4. **增量调试** - 可单独执行各阶段脚本进行问题定位

## 🎯 成功标准

当分析完成后，应该获得：
- **精确的最小依赖集合** - 无冗余，无遗漏
- **完整的符号闭包** - 内部依赖全部解析
- **清晰的外部依赖列表** - 需要vcpkg安装的包明确
- **可执行的复制清单** - 可直接用于文件复制操作

此时可以基于分析结果，精确复制所需文件到qt_pcb_project目录，开始Qt化改造工作。