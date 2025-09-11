
#ifndef INCLUDE_WILDCARDS_AND_FILES_EXT_H_
#define INCLUDE_WILDCARDS_AND_FILES_EXT_H_

#include <kicommon.h>
#include <string>
#include <vector>
#include <QString>

// Compare extension against reference extensions to see if it matches any
// Uses C++ regular expression functionality for comparison
// Reference extensions can be regular expressions themselves
// For example ^g.* can be used to see if first character is g
// Reference extensions are concatenated as alternatives (e.g. (dxf|svg|^g.*))
// Returns if the extension matches any reference extensions
KICOMMON_API bool compareFileExtensions( const std::string&              aExtension,
                                         const std::vector<std::string>& aReference, bool aCaseSensitive = false );

// Build wildcard extension file dialog filter to add to base message dialog
// For opening .txt files: base message "Text files", ext list " (*.txt)|*.txt"
// Returned string is " (*.txt)|*.txt", final dialog message is "Text files (*.txt)|*.txt"
// Produces case-insensitive filter (.txt, .TXT and .tXT all match)
// Empty aExts means "allow all files"

KICOMMON_API QString AddFileExtListToFilter( const std::vector<std::string>& aExts );

// Format wildcard extension to support case sensitive file dialogs
// GTK+ file dialog wildcards are case sensitive, so all lower case means
// only lower case extensions show up in dialog
// GTK+ supports regular expressions, so extension is converted to regex
// (sch -> [sS][cC][hH]) when Qt is built against GTK+
// When calling QFileDialog with default file, include file extension
// with file name to avoid appending regex as extension

KICOMMON_API QString formatWildcardExt( const QString& aWildcard );


class KICOMMON_API FILEEXT
{
public:
    FILEEXT() = delete;

    // File Extension Definitions
    // Do not change these - create new definitions if different extension needed
    static const std::string BackupFileSuffix;
    static const std::string LockFilePrefix;
    static const std::string LockFileExtension;
    static const std::string AutoSaveFilePrefix;

    static const std::string SchematicSymbolFileExtension;
    static const std::string LegacySymbolLibFileExtension;
    static const std::string LegacySymbolDocumentFileExtension;
    static const std::string SchematicBackupFileExtension;

    static const std::string VrmlFileExtension;
    static const std::string ProjectFileExtension;
    static const std::string LegacyProjectFileExtension;
    static const std::string ProjectLocalSettingsFileExtension;
    static const std::string LegacySchematicFileExtension;
    static const std::string CadstarSchematicFileExtension;
    static const std::string CadstarPartsLibraryFileExtension;
    static const std::string KiCadSchematicFileExtension;
    static const std::string IbisFileExtension;
    static const std::string SpiceFileExtension;
    static const std::string SpiceModelFileExtension;
    static const std::string SpiceSubcircuitFileExtension;
    static const std::string CadstarNetlistFileExtension;
    static const std::string OrCadPcb2NetlistFileExtension;
    static const std::string NetlistFileExtension;
    static const std::string AllegroNetlistFileExtension;
    static const std::string PADSNetlistFileExtension;
    static const std::string GerberFileExtension;
    static const std::string GerberJobFileExtension;
    static const std::string HtmlFileExtension;
    static const std::string EquFileExtension;
    static const std::string HotkeyFileExtension;
    static const std::string DatabaseLibraryFileExtension;
    static const std::string HTTPLibraryFileExtension;

    static const std::string ArchiveFileExtension;

    static const std::string LegacyPcbFileExtension;
    static const std::string EaglePcbFileExtension;
    static const std::string CadstarPcbFileExtension;
    static const std::string KiCadPcbFileExtension;
    #define PcbFileExtension    KiCadPcbFileExtension       // symlink choice
    static const std::string KiCadSymbolLibFileExtension;
    static const std::string DrawingSheetFileExtension;
    static const std::string DesignRulesFileExtension;

    static const std::string LegacyFootprintLibPathExtension;
    static const std::string PdfFileExtension;
    static const std::string MacrosFileExtension;
    static const std::string FootprintAssignmentFileExtension;
    static const std::string DrillFileExtension;
    static const std::string SVGFileExtension;
    static const std::string ReportFileExtension;
    static const std::string FootprintPlaceFileExtension;
    static const std::string KiCadFootprintFileExtension;
    static const std::string KiCadFootprintLibPathExtension;
    static const std::string AltiumFootprintLibPathExtension;
    static const std::string CadstarFootprintLibPathExtension;
    static const std::string GedaPcbFootprintLibFileExtension;
    static const std::string EagleFootprintLibPathExtension;
    static const std::string SpecctraDsnFileExtension;
    static const std::string SpecctraSessionFileExtension;
    static const std::string IpcD356FileExtension;
    static const std::string Ipc2581FileExtension;
    static const std::string WorkbookFileExtension;

