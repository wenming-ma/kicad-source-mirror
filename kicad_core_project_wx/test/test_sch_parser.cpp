/*
 * Schematic Parser Test Program
 * Tests parsing .kicad_sch files and prints schematic information
 */

#include <iostream>
#include <string>
#include <wx/string.h>
#include <wx/filename.h>
#include <wx/app.h>

// Include necessary KiCad headers for schematic parsing
#include <schematic.h>
#include <sch_screen.h>
#include <sch_sheet.h>
#include <sch_symbol.h>
#include <sch_pin.h>
#include <sch_field.h>
#include <sch_line.h>
#include <sch_junction.h>
#include <sch_label.h>
#include <sch_text.h>
#include <sch_no_connect.h>
#include <sch_bus_entry.h>
#include <connection_graph.h>
#include <sch_io/kicad_sexpr/sch_io_kicad_sexpr.h>
#include <sch_io/sch_io_mgr.h>
#include <pgm_base.h>
#include <kiface_base.h>
#include <settings/settings_manager.h>
#include <project.h>
#include <sch_sheet_path.h>

// Simple wxApp implementation for minimal initialization
class SCH_TEST_APP : public wxApp
{
public:
    bool OnInit() override
    {
        // Minimal initialization
        return true;
    }
};

wxIMPLEMENT_APP_NO_MAIN(SCH_TEST_APP);

// Simple PGM implementation
class SCH_TEST_PGM : public PGM_BASE
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

// Helper function to print schematic statistics
void PrintSchematicStatistics(SCHEMATIC* schematic)
{
    if (!schematic) {
        std::cout << "ERROR: Schematic is null!" << std::endl;
        return;
    }

    std::cout << "\n=== SCHEMATIC STATISTICS ===" << std::endl;

    // Get the root sheet
    SCH_SHEET& root_sheet = schematic->Root();
    SCH_SCREEN* screen = root_sheet.GetScreen();

    if (!screen) {
        std::cout << "ERROR: Root screen is null!" << std::endl;
        return;
    }

    std::cout << "Root sheet name: " << root_sheet.GetName().utf8_str() << std::endl;
    std::cout << "Root sheet filename: " << root_sheet.GetFileName().utf8_str() << std::endl;

    // Count different types of items on the schematic
    size_t symbol_count = 0;
    size_t wire_count = 0;
    size_t label_count = 0;
    size_t junction_count = 0;
    size_t noconnect_count = 0;
    size_t bus_entry_count = 0;
    size_t text_count = 0;
    size_t sheet_count = 0;

    // Iterate through all items in the schematic
    for (SCH_ITEM* item : screen->Items()) {
        switch (item->Type()) {
            case SCH_SYMBOL_T:
                symbol_count++;
                break;
            case SCH_LINE_T:
                wire_count++;
                break;
            case SCH_LABEL_T:
            case SCH_GLOBAL_LABEL_T:
            case SCH_HIER_LABEL_T:
                label_count++;
                break;
            case SCH_JUNCTION_T:
                junction_count++;
                break;
            case SCH_NO_CONNECT_T:
                noconnect_count++;
                break;
            case SCH_BUS_WIRE_ENTRY_T:
            case SCH_BUS_BUS_ENTRY_T:
                bus_entry_count++;
                break;
            case SCH_TEXT_T:
                text_count++;
                break;
            case SCH_SHEET_T:
                sheet_count++;
                break;
            default:
                break;
        }
    }

    std::cout << "\n=== ITEM COUNTS ===" << std::endl;
    std::cout << "Symbols: " << symbol_count << std::endl;
    std::cout << "Wires: " << wire_count << std::endl;
    std::cout << "Labels: " << label_count << std::endl;
    std::cout << "Junctions: " << junction_count << std::endl;
    std::cout << "No-connects: " << noconnect_count << std::endl;
    std::cout << "Bus entries: " << bus_entry_count << std::endl;
    std::cout << "Text items: " << text_count << std::endl;
    std::cout << "Sub-sheets: " << sheet_count << std::endl;

    // Print detailed symbol information
    std::cout << "\n=== SYMBOL DETAILS ===" << std::endl;
    size_t symbol_index = 0;

    for (SCH_ITEM* item : screen->Items()) {
        if (item->Type() == SCH_SYMBOL_T) {
            SCH_SYMBOL* symbol = static_cast<SCH_SYMBOL*>(item);

            if (symbol_index < 10) { // Show first 10 symbols
                SCH_SHEET_PATH sheet_path;
                sheet_path.push_back(&root_sheet);

                wxString ref = symbol->GetRef(&sheet_path);
                wxString value = symbol->GetValue(false, &sheet_path, false);

                std::cout << "  [" << (symbol_index + 1) << "] Reference: "
                          << ref.utf8_str()
                          << ", Value: " << value.utf8_str()
                          << ", LibId: " << std::string(symbol->GetLibId().Format())
                          << std::endl;

                std::cout << "      Position: (" << symbol->GetPosition().x
                          << ", " << symbol->GetPosition().y << ")"
                          << ", Unit: " << symbol->GetUnit()
                          << ", Pins: " << symbol->GetPins().size()
                          << std::endl;

                // Show field information
                std::vector<SCH_FIELD*> fields;
                symbol->GetFields(fields, false);
                if (!fields.empty()) {
                    std::cout << "      Fields: ";
                    for (size_t i = 0; i < fields.size() && i < 3; ++i) {
                        std::cout << fields[i]->GetName().utf8_str()
                                  << "=\"" << fields[i]->GetText().utf8_str() << "\"";
                        if (i < fields.size() - 1 && i < 2) std::cout << ", ";
                    }
                    if (fields.size() > 3) {
                        std::cout << " + " << (fields.size() - 3) << " more";
                    }
                    std::cout << std::endl;
                }
            }
            symbol_index++;
        }
    }

    if (symbol_count > 10) {
        std::cout << "  ... and " << (symbol_count - 10) << " more symbols" << std::endl;
    }

    // Print wire/connection information
    std::cout << "\n=== WIRE DETAILS ===" << std::endl;
    size_t wire_index = 0;

    for (SCH_ITEM* item : screen->Items()) {
        if (item->Type() == SCH_LINE_T) {
            SCH_LINE* wire = static_cast<SCH_LINE*>(item);

            if (wire_index < 5) { // Show first 5 wires
                std::cout << "  [" << (wire_index + 1) << "] From: ("
                          << wire->GetStartPoint().x << ", " << wire->GetStartPoint().y
                          << ") To: (" << wire->GetEndPoint().x << ", " << wire->GetEndPoint().y
                          << "), Layer: " << wire->GetLayer() << std::endl;
            }
            wire_index++;
        }
    }

    if (wire_count > 5) {
        std::cout << "  ... and " << (wire_count - 5) << " more wires" << std::endl;
    }

    // Print label information
    std::cout << "\n=== LABEL DETAILS ===" << std::endl;
    size_t label_index = 0;

    for (SCH_ITEM* item : screen->Items()) {
        if (item->Type() == SCH_LABEL_T ||
            item->Type() == SCH_GLOBAL_LABEL_T ||
            item->Type() == SCH_HIER_LABEL_T) {

            SCH_LABEL_BASE* label = static_cast<SCH_LABEL_BASE*>(item);

            if (label_index < 10) { // Show first 10 labels
                const char* type_str = "Label";
                if (item->Type() == SCH_GLOBAL_LABEL_T) type_str = "Global";
                else if (item->Type() == SCH_HIER_LABEL_T) type_str = "Hierarchical";

                std::cout << "  [" << (label_index + 1) << "] " << type_str
                          << ": \"" << label->GetText().utf8_str() << "\""
                          << " at (" << label->GetPosition().x << ", " << label->GetPosition().y << ")"
                          << std::endl;
            }
            label_index++;
        }
    }

    if (label_count > 10) {
        std::cout << "  ... and " << (label_count - 10) << " more labels" << std::endl;
    }
}

