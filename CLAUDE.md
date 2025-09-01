# KiCad PCB Dependency Analysis and Minimum Set Extraction Guide

## 🎯 Current Task Objective
**Use the three-stage symbol dependency closure analysis method to collect the minimum compilation set**

## 📊 Strategy Overview
- **Analysis Method**: Three-stage analysis process in scripts directory
- **Analysis Tools**: `scripts/20_build_tu_index.py` → `scripts/30_resolve_minset.py`
- **Seed Files**: KiCad PCB IO core files (`pcb_io_kicad_sexpr_parser.cpp`, etc.)
- **Output Results**: Minimum compilable source file set + symbol dependency analysis

## 📋 Detailed Collection Strategy
**For complete collection strategy details, see: `scripts/claude.md`**

The `scripts/claude.md` file contains:
- Third-party dependency management strategy
- Three-stage analysis process details
- Tool prerequisites and environment setup
- Execution checklist and command sequences
- Expected output scale and success criteria

## 🚀 Quick Start
1. Review detailed strategy: `type scripts\claude.md`
2. Execute three-stage analysis:
   ```powershell
   python scripts/20_build_tu_index.py
   python scripts/30_resolve_minset.py
   ```
3. Check results: `type build\minset_sources.json`

## 📁 Next Step: Create Qt PCB Sub-project
**After collecting the minimum set, create a sub-project with the same folder structure**

### File Copying Strategy
1. **Create qt_pcb_project directory** with KiCad folder structure
2. **Copy minimum source files** to corresponding locations
3. **Copy generated build files** (headers, protobuf files) to appropriate directories
4. **Copy and simplify CMakeLists.txt** files for only copied files
5. **Handle thirdparty dependencies** according to dependency management strategy

### 🚀 Compilation Guidelines
**IMPORTANT: Strictly follow KiCad's original compilation method**

#### Core Principles:
1. **Maintain KiCad's CMake structure**: Use existing subdirectory CMakeLists.txt files
2. **Only difference**: Compile a subset of files, everything else identical
3. **Preserve build system**: Each subdirectory has its own CMakeLists.txt
4. **Root CMakeLists.txt**: Uses `add_subdirectory()` to include subdirectories
5. **No custom build logic**: Follow KiCad's proven build patterns

#### Implementation Rules:
- Copy all relevant CMakeLists.txt files from subdirectories
- Modify subdirectory CMakeLists.txt to only build copied source files
- Keep root CMakeLists.txt structure identical to KiCad
- Maintain same target names and dependencies as original
- Use same compiler flags and definitions as KiCad

#### 🚨 Critical Debugging Principle:
**NEVER modify source code during compilation issues - focus on missing files and paths**

When encountering compilation errors:
1. **DO NOT modify source code** - KiCad's original files are proven to compile successfully
2. **Identify missing dependencies**: Check for missing header files, generated files, or build artifacts
3. **Fix path issues**: Ensure all include directories and build paths are correctly configured
4. **Copy missing files**: If files are missing, copy them from original KiCad build directory
5. **Preserve original logic**: Keep all source code identical to KiCad's working version

**Root Cause**: If KiCad compiles successfully, copied subset should too - issues are always:
- Missing header files or generated files
- Incorrect include paths or build configuration  
- Missing build artifacts (config.h, version headers, etc.)
- Incomplete file copying from original build

#### 🔧 **CRITICAL BUILD RULE - CMakeLists.txt Handling**
**DO NOT copy files not in minimum set - COMMENT OUT references in existing CMakeLists.txt**

**When encountering missing files/directories errors in CMakeLists.txt:**
1. **DO NOT copy missing directories or files** - If they're not in the minimum set, they shouldn't be copied
2. **COMMENT OUT missing references**: Comment out `add_subdirectory()`, `add_dependencies()`, `install()`, `make_lexer()`, `generate_lemon_grammar()` calls
3. **Preserve CMakeLists.txt structure**: Keep all existing CMakeLists.txt files, only comment out problematic lines
4. **Add NOT in minset comments**: Mark commented lines with `# NOT in minset: <reason>`
5. **Fix incomplete commenting**: When commenting `set_source_files_properties()`, ensure entire function call including closing `)` is commented

