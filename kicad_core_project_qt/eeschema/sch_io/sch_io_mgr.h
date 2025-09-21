
#ifndef _SCH_IO_MGR_H_
#define _SCH_IO_MGR_H_

#include <import_export.h>
#include <map>
#include <enum_vector.h>
#include <reporter.h>
#include <i18n_utility.h>
#include <io/io_base.h>
#include <io/io_mgr.h>
#include <QString>


class SCH_SHEET;
class SCH_SCREEN;
class SCH_IO;
class SCHEMATIC;
class SYMBOL_LIB_TABLE;
class KIWAY;
class LIB_SYMBOL;
class SYMBOL_LIB;
class PROGRESS_REPORTER;


/**
 * A factory which returns an instance of a #SCH_IO.
 */
class SCH_IO_MGR : public IO_MGR
{
public:

    /**
     * A set of file types that the #SCH_IO_MGR knows about, and for which there
     * has been a plugin written, in alphabetical order.
     */
    // clang-format off
    DEFINE_ENUM_VECTOR( SCH_FILE_T,
    {
        SCH_KICAD,            ///< The s-expression version of the schematic.
        SCH_LEGACY,           ///< Legacy Eeschema file formats prior to s-expression.
        SCH_ALTIUM,           ///< Altium file format
        SCH_CADSTAR_ARCHIVE,  ///< CADSTAR Schematic Archive
        SCH_DATABASE,         ///< KiCad database library
        SCH_EAGLE,            ///< Autodesk Eagle file format
        SCH_EASYEDA,          ///< EasyEDA Std schematic file
        SCH_EASYEDAPRO,       ///< EasyEDA Pro archive
        SCH_LTSPICE,          ///< LtSpice Schematic format
        SCH_HTTP,             ///< KiCad HTTP library

        // Add your schematic type here.
        SCH_FILE_UNKNOWN
    } )
    // clang-format on

    /**
     * Return a #SCH_IO which the caller can use to import, export, save, or load
     * design documents.
     *
     * @param aFileType is from #SCH_FILE_T and tells which plugin to find.
     *
     * @return the plugin corresponding to aFileType or NULL if not found.
     *  Caller owns the returned object.
     */
    APIEXPORT
    static SCH_IO* FindPlugin( SCH_FILE_T aFileType );

    /**
     * Return a brief name for a plugin, given aFileType enum.
     */
    static const QString ShowType( SCH_FILE_T aFileType );

    /**
     * Return the #SCH_FILE_T from the corresponding plugin type name: "kicad", "legacy", etc.
     */
    static SCH_FILE_T EnumFromStr( const QString& aFileType );

    /**
     * Return a plugin type given a symbol library using the file extension of \a aLibPath.
     */
    static SCH_FILE_T GuessPluginTypeFromLibPath( const QString& aLibPath, int aCtl = 0 );

    /**
     * Return a plugin type given a schematic using the file extension of \a aSchematicPath.
     */
    static SCH_FILE_T GuessPluginTypeFromSchPath( const QString& aSchematicPath, int aCtl = 0 );

    /**
     * Convert a schematic symbol library to the latest KiCad format
     */
    static bool ConvertLibrary( std::map<std::string, UTF8>* aOldFileProps, const QString& aOldFilePath,
                                const QString& aNewFilepath );
};

#endif // _SCH_IO_MGR_H_
