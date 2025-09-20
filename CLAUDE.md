# KiCad Core Project WX - 最小集合迁移项目

## 项目概述
本项目旨在从 KiCad 源码中提取最小可编译集合，并保持在 `kicad_core_project_wx` 目录中，为后续的 wxWidgets 到 Qt 框架迁移做准备。

## 已完成的工作

### 1. 最小集合收集与分析
- **分析工具**: 使用 `scripts/20_build_tu_index.py` 和 `scripts/30_resolve_minset.py` 进行符号依赖分析
- **分析结果**:
  - 源文件列表: `scripts/tem/minset_sources.json` 
  - 头文件列表: `scripts/tem/minset_headers.json`
  - 未解析符号: `scripts/tem/unresolved_symbols.json`

### 2. 文件复制与组织
- **复制脚本**: `scripts/copy_qt_project.py`
- **目标目录**: `kicad_core_project_wx/`
- **构建路径映射**: Y:\wenming\kicad\build\x64-Debug -> generated/
- **复制日志**: `kicad_core_project_wx/file_copy_log.txt` (记录所有文件状态)
- **复制统计**: `kicad_core_project_wx/copy_summary.json`
### 3. CMake 配置更新
- **更新脚本**: `scripts/update_cmake_proper.py`
- **更新策略**: 保持与 KiCad 原始 CMakeLists.txt 相同的文件结构和组织方式


### 4. 目录结构
```
kicad_core_project_wx/
├── bitmap2component/     # 位图组件相关代码
├── common/               # 公共代码库
├── eeschema/            # 原理图编辑器核心代码
│   ├── dialogs/         # 对话框
│   ├── erc/            # 电气规则检查
│   ├── sch_io/         # 原理图 IO 插件
│   ├── sim/            # 仿真相关
│   ├── symbol_editor/   # 符号编辑器
│   ├── sync_sheet_pin/ # 同步引脚
│   ├── tools/          # 工具类
│   └── widgets/        # 控件类
├── generated/           # 构建生成的文件
├── include/            # 头文件
├── libs/               # 核心库
├── pcbnew/             # PCB 编辑器
├── scripting/          # Python 脚本支持
├── test/               # 测试代码
└── thirdparty/         # 第三方库
```

## 关键文件说明

| 文件/目录 | 说明 |
|----------|------|
| `scripts/seeds.txt` | 种子文件配置 |
| `scripts/tem/` | 分析结果存储目录 |
| `kicad_core_project_wx/file_copy_log.txt` | 详细的文件复制日志 |
| `kicad_core_project_wx/copy_summary.json` | 复制统计摘要 |

### 5. 未使用符号处理
- **符号列表**: `scripts/unused_symbols.txt` (125个未使用符号)
- **处理策略**: 使用专门的 `symbol-dependency-analyzer` 代理进行精确处理
- **调度方式**: 每次并行调度10个代理，分别处理不同的符号
- **进度管理**: 在 `scripts/unused_symbols.txt` 中标记处理状态
  - `[已处理]` - 符号已完成分析并注释
  - `[跳过]` - 符号不存在或为幻影符号
  - 未标记 - 待处理符号


## 当前工作阶段：已处理符号及后续整理

### 已完成的符号处理
**第一批处理完成 (90个符号)**:

以下符号已通过 `symbol-dependency-analyzer` 代理成功分析并注释其依赖：

**模板特化相关**:
- `??$MigrateSimModel@VLIB_SYMBOL@@@SIM_MODEL@@SAXAEAVLIB_SYMBOL@@PEBVPROJECT@@@Z` (SIM_MODEL::MigrateSimModel<LIB_SYMBOL> 模板特化)
- `??$MigrateSimModel@VSCH_SYMBOL@@@SIM_MODEL@@SAXAEAVSCH_SYMBOL@@PEBVPROJECT@@@Z` (SIM_MODEL::MigrateSimModel<SCH_SYMBOL> 模板特化)

**构造函数/析构函数**:
- API_PLUGIN_MANAGER、DIALOG_CONFIGURE_PATHS、DIALOG_DATABASE_LIB_SETTINGS、DIALOG_LIST_HOTKEYS、DIALOG_MIGRATE_SETTINGS、EDA_BASE_FRAME、HTML_MESSAGE_BOX、NGSPICE_SETTINGS、SCH_COMMIT、SIM_LIB_MGR、TOOL_MANAGER 等类的构造/析构函数

**成员函数方法**:
- EDA_BASE_FRAME、SCH_EDIT_FRAME、TOOL_MANAGER、HTML_MESSAGE_BOX、DIALOG_SHIM 等关键类的成员方法
- SCH_IO_MGR 静态方法
- 各种工具类和对话框的事件处理方法

**全局函数**:
- SchGetLibSymbol、ShowAboutDialog 等全局辅助函数

### 后续工作计划

