/*
 * PCB Parser Test Program
 * Tests parsing .kicad_pcb files and prints component information
 */

#include <iostream>
#include <string>
#include <QString>
#include <QFileInfo>
#include <QCoreApplication>

// Include necessary KiCad headers
#include <board.h>
#include <footprint.h>
#include <pad.h>
#include <pcb_track.h>
#include <pcb_shape.h>
#include <pcb_text.h>
#include <zone.h>
#include <layer_ids.h>
#include <pcb_io/kicad_sexpr/pcb_io_kicad_sexpr.h>
#include <pcb_io/pcb_io_mgr.h>
#include <pgm_base.h>
#include <kiface_base.h>
#include <settings/settings_manager.h>
#include <board_design_settings.h>
#include <geometry/shape.h>
#include <geometry/shape_circle.h>
#include <geometry/shape_rect.h>
#include <geometry/shape_poly_set.h>
#include <geometry/shape_compound.h>
#include <geometry/shape_segment.h>
#include <padstack.h>
#include <convert_basic_shapes_to_polygon.h>

// Simple QCoreApplication wrapper for minimal initialization
class PCB_TEST_APP
{
public:
    static QCoreApplication* createApp(int& argc, char* argv[])
    {
        return new QCoreApplication(argc, argv);
    }
};

// Simple PGM implementation
class PCB_TEST_PGM : public PGM_BASE
{
public:
    // Required abstract method
    void MacOpenFile( const QString& aFileName ) override {}
    
    // Override virtual methods with correct signatures
    const QString& GetKicadEnvVariable() const override 
    { 
        static QString dummy;
        return dummy; 
    }
    
    const QString& GetExecutablePath() const override 
    { 
        static QString dummy;
        return dummy; 
    }
};

// Helper function to print BOARD statistics
void PrintBoardStatistics(BOARD* board)
{
    if (!board) {
        std::cout << "ERROR: Board is null!" << std::endl;
        return;
    }

    std::cout << "\n=== PCB BOARD STATISTICS ===" << std::endl;
    
    // Board basic info
    std::cout << "Board thickness: " << board->GetDesignSettings().GetBoardThickness() << " nanometers" << std::endl;
    std::cout << "Copper layer count: " << board->GetCopperLayerCount() << std::endl;
    
    // Footprints
    const FOOTPRINTS& footprints = board->Footprints();
    std::cout << "\nFootprints: " << footprints.size() << std::endl;
    
    for (size_t i = 0; i < footprints.size() && i < 10; ++i) {
        const FOOTPRINT* fp = footprints[i];
        std::cout << "  [" << i+1 << "] Reference: " << fp->GetReference().toUtf8().constData()
                  << ", Value: " << fp->GetValue().toUtf8().constData()
                  << ", Position: (" << fp->GetPosition().x << ", " << fp->GetPosition().y << ")"
                  << ", Pads: " << fp->Pads().size()
                  << std::endl;
        
        // Print first few pads for each footprint
        const auto& pads = fp->Pads();
        for (size_t j = 0; j < pads.size() && j < 3; ++j) {
            const PAD* pad = pads[j];
            std::cout << "    Pad[" << j+1 << "]: " << pad->GetNumber().toUtf8().constData()
                      << " at (" << pad->GetPosition().x << ", " << pad->GetPosition().y << ")"
                      << " size: " << pad->GetSize(F_Cu).x << "x" << pad->GetSize(F_Cu).y
                      << std::endl;
        }
        if (pads.size() > 3) {
            std::cout << "    ... and " << (pads.size() - 3) << " more pads" << std::endl;
        }
    }
    
    if (footprints.size() > 10) {
        std::cout << "  ... and " << (footprints.size() - 10) << " more footprints" << std::endl;
    }
    
    // Tracks
    const TRACKS& tracks = board->Tracks();
    std::cout << "\nTracks: " << tracks.size() << std::endl;
    
    size_t track_count = 0, via_count = 0, arc_count = 0;
    for (const PCB_TRACK* track : tracks) {
        switch (track->Type()) {
            case PCB_TRACE_T: track_count++; break;
            case PCB_VIA_T: via_count++; break;
            case PCB_ARC_T: arc_count++; break;
            default: break;
        }
    }
    
    std::cout << "  - Track segments: " << track_count << std::endl;
    std::cout << "  - Vias: " << via_count << std::endl;
    std::cout << "  - Arc segments: " << arc_count << std::endl;
    
    // Zones
    const ZONES& zones = board->Zones();
    std::cout << "\nZones: " << zones.size() << std::endl;
    
    for (size_t i = 0; i < zones.size() && i < 5; ++i) {
        const ZONE* zone = zones[i];
        std::cout << "  [" << i+1 << "] Net: " << zone->GetNetname().toUtf8().constData()
                  << ", Layer: " << zone->GetLayer()
                  << ", Priority: " << zone->GetAssignedPriority()
                  << std::endl;
    }
    
    if (zones.size() > 5) {
        std::cout << "  ... and " << (zones.size() - 5) << " more zones" << std::endl;
    }
    
    // Graphics/Drawings
    const DRAWINGS& drawings = board->Drawings();
    std::cout << "\nDrawings/Graphics: " << drawings.size() << std::endl;
    
    size_t shape_count = 0, text_count = 0, other_count = 0;
    for (const BOARD_ITEM* item : drawings) {
        switch (item->Type()) {
            case PCB_SHAPE_T: shape_count++; break;
            case PCB_TEXT_T: text_count++; break;
            default: other_count++; break;
        }
    }
    
    std::cout << "  - Shapes: " << shape_count << std::endl;
    std::cout << "  - Text: " << text_count << std::endl;
    std::cout << "  - Other: " << other_count << std::endl;
    
    // Net information
    const NETINFO_LIST& nets = board->GetNetInfo();
    std::cout << "\nNets: " << nets.GetNetCount() << std::endl;
    
    for (size_t i = 0; i < std::min(static_cast<size_t>(nets.GetNetCount()), static_cast<size_t>(10)); ++i) {
        const NETINFO_ITEM* net = nets.GetNetItem(i);
        if (net) {
            std::cout << "  [" << i+1 << "] " << net->GetNetname().toUtf8().constData()
                      << " (code: " << net->GetNetCode() << ")" << std::endl;
        }
    }
    
    if (nets.GetNetCount() > 10) {
        std::cout << "  ... and " << (nets.GetNetCount() - 10) << " more nets" << std::endl;
    }
}

