# KiCad Core Project WX - 最小集合迁移项目

## 项目概述
本项目旨在从 KiCad 源码中提取最小可编译集合，并保持在 `kicad_core_project_wx` 目录中，为后续的 wxWidgets 到 Qt 框架迁移做准备。

## 已完成的工作

### 1. 最小集合收集与分析
- **分析工具**: 使用 `scripts/20_build_tu_index.py` 和 `scripts/30_resolve_minset.py` 进行符号依赖分析
- **分析结果**:
  - 源文件列表: `scripts/tem/minset_sources.json` (263个文件)
  - 头文件列表: `scripts/tem/minset_headers.json` (609个文件)
  - 未解析符号: `scripts/tem/unresolved_symbols.json`

### 2. 文件复制与组织
- **复制脚本**: `scripts/copy_qt_project.py`
- **目标目录**: `kicad_core_project_wx/`
- **构建路径映射**: Y:\wenming\kicad\build\x64-Debug -> generated/
- **复制日志**: `kicad_core_project_wx/file_copy_log.txt` (记录所有文件状态)
- **复制统计**: `kicad_core_project_wx/copy_summary.json`
  - 总计处理 897 个文件
  - 新复制 208 个文件
  - 跳过已存在 689 个文件

### 3. CMake 配置更新
- **更新脚本**: `scripts/update_cmake_proper.py`
- **更新策略**: 保持与 KiCad 原始 CMakeLists.txt 相同的文件结构和组织方式
- **已更新的 CMakeLists.txt**:
  - `kicad_core_project_wx/CMakeLists.txt` (主配置，已添加新子目录)
  - `kicad_core_project_wx/bitmap2component/CMakeLists.txt`
  - `kicad_core_project_wx/eeschema/CMakeLists.txt`

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
1. **批量并行处理**: 10个代理并行工作效率高
2. **精确符号匹配**: mangled 符号名完整匹配避免误操作
3. **渐进式清理**: 逐步注释保持代码稳定性
4. **上下文保持**: 依赖分析确保功能模块完整性


## 技术架构说明

### 代理架构
- **主调度器**: 负责任务分配和进度管理
- **子代理**: `symbol-dependency-analyzer` - 专门处理单个符号的依赖分析
- **并发控制**: 最多同时运行10个子代理，避免资源冲突

### 处理精度提升
- **精确符号匹配**: 每个代理只处理一个特定符号
- **上下文感知**: 分析符号周围的依赖代码块
- **安全注释**: 保持代码结构完整性，避免编译错误

## 注意事项
- 生成文件的实际路径在 Y:\wenming\kicad\build\x64-Debug
- CMakeLists.txt 保持与 KiCad 原始结构一致，便于维护
- 所有新增文件已整合到相应的 CMake 配置中
- 符号处理采用渐进式方法，确保每步都能维持编译完整性

## 6. 基于 unused_symbols.txt 的链接错误解决策略

### 核心原则
基于 `scripts/unused_symbols.txt` 中的文件和符号列表，采用以下策略：

**删除文件策略**:
1. 删除 `unused_symbols.txt` 中对应的源文件(.cpp)和头文件(.h)
2. 注释编译文件中对已删除头文件的 `#include` 引用
3. 不修改 `unused_symbols.txt` 中列出的文件

**符号处理策略**:
- 如果符号在 `unused_symbols.txt` 中：注释调用该符号的代码
- 如果符号不在列表中：补充缺失的实现或依赖

### 已处理内容
- **删除21个未使用文件** (基于 unused_symbols.txt 提取的唯一文件路径)
- **序列化方法修复** (Serialize/Deserialize 方法声明和实现同步注释)
- **批量头文件引用修复** (注释对已删除头文件的 #include)
- **关键依赖补充** (APP_PROGRESS_DIALOG, SCHEMATIC_LEXER)

### 操作命令示例
```bash
# 1. 提取并删除未使用文件
awk -F'\t' 'NF==2 {print $2}' scripts/unused_symbols.txt | sort | uniq
rm -f [文件列表]

# 2. 批量注释头文件引用
sed -i 's|#include <头文件>|// #include <头文件> // UNUSED_SYMBOL: Header file deleted|g' *.cpp
```
