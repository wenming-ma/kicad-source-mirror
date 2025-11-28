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
#include <algorithm>
#include <map>
#include <rectpack2d/finders_interface.h>


SCHEMATIC_LAYOUT_TOOL::SCHEMATIC_LAYOUT_TOOL() : PCB_TOOL_BASE( "pcbnew.SchematicLayout" )
{
}


SCHEMATIC_LAYOUT_TOOL::~SCHEMATIC_LAYOUT_TOOL()
{
}


bool SCHEMATIC_LAYOUT_TOOL::parseSchematicPositions( const wxString& aSchematicPath,
                                                      std::map<KIID_PATH, VECTOR2I>& aPositions,
                                                      std::map<KIID_PATH, int>& aSheetOffsets )
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
        const double OFFSET_FACTOR = 0.05;  // Reduced from 1.25 to prevent coordinate overflow
        const int MAX_SHEET_OFFSET = 100 * SCH_IU_PER_MM;  // Max 100mm spacing per sheet
        const int MAX_TOTAL_OFFSET = 1000 * SCH_IU_PER_MM;  // Max 1000mm total offset

        for( const SCH_SHEET_PATH& sheetPath : sheetList )
        {
            SCH_SCREEN* screen = sheetPath.LastScreen();
            if( !screen )
                continue;

            wxString pathString = sheetPath.PathAsString();
            KIID_PATH pathKey( pathString );
            aSheetOffsets[pathKey] = currentYOffset;

            for( SCH_ITEM* item : screen->Items().OfType( SCH_SYMBOL_T ) )
            {
                SCH_SYMBOL* symbol = static_cast<SCH_SYMBOL*>( item );
                VECTOR2I pos = symbol->GetPosition();
                KIID_PATH fullPath( pathString );
                fullPath.push_back( symbol->m_Uuid );
                aPositions[fullPath] = pos;
            }

            BOX2I bounds = getScreenBoundingBox( screen );
            if( bounds.GetHeight() > 0 )
            {
                int sheetSpacing = static_cast<int>( bounds.GetHeight() * OFFSET_FACTOR );
                // Clamp to maximum allowed spacing to prevent coordinate overflow
                sheetSpacing = std::min( sheetSpacing, MAX_SHEET_OFFSET );

                // Check total offset doesn't exceed maximum
                if( currentYOffset + sheetSpacing <= MAX_TOTAL_OFFSET )
                    currentYOffset += sheetSpacing;
            }
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
    std::map<KIID_PATH, int> sheetOffsets;

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
        KIID_PATH sheetKey = fpPath;
        if( sheetKey.size() > 0 )
            sheetKey.pop_back();  // Remove symbol UUID to get sheet path
        int yOffset = sheetOffsets.count( sheetKey ) > 0 ? sheetOffsets[sheetKey] : 0;

        VECTOR2I newPos( schPos.x * POS_SCALE, ( schPos.y + yOffset ) * POS_SCALE );

        commit.Modify( fp );
        fp->SetPosition( newPos );
        movedCount++;
    }

    if( movedCount > 0 )
    {
        commit.Push( _( "Layout from Schematic" ) );

        // Group footprints by sheet and compact each group
        std::map<KIID_PATH, std::vector<FOOTPRINT*>> sheetGroups;

        for( FOOTPRINT* fp : brd->Footprints() )
        {
            KIID_PATH fpPath = fp->GetPath();
            if( positions.count( fpPath ) == 0 )
                continue;  // Not from schematic

            // Get sheet path (remove symbol UUID)
            KIID_PATH sheetPath = fpPath;
            if( sheetPath.size() > 0 )
                sheetPath.pop_back();

            sheetGroups[sheetPath].push_back( fp );
        }

        // Aggressive compaction: shrink gaps while preserving topology
        BOARD_COMMIT compactCommit( frame() );
        std::vector<double> compressionRatios;

        for( auto& [sheetPath, footprints] : sheetGroups )
        {
            wxString sheetName = sheetPath.AsString();
            if( sheetName.IsEmpty() )
                sheetName = "Root";

            for( FOOTPRINT* fp : footprints )
                compactCommit.Modify( fp );

            // Use aggressive compaction (NEW)
            double ratio = compactFootprintsAggressively( footprints, sheetName );
            compressionRatios.push_back( ratio );
        }
        compactCommit.Push( _( "Compact layout" ) );

        // Sheet-level packing: arrange sheet groups using rectangle packing
        BOARD_COMMIT packCommit( frame() );

        // Mark all footprints for modification
        for( auto& [sheetPath, footprints] : sheetGroups )
        {
            for( FOOTPRINT* fp : footprints )
                packCommit.Modify( fp );
        }

        // Apply rectangle packing algorithm
        packSheetGroups( sheetGroups );

        packCommit.Push( _( "Pack sheet groups" ) );

        // Calculate average compression percentage
        double avgCompression = 0.0;
        for( double r : compressionRatios )
            avgCompression += r;
        if( !compressionRatios.empty() )
            avgCompression /= compressionRatios.size();
        int compressionPercent = (int)( (1.0 - avgCompression) * 100 );

        // Updated success message with compression stats
        wxString msg = wxString::Format(
            _( "Layout complete!\n\n"
               "Moved %d footprint(s), compacted %d sheet group(s).\n"
               "Sheet-level packing applied.\n"
               "Average compression: %d%% (spacing reduced)\n"
               "Skipped %d locked footprint(s)." ),
            movedCount,
            (int)sheetGroups.size(),
            compressionPercent,
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


BOX2I SCHEMATIC_LAYOUT_TOOL::getScreenBoundingBox( SCH_SCREEN* aScreen ) const
{
    BOX2I bbox;

    if( !aScreen )
        return bbox;

    for( SCH_ITEM* item : aScreen->Items() )
        bbox.Merge( item->GetBoundingBox() );

    return bbox;
}


VECTOR2I SCHEMATIC_LAYOUT_TOOL::calculateCentroid(
    const std::vector<FOOTPRINT*>& aFootprints ) const
{
    if( aFootprints.empty() )
        return VECTOR2I( 0, 0 );

    long long sumX = 0, sumY = 0;

    for( const FOOTPRINT* fp : aFootprints )
    {
        VECTOR2I pos = fp->GetPosition();
        sumX += pos.x;
        sumY += pos.y;
    }

    return VECTOR2I( sumX / aFootprints.size(), sumY / aFootprints.size() );
}


bool SCHEMATIC_LAYOUT_TOOL::hasOverlaps(
    const std::vector<FOOTPRINT*>& aFootprints ) const
{
    const int CLEARANCE = 100000;  // 0.1mm in PCB_IU (nanometer units)

    for( size_t i = 0; i < aFootprints.size(); ++i )
    {
        BOX2I bbox_i = aFootprints[i]->GetBoundingBox( false, false );
        bbox_i.Inflate( -CLEARANCE / 2 );  // Shrink by half clearance each side

        for( size_t j = i + 1; j < aFootprints.size(); ++j )
        {
            BOX2I bbox_j = aFootprints[j]->GetBoundingBox( false, false );
            bbox_j.Inflate( -CLEARANCE / 2 );

            if( bbox_i.Intersects( bbox_j ) )
                return true;
        }
    }

    return false;
}


void SCHEMATIC_LAYOUT_TOOL::scaleTowardCentroid(
    std::vector<FOOTPRINT*>& aFootprints,
    const VECTOR2I& aCentroid,
    double aScaleFactor,
    const std::map<FOOTPRINT*, VECTOR2I>& aOriginalPositions )
{
    for( FOOTPRINT* fp : aFootprints )
    {
        VECTOR2I originalPos = aOriginalPositions.at( fp );
        VECTOR2I vectorFromCentroid = originalPos - aCentroid;

        // Scale the vector
        VECTOR2I scaledVector(
            static_cast<int>( vectorFromCentroid.x * aScaleFactor ),
            static_cast<int>( vectorFromCentroid.y * aScaleFactor )
        );

        // New position = centroid + scaled vector
        VECTOR2I newPos = aCentroid + scaledVector;
        fp->SetPosition( newPos );
    }
}


double SCHEMATIC_LAYOUT_TOOL::compactFootprintsAggressively(
    std::vector<FOOTPRINT*>& aFootprints,
    const wxString& aGroupName )
{
    if( aFootprints.empty() )
        return 1.0;

    // Step 1: Calculate geometric center
    VECTOR2I centroid = calculateCentroid( aFootprints );

    // Step 2: Save original positions
    std::map<FOOTPRINT*, VECTOR2I> originalPositions;
    for( FOOTPRINT* fp : aFootprints )
        originalPositions[fp] = fp->GetPosition();

    // Step 3: Binary search for optimal compression
    double minScale = 0.05;  // Allow up to 95% compression
    double maxScale = 1.0;   // No compression (original positions)
    double bestScale = 1.0;

    const int MAX_ITERATIONS = 15;
    const double PRECISION = 0.01;  // 1% precision

    for( int iter = 0; iter < MAX_ITERATIONS; ++iter )
    {
        double testScale = (minScale + maxScale) / 2.0;

        // Apply test scaling
        scaleTowardCentroid( aFootprints, centroid, testScale, originalPositions );

        // Check for overlaps
        bool overlaps = hasOverlaps( aFootprints );

        if( overlaps )
        {
            // Too aggressive, need to relax compression
            minScale = testScale;
        }
        else
        {
            // Success! Try more aggressive compression
            bestScale = testScale;
            maxScale = testScale;

            // Check if converged
            if( (maxScale - minScale) < PRECISION )
                break;
        }
    }

    // Step 4: Apply best found scale
    scaleTowardCentroid( aFootprints, centroid, bestScale, originalPositions );

    // Step 5: Verify no overlaps (safety assertion)
    wxASSERT_MSG( !hasOverlaps( aFootprints ),
                  wxString::Format( "Overlaps detected after compaction in group '%s'! "
                                   "This indicates a bug in the algorithm.",
                                   aGroupName ) );

    return bestScale;  // Return compression ratio for reporting
}


BOX2I SCHEMATIC_LAYOUT_TOOL::calculateGroupBoundingBox(
    const std::vector<FOOTPRINT*>& aFootprints ) const
{
    BOX2I bbox;

    if( aFootprints.empty() )
        return bbox;

    // Initialize with first footprint
    bbox = aFootprints[0]->GetBoundingBox( false, false );

    // Merge with remaining footprints
    for( size_t i = 1; i < aFootprints.size(); ++i )
    {
        bbox.Merge( aFootprints[i]->GetBoundingBox( false, false ) );
    }

    return bbox;
}


void SCHEMATIC_LAYOUT_TOOL::packSheetGroups(
    std::map<KIID_PATH, std::vector<FOOTPRINT*>>& aSheetGroups )
{
    if( aSheetGroups.empty() )
        return;

    // Prepare rectangles for packing
    struct SheetRect
    {
        KIID_PATH sheetPath;
        std::vector<FOOTPRINT*> footprints;
        BOX2I originalBBox;
        int width_mm;
        int height_mm;
    };

    std::vector<SheetRect> sheetRects;

    for( auto& [sheetPath, footprints] : aSheetGroups )
    {
        if( footprints.empty() )
            continue;

        BOX2I bbox = calculateGroupBoundingBox( footprints );

        SheetRect rect;
        rect.sheetPath = sheetPath;
        rect.footprints = footprints;
        rect.originalBBox = bbox;
        // Convert to mm and round up
        rect.width_mm = ( bbox.GetWidth() + PCB_IU_PER_MM - 1 ) / PCB_IU_PER_MM;
        rect.height_mm = ( bbox.GetHeight() + PCB_IU_PER_MM - 1 ) / PCB_IU_PER_MM;

        sheetRects.push_back( rect );
    }

    if( sheetRects.empty() )
        return;

    // Use rectpack2d to pack the sheets
    using spaces_type = rectpack2D::empty_spaces<false, rectpack2D::default_empty_spaces>;
    using rect_type = rectpack2D::output_rect_t<spaces_type>;

    std::vector<rect_type> rectangles;
    for( const auto& sr : sheetRects )
    {
        rectangles.push_back( rect_type( 0, 0, sr.width_mm, sr.height_mm ) );
    }

    // Add 5mm spacing between sheets
    const int SHEET_SPACING_MM = 5;

    // Find best packing
    auto report_successful = [](rect_type&) { return rectpack2D::callback_result::CONTINUE_PACKING; };
    auto report_failed = [](rect_type&) { return rectpack2D::callback_result::ABORT_PACKING; };

    const auto max_side = 2000;  // Maximum 2000mm canvas
    const auto discard_step = 1;

    const auto result_size = rectpack2D::find_best_packing<spaces_type>(
        rectangles,
        make_finder_input( max_side, discard_step, report_successful, report_failed, rectpack2D::flipping_option::DISABLED )
    );

    // Apply packing results: move each sheet group to its packed position
    for( size_t i = 0; i < sheetRects.size() && i < rectangles.size(); ++i )
    {
        const auto& packedRect = rectangles[i];
        auto& sheetRect = sheetRects[i];

        // Calculate the offset from original position to packed position
        VECTOR2I originalTopLeft( sheetRect.originalBBox.GetX(), sheetRect.originalBBox.GetY() );
        VECTOR2I packedTopLeft(
            packedRect.x * PCB_IU_PER_MM + SHEET_SPACING_MM * PCB_IU_PER_MM * (int)i,
            packedRect.y * PCB_IU_PER_MM
        );

        VECTOR2I offset = packedTopLeft - originalTopLeft;

        // Move all footprints in this sheet by the offset
        for( FOOTPRINT* fp : sheetRect.footprints )
        {
            VECTOR2I newPos = fp->GetPosition() + offset;
            fp->SetPosition( newPos );
        }
    }
}


void SCHEMATIC_LAYOUT_TOOL::setTransitions()
{
    Go( &SCHEMATIC_LAYOUT_TOOL::layoutFromSchematic, PCB_ACTIONS::layoutFromSchematic.MakeEvent() );
}
