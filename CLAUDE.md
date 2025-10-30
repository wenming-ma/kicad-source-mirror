# Schematic-Based PCB Layout Feature Development

## Project Overview

This document tracks the development of a new feature for KiCad PCBnew: **automatic PCB layout based on schematic component positions**.

### Current Branch
- **Branch name:** `schematic-based-pcb-layout`
- **Base branch:** `master`

---

## Feature Goal

Enable PCB designers to automatically arrange footprints on the PCB to match the spatial layout of components in the schematic editor.

### Use Cases
1. **Initial component placement**: When creating a new PCB from schematic, automatically position footprints to mirror the schematic layout
2. **Hierarchical sheet organization**: Different hierarchical sheets are laid out vertically with configurable spacing
3. **Logical grouping**: Components placed near each other in schematic (e.g., decoupling capacitors near ICs) remain grouped on PCB

### Inspiration
Based on the Python plugin `SchematicPositionsToLayout.py` from https://github.com/ian-ross/kicad-plugins, but implemented natively in C++ for better integration.

---

## Implementation Approach

### Architecture Decision: Direct File Parsing vs KIWAY Communication

**Chosen Approach:** Direct schematic file parsing
- ✅ Simpler architecture (single module modification)
- ✅ Works without eeschema running
- ✅ No cross-module dependencies
- ✅ Consistent with how Python plugin works

**Alternative Considered:** KIWAY inter-process communication
- ❌ More complex (requires changes to eeschema, pcbnew, and mail protocol)
- ❌ Requires eeschema to be running
- ❌ Adds tight coupling between modules

### Technical Stack

**Component Matching Strategy:**
- Uses `KIID_PATH` (UUID-based hierarchical path) to match schematic symbols with PCB footprints
- Same mechanism as KiCad's "Update PCB from Schematic" functionality
- Ensures reliable matching even when references change

**Key KiCad APIs Used:**
- `SCH_IO_MGR::FindPlugin()` - Load schematic files
- `SCH_PLUGIN::Load()` - Parse .kicad_sch files
- `SCH_SHEET_PATH::PathAsString()` - Get hierarchical path (critical for path format consistency)
- `FOOTPRINT::GetPath()` / `SetPosition()` - Match and move footprints
- `BOARD_COMMIT` - Undo/redo support

---

## Current Implementation Status

### ✅ Completed

1. **Project Structure**
   - Git submodule added: `kicad-plugins/` (reference Python implementation)
   - Build integration: Added to CMakeLists.txt

2. **Core Tool Implementation**
   - New tool: `SCHEMATIC_LAYOUT_TOOL` (`pcbnew/tools/schematic_layout_tool.{h,cpp}`)
   - Action registered: `PCB_ACTIONS::layoutFromSchematic`
   - Menu integration: "Place → Layout from Schematic"

3. **Schematic Parsing (Phase 1 - Testing)**
   - Parses .kicad_sch files using `SCH_IO_MGR`
   - Extracts component positions from all hierarchical sheets
   - Builds KIID_PATH → position mapping
   - Matches with PCB footprints using UUID-based paths
   - Comprehensive debug logging for validation

4. **Critical Bug Fixes**
   - **KIID_PATH Format Issue:** Fixed mismatch between schematic and PCB path formats
     - Root sheet UUID handling: Now uses `PathAsString()` to skip root sheet UUID
     - Ensures 100% matching with how netlist updates work
   - **File Structure:** Handles project layout with separate sch/ and pcb/ folders

### 🚧 In Progress

- **Validation Phase:** Testing schematic parsing and footprint matching
- Currently outputs detailed logs but does NOT move footprints (safe testing mode)

### 📋 Planned (Not Yet Implemented)

1. **Phase 2: Footprint Movement**
   - Calculate PCB positions from schematic positions (coordinate scaling)
   - Apply sheet offsets for hierarchical designs
   - Move footprints using `BOARD_COMMIT` for undo support
   - Respect locked footprints
   - Handle selection (move only selected footprints if selection exists)

2. **Phase 3: User Options**
   - Configuration dialog (optional)
   - Coordinate scaling factor
   - Sheet spacing multiplier
   - Move all vs. move selected option

3. **Phase 4: Polish**
   - Progress reporting for large designs
   - Error handling and user feedback
   - Documentation

---

## Key Technical Insights

### KIID_PATH Matching Mechanism

**Understanding the Flow:**

1. **Schematic Creation**
   - Each symbol gets a UUID when created
   - Sheet hierarchy also has UUIDs

2. **Netlist Export** (eeschema → pcbnew)
   - Exports: `PathAsString()` → skips root sheet UUID
   - Format: `"/"` for root, `"/subsheet_uuid/"` for sub-sheets
   - Includes symbol UUID separately

