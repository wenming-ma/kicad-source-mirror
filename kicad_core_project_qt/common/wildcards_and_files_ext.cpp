#include <regex>
#include <wildcards_and_files_ext.h>
#include <QFileDialog>
#include <QRegularExpression>
#include <QCoreApplication>

bool compareFileExtensions( const std::string& aExtension,
                            const std::vector<std::string>& aReference, bool aCaseSensitive )
{
    // Form the regular expression string by placing all possible extensions into it as alternatives
    std::string regexString = "(";
    bool        first = true;

    for( const std::string& ext : aReference )
    {
        // The | separate goes between the extensions
        if( !first )
            regexString += "|";
        else
            first = false;

        regexString += ext;
    }

    regexString += ")";

    // Create the regex and see if it matches
    std::regex extRegex( regexString, aCaseSensitive ? std::regex::ECMAScript : std::regex::icase );
    return std::regex_match( aExtension, extRegex );
}


QString formatWildcardExt( const QString& aWildcard )
{
    QString wc;
#if defined( __unix__ )

    for( const auto& ch : aWildcard )
    {
        if( ch.isLetter() )
            wc += QString( "[%1%2]" ).arg( ch.toLower() ).arg( ch.toUpper() );
        else
            wc += ch;
    }

    return wc;
#else
    wc = aWildcard;

    return wc;
#endif
}


QString AddFileExtListToFilter( const std::vector<std::string>& aExts )
{
    if( aExts.size() == 0 )
    {
        // The "all files" wildcard is different on different systems
        QString filter;
        filter += " (";
        filter += "*";
        filter += ")|";
        filter += "*";
        return filter;
    }

    QString files_filter = " (";

    // Add extensions to the info message:
    for( const std::string& ext : aExts )
    {
        if( files_filter.length() > 2 )
            files_filter += "; ";

        files_filter += "*.";
        files_filter += QString::fromStdString( ext );
    }

    files_filter += ")|*.";

    // Add extensions to the filter list, using a formatted string (GTK specific):
    bool first = true;

    for( const std::string& ext : aExts )
    {
        if( !first )
            files_filter += ";*.";

        first = false;

        files_filter += formatWildcardExt( QString::fromStdString( ext ) );
    }

    return files_filter;
}

const std::string FILEEXT::BackupFileSuffix( "-bak" );
const std::string FILEEXT::LockFilePrefix( "~" );
const std::string FILEEXT::LockFileExtension( "lck" );
const std::string FILEEXT::AutoSaveFilePrefix( "_autosave-" );

const std::string FILEEXT::KiCadSymbolLibFileExtension( "kicad_sym" );
const std::string FILEEXT::SchematicSymbolFileExtension( "sym" );
const std::string FILEEXT::LegacySymbolLibFileExtension( "lib" );
const std::string FILEEXT::LegacySymbolDocumentFileExtension( "dcm" );

const std::string FILEEXT::VrmlFileExtension( "wrl" );

const std::string FILEEXT::ProjectFileExtension( "kicad_pro" );
const std::string FILEEXT::LegacyProjectFileExtension( "pro" );
const std::string FILEEXT::ProjectLocalSettingsFileExtension( "kicad_prl" );
const std::string FILEEXT::LegacySchematicFileExtension( "sch" );
const std::string FILEEXT::CadstarSchematicFileExtension( "csa" );
const std::string FILEEXT::CadstarPartsLibraryFileExtension( "lib" );
const std::string FILEEXT::KiCadSchematicFileExtension( "kicad_sch" );
const std::string FILEEXT::SpiceFileExtension( "cir" );
const std::string FILEEXT::SpiceModelFileExtension( "model" );
const std::string FILEEXT::SpiceSubcircuitFileExtension( "sub" );
const std::string FILEEXT::IbisFileExtension( "ibs" );
const std::string FILEEXT::CadstarNetlistFileExtension( "frp" );
const std::string FILEEXT::OrCadPcb2NetlistFileExtension( "net" );
const std::string FILEEXT::NetlistFileExtension( "net" );
const std::string FILEEXT::AllegroNetlistFileExtension( "txt" );
const std::string FILEEXT::PADSNetlistFileExtension( "asc" );
const std::string FILEEXT::FootprintAssignmentFileExtension( "cmp" );
const std::string FILEEXT::GerberFileExtension( "gbr" );
const std::string FILEEXT::GerberJobFileExtension( "gbrjob" );
const std::string FILEEXT::HtmlFileExtension( "html" );
const std::string FILEEXT::EquFileExtension( "equ" );
const std::string FILEEXT::HotkeyFileExtension( "hotkeys" );
const std::string FILEEXT::DatabaseLibraryFileExtension( "kicad_dbl" );
const std::string FILEEXT::HTTPLibraryFileExtension( "kicad_httplib" );
const std::string FILEEXT::KiCadJobSetFileExtension( "kicad_jobset" );

