# KiCad PCB最小可编译依赖集合分析报告

**基于BOARD和FOOTPRINT核心数据的编译期依赖分析结果**

## 📊 统计摘要

- **最小源文件集合**: 1 个 .cpp/.cc 文件
- **最小头文件集合**: 8 个 .h/.hpp 文件
- **未解析符号**: 385 个 (需要外部库)

## 🔧 最小源文件集合 (.cpp/.cc)

以下源文件构成了BOARD/FOOTPRINT功能的最小编译单元：

  1. `pcbnew\board.cpp`

## 📁 最小头文件闭包 (.h/.hpp)

以下头文件是上述源文件的完整依赖闭包：

### Userswenming_masourcereposkicad-source-mirrorvcpkg_installedx64-windowsincludewx/

- `Userswenming_masourcereposkicad-source-mirrorvcpkg_installedx64-windowsincludewx\types.h(334):`

### 根目录/

- `C2371:`
- `Userswenming_masourcereposkicad-source-mirrorincludebase_set.h(38):`
- `error`
- `note:`
- `“ssize_t”:`
- `参见“ssize_t”的声明`
- `重定义；不同的基类型`

## ⚠️ 未解析符号 (外部依赖)

以下符号需要通过外部库或vcpkg安装包提供：

### Windows/MSVC运行时

- `??_7type_info@@6B@`
- `??_E?$_Associated_state@_K@std@@UEAAPEAXI@Z`
- `??_E?$_Ref_count@VCONNECTIVITY_DATA@@@std@@UEAAPEAXI@Z`
- `??_E?$_Ref_count_obj2@V?$promise@_K@std@@@std@@UEAAPEAXI@Z`
- `??_E?$_Ref_count_obj2@VNETCLASS@@@std@@UEAAPEAXI@Z`
- `??_E?$wxAnyValueTypeImpl@V?$optional@H@std@@@@UEAAPEAXI@Z`
- `??_E?$wxAnyValueTypeImpl@V?$optional@N@std@@@@UEAAPEAXI@Z`
- `??_E?$wxAnyValueTypeImplBase@V?$optional@H@std@@@@UEAAPEAXI@Z`
- `??_E?$wxAnyValueTypeImplBase@V?$optional@N@std@@@@UEAAPEAXI@Z`
- `??_EBOARD@@UEAAPEAXI@Z`
- `??_EBOARD_ITEM@@UEAAPEAXI@Z`
- `??_EBOARD_ITEM_CONTAINER@@UEAAPEAXI@Z`
- `??_EBOARD_STACKUP@@UEAAPEAXI@Z`
- `??_EDELETED_BOARD_ITEM@@UEAAPEAXI@Z`
- `??_ENETCLASS@@UEAAPEAXI@Z`
- `??_E_Future_error_category2@std@@UEAAPEAXI@Z`
- `??_Ebad_alloc@std@@UEAAPEAXI@Z`
- `??_Ebad_array_new_length@std@@UEAAPEAXI@Z`
- `??_Ebad_optional_access@std@@UEAAPEAXI@Z`
- `??_Eexception@std@@UEAAPEAXI@Z`
- `??_Efuture_error@std@@UEAAPEAXI@Z`
- `??_Elogic_error@std@@UEAAPEAXI@Z`
- `??_M@YAXPEAX_K1P6AX0@Z@Z`
- `??_V@YAXPEAX_K@Z`
- `__imp_??0?$vector@VKIID@@V?$allocator@VKIID@@@std@@@std@@QEAA@AEBV01@@Z`
- `__imp_??0?$wxArgNormalizerWchar@AEBVwxString@@@@QEAA@AEBVwxString@@PEBVwxFormatString@@I@Z`
- `__imp_??0BASE_SET@@QEAA@AEBV0@@Z`
- `__imp_??0GAL_SET@@QEAA@AEBV0@@Z`
- `__imp_??0GAL_SET@@QEAA@XZ`
- `__imp_??0KIID@@QEAA@AEBVwxString@@@Z`
- `__imp_??0LSET@@QEAA@AEBV0@@Z`
- `__imp_??0LSET@@QEAA@AEBVBASE_SET@@@Z`
- `__imp_??0LSET@@QEAA@V?$initializer_list@W4PCB_LAYER_ID@@@std@@@Z`
- `__imp_??0LSET@@QEAA@XZ`
- `__imp_??0NETCLASS@@QEAA@AEBVwxString@@_N@Z`
- `__imp_??0PAGE_INFO@@QEAA@AEBVwxString@@_N@Z`
- `__imp_??0TITLE_BLOCK@@QEAA@XZ`
- `__imp_??0_Lockit@std@@QEAA@H@Z`
- `__imp_??0wxAnyValueType@@QEAA@XZ`
- `__imp_??0wxArrayString@@QEAA@XZ`
- `__imp_??0wxFileName@@QEAA@AEBVwxString@@W4wxPathFormat@@@Z`
- `__imp_??0wxFormatString@@QEAA@AEBVwxString@@@Z`
- `__imp_??0wxFormatString@@QEAA@PEBD@Z`
- `__imp_??0wxFormatString@@QEAA@PEB_W@Z`
- `__imp_??0wxString@@QEAA@$$QEAV0@@Z`
- `__imp_??0wxString@@QEAA@AEBV0@@Z`
- `__imp_??0wxString@@QEAA@D_K@Z`
- `__imp_??0wxString@@QEAA@PEBD@Z`
- `__imp_??0wxString@@QEAA@PEB_W@Z`
- `__imp_??0wxString@@QEAA@VwxUniChar@@_K@Z`
- ... 还有 160 个符号

