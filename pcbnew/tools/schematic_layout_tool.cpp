#include "schematic_layout_tool.h"
#include <tools/pcb_actions.h>
#include <board.h>
#include <board_commit.h>
#include <footprint.h>
#include <pcb_edit_frame.h>
#include <widgets/wx_infobar.h>
#include <base_units.h>
#include <sch_io_mgr.h>
#include <schematic.h>
#include <sch_sheet.h>
#include <sch_screen.h>
#include <sch_symbol.h>
#include <sch_sheet_path.h>


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
    try
    {
        SCH_PLUGIN* plugin = SCH_IO_MGR::FindPlugin( SCH_IO_MGR::SCH_FILE_T::SCH_KICAD );
        if( !plugin )
            return false;

        SCHEMATIC* schematic = new SCHEMATIC( board()->GetProject() );
        SCH_SHEET* rootSheet = plugin->Load( aSchematicPath, schematic );

        if( !rootSheet )
        {
            delete schematic;
            SCH_IO_MGR::ReleasePlugin( plugin );
            return false;
        }

        schematic->SetRoot( rootSheet );
        SCH_SHEET_LIST sheetList = schematic->GetSheets();

        int currentYOffset = 0;
        const double OFFSET_FACTOR = 1.25;

        for( const SCH_SHEET_PATH& sheetPath : sheetList )
        {
            SCH_SCREEN* screen = sheetPath.LastScreen();
            if( !screen )
                continue;

            wxString pathString = sheetPath.PathAsString();
            aSheetOffsets[pathString] = currentYOffset;

            for( SCH_ITEM* item : screen->Items().OfType( SCH_SYMBOL_T ) )
            {
                SCH_SYMBOL* symbol = static_cast<SCH_SYMBOL*>( item );
                VECTOR2I pos = symbol->GetPosition();
                KIID_PATH fullPath( pathString );
                fullPath.push_back( symbol->m_Uuid );
                aPositions[fullPath] = pos;
            }

            BOX2I bounds = screen->GetBoundingBox();
            if( bounds.GetHeight() > 0 )
                currentYOffset += static_cast<int>( bounds.GetHeight() * OFFSET_FACTOR );
        }

        delete schematic;
        SCH_IO_MGR::ReleasePlugin( plugin );

        return true;
    }
    catch( ... )
    {
        return false;
    }
}


int SCHEMATIC_LAYOUT_TOOL::layoutFromSchematic( const TOOL_EVENT& aEvent )
{
    BOARD* brd = board();
    if( !brd )
        return 0;

    wxFileName pcbPath( brd->GetFileName() );
    wxFileName projectRoot = pcbPath;
    projectRoot.RemoveLastDir();

    wxString schFolderPath = projectRoot.GetPath() + wxFileName::GetPathSeparator() + "sch";

    if( !wxFileName::DirExists( schFolderPath ) )
    {
        wxString msg = wxString::Format(
            _( "Schematic folder not found:\n%s\n\nExpected project structure:\n  project/sch/  (schematic files)\n  project/pcb/  (PCB files)" ),
            schFolderPath );

        frame()->GetInfoBar()->RemoveAllButtons();
        frame()->GetInfoBar()->ShowMessageFor( msg, 10000, wxICON_ERROR );
        return 0;
    }

    wxFileName schPath;
    schPath.SetPath( schFolderPath );
    schPath.SetName( pcbPath.GetName() );
    schPath.SetExt( "kicad_sch" );

    if( !schPath.FileExists() )
    {
        wxString msg = wxString::Format(
            _( "Schematic file not found:\n%s\n\nExpected schematic file with same name as PCB in sch/ folder." ),
            schPath.GetFullPath() );

        frame()->GetInfoBar()->RemoveAllButtons();
        frame()->GetInfoBar()->ShowMessageFor( msg, 10000, wxICON_ERROR );
        return 0;
    }

    std::map<KIID_PATH, VECTOR2I> positions;
    std::map<wxString, int> sheetOffsets;

    if( !parseSchematicPositions( schPath.GetFullPath(), positions, sheetOffsets ) )
    {
        wxString msg = _( "Failed to parse schematic file." );
        frame()->GetInfoBar()->RemoveAllButtons();
        frame()->GetInfoBar()->ShowMessageFor( msg, 10000, wxICON_ERROR );
        return 0;
    }

    BOARD_COMMIT commit( frame() );
    constexpr int POS_SCALE = static_cast<int>( PCB_IU_PER_MM / SCH_IU_PER_MM );

    bool selectionActive = false;
    for( FOOTPRINT* fp : brd->Footprints() )
    {
        if( fp->IsSelected() )
        {
            selectionActive = true;
            break;
        }
    }

    int movedCount = 0;
    int skippedLocked = 0;

    for( FOOTPRINT* fp : brd->Footprints() )
    {
        KIID_PATH fpPath = fp->GetPath();

        if( positions.count( fpPath ) == 0 )
            continue;

        if( fp->IsLocked() )
        {
            skippedLocked++;
            continue;
        }

        if( selectionActive && !fp->IsSelected() )
            continue;

        VECTOR2I schPos = positions[fpPath];
        wxString sheetPath = fpPath.AsString();
        int lastSlash = sheetPath.Find( '/', true );
        wxString sheetKey = ( lastSlash == 0 ) ? "" : sheetPath.Mid( 0, lastSlash );
        int yOffset = sheetOffsets.count( sheetKey ) > 0 ? sheetOffsets[sheetKey] : 0;

        VECTOR2I newPos( schPos.x * POS_SCALE, ( schPos.y + yOffset ) * POS_SCALE );

        commit.Modify( fp );
        fp->SetPosition( newPos );
        movedCount++;
    }

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

    return 0;
}


void SCHEMATIC_LAYOUT_TOOL::setTransitions()
{
    Go( &SCHEMATIC_LAYOUT_TOOL::layoutFromSchematic, PCB_ACTIONS::layoutFromSchematic.MakeEvent() );
}