const std::string FILEEXT::ArchiveFileExtension( "zip" );

const std::string FILEEXT::LegacyPcbFileExtension( "brd" );
const std::string FILEEXT::EaglePcbFileExtension( "brd" );
const std::string FILEEXT::CadstarPcbFileExtension( "cpa" );
const std::string FILEEXT::KiCadPcbFileExtension( "kicad_pcb" );
const std::string FILEEXT::DrawingSheetFileExtension( "kicad_wks" );
const std::string FILEEXT::DesignRulesFileExtension( "kicad_dru" );

const std::string FILEEXT::PdfFileExtension( "pdf" );
const std::string FILEEXT::MacrosFileExtension( "mcr" );
const std::string FILEEXT::DrillFileExtension( "drl" );
const std::string FILEEXT::SVGFileExtension( "svg" );
const std::string FILEEXT::ReportFileExtension( "rpt" );
const std::string FILEEXT::FootprintPlaceFileExtension( "pos" );

const std::string FILEEXT::KiCadFootprintLibPathExtension( "pretty" ); // this is a directory
const std::string FILEEXT::LegacyFootprintLibPathExtension( "mod" );   // this is a file
const std::string FILEEXT::AltiumFootprintLibPathExtension( "PcbLib" ); // this is a file
const std::string FILEEXT::CadstarFootprintLibPathExtension( "cpa" );   // this is a file
const std::string FILEEXT::EagleFootprintLibPathExtension( "lbr" );     // this is a file
const std::string FILEEXT::GedaPcbFootprintLibFileExtension( "fp" );    // this is a file

const std::string FILEEXT::KiCadFootprintFileExtension( "kicad_mod" );
const std::string FILEEXT::SpecctraDsnFileExtension( "dsn" );
const std::string FILEEXT::SpecctraSessionFileExtension( "ses" );
const std::string FILEEXT::IpcD356FileExtension( "d356" );
const std::string FILEEXT::Ipc2581FileExtension( "xml" );
const std::string FILEEXT::WorkbookFileExtension( "wbk" );

const std::string FILEEXT::KiCadDesignBlockLibPathExtension( "kicad_blocks" ); // this is a directory
const std::string FILEEXT::KiCadDesignBlockPathExtension( "kicad_block" );     // this is a directory

const std::string FILEEXT::PngFileExtension( "png" );
const std::string FILEEXT::JpegFileExtension( "jpg" );
const std::string FILEEXT::TextFileExtension( "txt" );
const std::string FILEEXT::MarkdownFileExtension( "md" );
const std::string FILEEXT::CsvFileExtension( "csv" );
const std::string FILEEXT::XmlFileExtension( "xml" );
const std::string FILEEXT::JsonFileExtension( "json" );
const std::string FILEEXT::PythonFileExtension( "py" );

const std::string FILEEXT::StepFileExtension( "step" );
const std::string FILEEXT::StepFileAbrvExtension( "stp" );
const std::string FILEEXT::GltfBinaryFileExtension( "glb" );
const std::string FILEEXT::BrepFileExtension( "brep" );
const std::string FILEEXT::XaoFileExtension( "xao" );
const std::string FILEEXT::PlyFileExtension( "ply" );
const std::string FILEEXT::StlFileExtension( "stl" );

const std::string FILEEXT::GencadFileExtension( "cad" );

const QString
        FILEEXT::GerberFileExtensionsRegex( "(gbr|gko|pho|(g[tb][alops])|(gm?\\d\\d*)|(gp[tb]))" );

const std::string FILEEXT::FootprintLibraryTableFileName( "fp-lib-table" );
const std::string FILEEXT::SymbolLibraryTableFileName( "sym-lib-table" );
const std::string FILEEXT::DesignBlockLibraryTableFileName( "design-block-lib-table" );

