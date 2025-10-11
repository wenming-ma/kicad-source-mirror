# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

KiCad is a professional open-source EDA (Electronic Design Automation) suite for PCB schematic capture and layout design. This is a modified fork undergoing a major framework migration from wxWidgets to Qt while maintaining 100% functional equivalence.

**Critical Context**: This repository is on branch `9.0` and is in the middle of a wxWidgets-to-Qt transformation project. The transformation is approximately 99% complete. All code modifications must preserve existing business logic and only transform UI framework calls.

## Build System

### Prerequisites
- CMake 3.22 or later
- C++20 compiler (MSVC, GCC, or Clang)
- vcpkg package manager (path: `D:\Tools\vcpkg`)
- Python 3.6+ with development headers
- Dependencies managed via vcpkg: wxWidgets, Qt, Boost, Cairo, OpenCascade, OpenGL, protobuf, ngspice, libgit2, and others

### Build Commands

**Configure (Debug build):**
```bash
cmake --preset x64-debug
```

**Configure (Release build):**
```bash
cmake --preset x64-release
```

**Build:**
```bash
cmake --build build/x64-debug
# or
cmake --build build/x64-release
```

**Install:**
```bash
cmake --build build/x64-debug --target install
```

**Single build workflow (configure + build + install):**
```bash
cmake --workflow --preset kicad
```

### Important Build Options

Key CMake cache variables (see CMakeLists.txt lines 107-282):
- `CMAKE_BUILD_TYPE`: `Debug`, `Release`, `RelWithDebInfo`, or `QABuild` (default: Release)
- `KICAD_BUILD_QA_TESTS`: Enable unit tests (default: OFF in this repo)
- `KICAD_BUILD_I18N`: Build translation libraries (default: OFF)
- `KICAD_SCRIPTING_WXPYTHON`: Build wxPython scripting (default: OFF)
- `KICAD_USE_PCH`: Enable precompiled headers (default: OFF)
- `KICAD_IPC_API`: Enable IPC API (default: ON)

## Repository Structure

