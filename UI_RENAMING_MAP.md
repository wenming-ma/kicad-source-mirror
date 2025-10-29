# UI Text Renaming Map - Footprint Placement Feature

## Renaming Strategy: Simplified Symmetric Design (Plan 1C)

### Design Principles
1. **Keep "footprint" terminology** - Industry standard term
2. **Perfect symmetry** - All menu items follow same pattern
3. **Remove brand identifiers** - No "Auto-Place" or "KiCad-specific" terms
4. **Natural language** - Use common professional terms

---

## Complete Text Mapping Table

| # | Original KiCad Text | New Text (Plan 1C) | File Location | Line |
|---|---------------------|-------------------|---------------|------|
| 1 | `Auto-Place Footprints` | `Footprint Placement` | menubar_pcb_editor.cpp | 353 |
| 2 | `Place Selected Footprints` | `Place Selected Footprints` | pcb_actions.cpp | 1415 |
| 3 | `Place Off-Board Footprints` | `Place External Footprints` | pcb_actions.cpp | 1420 |
| 4 | `Performs automatic placement of selected components` | `Places selected footprints optimally on the board` | pcb_actions.cpp | 1416 |
| 5 | `Performs automatic placement of components outside board area` | `Places external footprints into optimal board positions` | pcb_actions.cpp | 1421 |
| 6 | `Autoplace Components` | `Footprint Placement` | autoplace_tool.cpp | 89 |
| 7 | `Autoplacing components...` | `Placing footprints...` | ar_autoplacer.cpp | 890 |
| 8 | `Autoplacing %s` | `Placing %s` | ar_autoplacer.cpp | 906 |
| 9 | `Autoplace components` | `Place footprints` | autoplace_tool.cpp | 95 |

---

## UI Visualization

### Before (KiCad Style)
```
Main Menu: Place
  └── Auto-Place Footprints               ← KiCad-specific
        ├── Place Off-Board Footprints    ← Technical jargon
        └── Place Selected Footprints

Progress Dialog:
  ┌─────────────────────────────┐
  │ Autoplace Components        │ ← KiCad term
  ├─────────────────────────────┤
  │ Autoplacing components...   │ ← Brand identifier
  │ Autoplacing R1              │
  └─────────────────────────────┘

Undo History:
  - Autoplace components          ← Inconsistent terminology
```

### After (Generic Professional Style)
```
Main Menu: Place
  └── Footprint Placement                 ← Generic professional
        ├── Place External Footprints     ← Clear & symmetric
        └── Place Selected Footprints     ← Parallel structure

Progress Dialog:
  ┌─────────────────────────────┐
  │ Footprint Placement         │ ← Professional neutral
  ├─────────────────────────────┤
  │ Placing footprints...       │ ← Natural language
  │ Placing R1                  │
  └─────────────────────────────┘

Undo History:
  - Place footprints              ← Consistent terminology
```

---

## Detailed Changes by File

### File 1: pcbnew/menubar_pcb_editor.cpp
**Purpose**: Main menu structure

| Line | Original Code | New Code |
|------|--------------|----------|
| 353 | `autoplaceSubmenu->SetTitle( _( "Auto-Place Footprints" ) );` | `autoplaceSubmenu->SetTitle( _( "Footprint Placement" ) );` |

**Impact**: Main submenu title visible in menu bar

---

### File 2: pcbnew/tools/pcb_actions.cpp
**Purpose**: Tool action definitions (menu items + tooltips)

| Line | Context | Original | New |
|------|---------|----------|-----|
| 1415 | Menu item name | `_( "Place Selected Footprints" )` | `_( "Place Selected Footprints" )` |
| 1416 | Tooltip | `_( "Performs automatic placement of selected components" )` | `_( "Places selected footprints optimally on the board" )` |
| 1420 | Menu item name | `_( "Place Off-Board Footprints" )` | `_( "Place External Footprints" )` |
| 1421 | Tooltip | `_( "Performs automatic placement of components outside board area" )` | `_( "Places external footprints into optimal board positions" )` |

**Impact**:
- Menu item labels shown in submenu
- Tooltip text shown on hover
- "Off-Board" → "External" (more universal term)
- "components" → "footprints" (consistent terminology)

---

### File 3: pcbnew/autorouter/autoplace_tool.cpp
**Purpose**: Progress dialog and undo system

| Line | Context | Original | New |
|------|---------|----------|-----|
| 89 | Progress dialog title | `_( "Autoplace Components" )` | `_( "Footprint Placement" )` |
| 95 | Undo/Redo commit message | `_( "Autoplace components" )` | `_( "Place footprints" )` |

