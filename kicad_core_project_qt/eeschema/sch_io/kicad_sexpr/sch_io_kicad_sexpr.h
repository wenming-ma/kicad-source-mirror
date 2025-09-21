
#ifndef SCH_IO_KICAD_SEXPR_H_
#define SCH_IO_KICAD_SEXPR_H_

#include <memory>
#include <sch_io/sch_io.h>
#include <sch_io/sch_io_mgr.h>
#include <sch_file_versions.h>
#include <sch_sheet_path.h>
#include <stack>
#include <wildcards_and_files_ext.h>
#include <QString>
#include <richio.h>


class KIWAY;
class LINE_READER;
class SCH_SCREEN;
class SCH_SHEET;
struct SCH_SHEET_INSTANCE;
class SCH_BITMAP;
class SCH_JUNCTION;
class SCH_NO_CONNECT;
class SCH_LINE;
class SCH_SHAPE;
class SCH_RULE_AREA;
class SCH_BUS_ENTRY_BASE;
class SCH_TEXT;
class SCH_TEXTBOX;
class SCH_TABLE;
class SCH_SYMBOL;
class SCH_FIELD;
struct SCH_SYMBOL_INSTANCE;
class SCH_SELECTION;
class SCH_IO_KICAD_SEXPR_LIB_CACHE;
class LIB_SYMBOL;
class SYMBOL_LIB;
class BUS_ALIAS;

/**
 * A #SCH_IO derivation for loading schematic files using the new s-expression
 * file format.
 *
 * As with all SCH_IOs there is no UI dependencies i.e. windowing calls allowed.
 */
class SCH_IO_KICAD_SEXPR : public SCH_IO
{
public:

    SCH_IO_KICAD_SEXPR();
    virtual ~SCH_IO_KICAD_SEXPR();

    const IO_BASE::IO_FILE_DESC GetSchematicFileDesc() const override
    {
        return IO_BASE::IO_FILE_DESC( _HKI( "KiCad s-expression schematic files" ),
                                      { FILEEXT::KiCadSchematicFileExtension } );
    }

    const IO_BASE::IO_FILE_DESC GetLibraryDesc() const override
    {
        return IO_BASE::IO_FILE_DESC( _HKI( "KiCad symbol library files" ),
                                      { FILEEXT::KiCadSymbolLibFileExtension } );
    }

    /**
     * The property used internally by the plugin to enable cache buffering which prevents
     * the library file from being written every time the cache is changed.  This is useful
     * when writing the schematic cache library file or saving a library to a new file name.
     */
    static const char* PropBuffering;

    int GetModifyHash() const override;

    SCH_SHEET* LoadSchematicFile( const QString& aFileName, SCHEMATIC* aSchematic,
                                  SCH_SHEET*             aAppendToMe = nullptr,
                                  const std::map<std::string, UTF8>* aProperties = nullptr ) override;

    void LoadContent( LINE_READER& aReader, SCH_SHEET* aSheet,
                      int aVersion = SEXPR_SCHEMATIC_FILE_VERSION );

    void SaveSchematicFile( const QString& aFileName, SCH_SHEET* aSheet, SCHEMATIC* aSchematic,
                            const std::map<std::string, UTF8>* aProperties = nullptr ) override;

    void Format( SCH_SHEET* aSheet );

    void Format( SCH_SELECTION* aSelection, SCH_SHEET_PATH* aSelectionPath,
                 SCHEMATIC& aSchematic, OUTPUTFORMATTER* aFormatter, bool aForClipboard );