### 其他

- `??0BOARD_DESIGN_SETTINGS@@QEAA@PEAVJSON_SETTINGS@@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z`
- `??0BOARD_STACKUP@@QEAA@AEBV0@@Z`
- `??0BOARD_STACKUP@@QEAA@XZ`
- `??0COMPONENT_CLASS_MANAGER@@QEAA@XZ`
- `??0CONNECTIVITY_DATA@@QEAA@XZ`
- `??0EDA_ITEM@@IEAA@PEAV0@W4KICAD_T@@_N2@Z`
- `??0NETINFO_ITEM@@QEAA@PEAVBOARD@@AEBVwxString@@H@Z`
- `??0NETINFO_LIST@@QEAA@PEAVBOARD@@@Z`
- `??0PCB_PLOT_PARAMS@@QEAA@XZ`
- `??0SHAPE_POLY_SET@@QEAA@XZ`
- `??0ZONE@@QEAA@PEAVBOARD_ITEM_CONTAINER@@@Z`
- `??1BOARD_ITEM@@UEAA@XZ`
- `??1CONNECTIVITY_DATA@@QEAA@XZ`
- `??1NETINFO_LIST@@QEAA@XZ`
- `??1SHAPE_POLY_SET@@UEAA@XZ`
- `??2@YAPEAX_K@Z`
- `??3@YAXPEAX@Z`
- `??3@YAXPEAX_K@Z`
- `??8BOARD_DESIGN_SETTINGS@@QEBA_NAEBV0@@Z`
- `?Add@CONNECTIVITY_DATA@@QEAA_NPEAVBOARD_ITEM@@@Z`
- `?AddFile@EMBEDDED_FILES@@QEAAPEAUEMBEDDED_FILE@1@AEBVwxFileName@@_N@Z`
- `?AddHole@SHAPE_POLY_SET@@QEAAHAEBVSHAPE_LINE_CHAIN@@H@Z`
- `?Append@SHAPE_POLY_SET@@QEAAXAEBV?$VECTOR2@H@@HH@Z`
- `?AppendNet@NETINFO_LIST@@IEAAXPEAVNETINFO_ITEM@@@Z`
- `?BoardCopperLayerCount@BOARD_ITEM@@UEBAHXZ`
- `?BoardLayerCount@BOARD_ITEM@@UEBAHXZ`
- `?BoardLayerSet@BOARD_ITEM@@UEBA?AVLSET@@XZ`
- `?BoardLevelItems@GENERAL_COLLECTOR@@2V?$vector@W4KICAD_T@@V?$allocator@W4KICAD_T@@@std@@@std@@B`
- `?BottomLayer@PCB_VIA@@QEBA?AW4PCB_LAYER_ID@@XZ`
- `?Build@CONNECTIVITY_DATA@@QEAA_NPEAVBOARD@@PEAVPROGRESS_REPORTER@@@Z`
- `?BuildBoardPolygonOutlines@@YA_NPEAVBOARD@@AEAVSHAPE_POLY_SET@@HHPEBV?$function@$$A6AXAEBVwxString@@PEAVBOARD_ITEM@@1AEBV?$VECTOR2@H@@@Z@std@@_N@Z`
- `?BuildDefaultStackupList@BOARD_STACKUP@@QEAAXPEBVBOARD_DESIGN_SETTINGS@@H@Z`
- `?CacheTriangulation@ZONE@@QEAAXW4PCB_LAYER_ID@@@Z`
- `?Clone@EDA_ITEM@@UEBAPEAV1@XZ`
- `?Compare@EDA_SHAPE@@QEBAHPEBV1@@Z`
- `?Compare@EDA_TEXT@@QEBAHPEBV1@@Z`
- `?Compare@PCB_TABLE@@SAHPEBV1@0@Z`
- `?CopyFrom@BOARD_ITEM@@UEAAXPEBV1@@Z`
- `?Deserialize@BOARD_STACKUP@@UEAA_NAEBVAny@protobuf@google@@@Z`
- `?Deserialize@NETCLASS@@UEAA_NAEBVAny@protobuf@google@@@Z`
- `?Deserialize@SERIALIZABLE@@UEAA_NAEBVAny@protobuf@google@@@Z`
- `?DeserializeFromString@PCB_MARKER@@SAPEAV1@AEBVwxString@@@Z`
- `?Dirty@CN_ANCHOR@@QEBA_NXZ`
- `?Duplicate@BOARD_ITEM@@UEBAPEAV1@XZ`
- `?Flip@BOARD_ITEM@@UEAAXAEBV?$VECTOR2@H@@W4FLIP_DIRECTION@@@Z`
- `?GetBiggestClearanceValue@BOARD_DESIGN_SETTINGS@@QEBAHXZ`
- `?GetBoard@BOARD_ITEM@@UEAAPEAVBOARD@@XZ`
- `?GetBoard@BOARD_ITEM@@UEBAPEBVBOARD@@XZ`
- `?GetBoundingBox@EDA_ITEM@@UEBA?BV?$BOX2@V?$VECTOR2@H@@@@XZ`
- `?GetBoundingBox@FOOTPRINT@@QEBA?BV?$BOX2@V?$VECTOR2@H@@@@_N@Z`
- ... 还有 125 个符号

## 🚀 使用建议

### 1. 源文件复制
```bash
# 将最小源文件集合复制到目标项目
# 建议保持相对目录结构
```

### 2. 头文件复制
```bash
# 将头文件闭包复制到目标项目
# 注意保持include路径的一致性
```

### 3. 外部依赖安装
通过vcpkg安装以下包来解决外部符号依赖：
- wxwidgets (如果使用Qt替代，可能不需要)
- boost
- openssl
- sqlite3
- cairo
- 其他根据未解析符号确定的包

## ⚠️ 重要提醒

1. **编译验证**: 此分析基于静态依赖，复制后需要验证编译通过
2. **功能测试**: 最小集合可能缺少运行时动态加载的组件
3. **增量添加**: 如果功能不完整，可以以此为基础逐步添加缺失部分
4. **版本兼容**: 确保外部库版本与KiCad编译环境兼容

---
**报告生成时间**: 1756464522.142907
**工具**: KiCad依赖分析工具链 (clang-scan-deps + 符号分析)
**目标**: KiCad Qt移植项目最小依赖集合