// Function to print detailed pad render data
void PrintPadRenderData(const PAD* pad)
{
    if (!pad) {
        std::cout << "ERROR: Pad is null!" << std::endl;
        return;
    }
    
    std::cout << "\n=== PAD RENDER DATA ===" << std::endl;
    std::cout << "Pad Number: " << pad->GetNumber().toUtf8().constData() << std::endl;
    std::cout << "Position: (" << pad->GetPosition().x << ", " << pad->GetPosition().y << ")" << std::endl;
    std::cout << "Orientation: " << pad->GetOrientation().AsDegrees() << " degrees" << std::endl;
    
    // Pad attributes
    std::cout << "\n--- Pad Attributes ---" << std::endl;
    std::cout << "Attribute: ";
    switch (pad->GetAttribute()) {
        case PAD_ATTRIB::PTH: std::cout << "PTH (Plated Through Hole)"; break;
        case PAD_ATTRIB::SMD: std::cout << "SMD (Surface Mount)"; break;
        case PAD_ATTRIB::CONN: std::cout << "CONN (Connector)"; break;
        case PAD_ATTRIB::NPTH: std::cout << "NPTH (Non-Plated Through Hole)"; break;
        default: std::cout << "Unknown"; break;
    }
    std::cout << std::endl;
    
    std::cout << "Property: ";
    switch (pad->GetProperty()) {
        case PAD_PROP::NONE: std::cout << "None"; break;
        case PAD_PROP::BGA: std::cout << "BGA"; break;
        case PAD_PROP::FIDUCIAL_GLBL: std::cout << "Fiducial Global"; break;
        case PAD_PROP::FIDUCIAL_LOCAL: std::cout << "Fiducial Local"; break;
        case PAD_PROP::TESTPOINT: std::cout << "Test Point"; break;
        case PAD_PROP::HEATSINK: std::cout << "Heat Sink"; break;
        case PAD_PROP::CASTELLATED: std::cout << "Castellated"; break;
        case PAD_PROP::MECHANICAL: std::cout << "Mechanical"; break;
        default: std::cout << "Unknown"; break;
    }
    std::cout << std::endl;
    
    // Layer information
    std::cout << "\n--- Layer Information ---" << std::endl;
    LSET layers = pad->GetLayerSet();
    std::cout << "Present on layers: ";
    for (int layer = 0; layer < PCB_LAYER_ID_COUNT; ++layer) {
        if (layers.test(layer)) {
            std::cout << layer << " ";
        }
    }
    std::cout << std::endl;
    
    // Shape information for each copper layer
    std::cout << "\n--- Shape Information per Layer ---" << std::endl;
    const std::vector<PCB_LAYER_ID> copperLayers = {
        F_Cu, B_Cu, In1_Cu, In2_Cu, In3_Cu, In4_Cu
    };
    
    for (PCB_LAYER_ID layer : copperLayers) {
        if (!pad->IsOnLayer(layer))
            continue;
            
        std::cout << "\nLayer " << layer << ":" << std::endl;
        
        // Shape type
        PAD_SHAPE shape = pad->GetShape(layer);
        std::cout << "  Shape: ";
        switch (shape) {
            case PAD_SHAPE::CIRCLE: std::cout << "CIRCLE"; break;
            case PAD_SHAPE::RECTANGLE: std::cout << "RECTANGLE"; break;
            case PAD_SHAPE::OVAL: std::cout << "OVAL"; break;
            case PAD_SHAPE::TRAPEZOID: std::cout << "TRAPEZOID"; break;
            case PAD_SHAPE::ROUNDRECT: std::cout << "ROUNDRECT"; break;
            case PAD_SHAPE::CHAMFERED_RECT: std::cout << "CHAMFERED_RECT"; break;
            case PAD_SHAPE::CUSTOM: std::cout << "CUSTOM"; break;
            default: std::cout << "Unknown"; break;
        }
        std::cout << std::endl;
        
        // Size
        VECTOR2I size = pad->GetSize(layer);
        std::cout << "  Size: " << size.x << " x " << size.y << " nm" << std::endl;
        
        // Offset
        VECTOR2I offset = pad->GetOffset(layer);
        std::cout << "  Offset: (" << offset.x << ", " << offset.y << ")" << std::endl;
        
        // Delta (for trapezoid)
        if (shape == PAD_SHAPE::TRAPEZOID) {
            VECTOR2I delta = pad->GetDelta(layer);
            std::cout << "  Delta: (" << delta.x << ", " << delta.y << ")" << std::endl;
        }
        
        // Round rect ratio
        if (shape == PAD_SHAPE::ROUNDRECT) {
            std::cout << "  Round Rect Ratio: " << pad->GetRoundRectRadiusRatio(layer) << std::endl;
            std::cout << "  Round Rect Radius: " << pad->GetRoundRectCornerRadius(layer) << " nm" << std::endl;
        }
        
        // Chamfered rect info
        if (shape == PAD_SHAPE::CHAMFERED_RECT) {
            std::cout << "  Chamfer Ratio: " << pad->GetChamferRectRatio(layer) << std::endl;
            std::cout << "  Chamfer Positions: " << pad->GetChamferPositions(layer) << std::endl;
        }
    }
    
    // Drill information
    std::cout << "\n--- Drill Information ---" << std::endl;
    if (pad->HasHole()) {
        VECTOR2I drillSize = pad->GetDrillSize();
        std::cout << "Drill Size: " << drillSize.x << " x " << drillSize.y << " nm" << std::endl;
        std::cout << "Drill Shape: ";
        switch (pad->GetDrillShape()) {
            case PAD_DRILL_SHAPE::UNDEFINED: std::cout << "UNDEFINED"; break;
            case PAD_DRILL_SHAPE::CIRCLE: std::cout << "CIRCLE"; break;
            case PAD_DRILL_SHAPE::OBLONG: std::cout << "OBLONG"; break;
            default: std::cout << "Unknown"; break;
        }
        std::cout << std::endl;
    } else {
        std::cout << "No drill hole" << std::endl;
    }
    
    // Clearances and margins
    std::cout << "\n--- Clearances and Margins ---" << std::endl;
    if (pad->GetLocalClearance().has_value()) {
        std::cout << "Local Clearance: " << pad->GetLocalClearance().value() << " nm" << std::endl;
    }
    if (pad->GetLocalSolderMaskMargin().has_value()) {
        std::cout << "Solder Mask Margin: " << pad->GetLocalSolderMaskMargin().value() << " nm" << std::endl;
    }
    if (pad->GetLocalSolderPasteMargin().has_value()) {
        std::cout << "Solder Paste Margin: " << pad->GetLocalSolderPasteMargin().value() << " nm" << std::endl;
    }
    if (pad->GetLocalSolderPasteMarginRatio().has_value()) {
        std::cout << "Solder Paste Margin Ratio: " << pad->GetLocalSolderPasteMarginRatio().value() << std::endl;
    }
    
    // Thermal settings
    std::cout << "\n--- Thermal Settings ---" << std::endl;
    std::cout << "Thermal Spoke Angle: " << pad->GetThermalSpokeAngle().AsDegrees() << " degrees" << std::endl;
    std::cout << "Thermal Gap: " << pad->GetThermalGap() << " nm" << std::endl;
    if (pad->GetLocalThermalSpokeWidthOverride().has_value()) {
        std::cout << "Thermal Spoke Width: " << pad->GetLocalThermalSpokeWidthOverride().value() << " nm" << std::endl;
    }
    
    // Zone connection
    std::cout << "Zone Connection: ";
    switch (pad->GetLocalZoneConnection()) {
        case ZONE_CONNECTION::INHERITED: std::cout << "INHERITED"; break;
        case ZONE_CONNECTION::NONE: std::cout << "NONE"; break;
        case ZONE_CONNECTION::THERMAL: std::cout << "THERMAL"; break;
        case ZONE_CONNECTION::FULL: std::cout << "FULL"; break;
        case ZONE_CONNECTION::THT_THERMAL: std::cout << "THT_THERMAL"; break;
        default: std::cout << "Unknown"; break;
    }
    std::cout << std::endl;
    
    // Net information
    std::cout << "\n--- Net Information ---" << std::endl;
    std::cout << "Net Code: " << pad->GetNetCode() << std::endl;
    std::cout << "Net Name: " << pad->GetNetname().toUtf8().constData() << std::endl;
    std::cout << "Pin Function: " << pad->GetPinFunction().toUtf8().constData() << std::endl;
    std::cout << "Pin Type: " << pad->GetPinType().toUtf8().constData() << std::endl;
}