3. **PCB Footprint Path Assignment**
   - On first "Update PCB from Schematic":
     - Loads footprint from library by name (LIB_ID: "Library:FootprintName")
     - Sets `footprint->SetPath(sheet_path + symbol_uuid)`
   - Path format matches netlist (no root sheet UUID)

4. **Our Tool Must Match This Format**
   - ✅ Use `PathAsString()` not `Path()`
   - ✅ Skip root sheet UUID (start from index 1)
   - ✅ Construct: `KIID_PATH(pathString) + symbol_uuid`

### Project File Structure Assumption

```
MyProject/
  ├── sch/
  │   └── myboard.kicad_sch
  └── pcb/
      └── myboard.kicad_pcb
```

Tool automatically looks for schematic in `../sch/` folder relative to PCB file.

---

## Files Modified/Created

### New Files
- `pcbnew/tools/schematic_layout_tool.h`
- `pcbnew/tools/schematic_layout_tool.cpp`

### Modified Files
- `pcbnew/tools/pcb_actions.h` - Action declaration
- `pcbnew/tools/pcb_actions.cpp` - Action implementation
- `pcbnew/pcb_edit_frame.cpp` - Tool registration
- `pcbnew/menubar_pcb_editor.cpp` - Menu item
- `pcbnew/CMakeLists.txt` - Build system
- `.gitmodules` - kicad-plugins submodule

### Dependencies
- Git submodule: `kicad-plugins/` (reference implementation)

---

## Testing Strategy

### Phase 1: Schematic Parsing Validation (Current)
- Run tool on test projects
- Check debug logs for:
  - All symbols detected
  - KIID_PATH format correct
  - Footprints matched successfully
- Verify no crashes, no side effects

### Phase 2: Layout Movement (Next)
- Test with simple designs first
- Verify undo/redo works
- Test hierarchical sheets
- Test edge cases (locked footprints, missing symbols, etc.)

---

## Known Issues & Solutions

### ✅ Solved Issues

1. **KIID_PATH Mismatch** (CRITICAL)
   - **Problem:** `sheetPath.Path()` includes root sheet UUID, but PCB paths don't
   - **Solution:** Use `KIID_PATH(sheetPath.PathAsString())` to skip root sheet
   - **Reference:** `eeschema/sch_sheet_path.cpp:271` - "Start at 1 to avoid the root sheet"

2. **Footprint Loading by Name**
   - **Question:** How does first netlist import know which footprint to load?
   - **Answer:** Uses Footprint field (LIB_ID) from schematic: "Library:FootprintName"
   - **Path Assignment:** Happens in `updateFootprintParameters()` after loading

3. **File Structure**
   - **Assumption:** PCB in `pcb/`, schematic in `sch/`
   - **Implementation:** Tool navigates from PCB path up to project root, then into sch/

---

## Development Guidelines

### Code Style
- Follow existing KiCad conventions
- Comprehensive comments for complex logic
- Debug logging during development (remove or conditionalize for release)

### Commit Strategy
- Small, focused commits
- Test compilation after each change
- Document in commit messages

### User-Facing Guidelines (Future)
- Recommend users save schematic before running tool
- Warn if schematic file is older than PCB
- Suggest running "Update PCB from Schematic" first to sync paths

---

## Next Steps

1. **Validate Current Implementation**
   - Test on multiple projects
   - Verify 100% matching rate
   - Confirm debug logs show correct data

2. **Implement Movement Logic**
   - Design coordinate transformation algorithm
   - Add BOARD_COMMIT for undo support
   - Handle edge cases

3. **User Testing**
   - Create test scenarios
   - Gather feedback
   - Iterate on UX

4. **Documentation**
   - User manual updates
   - Feature documentation
   - Code comments

---

## References

### KiCad Source Code Key Areas
- `eeschema/sch_io_mgr.h` - Schematic file loading
- `eeschema/sch_sheet_path.{h,cpp}` - Hierarchical path handling
- `pcbnew/netlist_reader/board_netlist_updater.cpp` - How netlist updates work
- `pcbnew/autorouter/autoplace_tool.cpp` - Similar tool pattern

### External Resources
- Python plugin inspiration: https://github.com/ian-ross/kicad-plugins
- KiCad documentation: https://docs.kicad.org/

---

## Contact / Questions

For questions about this feature development:
- Check this document first
- Review commit history on `schematic-based-pcb-layout` branch
- Check debug logs in `schematic-positions-to-layout.debug` file (created in project directory when tool runs)

---

**Last Updated:** 2025-01-XX (adjust as needed)
**Status:** Phase 1 - Parsing & Validation Complete, Testing in Progress