const std::string FILEEXT::KiCadUriPrefix( "kicad-embed" );


bool FILEEXT::IsGerberFileExtension( const QString& ext )
{
    static QRegularExpression gerberRE( GerberFileExtensionsRegex, QRegularExpression::CaseInsensitiveOption );

    return gerberRE.match( ext ).hasMatch();
}


QString FILEEXT::AllFilesWildcard()
{
    return QCoreApplication::translate("FILEEXT", "All files") + AddFileExtListToFilter( {} );
}


QString FILEEXT::KiCadSymbolLibFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "KiCad symbol library files")
            + AddFileExtListToFilter( { KiCadSymbolLibFileExtension } );
}


QString FILEEXT::ProjectFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "KiCad project files") + AddFileExtListToFilter( { ProjectFileExtension } );
}


QString FILEEXT::LegacyProjectFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "KiCad legacy project files")
            + AddFileExtListToFilter( { LegacyProjectFileExtension } );
}


QString FILEEXT::AllProjectFilesWildcard()
{
    return QCoreApplication::translate("FILEEXT", "All KiCad project files")
            + AddFileExtListToFilter( { ProjectFileExtension, LegacyProjectFileExtension } );
}


QString FILEEXT::AllSchematicFilesWildcard()
{
    return QCoreApplication::translate("FILEEXT", "All KiCad schematic files")
            + AddFileExtListToFilter( { KiCadSchematicFileExtension, LegacySchematicFileExtension } );
}


QString FILEEXT::LegacySchematicFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "KiCad legacy schematic files")
            + AddFileExtListToFilter( { LegacySchematicFileExtension } );
}


QString FILEEXT::KiCadSchematicFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "KiCad s-expression schematic files")
            + AddFileExtListToFilter( { KiCadSchematicFileExtension } );
}


QString FILEEXT::AltiumProjectFilesWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Altium Project files") + AddFileExtListToFilter( { "PrjPcb" } );
}


QString FILEEXT::CadstarArchiveFilesWildcard()
{
    return QCoreApplication::translate("FILEEXT", "CADSTAR Archive files") + AddFileExtListToFilter( { "csa", "cpa" } );
}


QString FILEEXT::EagleFilesWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Eagle XML files") + AddFileExtListToFilter( { "sch", "brd" } );
}


QString FILEEXT::OrCadPcb2NetlistFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "OrcadPCB2 netlist files")
            + AddFileExtListToFilter( { OrCadPcb2NetlistFileExtension } );
}


QString FILEEXT::NetlistFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "KiCad netlist files") + AddFileExtListToFilter( { "net" } );
}


QString FILEEXT::AllegroNetlistFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Allegro netlist files")
            + AddFileExtListToFilter( { AllegroNetlistFileExtension } );
}


QString FILEEXT::PADSNetlistFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "PADS netlist files") + AddFileExtListToFilter( { PADSNetlistFileExtension } );
}


QString FILEEXT::EasyEdaArchiveWildcard()
{
    return QCoreApplication::translate("FILEEXT", "EasyEDA (JLCEDA) Std backup archive") + AddFileExtListToFilter( { "zip" } );
}


QString FILEEXT::EasyEdaProFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "EasyEDA (JLCEDA) Pro files") + AddFileExtListToFilter( { "epro", "zip" } );
}


QString FILEEXT::PcbFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "KiCad printed circuit board files")
           + AddFileExtListToFilter( { KiCadPcbFileExtension } );
}


QString FILEEXT::KiCadFootprintLibFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "KiCad footprint files") + AddFileExtListToFilter( { KiCadFootprintFileExtension } );
}


QString FILEEXT::KiCadFootprintLibPathWildcard()
{
    return QCoreApplication::translate("FILEEXT", "KiCad footprint library paths")
            + AddFileExtListToFilter( { KiCadFootprintLibPathExtension } );
}


QString FILEEXT::KiCadDesignBlockPathWildcard()
{
    return QCoreApplication::translate("FILEEXT", "KiCad design block path")
           + AddFileExtListToFilter( { KiCadDesignBlockPathExtension } );
}


QString FILEEXT::KiCadDesignBlockLibPathWildcard()
{
    return QCoreApplication::translate("FILEEXT", "KiCad design block library paths")
           + AddFileExtListToFilter( { KiCadDesignBlockLibPathExtension } );
}