**Examples:**
```cmake
# add_subdirectory( missing_dir )  # NOT in minset: missing directory
# make_lexer( target file.keywords )  # NOT in minset: lexer generation
# set_source_files_properties( file.cpp PROPERTIES  # NOT in minset: file.cpp
#     COMPILE_DEFINITIONS "FLAG"
#     )  # Ensure closing parenthesis is also commented
```

**This approach ensures:**
- No files outside minimum set are copied
- CMakeLists.txt files remain valid
- Build system focuses only on minimum set files
- Easy to track what was excluded and why

### Execution Steps
```powershell
# Step 1: Execute copying script
python scripts/60_copy_minset.py

# Step 2: Verify copied structure
ls qt_pcb_project
```

**Implementation details in: `scripts/60_copy_minset.py`**

## 📦 Minimum Set Third-party Dependencies

**Analysis results saved in: `build/minset_thirdparty_deps.json`**

### Required Libraries (13 total)
- **wxwidgets** - Used by 281 files (core UI framework)
- **opencascade** - Used by 197 files (3D geometry)
- **boost** - Used by 142 files (utilities)
- **glm** - Used by 26 files (math)
- **protobuf** - Used by 25 files (serialization)
- **python** - Used by 3 files
- **curl** - Used by 3 files
- **harfbuzz, nng, opengl** - Used by 2 files each
- **libgit2, ngspice, zstd** - Used by 1 file each

### Required vcpkg Packages (25)
boost-algorithm, boost-bimap, boost-filesystem, boost-functional, boost-iterator, boost-locale, boost-optional, boost-property-tree, boost-ptr-container, boost-random, boost-range, boost-test, boost-uuid, curl, glm, harfbuzz, libgit2, ngspice, nng, opencascade, opengl, protobuf, python3, wxwidgets, zstd

## 🔧 Offline vcpkg Configuration Guidelines

### Using Pre-installed vcpkg Dependencies
**当使用离线vcpkg包时，所有依赖包已安装到本地目录，无需联网下载**

#### Configuration Steps:
1. **vcpkg_installed Directory Structure**:
   ```
   qt_pcb_project/
   ├── vcpkg_installed/
   │   ├── scripts/buildsystems/vcpkg.cmake  (toolchain file)
   │   ├── x64-windows/                      (installed packages)
   │   │   ├── bin/
   │   │   ├── include/
   │   │   ├── lib/
   │   │   └── share/                        (CMake config files)
   │   └── vcpkg/
   ```

2. **CMake Configuration**:
   - **Toolchain Path**: Use local vcpkg toolchain file
   ```cmake
   "toolchainFile": "${sourceDir}/vcpkg_installed/scripts/buildsystems/vcpkg.cmake"
   ```
   
   - **Package Finding**: Set appropriate root directories
   ```cmake
   set( wxWidgets_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows" )
   ```

3. **Build Command**:
   ```powershell
   cd qt_pcb_project
   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=vcpkg_installed/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug
   cmake --build build
   ```

#### Key Points:
- **No vcpkg.json required**: All packages pre-installed
- **No internet connection needed**: Everything offline
- **Use local toolchain**: Point to `vcpkg_installed/scripts/buildsystems/vcpkg.cmake`
- **Package configs available**: All CMake config files in `x64-windows/share/`

#### Troubleshooting:
- If CMake can't find packages, check `vcpkg_installed/x64-windows/share/` directory
- Ensure toolchain file path is correct in CMakePresets.json
- For missing PkgConfig, use `find_package( PkgConfig QUIET )` instead of REQUIRED