// Function to test pad shape calculation
void TestPadShapeCalculation(PAD* pad)
{
    if (!pad) {
        std::cout << "ERROR: Pad is null!" << std::endl;
        return;
    }
    
    std::cout << "\n=== PAD SHAPE CALCULATION ===" << std::endl;
    std::cout << "Testing shape calculation for pad: " << pad->GetNumber().toUtf8().constData() << std::endl;
    
    // Build effective shapes (this is what would be done before rendering)
    pad->BuildEffectiveShapes();
    
    // Test shape calculation for each copper layer
    const std::vector<PCB_LAYER_ID> testLayers = { F_Cu, B_Cu };
    
    for (PCB_LAYER_ID layer : testLayers) {
        if (!pad->IsOnLayer(layer))
            continue;
            
        std::cout << "\n--- Layer " << layer << " Shape Data ---" << std::endl;
        
        // Get effective shape (used for collision detection and rendering)
        std::shared_ptr<SHAPE> effectiveShape = pad->GetEffectiveShape(layer);
        if (effectiveShape) {
            std::cout << "Effective Shape Type: ";
            switch (effectiveShape->Type()) {
                case SH_RECT: std::cout << "RECTANGLE"; break;
                case SH_CIRCLE: std::cout << "CIRCLE"; break;
                case SH_SEGMENT: std::cout << "SEGMENT"; break;
                case SH_SIMPLE: std::cout << "SIMPLE_POLYGON"; break;
                case SH_POLY_SET: std::cout << "POLYGON_SET"; break;
                case SH_COMPOUND: std::cout << "COMPOUND"; break;
                case SH_ARC: std::cout << "ARC"; break;
                case SH_NULL: std::cout << "NULL"; break;
                default: std::cout << "Unknown"; break;
            }
            std::cout << std::endl;
            
            // Get bounding box
            BOX2I bbox = effectiveShape->BBox();
            std::cout << "Bounding Box: (" << bbox.GetX() << ", " << bbox.GetY() 
                      << ") size: " << bbox.GetWidth() << " x " << bbox.GetHeight() << std::endl;
        }
        
        // Get effective polygon (used for zone filling and some rendering)
        const std::shared_ptr<SHAPE_POLY_SET>& effectivePoly = pad->GetEffectivePolygon(layer);
        if (effectivePoly && effectivePoly->OutlineCount() > 0) {
            std::cout << "Effective Polygon:" << std::endl;
            std::cout << "  Outline Count: " << effectivePoly->OutlineCount() << std::endl;
            std::cout << "  Total Vertices: " << effectivePoly->TotalVertices() << std::endl;
            std::cout << "  Area: " << effectivePoly->Area() << " nm^2" << std::endl;
            
            // Print first few vertices
            if (effectivePoly->OutlineCount() > 0) {
                const SHAPE_LINE_CHAIN& outline = effectivePoly->Outline(0);
                std::cout << "  First outline vertices (max 5): " << std::endl;
                for (int i = 0; i < std::min(5, outline.PointCount()); ++i) {
                    const VECTOR2I& pt = outline.CPoint(i);
                    std::cout << "    [" << i << "]: (" << pt.x << ", " << pt.y << ")" << std::endl;
                }
                if (outline.PointCount() > 5) {
                    std::cout << "    ... and " << (outline.PointCount() - 5) << " more vertices" << std::endl;
                }
            }
        }
        
        // Test conversion to polygon for rendering
        SHAPE_POLY_SET renderPolygon;
        int clearance = 0;  // No additional clearance for base rendering
        int maxError = 100; // 100nm max error for arc approximation
        pad->TransformShapeToPolygon(renderPolygon, layer, clearance, maxError, ERROR_INSIDE);
        
        std::cout << "Render Polygon (TransformShapeToPolygon):" << std::endl;
        std::cout << "  Outline Count: " << renderPolygon.OutlineCount() << std::endl;
        std::cout << "  Total Vertices: " << renderPolygon.TotalVertices() << std::endl;
    }
    
    // Get effective hole shape if pad has a hole
    if (pad->HasHole()) {
        std::cout << "\n--- Hole Shape Data ---" << std::endl;
        std::shared_ptr<SHAPE_SEGMENT> holeShape = pad->GetEffectiveHoleShape();
        if (holeShape) {
            std::cout << "Hole Segment: (" << holeShape->GetSeg().A.x << ", " << holeShape->GetSeg().A.y 
                      << ") to (" << holeShape->GetSeg().B.x << ", " << holeShape->GetSeg().B.y << ")" << std::endl;
            std::cout << "Hole Width (diameter): " << holeShape->GetWidth() << " nm" << std::endl;
        }
        
        // Test hole polygon conversion
        SHAPE_POLY_SET holePolygon;
        int maxError = 100; // 100nm max error for arc approximation
        if (pad->TransformHoleToPolygon(holePolygon, 0, maxError, ERROR_INSIDE)) {
            std::cout << "Hole Polygon:" << std::endl;
            std::cout << "  Outline Count: " << holePolygon.OutlineCount() << std::endl;
            std::cout << "  Total Vertices: " << holePolygon.TotalVertices() << std::endl;
        }
    }
    
    // Custom shape primitives (if any)
    if (pad->GetShape(F_Cu) == PAD_SHAPE::CUSTOM) {
        std::cout << "\n--- Custom Shape Primitives ---" << std::endl;
        const std::vector<std::shared_ptr<PCB_SHAPE>>& primitives = pad->GetPrimitives(F_Cu);
        std::cout << "Primitive Count: " << primitives.size() << std::endl;
        for (size_t i = 0; i < primitives.size() && i < 5; ++i) {
            const PCB_SHAPE* shape = primitives[i].get();
            std::cout << "  Primitive [" << i << "]: ";
            switch (shape->GetShape()) {
                case SHAPE_T::SEGMENT: std::cout << "SEGMENT"; break;
                case SHAPE_T::RECTANGLE: std::cout << "RECTANGLE"; break;
                case SHAPE_T::ARC: std::cout << "ARC"; break;
                case SHAPE_T::CIRCLE: std::cout << "CIRCLE"; break;
                case SHAPE_T::POLY: std::cout << "POLYGON"; break;
                case SHAPE_T::BEZIER: std::cout << "BEZIER"; break;
                default: std::cout << "Unknown"; break;
            }
            std::cout << std::endl;
        }
    }
    
    // Bounding box and radius
    std::cout << "\n--- Overall Geometry ---" << std::endl;
    BOX2I padBBox = pad->GetBoundingBox();
    std::cout << "Pad Bounding Box: (" << padBBox.GetX() << ", " << padBBox.GetY() 
              << ") size: " << padBBox.GetWidth() << " x " << padBBox.GetHeight() << std::endl;
    std::cout << "Bounding Radius: " << pad->GetBoundingRadius() << " nm" << std::endl;
}