#### 1. 编译验证阶段
- **编译测试**: 对 `kicad_core_project_wx` 目录进行完整编译测试
- **错误修复**: 识别并修复因符号注释导致的编译错误
- **依赖检查**: 验证核心功能模块的依赖完整性

#### 2. 剩余符号处理
- **剩余符号统计**: 从原始125个符号中减去已处理的90个，剩余约35个符号
- **分批处理**: 继续使用 `symbol-dependency-analyzer` 代理处理剩余符号
- **进度跟踪**: 在 `scripts/unused_symbols.txt` 中维护处理状态标记

#### 3. 代码清理与优化
- **死代码移除**: 安全移除被注释的无用代码段
- **包含优化**: 清理不再需要的头文件包含
- **CMake 调整**: 根据实际使用情况优化 CMakeLists.txt 配置

### 符号处理成果统计
- **已处理**: 90个符号 (72% 完成率)
- **待处理**: ~35个符号 (28% 待完成)
- **处理方式**: 精确依赖分析 + 安全注释
- **质量保证**: 每个符号单独分析，保持编译完整性

### 技术经验总结
1. **高效并行处理**: 每次并行10个代理工作，平衡效率与资源使用
2. **分批处理策略**: 大量链接错误文件时分批处理，避免系统过载
3. **单文件专注**: 每个代理只处理一个文件，避免冲突和重复工作
4. **上下文感知修复**: 智能注释保持程序逻辑完整性
5. **链接错误驱动**: 直接基于错误信息进行精确修复，无需全局搜索
6. **进度可视化**: 通过 `[已处理]` 标志跟踪修复进度，便于管理大量错误
7. **责任分离**: 代理专注修复，主调度器负责进度管理


## 技术架构说明

### 代理架构
- **主调度器**: 负责解析链接错误并分配任务给子代理
- **子代理**: `symbol-dependency-analyzer` - 专门处理单个文件的链接错误修复
- **并发控制**: 每次并行启动10个子代理，避免资源过载
- **分批处理**: 超过10个文件时分批进行，等待前一批完成后启动下一批
- **单文件专注**: 每个代理只负责一个文件，不需要协调或感知其他代理

### 处理精度提升
- **链接错误驱动**: 直接基于链接错误信息确定需要处理的文件和符号
- **单文件范围**: 每个代理只在分配的文件中搜索和修复符号使用
- **上下文感知注释**: 分析符号使用上下文，保持程序逻辑和控制流完整性
- **智能替换**: 用适当的默认值或假设条件替换被注释的符号调用

## 注意事项
- 生成文件的实际路径在 Y:\wenming\kicad\build\x64-Debug
- CMakeLists.txt 保持与 KiCad 原始结构一致，便于维护
- 所有新增文件已整合到相应的 CMake 配置中
- 符号处理采用渐进式方法，确保每步都能维持编译完整性

## 6. 基于链接错误的并行修复策略

### 核心原则
**所有符号的注释与否必须严格以 `scripts/unused_symbols.txt` 中的内容为主**

### 链接错误并行处理流程

**第一阶段：链接错误解析**
1. 读取 `kicad_core_project_wx\link-error.txt` 文件（每行一个完整的链接错误）
2. 解析每个错误行，提取：
   - 错误类型（LNK2001/LNK2019）
   - 完整的符号描述和 mangled 符号名
   - 引用该符号的具体源文件（从.obj路径提取.cpp文件名）
3. 验证符号是否在 `scripts/unused_symbols.txt` 中
4. 按文件名分组，准备分配给不同代理

**第二阶段：并行代理调度**
1. **批量并行处理**: 每次并行启动10个 `symbol-dependency-analyzer` 代理
2. **单文件职责**: 每个代理只负责修复分配给它的一个文件中的符号使用
3. **分批处理**: 如果链接错误文件超过10个，分批进行处理
4. **并行执行**: 同批次的代理同时工作，互不干扰
5. **进度跟踪**: 代理只负责修复，不修改link-error.txt文件

**第三阶段：上下文感知修复**
每个代理在其分配的文件中：
1. 查找未解析符号的使用位置
2. 分析使用上下文（条件语句、返回值、循环等）
3. 应用智能注释策略：
   - 保持控制流逻辑（if-else平衡）
   - 提供适当的默认返回值
   - 维护变量生命周期

**第四阶段：进度标记**
主调度器（我）在代理完成后：
1. 监控代理完成状态
2. 代理完成文件修复后，在 `link-error.txt` 对应行前添加完成标志
3. 标志格式：`[已处理] Error LNK2001 unresolved external symbol...`
4. 继续处理下一批未标记的错误行

### 符号处理策略（更新）
- **符号在 unused_symbols.txt 中**: 注释其使用，但保持程序逻辑完整性
- **符号不在 unused_symbols.txt 中**: 必须保留或补充实现
- **上下文保持**: 注释符号调用时，用适当的假设值维持程序流程