**Impact**:
- Progress dialog window title
- Undo/Redo history list entry
- "Components" → "footprints" consistency

---

### File 4: pcbnew/autorouter/ar_autoplacer.cpp
**Purpose**: Progress reporter messages during operation

| Line | Context | Original | New |
|------|---------|----------|-----|
| 890 | Progress status message | `_( "Autoplacing components..." )` | `_( "Placing footprints..." )` |
| 906 | Individual item status | `_( "Autoplacing %s" )` | `_( "Placing %s" )` |

**Impact**:
- Progress bar status text
- Per-footprint processing message
- Removes "Auto-" prefix (brand identifier)

---

## Terminology Standardization

### Removed Terms (KiCad-specific)
- ❌ "Auto-Place" → Generic
- ❌ "Autoplace" → Simplified
- ❌ "Autoplacing" → Natural verb

### Adopted Terms (Industry-standard)
- ✅ "Footprint Placement" - Professional neutral
- ✅ "Place [X] Footprints" - Clear action verb
- ✅ "External" (vs "Off-Board") - Universal term
- ✅ "Placing" - Simple present continuous

### Word Choice Analysis

| Original | New | Reason |
|----------|-----|--------|
| Auto-Place | Placement | Remove automation emphasis |
| Off-Board | External | More universal, less technical |
| Components | Footprints | Industry-standard PCB term |
| Autoplacing | Placing | Simple, natural language |

---

## Symmetry Analysis

### Menu Structure Symmetry ✅

```
Footprint Placement
  ├── Place Selected Footprints    ← Pattern: Place + [Adjective] + Footprints
  └── Place External Footprints    ← Pattern: Place + [Adjective] + Footprints
```

**Parallel Structure Score**: 10/10
- Same verb: "Place"
- Same noun: "Footprints"
- Only difference: Adjective (Selected/External)

### Progress Messages Symmetry ✅

```
Dialog Title:  "Footprint Placement"
Main Status:   "Placing footprints..."      ← Verb form of title
Item Status:   "Placing R1"                 ← Same verb, specific target
```

**Consistency Score**: 10/10

---

## Translation Notes (for internationalization)

If your software supports multiple languages, update these strings in .po/.pot files:

```
# English (en)
msgid "Footprint Placement"
msgstr "Footprint Placement"

# Example for other languages (if needed)
# German: "Footprint-Platzierung"
# French: "Placement de Footprints"
# Chinese: "封装布局"
```

---

## Testing Checklist

After applying changes, verify:

- [ ] Main menu shows "Footprint Placement"
- [ ] Submenu items use consistent "Place [X] Footprints" pattern
- [ ] No "Auto-Place" or "Autoplace" visible anywhere in UI
- [ ] Progress dialog title is "Footprint Placement"
- [ ] Progress messages show "Placing footprints..."
- [ ] Individual footprint messages show "Placing [Reference]"
- [ ] Undo history shows "Place footprints"
- [ ] Tooltips use natural language (no "automatic")
- [ ] All text is grammatically correct
- [ ] Terminology is consistent across all UI elements

---

## Summary Statistics

- **Total changes**: 9 text strings
- **Files modified**: 4 files
- **Menu items**: 3 (1 title + 2 items)
- **Progress messages**: 3
- **Tooltips**: 2
- **System messages**: 1 (undo)
- **Symmetry score**: 10/10
- **Brand removal**: 100%

---

## Compatibility Notes

### What is NOT changed:
- ✅ Internal function names (e.g., `autoplaceSelected`)
- ✅ Class names (e.g., `AUTOPLACE_TOOL`)
- ✅ File names (e.g., `autoplace_tool.cpp`)
- ✅ Code comments
- ✅ Log messages (internal)
- ✅ Any functionality

### Only UI-visible text is changed

This ensures:
1. Code remains functional
2. Easy to search in codebase
3. No breaking changes
4. Only user perception changes

---

## Implementation Status

- [x] Mapping document created
- [ ] File 1 modified: menubar_pcb_editor.cpp
- [ ] File 2 modified: pcb_actions.cpp
- [ ] File 3 modified: autoplace_tool.cpp
- [ ] File 4 modified: ar_autoplacer.cpp
- [ ] Compiled and tested
- [ ] UI verification complete

---

Generated: 2025-01-XX
Version: 1.0
Strategy: Plan 1C - Simplified Symmetric Design