    void EnumerateSymbolLib( QStringList&    aSymbolNameList,
                             const QString&   aLibraryPath,
                             const std::map<std::string, UTF8>* aProperties = nullptr ) override;
    void EnumerateSymbolLib( std::vector<LIB_SYMBOL*>& aSymbolList,
                             const QString&           aLibraryPath,
                             const std::map<std::string, UTF8>*         aProperties = nullptr ) override;
    LIB_SYMBOL* LoadSymbol( const QString& aLibraryPath, const QString& aAliasName,
                            const std::map<std::string, UTF8>* aProperties = nullptr ) override;
    void SaveSymbol( const QString& aLibraryPath, const LIB_SYMBOL* aSymbol,
                     const std::map<std::string, UTF8>* aProperties = nullptr ) override;
    void DeleteSymbol( const QString& aLibraryPath, const QString& aSymbolName,
                       const std::map<std::string, UTF8>* aProperties = nullptr ) override;
    void CreateLibrary( const QString& aLibraryPath,
                        const std::map<std::string, UTF8>* aProperties = nullptr ) override;
    bool DeleteLibrary( const QString& aLibraryPath,
                        const std::map<std::string, UTF8>* aProperties = nullptr ) override;
    void SaveLibrary( const QString& aLibraryPath,
                      const std::map<std::string, UTF8>* aProperties = nullptr ) override;

    bool CanReadLibrary( const QString& aLibraryPath ) const override;

    bool IsLibraryWritable( const QString& aLibraryPath ) override;

    void GetAvailableSymbolFields( std::vector<QString>& aNames ) override;
    void GetDefaultSymbolFields( std::vector<QString>& aNames ) override;

    const QString& GetError() const override { return m_error; }

    static std::vector<LIB_SYMBOL*> ParseLibSymbols( std::string& aSymbolText,
                                                     std::string  aSource,
                                                     int aFileVersion = SEXPR_SCHEMATIC_FILE_VERSION );
    static void FormatLibSymbol( LIB_SYMBOL* aPart, OUTPUTFORMATTER& aFormatter );

private:
    void loadHierarchy( const SCH_SHEET_PATH& aParentSheetPath, SCH_SHEET* aSheet );
    void loadFile( const QString& aFileName, SCH_SHEET* aSheet );

    void saveSymbol( SCH_SYMBOL* aSymbol, const SCHEMATIC& aSchematic,
                     const SCH_SHEET_LIST& aSheetList, bool aForClipboard,
                     const SCH_SHEET_PATH* aRelativePath = nullptr );
    void saveField( SCH_FIELD* aField );
    void saveBitmap( const SCH_BITMAP& aBitmap );
    void saveSheet( SCH_SHEET* aSheet, const SCH_SHEET_LIST& aSheetList );
    void saveJunction( SCH_JUNCTION* aJunction );
    void saveNoConnect( SCH_NO_CONNECT* aNoConnect );
    void saveBusEntry( SCH_BUS_ENTRY_BASE* aBusEntry );
    void saveLine( SCH_LINE* aLine );
    void saveShape( SCH_SHAPE* aShape );
    void saveRuleArea( SCH_RULE_AREA* aRuleArea );
    void saveText( SCH_TEXT* aText );
    void saveTextBox( SCH_TEXTBOX* aText );
    void saveTable( SCH_TABLE* aTable );
    void saveBusAlias( std::shared_ptr<BUS_ALIAS> aAlias );
    void saveInstances( const std::vector<SCH_SHEET_INSTANCE>& aSheets );

    void cacheLib( const QString& aLibraryFileName, const std::map<std::string, UTF8>* aProperties );
    bool isBuffering( const std::map<std::string, UTF8>* aProperties );

protected:
    int                     m_version;          ///< Version of file being loaded.
    int                     m_nextFreeFieldId;
    bool                    m_appending;        ///< Schematic load append status.
    QString                m_error;            ///< For throwing exceptions or errors on partial
                                                ///<  loads.

    QString                m_path;             ///< Root project path for loading child sheets.
    std::stack<QString>    m_currentPath;      ///< Stack to maintain nested sheet paths
    SCH_SHEET*              m_rootSheet;        ///< The root sheet of the schematic being loaded.
    SCH_SHEET_PATH          m_currentSheetPath;
    SCHEMATIC*              m_schematic;
    OUTPUTFORMATTER*        m_out;              ///< The formatter for saving SCH_SCREEN objects.
    SCH_IO_KICAD_SEXPR_LIB_CACHE* m_cache;

    /// initialize PLUGIN like a constructor would.
    void init( SCHEMATIC* aSchematic, const std::map<std::string, UTF8>* aProperties = nullptr );
};

#endif  // SCH_IO_KICAD_SEXPR_H_