### 已处理内容
- **删除21个未使用文件** (基于 unused_symbols.txt 提取的唯一文件路径)
- **序列化方法修复** (Serialize/Deserialize 方法声明和实现同步注释)
- **批量头文件引用修复** (注释对已删除头文件的 #include)
- **关键依赖补充** (APP_PROGRESS_DIALOG, SCHEMATIC_LEXER)

### 操作命令示例

**传统批量处理方式**:
```bash
# 1. 提取并删除未使用文件
awk -F'\t' 'NF==2 {print $2}' scripts/unused_symbols.txt | sort | uniq
rm -f [文件列表]

# 2. 批量注释头文件引用
sed -i 's|#include <头文件>|// #include <头文件> // UNUSED_SYMBOL: Header file deleted|g' *.cpp
```

**新的并行链接错误修复方式**:
```bash
# 1. 读取链接错误文件
# 用户已将所有链接错误放入 kicad_core_project_wx\link-error.txt

# 2. 解析错误文件，提取关键信息
# 从每行错误中提取：符号名、mangled名、文件名
# 示例错误行：
# Error	LNK2001	unresolved external symbol "DIALOG_SHIM::ShowModal()" (?ShowModal@DIALOG_SHIM@@UEAAHXZ)	...dialog_print_generic.cpp.obj

# 3. 按文件分组，为每个文件并行启动代理
# 每个代理收到完整的错误信息：符号描述 + mangled名 + 文件名
```

**代理调用示例（每次并行10个）**:
```
# 第一批（并行10个代理，传递完整链接错误信息）
Task(symbol-dependency-analyzer): "修复文件 excellon_read_drill_file.cpp 中的链接错误：
Error LNK2001 unresolved external symbol 'public: virtual int __cdecl DIALOG_SHIM::ShowModal(void)' (?ShowModal@DIALOG_SHIM@@UEAAHXZ)"

Task(symbol-dependency-analyzer): "修复文件 dialog_print_generic_base.cpp 中的链接错误：
Error LNK2001 unresolved external symbol 'public: virtual int __cdecl DIALOG_SHIM::ShowModal(void)' (?ShowModal@DIALOG_SHIM@@UEAAHXZ)"

Task(symbol-dependency-analyzer): "修复文件 eda_view_switcher_base.cpp 中的链接错误：
Error LNK2001 unresolved external symbol 'public: virtual int __cdecl DIALOG_SHIM::ShowModal(void)' (?ShowModal@DIALOG_SHIM@@UEAAHXZ)"

Task(symbol-dependency-analyzer): "修复文件 dialog_print_generic.cpp 中的链接错误：
Error LNK2001 unresolved external symbol 'public: virtual int __cdecl DIALOG_SHIM::ShowModal(void)' (?ShowModal@DIALOG_SHIM@@UEAAHXZ)"

Task(symbol-dependency-analyzer): "修复文件 hotkey_cycle_popup.cpp 中的链接错误：
Error LNK2001 unresolved external symbol 'public: virtual int __cdecl DIALOG_SHIM::ShowModal(void)' (?ShowModal@DIALOG_SHIM@@UEAAHXZ)"

# ... 继续其他5个代理

# 如果有更多文件，等第一批完成后启动第二批
```

**关键要点**：
- 每个代理收到完整的链接错误信息（包括符号描述和mangled名）
- 明确指定需要修复的文件名
- 代理可以直接解析错误信息，无需额外搜索
- 代理完成后，主调度器负责标记进度

**进度标记示例**：
```bash
# 代理完成前（原始错误行）：
Error	LNK2001	unresolved external symbol "public: virtual int __cdecl DIALOG_SHIM::ShowModal(void)" (?ShowModal@DIALOG_SHIM@@UEAAHXZ)	...dialog_print_generic.cpp.obj

# 代理完成后（添加标志）：
[已处理] Error	LNK2001	unresolved external symbol "public: virtual int __cdecl DIALOG_SHIM::ShowModal(void)" (?ShowModal@DIALOG_SHIM@@UEAAHXZ)	...dialog_print_generic.cpp.obj
```

**工作流程**：
1. 启动10个代理处理未标记的错误行
2. 代理完成后，给对应错误行添加 `[已处理]` 标志
3. 继续处理下一批未标记的错误行，直到全部完成

## 7. 编译错误修复策略

### 核心原则
**以 `scripts/unused_symbols.txt` 为准判断符号是否应该注释**

### 处理规则
1. **符号不在 unused_symbols.txt**：恢复声明和实现
2. **符号在 unused_symbols.txt**：保持注释，相关调用点也注释

### 示例
- `Activate()`, `Wait()` → 恢复（不在列表）
- `HTML_MESSAGE_BOX` 调用 → 注释（在列表）