// Main test function for pad workflow
void TestPadWorkflow(BOARD* board)
{
    if (!board) {
        std::cout << "ERROR: Board is null!" << std::endl;
        return;
    }
    
    std::cout << "\n\n=== TESTING PAD WORKFLOW: File -> Parser -> PAD Object -> Shape Calculation ===" << std::endl;
    std::cout << "=================================================================================" << std::endl;
    
    const FOOTPRINTS& footprints = board->Footprints();
    
    // Find a footprint with pads to test
    FOOTPRINT* testFootprint = nullptr;
    for (FOOTPRINT* fp : footprints) {
        if (fp->Pads().size() > 0) {
            testFootprint = fp;
            break;
        }
    }
    
    if (!testFootprint) {
        std::cout << "ERROR: No footprint with pads found!" << std::endl;
        return;
    }
    
    std::cout << "\nSelected footprint for testing: " << testFootprint->GetReference().toUtf8().constData() << std::endl;
    std::cout << "Total pads in footprint: " << testFootprint->Pads().size() << std::endl;
    
    // Test up to 3 different types of pads
    int padCount = 0;
    const int maxPadsToTest = 3;
    
    for (PAD* pad : testFootprint->Pads()) {
        if (padCount >= maxPadsToTest)
            break;
            
        std::cout << "\n========================================" << std::endl;
        std::cout << "Testing Pad " << (padCount + 1) << " of " << maxPadsToTest << std::endl;
        std::cout << "========================================" << std::endl;
        
        // Step 1: Print all pad render data
        PrintPadRenderData(pad);
        
        // Step 2: Test shape calculation
        TestPadShapeCalculation(pad);
        
        padCount++;
    }
    
    std::cout << "\n=== PAD WORKFLOW TEST COMPLETED ===" << std::endl;
}

