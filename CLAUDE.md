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

## 下一步计划
1. 验证 kicad_core_project_wx 的编译
2. 解决未解析的外部依赖
3. 开始 wxWidgets 到 Qt 的框架迁移工作

## 注意事项
- 生成文件的实际路径在 Y:\wenming\kicad\build\x64-Debug
- CMakeLists.txt 保持与 KiCad 原始结构一致，便于维护
- 所有新增文件已整合到相应的 CMake 配置中