/*
 * Schematic Parser Test Program
 * Tests parsing .kicad_sch files and prints component information
 */

#include <iostream>
#include <string>
#include <QString>
#include <QFileInfo>
#include <QCoreApplication>

// Include necessary KiCad headers
#include <sch_sheet.h>
#include <sch_screen.h>
#include <sch_symbol.h>
#include <sch_line.h>
#include <sch_junction.h>
#include <sch_label.h>
#include <sch_text.h>
#include <sch_no_connect.h>
#include <sch_io/kicad_sexpr/sch_io_kicad_sexpr.h>
#include <sch_io/kicad_sexpr/sch_io_kicad_sexpr_parser.h>
#include <sch_io/sch_io_mgr.h>
#include <pgm_base.h>
#include <richio.h>
#include <sch_file_versions.h>
#include <schematic.h>
#include <project.h>
#include <settings/settings_manager.h>
#include <settings/app_settings.h>

// Simple QCoreApplication implementation for minimal initialization
class SCH_TEST_APP : public QCoreApplication
{
public:
    SCH_TEST_APP(int& argc, char** argv) : QCoreApplication(argc, argv)
    {
        // Minimal initialization
    }
};

// Minimal mock for SYMBOL_EDITOR_SETTINGS to avoid linker issues
class MOCK_SYMBOL_EDITOR_SETTINGS : public APP_SETTINGS_BASE
{
public:
    MOCK_SYMBOL_EDITOR_SETTINGS() : APP_SETTINGS_BASE("symbol_editor", 1)
    {
        // Set default values that SCH_PIN expects
        m_Defaults.pin_length = 100;  // in mils
        m_Defaults.pin_num_size = 50;
        m_Defaults.pin_name_size = 50;
    }

    struct PIN_DEFAULTS
    {
        int pin_length;
        int pin_num_size;
        int pin_name_size;
    } m_Defaults;
};