### Core Components
- **eeschema/**: Schematic editor - transforms PCB schematics and circuit designs
- **pcbnew/**: PCB layout editor - handles printed circuit board design
- **gerbview/**: Gerber viewer - displays manufacturing files
- **3d-viewer/**: 3D visualization for PCB designs
- **cvpcb/**: Component to footprint assignment tool
- **common/**: Shared libraries and utilities used across all KiCad applications
- **libs/**: Core KiCad utilities (geometry, kimath, sexpr parsing)
- **include/**: Public header interfaces for common libraries
- **api/**: API definitions and IPC interfaces

### Supporting Directories
- **bitmap2component/**: Converts bitmaps to PCB artwork
- **pcb_calculator/**: PCB design calculator utilities
- **pagelayout_editor/**: Page layout and template editor
- **scripting/**: Python scripting integration (SWIG bindings)
- **qa/**: Quality assurance and unit tests
- **thirdparty/**: External dependencies (GLEW, pybind11, sentry-native, etc.)
- **cmake/**: CMake modules and build configuration
- **resources/**: Icons, bitmaps, project templates
- **translation/**: i18n translation data (managed via Weblate)

## Architecture

### Framework Migration Status

**Current State**: Repository is undergoing wxWidgets → Qt transformation (99% complete)
- Most source files in `eeschema/`, `common/`, `pcbnew/`, `gerbview/` have been transformed
- One remaining file needs cleanup: `scripting/python_scripting.cpp` (wxWidgets version checking)
- Build system supports both wxWidgets and Qt simultaneously during migration

**Important**: When modifying ANY code:
1. Never modify KiCad native types: `VECTOR2I`, `VECTOR2D`, `BOX2I`, `BOX2D`, `SHAPE_*`, `SEG`, `ANGLE`
2. Never change standard library containers: `std::vector`, `std::map`, `std::string`, etc.
3. Transform only wxWidgets UI elements to Qt equivalents
4. Preserve all business logic, algorithms, and data structures

### Key Type Mappings (wxWidgets → Qt)
- `wxString` → `QString` (with method updates: `.IsEmpty()` → `.isEmpty()`, `.Length()` → `.length()`)
- `wxVector` → `QVector` (only for wxWidgets containers, NOT std::vector)
- `wxArrayString` → `QStringList`
- `wxPoint` → `QPoint` (UI only, not geometry)
- `wxSize` → `QSize`
- `COLOR4D` → `QColor`

**Never Replace**: C++ standard library (`std::vector`, `std::map`), KiCad geometry types (`VECTOR2D`, `BOX2D`)

### Core Libraries

**libs/kimath**: Mathematical primitives - KiCad's native geometry implementations
- `VECTOR2D`, `VECTOR2I`: 2D vector types used throughout KiCad
- `BOX2D`, `BOX2I`: Bounding box types
- `SHAPE_*`: Geometric shape representations
- These types are KiCad-specific and must NEVER be replaced with Qt equivalents

**common/**: Shared functionality
- `EDA_BASE_FRAME`: Base class for all KiCad application frames
- `PLOTTER`: Unified plotting interface for various output formats
- Drawing and rendering infrastructure
- Configuration and settings management

### Build System Details

The project uses CMake with vcpkg for dependency management. The toolchain file is specified via `CMAKE_TOOLCHAIN_FILE` pointing to vcpkg's build system.

**Important CMakeLists.txt patterns**:
- Uses `add_subdirectory()` to include component builds (lines 1194-1225)
- Global C++20 standard requirement (line 292)
- Position-independent code globally enabled (line 289)
- Precompiled headers configurable via `KICAD_USE_PCH` option

## Development Guidelines

### Code Style
- Follow the [KiCad Code Style Guide](https://dev-docs.kicad.org/en/rules-guidelines/code-style/)
- Use clang-format for formatting (config: `.clang-format`)
- Class names should be UPPERCASE (KiCad convention)
- Maintain existing naming conventions
- No Chinese characters in code (per global instructions)

### Testing
Unit tests are in the `qa/` directory but are disabled by default in this build configuration (`KICAD_BUILD_QA_TESTS=OFF`).

To enable and run tests:
```bash
cmake --preset x64-debug -DKICAD_BUILD_QA_TESTS=ON
cmake --build build/x64-debug
ctest --test-dir build/x64-debug --output-on-failure
```

### Git Workflow
- Main development branch: `master`
- Current working branch: `9.0` (Qt transformation branch)
- Follow contribution guidelines in `CONTRIBUTING.md`
- Recent commits show active Qt transformation work (see commit messages like "原理图Qt改造后编译成功")

## Custom Agents

This repository includes specialized Claude Code agents in `.claude/agents/`:

1. **kicad-wx-to-qt-transformer**: Transforms wxWidgets code to Qt while preserving all business logic
2. **qt-compatibility-fixer**: Fixes API compatibility issues after transformation
3. **qt-transformation-verifier**: Verifies Qt transformation completeness
4. **qt-to-std-container-converter**: Converts Qt containers to std:: equivalents
5. **cmake-config-fixer**: Resolves CMake configuration errors
6. **symbol-dependency-analyzer**: Analyzes and comments out unused C++ symbols

**When working on framework transformation**: Always use the appropriate agent rather than manually transforming code.

## Important Constraints

### Absolutely Never
- Modify KiCad native geometry types (`VECTOR2D`, `BOX2D`, `VECTOR2I`, `BOX2I`, `SHAPE_*`)
- Change C++ standard library containers or types
- Alter business logic or algorithms
- Add features beyond wxWidgets → Qt replacement
- Rename files or folders (even if they contain "wx" in the name)
- Use Qt signals/slots unless replacing direct wx equivalents
- Include Chinese characters in any code or comments

### Always
- Preserve constructor signatures and initialization order
- Maintain class hierarchies and virtual function patterns
- Keep all error handling and conditional logic identical
- Use `std::shared_ptr`/`std::unique_ptr` (not Qt smart pointers)
- Verify changes maintain functional equivalence
- Think deeply before any modification (THINK HARD, THINK DEEP, THINK WIDE, THINK COMPREHENSIVE)

## Platform-Specific Notes

### Windows (Current Platform)
- Uses MSVC with `/utf-8`, `/permissive-`, `/Zc:__cplusplus` flags
- Debug info format: Embedded PDB when using ccache, ProgramDatabase otherwise
- UNICODE and _UNICODE defined globally
- vcpkg manages all dependencies

### Build Output
- Binary directory: `build/<preset-name>/`
- Install directory: `install/<preset-name>/`
- CMake generates `compile_commands.json` for LSP tools

## Key Files

- `CMakeLists.txt`: Main build configuration (1246 lines)
- `CMakePresets.json`: Build presets for Windows x64 debug/release
- `vcpkg.json`: Dependency manifest
- `AUTHORS.txt`: Contributors list
- `CONTRIBUTING.md`: Contribution guidelines
- `.clang-format`: Code formatting rules
- `uncrustify.cfg`: Alternative formatting configuration

## External References

- [KiCad Developer Documentation](https://dev-docs.kicad.org)
- [Build Instructions](https://dev-docs.kicad.org/en/build/)
- [Code Style Guide](https://dev-docs.kicad.org/en/rules-guidelines/code-style/)
- [UI Guidelines](https://dev-docs.kicad.org/en/rules-guidelines/ui/)
- [KiCad Forum](https://forum.kicad.info/)
- [GitLab Issue Tracker](https://gitlab.com/kicad/code/kicad/-/issues)
