/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2025 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef TOOLS_SCHEMATIC_LAYOUT_TOOL_H
#define TOOLS_SCHEMATIC_LAYOUT_TOOL_H

#include <tools/pcb_tool_base.h>
#include <map>

class KIID_PATH;


/**
 * Tool to layout PCB footprints based on schematic component positions.
 */
class SCHEMATIC_LAYOUT_TOOL : public PCB_TOOL_BASE
{
public:
    SCHEMATIC_LAYOUT_TOOL();
    ~SCHEMATIC_LAYOUT_TOOL();

    /// Bind handlers to corresponding TOOL_ACTIONs.
    void setTransitions() override;

private:
    /**
     * Layout footprints from schematic positions.
     * This is the main entry point that will be called from the menu/toolbar.
     */
    int layoutFromSchematic( const TOOL_EVENT& aEvent );

    /**
     * Parse schematic file and extract component positions.
     * @param aSchematicPath Path to the .kicad_sch file
     * @param aPositions Output map: KIID_PATH -> position
     * @param aSheetOffsets Output map: sheet path -> Y offset
     * @return true if parsing succeeded
     */
    bool parseSchematicPositions( const wxString& aSchematicPath,
                                  std::map<KIID_PATH, VECTOR2I>& aPositions,
                                  std::map<wxString, int>& aSheetOffsets );
};


#endif // TOOLS_SCHEMATIC_LAYOUT_TOOL_H
