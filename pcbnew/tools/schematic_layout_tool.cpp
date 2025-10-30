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

#include "schematic_layout_tool.h"
#include <tools/pcb_actions.h>
#include <board.h>
#include <board_commit.h>
#include <footprint.h>
#include <pcb_edit_frame.h>
#include <widgets/wx_infobar.h>

// Schematic-related includes
#include <base_units.h>
#include <sch_io_mgr.h>
#include <schematic.h>
#include <sch_sheet.h>
#include <sch_screen.h>
#include <sch_symbol.h>
#include <sch_sheet_path.h>

#include <wx/log.h>


SCHEMATIC_LAYOUT_TOOL::SCHEMATIC_LAYOUT_TOOL() : PCB_TOOL_BASE( "pcbnew.SchematicLayout" )
{
}


SCHEMATIC_LAYOUT_TOOL::~SCHEMATIC_LAYOUT_TOOL()
{
}


bool SCHEMATIC_LAYOUT_TOOL::parseSchematicPositions( const wxString& aSchematicPath,
                                                      std::map<KIID_PATH, VECTOR2I>& aPositions,
                                                      std::map<wxString, int>& aSheetOffsets )
{
    wxLogDebug( "=== parseSchematicPositions START ===" );
    wxLogDebug( "Schematic path: %s", aSchematicPath );

    try
    {
        // Get the schematic I/O plugin for KiCad format
        SCH_PLUGIN* plugin = SCH_IO_MGR::FindPlugin( SCH_IO_MGR::SCH_FILE_T::SCH_KICAD );
        if( !plugin )
        {
            wxLogError( "Failed to find SCH_KICAD plugin" );
            return false;
        }

        // Create a schematic object
        SCHEMATIC* schematic = new SCHEMATIC( board()->GetProject() );

        // Load the schematic file
        wxLogDebug( "Loading schematic file..." );
        SCH_SHEET* rootSheet = plugin->Load( aSchematicPath, schematic );

        if( !rootSheet )
        {
            wxLogError( "Failed to load schematic file" );
            delete schematic;
            SCH_IO_MGR::ReleasePlugin( plugin );
            return false;
        }

        schematic->SetRoot( rootSheet );
        wxLogDebug( "Schematic loaded successfully" );

        // Get all sheets in the hierarchy
        SCH_SHEET_LIST sheetList = schematic->GetSheets();
        wxLogDebug( "Total sheets in hierarchy: %zu", sheetList.size() );

        // Calculate vertical offsets for each sheet
        int currentYOffset = 0;
        const double OFFSET_FACTOR = 1.25;

        for( const SCH_SHEET_PATH& sheetPath : sheetList )
        {
            SCH_SCREEN* screen = sheetPath.LastScreen();
            if( !screen )
                continue;

            wxString pathString = sheetPath.PathAsString();
            wxLogDebug( "\n--- Processing sheet: %s ---", pathString );

            // Store offset for this sheet
            aSheetOffsets[pathString] = currentYOffset;
            wxLogDebug( "Sheet Y offset: %d", currentYOffset );

            // Process all symbols on this sheet
            int symbolCount = 0;
            for( SCH_ITEM* item : screen->Items().OfType( SCH_SYMBOL_T ) )
            {
                SCH_SYMBOL* symbol = static_cast<SCH_SYMBOL*>( item );

                // Get symbol position
                VECTOR2I pos = symbol->GetPosition();

                // Get symbol reference
                wxString reference = symbol->GetRef( &sheetPath );

                // Build the full KIID path: sheet path + symbol UUID
                // IMPORTANT: Use PathAsString() to match netlist format (skips root sheet UUID)
                // This ensures compatibility with how PCB footprints store their paths
                KIID_PATH fullPath( pathString );  // Construct from string (skips root sheet)
                fullPath.push_back( symbol->m_Uuid );

                // Store position
                aPositions[fullPath] = pos;

                symbolCount++;
                wxLogDebug( "  Symbol[%d]: Ref=%s, UUID=%s, Path=%s, Pos=(%d, %d)",
                           symbolCount,
                           reference,
                           symbol->m_Uuid.AsString(),
                           fullPath.AsString(),
                           pos.x, pos.y );
            }

            wxLogDebug( "Total symbols on this sheet: %d", symbolCount );

            // Calculate bounds for offset
            BOX2I bounds = screen->GetBoundingBox();
            if( bounds.GetHeight() > 0 )
            {
                currentYOffset += static_cast<int>( bounds.GetHeight() * OFFSET_FACTOR );
                wxLogDebug( "Sheet bounds height: %d, Next offset will be: %d",
                           bounds.GetHeight(), currentYOffset );
            }
        }

        wxLogDebug( "\n=== Parse Summary ===" );
        wxLogDebug( "Total sheets processed: %zu", sheetList.size() );
        wxLogDebug( "Total component positions extracted: %zu", aPositions.size() );
        wxLogDebug( "=== parseSchematicPositions END ===\n" );

        // Cleanup
        delete schematic;
        SCH_IO_MGR::ReleasePlugin( plugin );

        return true;
    }
    catch( const std::exception& e )
    {
        wxLogError( "Exception while parsing schematic: %s", e.what() );
        return false;
    }
    catch( ... )
    {
        wxLogError( "Unknown exception while parsing schematic" );
        return false;
    }
}


