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
| `kicad_core_project_wx/unused_symbols.txt` | 需要排除的符号列表 (73个符号及其定义文件) |

### 5. 未使用符号处理
- **符号列表**: `kicad_core_project_wx/unused_symbols.txt` (73个需要排除的符号)
- **符号来源**: 这些符号定义在将被排除编译的源文件中，需要注释掉它们的使用位置
- **符号分类**:
  - **SIM相关符号** (12个): MigrateSimModel模板、SIM_LIB_MGR、SPICE_VALUE等仿真相关
  - **对话框符号** (15个): DIALOG_DATABASE_LIB_SETTINGS、HTML_MESSAGE_BOX等UI对话框
  - **工具管理符号** (20个): TOOL_MANAGER的各种方法、SCH_SELECTION_TOOL等
  - **编辑框架符号** (18个): SCH_EDIT_FRAME的各种方法、SCH_BASE_FRAME等
  - **其他符号** (8个): API_PLUGIN_MANAGER、SCH_IO_MGR等
- **处理策略**: 使用专门的 `symbol-dependency-analyzer` 代理进行精确处理
- **调度方式**: 每次并行调度10个代理，分别处理不同的符号
- **进度管理**: 在 `kicad_core_project_wx/unused_symbols.txt` 中标记处理状态
  - `[已处理]` - 符号已完成分析并注释
  - `[跳过]` - 符号不存在或为幻影符号
  - 未标记 - 待处理符号


## 当前工作阶段：排除符号的使用点清理

### 核心工作原理
由于这73个符号定义在将被排除编译的源文件中，我们的任务是找到并注释掉这些符号在其他文件中的**使用点**，而不是定义点。

### 主要职责调整
作为总调度器，负责：
1. **符号分析任务分配**: 从 `kicad_core_project_wx/unused_symbols.txt` 中读取需要排除的符号列表
2. **代理调度管理**: 每次并行启动10个 `symbol-dependency-analyzer` 代理
3. **使用点定位**: 确保代理专注于找到符号的调用、实例化、引用等使用位置
4. **进度跟踪**: 监控各代理的处理进度和结果，及时在文件中标记处理状态
5. **质量保证**: 验证代理处理结果的正确性和完整性
6. **进度持久化**: 每批处理完成后，更新 `kicad_core_project_wx/unused_symbols.txt` 标记处理状态

### 符号处理流程
1. **任务分解**: 将73个符号分成批次，每批10个
2. **代理启动**: 为每个符号启动专门的 `symbol-dependency-analyzer` 代理，传递完整符号信息和文件路径
3. **使用点搜索**: 代理搜索符号在 `kicad_core_project_wx/` 目录中的所有使用位置
4. **安全注释**: 注释掉使用点，保持编译完整性
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

## 当前编译状况

### 编译错误根本原因
**目前的大部分编译错误来源于 `unused_symbols.txt` 中列出的73个符号的使用点尚未被系统性注释**。这些符号定义在将被排除编译的源文件中，但它们的使用点（函数调用、对象实例化、方法调用等）仍然存在于编译的文件中。

### 已知问题类别
1. **构造函数调用错误**: 如 `DIALOG_DATABASE_LIB_SETTINGS`、`HTML_MESSAGE_BOX` 等对话框类的构造函数被调用但定义缺失
2. **方法调用错误**: 如 `TOOL_MANAGER` 的各种方法被调用但定义缺失
3. **模板实例化错误**: 如 `MigrateSimModel<T>` 模板的使用点未完全清理
4. **静态方法调用错误**: 如 `SCH_IO_MGR` 的静态方法被调用但定义缺失
5. **间接依赖错误**: 通过头文件包含链导致的SIM相关符号引用

### 已修复的具体问题
- ✅ **语法错误**: 修复了 `erc_item.cpp` 中的变量重定义和if-else结构错误
- ✅ **构造函数参数错误**: 恢复了必要的 `TOOL_INTERACTIVE` 和 `SCH_TOOL_BASE` 构造函数
- ✅ **间接包含错误**: 注释掉了导致 `pegtl.hpp` 缺失的包含链
- ✅ **部分SIM符号**: 手动处理了部分SIM相关符号的使用点

## 下一步计划

### **第一优先级**: 系统性符号处理 (当前阶段)
**关键认识**: 必须使用 `symbol-dependency-analyzer` 代理系统性地处理 `unused_symbols.txt` 中的全部73个符号，这是解决编译问题的根本途径。

1. **批量代理处理**:
   - 每批10个符号，启动专门的 `symbol-dependency-analyzer` 代理
   - 严格按照改进后的代理提示词执行，专注于**使用点**而非定义点
   - 确保每个符号的所有调用、实例化、引用都被正确注释

2. **进度跟踪与质量控制**:
   - 在 `unused_symbols.txt` 中标记每个符号的处理状态
   - 验证代理处理结果的完整性和正确性
   - 及时修复代理处理中的遗漏或错误

3. **编译验证循环**:
   - 每处理一批符号后进行编译测试
   - 识别新出现的错误和遗漏的使用点
   - 调整处理策略，完善符号清理

### **第二阶段**: 残留问题清理
1. **错误分类处理**: 将编译错误按符号类别分组处理
2. **依赖链分析**: 识别复杂的间接依赖关系
3. **替代实现**: 为关键功能提供简化的替代实现

### **第三阶段**: 最终验证
1. **完整编译通过**: 确保项目能够成功编译
2. **功能完整性检查**: 验证核心功能未被破坏
3. **文档更新**: 记录所有修改和注意事项


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

## 关键经验总结

### 编译错误的根本解决方案
**重要经验**: 大部分编译错误不是由于缺失文件或配置问题，而是由于 `unused_symbols.txt` 中列出的符号使用点未被处理。解决编译问题的正确方法是：

1. **系统性符号处理**: 必须逐一处理 `unused_symbols.txt` 中的每个符号
2. **关注使用点**: 重点注释符号的调用、实例化、引用位置，而不是定义位置
3. **代理工具使用**: 使用改进后的 `symbol-dependency-analyzer` 代理确保处理的完整性
4. **循环验证**: 每批处理后进行编译验证，及时发现和修复遗漏

### 常见错误模式
- **构造函数缺失**: 通常由于对话框或管理类的构造函数调用未被注释
- **方法调用缺失**: 通常由于工具管理或编辑框架方法的调用未被注释
- **间接依赖**: 通过头文件包含链导致的符号引用，需要在包含链的源头切断