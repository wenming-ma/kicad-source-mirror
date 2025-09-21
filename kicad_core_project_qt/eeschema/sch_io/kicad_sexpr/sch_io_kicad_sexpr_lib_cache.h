
#ifndef SCH_IO_KICAD_SEXPR_LIB_CACHE_H_
#define SCH_IO_KICAD_SEXPR_LIB_CACHE_H_

#include "sch_io/sch_io_lib_cache.h"

class FILE_LINE_READER;
class SCH_PIN;
class SCH_TEXT;
class SCH_TEXTBOX;
class LINE_READER;
class SCH_IO_KICAD_SEXPR;

/**
 * A cache assistant for the KiCad s-expression symbol libraries.
 */
class SCH_IO_KICAD_SEXPR_LIB_CACHE : public SCH_IO_LIB_CACHE
{
public:
    SCH_IO_KICAD_SEXPR_LIB_CACHE( const QString& aLibraryPath );
    virtual ~SCH_IO_KICAD_SEXPR_LIB_CACHE();

    // Most all functions in this class throw IO_ERROR exceptions.  There are no
    // error codes nor user interface calls from here, nor in any SCH_IO objects.
    // Catch these exceptions higher up please.

    /// Save the entire library to file m_libFileName;
    void Save( const std::optional<bool>& aOpt = std::nullopt ) override;

    void Load() override;

    void DeleteSymbol( const QString& aName ) override;

    static void SaveSymbol( LIB_SYMBOL* aSymbol, OUTPUTFORMATTER& aFormatter,
                            const QString& aLibName = QString(), bool aIncludeData = true );

    void SetFileFormatVersionAtLoad( int aVersion ) { m_fileFormatVersionAtLoad = aVersion; }
    int GetFileFormatVersionAtLoad()  const { return m_fileFormatVersionAtLoad; }

private:
    friend SCH_IO_KICAD_SEXPR;

    int m_fileFormatVersionAtLoad;

    static void saveSymbolDrawItem( SCH_ITEM* aItem, OUTPUTFORMATTER& aFormatter );
    static void saveField( SCH_FIELD* aField, OUTPUTFORMATTER& aFormatter );
    static void savePin( SCH_PIN* aPin, OUTPUTFORMATTER& aFormatter );
    static void saveText( SCH_TEXT* aText, OUTPUTFORMATTER& aFormatter );
    static void saveTextBox( SCH_TEXTBOX* aTextBox, OUTPUTFORMATTER& aFormatter );

    static void saveDcmInfoAsFields( LIB_SYMBOL* aSymbol, OUTPUTFORMATTER& aFormatter,
                                     int& aNextFreeFieldId );
};

#endif    // SCH_IO_KICAD_SEXPR_LIB_CACHE_H_
