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

### Execution Steps
```powershell
# Step 1: Execute copying script
python scripts/60_copy_minset.py

# Step 2: Verify copied structure
ls qt_pcb_project
```

**Implementation details in: `scripts/60_copy_minset.py`**