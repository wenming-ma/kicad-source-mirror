# KiCad vcpkg 构建问题修复记录

## 项目级修复 (已集成到项目配置)

以下问题已通过 `vcpkg-overlay/` 目录中的 overlay ports 解决，其他开发者克隆项目后无需手动修复。

### 问题 1: boost-filesystem 构建失败 ✅ 已解决

**错误**: 找不到 `boost_atomic-vc140-mt-gd.lib`

**原因**: vcpkg baseline 版本未处理 MSVC vc144/vc145 版本标签重命名

**解决方案**: 创建了 `vcpkg-overlay/ports/boost-modular-build-helper/`，添加了 vc144/vc145 支持

---

### 问题 3: ngspice 下载失败 (404) ✅ 已解决

**错误**: SourceForge 返回 404

**原因**: ngspice-41.tar.gz 在 SourceForge 上不可用

**解决方案**: 创建了 `vcpkg-overlay/ports/ngspice/`，使用修正后的 SHA512 哈希值

**注意**: 如果自动下载仍然失败，需要手动准备 tarball:
1. 从 GitHub 下载: `https://github.com/ngspice/ngspice/archive/refs/tags/ngspice-41.tar.gz`
2. 解压后重命名目录 `ngspice-ngspice-41` -> `ngspice-41`
3. 重新打包为 `ngspice-41.tar.gz`
4. 放到 `$VCPKG_ROOT/downloads/ngspice-41.tar.gz`

---

### 问题 4: CMake Python distutils ✅ 已解决

**错误**: CMake 配置时 Python distutils 缺失

**解决方案**: 在 `CMakePresets.json` 中添加了 `PYTHON_EXECUTABLE` 指向 vcpkg 的 Python

---

## 需要手动修复的问题

以下问题无法通过项目配置解决，需要用户手动处理。

### 问题 2: fontconfig 构建失败 (Python distutils) ⚠️ 需手动修复

**错误**: `python is missing distutils`

**原因**: vcpkg 下载的 Python 3.14.2 移除了 distutils 模块

**修复**: 用 Python 3.11.8 替换 3.14.2:
```bash
cp -r D:/Tools/vcpkg/downloads/tools/python/python-3.11.8-x64/* \
      D:/Tools/vcpkg/downloads/tools/python/python-3.14.2-x64-1/
```

---

## 根本原因

使用了较新的工具链版本 (VS 2022 17.14 + Python 3.14)，vcpkg 尚未完全适配。

## 项目配置文件

- `vcpkg-configuration.json` - 添加了 `overlay-ports` 配置
- `vcpkg-overlay/ports/boost-modular-build-helper/` - boost vc144 修复
- `vcpkg-overlay/ports/ngspice/` - ngspice 下载修复
- `CMakePresets.json` - 添加了 `PYTHON_EXECUTABLE`