    static const std::string KiCadDesignBlockLibPathExtension;
    static const std::string KiCadDesignBlockPathExtension;

    static const std::string PngFileExtension;
    static const std::string JpegFileExtension;
    static const std::string TextFileExtension;
    static const std::string MarkdownFileExtension;
    static const std::string CsvFileExtension;
    static const std::string XmlFileExtension;
    static const std::string JsonFileExtension;
    static const std::string PythonFileExtension;

    static const std::string StepFileExtension;
    static const std::string StepFileAbrvExtension;
    static const std::string GltfBinaryFileExtension;
    static const std::string BrepFileExtension;
    static const std::string XaoFileExtension;
    static const std::string PlyFileExtension;
    static const std::string StlFileExtension;

    static const std::string GencadFileExtension;

    static const std::string KiCadJobSetFileExtension;

    static const QString GerberFileExtensionsRegex;

    static const std::string FootprintLibraryTableFileName;
    static const std::string SymbolLibraryTableFileName;
    static const std::string DesignBlockLibraryTableFileName;

    static const std::string KiCadUriPrefix;

    // File Wildcard Definitions
    // Do not change these - create new definitions if different wildcard needed
    // Handle GTK+ file dialog case sensitivity issue correctly

    static bool IsGerberFileExtension( const QString& ext );
    static QString AllFilesWildcard();

    static QString FootprintAssignmentFileWildcard();
    static QString DrawingSheetFileWildcard();
    static QString KiCadSymbolLibFileWildcard();
    static QString ProjectFileWildcard();
    static QString LegacyProjectFileWildcard();
    static QString AllProjectFilesWildcard();
    static QString AllSchematicFilesWildcard();
    static QString KiCadSchematicFileWildcard();
    static QString LegacySchematicFileWildcard();
    static QString BoardFileWildcard();
    static QString OrCadPcb2NetlistFileWildcard();
    static QString NetlistFileWildcard();
    static QString AllegroNetlistFileWildcard();
    static QString PADSNetlistFileWildcard();
    static QString HtmlFileWildcard();
    static QString CsvFileWildcard();
    static QString PcbFileWildcard();
    static QString CadstarArchiveFilesWildcard();
    static QString AltiumProjectFilesWildcard();
    static QString EagleFilesWildcard();
    static QString EasyEdaArchiveWildcard();
    static QString EasyEdaProFileWildcard();
    static QString PdfFileWildcard();
    static QString PSFileWildcard();
    static QString MacrosFileWildcard();
    static QString DrillFileWildcard();
    static QString SVGFileWildcard();
    static QString JsonFileWildcard();
    static QString ReportFileWildcard();
    static QString FootprintPlaceFileWildcard();
    static QString Shapes3DFileWildcard();
    static QString IDF3DFileWildcard();
    static QString DocModulesFileName();
    static QString KiCadFootprintLibFileWildcard();
    static QString KiCadFootprintLibPathWildcard();
    static QString KiCadDesignBlockLibPathWildcard();
    static QString KiCadDesignBlockPathWildcard();
    static QString TextFileWildcard();
    static QString ModLegacyExportFileWildcard();
    static QString ErcFileWildcard();
    static QString SpiceLibraryFileWildcard();
    static QString SpiceNetlistFileWildcard();
    static QString CadstarNetlistFileWildcard();
    static QString EquFileWildcard();
    static QString ZipFileWildcard();
    static QString GencadFileWildcard();
    static QString DxfFileWildcard();
    static QString GerberJobFileWildcard();
    static QString SpecctraDsnFileWildcard();
    static QString SpecctraSessionFileWildcard();
    static QString IpcD356FileWildcard();
    static QString WorkbookFileWildcard();
    static QString PngFileWildcard();
    static QString JpegFileWildcard();
    static QString HotkeyFileWildcard();
    static QString JobsetFileWildcard();
};


#endif  // INCLUDE_WILDCARDS_AND_FILES_EXT_H_