int main(int argc, char* argv[])
{
    std::cout << "=== KiCad PCB Parser Test ===" << std::endl;
    
    // Initialize Qt application
    QCoreApplication* app = PCB_TEST_APP::createApp(argc, argv);
    
    BOARD* board = nullptr;
    
    try {
        // Create a simple PGM instance
        PCB_TEST_PGM pgm;
        
        // Path to the PCB file
        QString pcbPath = "test/complex_hierarchy.kicad_pcb";
        
        if (!QFileInfo::exists(pcbPath)) {
            pcbPath = "complex_hierarchy.kicad_pcb";
            if (!QFileInfo::exists(pcbPath)) {
                std::cout << "ERROR: Cannot find PCB file!" << std::endl;
                std::cout << "Tried: test/complex_hierarchy.kicad_pcb and complex_hierarchy.kicad_pcb" << std::endl;
                delete app;
                return 1;
            }
        }
        
        std::cout << "Loading PCB file: " << pcbPath.toUtf8().constData() << std::endl;
        
        // Create PCB IO parser
        PCB_IO_KICAD_SEXPR pcbIO;
        
        // Load the board
        board = pcbIO.LoadBoard(pcbPath, nullptr);
        
        if (!board) {
            std::cout << "ERROR: Failed to load PCB file!" << std::endl;
            delete app;
            return 1;
        }
        
        std::cout << "PCB file loaded successfully!" << std::endl;
        
        // Print detailed board information
        PrintBoardStatistics(board);
        
        // Test the pad workflow
        TestPadWorkflow(board);
        
        std::cout << "\n=== Test completed successfully! ===" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        if (board) {
            try {
                delete board;
            }
            catch (...) {
                std::cout << "Error during cleanup - ignored" << std::endl;
            }
        }
        delete app;
        return 1;
    }
    catch (...) {
        std::cout << "Unknown exception caught!" << std::endl;
        if (board) {
            try {
                delete board;
            }
            catch (...) {
                std::cout << "Error during cleanup - ignored" << std::endl;
            }
        }
        delete app;
        return 1;
    }
    
    // Safe cleanup: Delete board before deleting Qt app
    if (board) {
        try {
            delete board;
            board = nullptr;
        }
        catch (...) {
            std::cout << "Warning: Exception during board cleanup - continuing..." << std::endl;
        }
    }
    
    // Clean up Qt application after board cleanup
    delete app;
    return 0;
}