int SCHEMATIC_LAYOUT_TOOL::layoutFromSchematic( const TOOL_EVENT& aEvent )
{
    wxLogDebug( "\n========================================" );
    wxLogDebug( "SCHEMATIC_LAYOUT_TOOL::layoutFromSchematic CALLED" );
    wxLogDebug( "========================================\n" );

    BOARD* brd = board();
    if( !brd )
    {
        wxLogError( "No board found!" );
        return 0;
    }

    // Get the schematic file path
    // Assumption: PCB files are in pcb/ folder, schematic files are in sch/ folder
    // Example: project/pcb/myboard.kicad_pcb -> project/sch/myboard.kicad_sch
    wxFileName pcbPath( brd->GetFileName() );
    wxLogDebug( "PCB file: %s", pcbPath.GetFullPath() );

    // Get the parent directory of the pcb folder (project root)
    wxFileName projectRoot = pcbPath;
    projectRoot.RemoveLastDir();  // Remove "pcb" folder

    // Build path to sch folder
    wxString schFolderPath = projectRoot.GetPath() + wxFileName::GetPathSeparator() + "sch";
    wxLogDebug( "Looking for schematic in folder: %s", schFolderPath );

    // Check if sch folder exists
    if( !wxFileName::DirExists( schFolderPath ) )
    {
        wxString msg = wxString::Format(
            _( "Schematic folder not found:\n%s\n\nExpected project structure:\n  project/sch/  (schematic files)\n  project/pcb/  (PCB files)" ),
            schFolderPath );

        wxLogError( "%s", msg );
        frame()->GetInfoBar()->RemoveAllButtons();
        frame()->GetInfoBar()->ShowMessageFor( msg, 10000, wxICON_ERROR );
        return 0;
    }

    // Build schematic file path (same base name as PCB)
    wxFileName schPath;
    schPath.SetPath( schFolderPath );
    schPath.SetName( pcbPath.GetName() );
    schPath.SetExt( "kicad_sch" );

    wxLogDebug( "Looking for schematic: %s", schPath.GetFullPath() );

    // Check if schematic file exists
    if( !schPath.FileExists() )
    {
        wxString msg = wxString::Format(
            _( "Schematic file not found:\n%s\n\nExpected schematic file with same name as PCB in sch/ folder." ),
            schPath.GetFullPath() );

        wxLogError( "%s", msg );
        frame()->GetInfoBar()->RemoveAllButtons();
        frame()->GetInfoBar()->ShowMessageFor( msg, 10000, wxICON_ERROR );
        return 0;
    }

    wxLogDebug( "Schematic file exists, starting parse..." );

    // Parse the schematic
    std::map<KIID_PATH, VECTOR2I> positions;
    std::map<wxString, int> sheetOffsets;

    if( !parseSchematicPositions( schPath.GetFullPath(), positions, sheetOffsets ) )
    {
        wxString msg = _( "Failed to parse schematic file. Check the log for details." );
        wxLogError( "%s", msg );

        frame()->GetInfoBar()->RemoveAllButtons();
        frame()->GetInfoBar()->ShowMessageFor( msg, 10000, wxICON_ERROR );
        return 0;
    }

    // Move footprints to match schematic positions
    wxLogDebug( "\n--- Moving footprints ---" );

    BOARD_COMMIT commit( frame() );

    // Coordinate scaling factor: schematic IU -> PCB IU
    // SCH_IU_PER_MM = 1e4, PCB_IU_PER_MM = 1e6, so ratio = 100
    constexpr int POS_SCALE = static_cast<int>( PCB_IU_PER_MM / SCH_IU_PER_MM );

    // Check if there are selected footprints
    bool selectionActive = false;
    for( FOOTPRINT* fp : brd->Footprints() )
    {
        if( fp->IsSelected() )
        {
            selectionActive = true;
            break;
        }
    }

    if( selectionActive )
        wxLogDebug( "Selection detected - will only move selected footprints" );
    else
        wxLogDebug( "No selection - will move all matched footprints" );

    int movedCount = 0;
    int skippedLocked = 0;
    int skippedUnmatched = 0;
    int totalFootprints = 0;

    for( FOOTPRINT* fp : brd->Footprints() )
    {
        totalFootprints++;
        KIID_PATH fpPath = fp->GetPath();
        wxString reference = fp->GetReference();
        VECTOR2I oldPos = fp->GetPosition();

        // Check if this footprint has a matching schematic component
        if( positions.count( fpPath ) == 0 )
        {
            skippedUnmatched++;
            wxLogDebug( "  SKIP (no match): %s", reference );
            continue;
        }

        // Check if footprint is locked
        if( fp->IsLocked() )
        {
            skippedLocked++;
            wxLogDebug( "  SKIP (locked): %s", reference );
            continue;
        }

        // If selection is active, only move selected footprints
        if( selectionActive && !fp->IsSelected() )
        {
            wxLogDebug( "  SKIP (not selected): %s", reference );
            continue;
        }

        // Get schematic position and sheet offset
        VECTOR2I schPos = positions[fpPath];

        // Find which sheet this component belongs to
        wxString sheetPath = fpPath.AsString();
        int lastSlash = sheetPath.Find( '/', true );  // Find last '/'
        wxString sheetKey;

        if( lastSlash == 0 )
        {
            // Root sheet component (path like "/uuid")
            sheetKey = "";
        }
        else
        {
            // Sub-sheet component (path like "/sheet-uuid/comp-uuid")
            // Sheet key is everything before the last slash
            sheetKey = sheetPath.Mid( 0, lastSlash );
        }

        int yOffset = sheetOffsets.count( sheetKey ) > 0 ? sheetOffsets[sheetKey] : 0;

        // Calculate new position
        // X: schematic_x * scale
        // Y: (schematic_y + sheet_offset) * scale
        VECTOR2I newPos( schPos.x * POS_SCALE,
                        ( schPos.y + yOffset ) * POS_SCALE );

        // Move footprint
        commit.Modify( fp );
        fp->SetPosition( newPos );
        movedCount++;

        wxLogDebug( "  MOVED: %s  Old=(%d,%d)  Sch=(%d,%d)  Offset=%d  New=(%d,%d)",
                   reference,
                   oldPos.x, oldPos.y,
                   schPos.x, schPos.y,
                   yOffset,
                   newPos.x, newPos.y );
    }

    wxLogDebug( "\n=== Movement Summary ===" );
    wxLogDebug( "Total footprints: %d", totalFootprints );
    wxLogDebug( "Moved: %d", movedCount );
    wxLogDebug( "Skipped (locked): %d", skippedLocked );
    wxLogDebug( "Skipped (no match): %d", skippedUnmatched );

    // Commit changes with undo support
    if( movedCount > 0 )
    {
        commit.Push( _( "Layout from Schematic" ) );

        wxString msg = wxString::Format(
            _( "Layout complete!\n\nMoved %d footprint(s) to match schematic positions.\nSkipped %d locked footprint(s)." ),
            movedCount,
            skippedLocked );

        frame()->GetInfoBar()->RemoveAllButtons();
        frame()->GetInfoBar()->ShowMessageFor( msg, 10000, wxICON_INFORMATION );
    }
    else
    {
        wxString msg = _( "No footprints were moved.\n\nPossible reasons:\n- All matched footprints are locked\n- No matching components found\n- Only unselected footprints matched (when selection active)" );

        frame()->GetInfoBar()->RemoveAllButtons();
        frame()->GetInfoBar()->ShowMessageFor( msg, 10000, wxICON_WARNING );
    }

    wxLogDebug( "\n========================================" );
    wxLogDebug( "SCHEMATIC_LAYOUT_TOOL::layoutFromSchematic FINISHED" );
    wxLogDebug( "========================================\n" );

    return 0;
}


void SCHEMATIC_LAYOUT_TOOL::setTransitions()
{
    Go( &SCHEMATIC_LAYOUT_TOOL::layoutFromSchematic, PCB_ACTIONS::layoutFromSchematic.MakeEvent() );
}