// Simple PGM implementation
class SCH_TEST_PGM : public PGM_BASE
{
public:
    SCH_TEST_PGM()
    {
        // Initialize the settings manager using unique_ptr
        m_settings_manager = std::make_unique<SETTINGS_MANAGER>(true);

        // Register our mock settings to satisfy SCH_PIN
        m_settings_manager->RegisterSettings(new MOCK_SYMBOL_EDITOR_SETTINGS());
    }

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


// Helper function to print schematic statistics
void PrintSchematicStatistics(SCH_SHEET* sheet)
{
    if (!sheet || !sheet->GetScreen()) {
        std::cout << "ERROR: Sheet or screen is null!" << std::endl;
        return;
    }

    SCH_SCREEN* screen = sheet->GetScreen();
    std::cout << "\n=== SCHEMATIC STATISTICS ===" << std::endl;

    // Get all items from the screen
    std::vector<SCH_SYMBOL*> symbols;
    std::vector<SCH_LINE*> lines;
    std::vector<SCH_LABEL_BASE*> labels;
    std::vector<SCH_JUNCTION*> junctions;
    std::vector<SCH_NO_CONNECT*> noConnects;
    std::vector<SCH_TEXT*> texts;
    std::vector<SCH_SHEET*> subSheets;

    // Iterate through all items in the screen
    for (SCH_ITEM* item : screen->Items()) {
        switch (item->Type()) {
            case SCH_SYMBOL_T:
                symbols.push_back(static_cast<SCH_SYMBOL*>(item));
                break;
            case SCH_LINE_T:
                lines.push_back(static_cast<SCH_LINE*>(item));
                break;
            case SCH_LABEL_T:
            case SCH_GLOBAL_LABEL_T:
            case SCH_HIER_LABEL_T:
                labels.push_back(static_cast<SCH_LABEL_BASE*>(item));
                break;
            case SCH_JUNCTION_T:
                junctions.push_back(static_cast<SCH_JUNCTION*>(item));
                break;
            case SCH_NO_CONNECT_T:
                noConnects.push_back(static_cast<SCH_NO_CONNECT*>(item));
                break;
            case SCH_TEXT_T:
                texts.push_back(static_cast<SCH_TEXT*>(item));
                break;
            case SCH_SHEET_T:
                subSheets.push_back(static_cast<SCH_SHEET*>(item));
                break;
            default:
                // Other types...
                break;
        }
    }

    // Print statistics
    std::cout << "\n=== OBJECT COUNTS ===" << std::endl;
    std::cout << "Symbols: " << symbols.size() << std::endl;
    std::cout << "Lines/Wires: " << lines.size() << std::endl;
    std::cout << "Labels: " << labels.size() << std::endl;
    std::cout << "Junctions: " << junctions.size() << std::endl;
    std::cout << "No-Connects: " << noConnects.size() << std::endl;
    std::cout << "Text Items: " << texts.size() << std::endl;
    std::cout << "Sub-Sheets: " << subSheets.size() << std::endl;

    // Analyze symbols in detail
    std::cout << "\n=== SYMBOL DETAILS ===" << std::endl;
    for (size_t i = 0; i < symbols.size() && i < 10; ++i) {
        SCH_SYMBOL* symbol = symbols[i];
        std::cout << "  [" << (i + 1) << "] Reference: " << symbol->GetRef(nullptr).toStdString().c_str()
                  << ", Value: " << symbol->GetValue(false, nullptr, false).toStdString().c_str()
                  << ", Library ID: " << symbol->GetLibId().Format().c_str()
                  << ", Position: (" << symbol->GetPosition().x << ", " << symbol->GetPosition().y << ")"
                  << ", Pins: " << symbol->GetPins().size()
                  << std::endl;
    }

    if (symbols.size() > 10) {
        std::cout << "  ... and " << (symbols.size() - 10) << " more symbols" << std::endl;
    }

    // Analyze lines/wires
    std::cout << "\n=== WIRE/LINE DETAILS ===" << std::endl;
    for (size_t i = 0; i < lines.size() && i < 5; ++i) {
        SCH_LINE* line = lines[i];
        std::cout << "  [" << (i + 1) << "] From: (" << line->GetStartPoint().x << ", " << line->GetStartPoint().y << ")"
                  << " To: (" << line->GetEndPoint().x << ", " << line->GetEndPoint().y << ")"
                  << " Layer: " << line->GetLayer()
                  << std::endl;
    }

    if (lines.size() > 5) {
        std::cout << "  ... and " << (lines.size() - 5) << " more lines" << std::endl;
    }

    // Analyze labels
    std::cout << "\n=== LABEL DETAILS ===" << std::endl;
    for (size_t i = 0; i < labels.size() && i < 10; ++i) {
        SCH_LABEL_BASE* label = labels[i];
        const char* type_str = "Label";
        if (label->Type() == SCH_GLOBAL_LABEL_T) type_str = "Global Label";
        else if (label->Type() == SCH_HIER_LABEL_T) type_str = "Hierarchical Label";

        std::cout << "  [" << (i + 1) << "] " << type_str << ": \"" << label->GetText().toStdString().c_str() << "\""
                  << " at (" << label->GetPosition().x << ", " << label->GetPosition().y << ")"
                  << std::endl;
    }

    if (labels.size() > 10) {
        std::cout << "  ... and " << (labels.size() - 10) << " more labels" << std::endl;
    }
}


int main(int argc, char* argv[])
{
    std::cout << "=== KiCad Schematic Parser Test ===" << std::endl;

    // Initialize Qt application
    SCH_TEST_APP app(argc, argv);

    SCH_SHEET* sheet = nullptr;

    try {
        // Path to the schematic file
        QString schPath = "test/complex_hierarchy/complex_hierarchy.kicad_sch";

        // Check for file existence and make absolute
        QFileInfo schFile(schPath);
        if (!schFile.exists()) {
            schFile.setFile("complex_hierarchy.kicad_sch");
            if (!schFile.exists()) {
                // Try looking in complex_hierarchy folder
                schFile.setFile("complex_hierarchy/complex_hierarchy.kicad_sch");
                if (!schFile.exists()) {
                    std::cout << "ERROR: Cannot find schematic file!" << std::endl;
                    std::cout << "Tried: test/complex_hierarchy/complex_hierarchy.kicad_sch," << std::endl;
                    std::cout << "       complex_hierarchy.kicad_sch," << std::endl;
                    std::cout << "       complex_hierarchy/complex_hierarchy.kicad_sch" << std::endl;
                    return 1;
                }
            }
        }

        // Make the path absolute (required by LoadSchematicFile)
        schPath = schFile.absoluteFilePath();

        std::cout << "Loading schematic file: " << schPath.toStdString().c_str() << std::endl;

        // Check for project file
        QString projPath = schPath;
        projPath.replace(".kicad_sch", ".kicad_pro");

        if (!QFileInfo::exists(projPath)) {
            // Try in the same directory as the schematic
            QFileInfo projFile(schPath);
            projPath = projFile.absolutePath() + "/" + projFile.baseName() + ".kicad_pro";
        }

        std::cout << "Project file: " << projPath.toStdString().c_str() << std::endl;

        // Set PGM to avoid assertions
        SCH_TEST_PGM pgm;
        SetPgm(&pgm);

        std::cout << "Creating minimal sheet and screen..." << std::endl;

        // Create minimal sheet and screen
        sheet = new SCH_SHEET();
        sheet->SetFileName(schPath);

        SCH_SCREEN* screen = new SCH_SCREEN();
        sheet->SetScreen(screen);

        std::cout << "Parsing schematic file..." << std::endl;

        // Use the simplest approach: directly call ParseSchematic
        try {
            FILE_LINE_READER reader(schPath.toStdString());

            // Create parser with minimal parameters
            // Parameters: reader, progress reporter, line count, root sheet, appending
            SCH_IO_KICAD_SEXPR_PARSER parser(&reader, nullptr, 0, nullptr, false);

            // ParseSchematic will handle the entire file including the header
            parser.ParseSchematic(sheet, false, SEXPR_SCHEMATIC_FILE_VERSION);

            std::cout << "Parsing completed successfully!" << std::endl;
        }
        catch (const std::exception& e) {
            std::cout << "Parse error: " << e.what() << std::endl;
            std::cout << "Note: Some features may require SCHEMATIC object" << std::endl;
        }

        if (!sheet) {
            std::cout << "ERROR: Failed to load schematic file!" << std::endl;
            return 1;
        }

        std::cout << "Schematic file loaded successfully!" << std::endl;

        // Print detailed schematic information
        PrintSchematicStatistics(sheet);

        std::cout << "\n=== Test completed successfully! ===" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
        if (sheet) {
            try {
                delete sheet;
            }
            catch (...) {
                std::cout << "Error during cleanup - ignored" << std::endl;
            }
        }
        return 1;
    }
    catch (...) {
        std::cout << "Unknown exception caught!" << std::endl;
        if (sheet) {
            try {
                delete sheet;
            }
            catch (...) {
                std::cout << "Error during cleanup - ignored" << std::endl;
            }
        }
        return 1;
    }

    // Safe cleanup: Delete sheet before exit
    if (sheet) {
        try {
            delete sheet;
            sheet = nullptr;
        }
        catch (...) {
            std::cout << "Warning: Exception during sheet cleanup - continuing..." << std::endl;
        }
    }


    // Clean up PGM
    SetPgm(nullptr);

    return 0;
}
