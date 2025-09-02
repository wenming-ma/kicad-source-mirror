/*
 * PCB Parser Test Program
 * Tests parsing .kicad_pcb files and prints component information
 */

#include <iostream>
#include <string>
#include <wx/string.h>
#include <wx/filename.h>
#include <wx/app.h>

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

// Simple wxApp implementation for minimal initialization
class PCB_TEST_APP : public wxApp
{
public:
    bool OnInit() override
    {
        // Minimal initialization
        return true;
    }
};

wxIMPLEMENT_APP_NO_MAIN(PCB_TEST_APP);

// Simple PGM implementation
class PCB_TEST_PGM : public PGM_BASE
{
public:
    // Required abstract method
    void MacOpenFile( const wxString& aFileName ) override {}
    
    // Override virtual methods with correct signatures
    const wxString& GetKicadEnvVariable() const override 
    { 
        static wxString dummy;
        return dummy; 
    }
    
    const wxString& GetExecutablePath() const override 
    { 
        static wxString dummy;
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
        std::cout << "  [" << i+1 << "] Reference: " << fp->GetReference().utf8_str()
                  << ", Value: " << fp->GetValue().utf8_str()
                  << ", Position: (" << fp->GetPosition().x << ", " << fp->GetPosition().y << ")"
                  << ", Pads: " << fp->Pads().size()
                  << std::endl;
        
        // Print first few pads for each footprint
        const auto& pads = fp->Pads();
        for (size_t j = 0; j < pads.size() && j < 3; ++j) {
            const PAD* pad = pads[j];
            std::cout << "    Pad[" << j+1 << "]: " << pad->GetNumber().utf8_str()
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
        std::cout << "  [" << i+1 << "] Net: " << zone->GetNetname().utf8_str()
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
            std::cout << "  [" << i+1 << "] " << net->GetNetname().utf8_str()
                      << " (code: " << net->GetNetCode() << ")" << std::endl;
        }
    }
    
    if (nets.GetNetCount() > 10) {
        std::cout << "  ... and " << (nets.GetNetCount() - 10) << " more nets" << std::endl;
    }
}

int main(int argc, char* argv[])
{
    std::cout << "=== KiCad PCB Parser Test ===" << std::endl;
    
    // Initialize wxWidgets
    wxInitialize();
    
    try {
        // Create a simple PGM instance
        PCB_TEST_PGM pgm;
        
        // Path to the PCB file
        wxString pcbPath = wxT("test/complex_hierarchy.kicad_pcb");
        
        if (!wxFileName::FileExists(pcbPath)) {
            pcbPath = wxT("complex_hierarchy.kicad_pcb");
            if (!wxFileName::FileExists(pcbPath)) {
                std::cout << "ERROR: Cannot find PCB file!" << std::endl;
                std::cout << "Tried: test/complex_hierarchy.kicad_pcb and complex_hierarchy.kicad_pcb" << std::endl;
                return 1;
            }
        }
        
        std::cout << "Loading PCB file: " << pcbPath.utf8_str() << std::endl;
        
        // Create PCB IO parser
        PCB_IO_KICAD_SEXPR pcbIO;
        
        // Load the board
        BOARD* board = pcbIO.LoadBoard(pcbPath, nullptr);
        
        if (!board) {
            std::cout << "ERROR: Failed to load PCB file!" << std::endl;
            return 1;
        }
        
        std::cout << "PCB file loaded successfully!" << std::endl;
        
        // Print detailed board information
        PrintBoardStatistics(board);
        
        // Clean up
        delete board;
        
        std::cout << "\n=== Test completed successfully! ===" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cout << "Unknown exception caught!" << std::endl;
        return 1;
    }
    
    wxUninitialize();
    return 0;
}