QString FILEEXT::DrawingSheetFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Drawing sheet files")
            + AddFileExtListToFilter( { DrawingSheetFileExtension } );
}


QString FILEEXT::FootprintAssignmentFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "KiCad symbol footprint link files")
            + AddFileExtListToFilter( { FootprintAssignmentFileExtension } );
}


QString FILEEXT::DrillFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Drill files")
            + AddFileExtListToFilter( { DrillFileExtension, "nc", "xnc", "txt" } );
}


QString FILEEXT::SVGFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "SVG files") + AddFileExtListToFilter( { SVGFileExtension } );
}


QString FILEEXT::HtmlFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "HTML files") + AddFileExtListToFilter( { "htm", "html" } );
}


QString FILEEXT::CsvFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "CSV Files") + AddFileExtListToFilter( { CsvFileExtension } );
}


QString FILEEXT::PdfFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Portable document format files") + AddFileExtListToFilter( { "pdf" } );
}


QString FILEEXT::PSFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "PostScript files") + AddFileExtListToFilter( { "ps" } );
}


QString FILEEXT::JsonFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Json files") + AddFileExtListToFilter( { JsonFileExtension } );
}


QString FILEEXT::ReportFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Report files") + AddFileExtListToFilter( { ReportFileExtension } );
}


QString FILEEXT::FootprintPlaceFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Component placement files") + AddFileExtListToFilter( { "pos" } );
}


QString FILEEXT::Shapes3DFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "VRML and X3D files") + AddFileExtListToFilter( { "wrl", "x3d" } );
}


QString FILEEXT::IDF3DFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "IDFv3 footprint files") + AddFileExtListToFilter( { "idf" } );
}


QString FILEEXT::TextFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Text files") + AddFileExtListToFilter( { "txt" } );
}


QString FILEEXT::ModLegacyExportFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Legacy footprint export files") + AddFileExtListToFilter( { "emp" } );
}


QString FILEEXT::ErcFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Electrical rule check file") + AddFileExtListToFilter( { "erc" } );
}


QString FILEEXT::SpiceLibraryFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "SPICE library file") + AddFileExtListToFilter( { "lib", "mod" } );
}


QString FILEEXT::SpiceNetlistFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "SPICE netlist file") + AddFileExtListToFilter( { "cir" } );
}


QString FILEEXT::CadstarNetlistFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "CadStar netlist file") + AddFileExtListToFilter( { "frp" } );
}


QString FILEEXT::EquFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Symbol footprint association files") + AddFileExtListToFilter( { "equ" } );
}


QString FILEEXT::ZipFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Zip file") + AddFileExtListToFilter( { "zip" } );
}


QString FILEEXT::GencadFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "GenCAD 1.4 board files") + AddFileExtListToFilter( { GencadFileExtension } );
}


QString FILEEXT::DxfFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "DXF Files") + AddFileExtListToFilter( { "dxf" } );
}


QString FILEEXT::GerberJobFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Gerber job file") + AddFileExtListToFilter( { GerberJobFileExtension } );
}


QString FILEEXT::SpecctraDsnFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Specctra DSN file")
            + AddFileExtListToFilter( { SpecctraDsnFileExtension } );
}


QString FILEEXT::SpecctraSessionFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Specctra Session file")
            + AddFileExtListToFilter( { SpecctraSessionFileExtension } );
}


QString FILEEXT::IpcD356FileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "IPC-D-356 Test Files")
            + AddFileExtListToFilter( { IpcD356FileExtension } );
}


QString FILEEXT::WorkbookFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Workbook file")
            + AddFileExtListToFilter( { WorkbookFileExtension } );
}


QString FILEEXT::PngFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "PNG file") + AddFileExtListToFilter( { "png" } );
}


QString FILEEXT::JpegFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Jpeg file") + AddFileExtListToFilter( { "jpg", "jpeg" } );
}


QString FILEEXT::HotkeyFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "Hotkey file") + AddFileExtListToFilter( { HotkeyFileExtension } );
}


QString FILEEXT::JobsetFileWildcard()
{
    return QCoreApplication::translate("FILEEXT", "KiCad jobset files") + AddFileExtListToFilter( { KiCadJobSetFileExtension } );
}