int main(int argc, char* argv[])
{
    std::cout << "=== KiCad Schematic Parser Test ===" << std::endl;

    // Initialize wxWidgets
    wxInitialize();

    SCHEMATIC* schematic = nullptr;

    try {
        // Create a simple PGM instance
        SCH_TEST_PGM pgm;

        // Path to the schematic file
        wxString schPath = wxT("test/complex_hierarchy.kicad_sch");

        if (!wxFileName::FileExists(schPath)) {
            schPath = wxT("complex_hierarchy.kicad_sch");
            if (!wxFileName::FileExists(schPath)) {
                std::cout << "ERROR: Cannot find schematic file!" << std::endl;
                std::cout << "Tried: test/complex_hierarchy.kicad_sch and complex_hierarchy.kicad_sch" << std::endl;
                wxUninitialize();
                return 1;
            }
        }

        std::cout << "Loading schematic file: " << schPath.utf8_str() << std::endl;

        // Create schematic IO parser
        SCH_IO_KICAD_SEXPR schIO;

        // Create a project for the schematic
        PROJECT project;

        // Create schematic
        schematic = new SCHEMATIC(&project);

        // Load the schematic
        SCH_SHEET* root_sheet = schIO.LoadSchematicFile(schPath, schematic);

        if (!root_sheet) {
            std::cout << "ERROR: Failed to load schematic file!" << std::endl;
            delete schematic;
            wxUninitialize();
            return 1;
        }

        std::cout << "Schematic file loaded successfully!" << std::endl;

        // Print detailed schematic information
        PrintSchematicStatistics(schematic);

        std::cout << "\n=== Test completed successfully! ===" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        if (schematic) {
            try {
                delete schematic;
            }
            catch (...) {
                std::cout << "Error during cleanup - ignored" << std::endl;
            }
        }
        wxUninitialize();
        return 1;
    }
    catch (...) {
        std::cout << "Unknown exception caught!" << std::endl;
        if (schematic) {
            try {
                delete schematic;
            }
            catch (...) {
                std::cout << "Error during cleanup - ignored" << std::endl;
            }
        }
        wxUninitialize();
        return 1;
    }

    // Safe cleanup: Delete schematic before wxUninitialize()
    if (schematic) {
        try {
            delete schematic;
            schematic = nullptr;
        }
        catch (...) {
            std::cout << "Warning: Exception during schematic cleanup - continuing..." << std::endl;
        }
    }

    // Uninitialize wxWidgets after schematic cleanup
    wxUninitialize();
    return 0;
}
