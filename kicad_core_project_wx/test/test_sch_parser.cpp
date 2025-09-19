/*
 * Simple Schematic Parser Test Program
 * Directly parses .kicad_sch files without full project initialization
 */

#include <iostream>
#include <string>
#include <wx/string.h>
#include <wx/filename.h>
#include <wx/app.h>

// Include minimal KiCad headers for direct schematic parsing
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
#include <sch_io/kicad_sexpr/sch_io_kicad_sexpr.h>
#include <schematic.h>
#include <project.h>
#include <project/project_file.h>
#include <project/project_local_settings.h>
#include <settings/settings_manager.h>
#include <pgm_base.h>

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

// Helper function to print schematic statistics
void PrintSchematicStatistics(SCH_SHEET* root_sheet)
{
    if (!root_sheet) {
        std::cout << "ERROR: Root sheet is null!" << std::endl;
        return;
    }

    std::cout << "\n=== SCHEMATIC STATISTICS ===" << std::endl;

    SCH_SCREEN* screen = root_sheet->GetScreen();

    if (!screen) {
        std::cout << "ERROR: Root screen is null!" << std::endl;
        return;
    }

    std::cout << "Root sheet name: " << root_sheet->GetName().utf8_str() << std::endl;
    std::cout << "Root sheet filename: " << root_sheet->GetFileName().utf8_str() << std::endl;

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
                // Use simplified methods without sheet path
                wxString ref = symbol->GetRef(nullptr);
                wxString value = symbol->GetValue(false, nullptr, false);

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
    std::cout << "=== Simple KiCad Schematic Parser Test ===" << std::endl;
    std::cout << "Initializing wxWidgets..." << std::endl;

    // Initialize wxWidgets
    wxInitialize();

    std::cout << "wxWidgets initialized successfully." << std::endl;

    SCH_SHEET* root_sheet = nullptr;
    SCHEMATIC* schematic = nullptr;
    PROJECT_FILE* projectFile = nullptr;
    PROJECT_LOCAL_SETTINGS* localSettings = nullptr;

    try {
        // Path to the schematic file - look in the complex_hierarchy folder copied to build directory
        wxString schPath = wxT("complex_hierarchy/complex_hierarchy.kicad_sch");
        wxString projectPath = wxT("complex_hierarchy/complex_hierarchy.kicad_pro");

        // Make paths absolute relative to current working directory
        wxFileName schFile(schPath);
        wxFileName projFile(projectPath);
        if (!schFile.IsAbsolute()) {
            schFile.MakeAbsolute();
            schPath = schFile.GetFullPath();
        }
        if (!projFile.IsAbsolute()) {
            projFile.MakeAbsolute();
            projectPath = projFile.GetFullPath();
        }

        if (!wxFileName::FileExists(schPath)) {
            std::cout << "ERROR: Cannot find schematic file!" << std::endl;
            std::cout << "Tried path: " << schPath.utf8_str() << std::endl;
            std::cout << "Current working directory: " << wxGetCwd().utf8_str() << std::endl;
            wxUninitialize();
            return 1;
        }

        if (!wxFileName::FileExists(projectPath)) {
            std::cout << "ERROR: Cannot find project file!" << std::endl;
            std::cout << "Tried path: " << projectPath.utf8_str() << std::endl;
            wxUninitialize();
            return 1;
        }

        std::cout << "Loading schematic file: " << schPath.utf8_str() << std::endl;

        // Create a minimal SCHEMATIC with simplified initialization
        std::cout << "Creating SCHEMATIC with simplified initialization..." << std::endl;

        // Create project with minimal initialization to avoid private member access
        PROJECT* project = new PROJECT();

        // Create minimal project file and local settings objects
        projectFile = new PROJECT_FILE(projectPath.ToStdString());
        localSettings = new PROJECT_LOCAL_SETTINGS(project, projectPath.ToStdString());

        std::cout << "Project objects created, proceeding with SCHEMATIC..." << std::endl;

        // Try to create SCHEMATIC but catch any initialization issues
        try {
            std::cout << "Attempting to create SCHEMATIC..." << std::endl;
            schematic = new SCHEMATIC(project);
            std::cout << "SCHEMATIC created successfully!" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Failed to create SCHEMATIC: " << e.what() << std::endl;
            delete project;
            delete projectFile;
            delete localSettings;
            wxUninitialize();
            return 1;
        } catch (...) {
            std::cout << "Failed to create SCHEMATIC: unknown error" << std::endl;
            delete project;
            delete projectFile;
            delete localSettings;
            wxUninitialize();
            return 1;
        }

        // Create schematic IO parser
        std::cout << "Creating SCH_IO_KICAD_SEXPR parser..." << std::endl;
        SCH_IO_KICAD_SEXPR schIO;

        // Load the schematic file using our minimal SCHEMATIC
        std::cout << "Loading schematic file..." << std::endl;
        try {
            root_sheet = schIO.LoadSchematicFile(schPath, schematic);
            std::cout << "Schematic loaded successfully!" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Exception during loading: " << e.what() << std::endl;
            delete schematic;
            delete projectFile;
            delete localSettings;
            wxUninitialize();
            return 1;
        } catch (...) {
            std::cout << "Unknown exception during loading" << std::endl;
            delete schematic;
            delete projectFile;
            delete localSettings;
            wxUninitialize();
            return 1;
        }

        if (!root_sheet) {
            std::cout << "ERROR: Failed to load schematic file!" << std::endl;
            delete schematic;
            delete projectFile;
            delete localSettings;
            wxUninitialize();
            return 1;
        }

        std::cout << "Schematic file loaded successfully!" << std::endl;

        // Print detailed schematic information
        PrintSchematicStatistics(root_sheet);

        std::cout << "\n=== Test completed successfully! ===" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        if (root_sheet) {
            try {
                delete root_sheet;
            }
            catch (...) {
                std::cout << "Error during cleanup - ignored" << std::endl;
            }
        }
        if (schematic) {
            try {
                delete schematic;
            }
            catch (...) {
                std::cout << "Error during schematic cleanup - ignored" << std::endl;
            }
        }

        wxUninitialize();
        return 1;
    }
    catch (...) {
        std::cout << "Unknown exception caught!" << std::endl;
        if (root_sheet) {
            try {
                delete root_sheet;
            }
            catch (...) {
                std::cout << "Error during cleanup - ignored" << std::endl;
            }
        }
        if (schematic) {
            try {
                delete schematic;
            }
            catch (...) {
                std::cout << "Error during schematic cleanup - ignored" << std::endl;
            }
        }

        wxUninitialize();
        return 1;
    }

    // Safe cleanup: Delete objects in reverse order
    if (root_sheet) {
        try {
            delete root_sheet;
            root_sheet = nullptr;
        }
        catch (...) {
            std::cout << "Warning: Exception during root_sheet cleanup - continuing..." << std::endl;
        }
    }

    // Clean up schematic if we created one
    if (schematic) {
        try {
            delete schematic;
            schematic = nullptr;
        }
        catch (...) {
            std::cout << "Warning: Exception during schematic cleanup - continuing..." << std::endl;
        }
    }

    // Clean up project file and local settings
    try {
        delete projectFile;
        delete localSettings;
    }
    catch (...) {
        std::cout << "Warning: Exception during project cleanup - continuing..." << std::endl;
    }

    // Uninitialize wxWidgets
    wxUninitialize();
    return 0;
}
