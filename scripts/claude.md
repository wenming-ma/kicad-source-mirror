# KiCad PCB Minimum Set Collection Strategy

## 🎯 Collection Objective
**Use symbol dependency closure analysis to find the precise minimum compilable set**

## 🛠️ Third-party Dependency Management Strategy

### KiCad Third-party Dependencies (Two Types):

#### 1. **thirdparty/ Directory Dependencies** (Built-in Third-party Libraries)
- **Location**: In `thirdparty/` directory
- **Characteristics**: Built into KiCad project, distributed with source code
- **Examples**: clipper2, magic_enum, fmt, nlohmann_json, etc.
- **Handling**: Copy from KiCad source to qt_pcb_project/thirdparty/ directory

#### 2. **vcpkg Dependencies** (External Package Manager)
- **Location**: Installed via vcpkg package manager
- **Characteristics**: Large third-party libraries, version managed by package manager
- **Examples**: wxWidgets, boost, cairo, glew, opencascade, etc.
- **Handling**: Inform user of installation requirements, user handles installation

### 🎯 Dependency Analysis Goal
**Through precise dependency analysis, find the complete minimum dependency set in one pass, avoiding inefficient trial-and-error compilation**

## 🛠️ Three-stage Analysis Process

### Tool Description
Use the improved three-stage analysis process in scripts directory:
1. `scripts/20_build_tu_index.py` - Build translation unit symbol index
2. `scripts/30_resolve_minset.py` - Resolve minimum set based on symbol dependencies

### Tool Prerequisites
- **Python 3.10+**: Ensure Python environment is available
- **Symbol extraction tools**: dumpbin.exe (MSVC) or objdump (GCC)
- **Build environment**: Ensure KiCad has been compiled successfully

### Seed File Configuration
Seed files configured in `scripts/seeds.txt`:
```
pcb_io_kicad_sexpr_parser.cpp
```

### Three-stage Analysis Process
1. **Stage 1 - Symbol Index Construction**: `scripts/20_build_tu_index.py`
   - Scan build artifacts (obj files)
   - Extract defined and undefined symbols for each translation unit
   - Build symbol-to-source-file mapping

2. **Stage 2 - Dependency Closure Resolution**: `scripts/30_resolve_minset.py`  
   - Start iteration from seed files
   - Resolve internal symbol dependencies (exclude external library symbols)
   - Calculate symbol transitive closure until convergence

3. **Stage 3 - Result Output**:
   - `build/minset_sources.json` - Minimum source file set
   - `build/unresolved_symbols.json` - Unresolved symbols (mainly external library symbols)

## 📋 Execution Checklist

### Tool Environment Verification
- [ ] Python 3.10+ environment ready
- [ ] dumpbin.exe or objdump tools available
- [ ] KiCad project successfully compiled, obj files exist
- [ ] Analysis scripts exist in scripts directory

### Execution Verification  
- [ ] Seed files configured in seeds.txt
- [ ] Symbol index construction successful
- [ ] Dependency closure calculation converged
- [ ] Output files generated completely

### Result Quality Verification
- [ ] Minimum set contains all seed files
- [ ] Source file count within reasonable range
- [ ] Unresolved symbols mainly external library symbols
- [ ] External dependency symbols correctly filtered

## 🚀 Execution Command Sequence

### Three-stage Execution Process
```powershell
# Execute in KiCad source root directory

# Stage 1: Build symbol index
python scripts/20_build_tu_index.py

# Stage 2: Resolve minimum set
python scripts/30_resolve_minset.py

# Check results
type build\minset_sources.json
type build\unresolved_symbols.json
```

## 📊 Expected Output Scale
Estimation based on PCB IO core file symbol dependency analysis:
- **Minimum source file set**: Determined by symbol closure calculation
- **Unresolved symbols**: Mainly external library symbols (boost, wx, cairo, etc.)
- **External dependencies**: thirdparty and vcpkg dependencies identified by symbol prefixes
- **Processing time**: Depends on build artifact scale and symbol complexity

## ⚠️ Important Reminders

1. **Symbol Analysis** - Based on build artifact symbol dependencies, more precise than text analysis
2. **External Library Filtering** - Correctly identify external library symbols through EXTERNAL_HINTS
3. **Seed Configuration** - Configure PCB IO related core files in seeds.txt
4. **Convergence Verification** - Ensure symbol closure iteration converges, no circular dependencies

## 🎯 Success Criteria

When three-stage analysis completes, should obtain:
- **Precise minimum dependency set** - Calculated through symbol dependency closure
- **Complete symbol mapping** - build/tu_index.json contains detailed symbol information
- **Clear external dependency identification** - Unresolved symbols mainly external libraries
- **Executable copy checklist** - minset_sources.json can be directly used for file copying

At this point, based on symbol-level precise analysis results, copy minimum necessary files to qt_pcb_project directory to begin Qt transformation work.