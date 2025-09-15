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
- **符号列表**: `kicad_core_project_wx/unused_symbols.txt` (125个未使用符号)
- **处理策略**: 使用专门的 `symbol-dependency-analyzer` 代理进行精确处理
- **调度方式**: 每次并行调度10个代理，分别处理不同的符号
- **进度管理**: 在 `kicad_core_project_wx/unused_symbols.txt` 中标记处理状态
  - `[已处理]` - 符号已完成分析并注释
  - `[跳过]` - 符号不存在或为幻影符号
  - 未标记 - 待处理符号


## 当前工作阶段：符号依赖分析与清理

### 主要职责调整
作为总调度器，负责：
1. **符号分析任务分配**: 从 `kicad_core_project_wx/unused_symbols.txt` 中读取未使用符号列表
2. **代理调度管理**: 每次并行启动10个 `symbol-dependency-analyzer` 代理
3. **任务协调**: 确保每个符号由专门代理处理，避免冲突
4. **进度跟踪**: 监控各代理的处理进度和结果，及时在文件中标记处理状态
5. **质量保证**: 验证代理处理结果的正确性和完整性
6. **进度持久化**: 每批处理完成后，更新 `kicad_core_project_wx/unused_symbols.txt` 标记处理状态

### 符号处理流程
1. **任务分解**: 将125个符号分成批次，每批10个
2. **代理启动**: 为每个符号启动专门的 `symbol-dependency-analyzer` 代理，传递完整符号信息和文件路径
3. **并行处理**: 同时处理10个符号的依赖分析和代码注释（仅在 `kicad_core_project_wx/` 目录）
4. **结果收集**: 汇总各代理的处理结果和修改报告
5. **进度更新**: 在 `kicad_core_project_wx/unused_symbols.txt` 中标记已处理符号
6. **验证检查**: 确保所有修改保持代码编译完整性

### 代理调用规范
每次调用 `symbol-dependency-analyzer` 代理时，需要提供完整信息：

**必须提供的完整符号信息：**
- **完整 mangled 符号名**: 例如 `??0DIALOG_CONFIGURE_PATHS@@QEAA@PEAVwxWindow@@@Z`
- **符号所在原始文件**: 例如 `common\dialogs\dialog_configure_paths.cpp`
- **符号类型说明**: 构造函数、析构函数、模板特化等
- **工作目标目录**: 仅在 `kicad_core_project_wx/` 目录中进行修改

**标准调用格式示例：**
```
Analyze and safely comment out the unused C++ symbol "??0DIALOG_CONFIGURE_PATHS@@QEAA@PEAVwxWindow@@@Z" found in file "common\dialogs\dialog_configure_paths.cpp". This appears to be the constructor for DIALOG_CONFIGURE_PATHS taking a wxWindow pointer. Find all usages of this constructor and its dependencies in the kicad_core_project_wx directory only, then comment them out while maintaining compilation integrity. Provide a detailed report of what was commented out and why.
```

**关键要求：**
1. 传递完整的符号名称，不能截断
2. 指明符号的具体文件位置
3. 明确只在 `kicad_core_project_wx/` 目录工作
4. 要求代理提供详细的处理报告

## 下一步计划
1. 实现符号依赖分析调度器
2. 启动第一批10个 symbol-dependency-analyzer 代理
3. 验证代理处理结果的准确性
4. 逐批处理完所有125个未使用符号
5. 验证 kicad_core_project_wx 的编译完整性
6. 开始 wxWidgets 到 Qt 的框架迁移工作

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