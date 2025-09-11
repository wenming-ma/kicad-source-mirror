
#ifndef DESIGN_BLOCK_LIB_TABLE_H_
#define DESIGN_BLOCK_LIB_TABLE_H_

#include <kicommon.h>
#include <lib_table_base.h>
#include <design_block_io.h>
#include <design_block_info_impl.h>

class DESIGN_BLOCK;
class DESIGN_BLOCK_LIB_TABLE_GRID;

class KICOMMON_API DESIGN_BLOCK_LIB_TABLE_ROW : public LIB_TABLE_ROW
{
public:
    DESIGN_BLOCK_LIB_TABLE_ROW( const QString& aNick, const QString& aURI, const QString& aType,
                                const QString& aOptions, const QString& aDescr = QString() ) :
            LIB_TABLE_ROW( aNick, aURI, aOptions, aDescr )
    {
        SetType( aType );
    }

    DESIGN_BLOCK_LIB_TABLE_ROW() :
            LIB_TABLE_ROW(),
            type( DESIGN_BLOCK_IO_MGR::KICAD_SEXP )
    {
    }

    bool operator==( const DESIGN_BLOCK_LIB_TABLE_ROW& aRow ) const;

    bool operator!=( const DESIGN_BLOCK_LIB_TABLE_ROW& aRow ) const
    {
        return !( *this == aRow );
    }

    const QString GetType() const override { return DESIGN_BLOCK_IO_MGR::ShowType( type ); }

    void SetType( const QString& aType ) override;

    bool LibraryExists() const override
    {
        if( plugin )
            return plugin->CanReadLibrary( GetFullURI( true ) );

        return false;
    }

    DESIGN_BLOCK_IO_MGR::DESIGN_BLOCK_FILE_T GetFileType() { return type; }

protected:
    DESIGN_BLOCK_LIB_TABLE_ROW( const DESIGN_BLOCK_LIB_TABLE_ROW& aRow ) :
            LIB_TABLE_ROW( aRow ),
            type( aRow.type )
    {
    }

private:
    virtual LIB_TABLE_ROW* do_clone() const override
    {
        return new DESIGN_BLOCK_LIB_TABLE_ROW( *this );
    }

    void setPlugin( DESIGN_BLOCK_IO* aPlugin ) { plugin.reset( aPlugin ); }

    friend class DESIGN_BLOCK_LIB_TABLE;

private:
    IO_RELEASER<DESIGN_BLOCK_IO>             plugin;
    DESIGN_BLOCK_IO_MGR::DESIGN_BLOCK_FILE_T type;
};


class KICOMMON_API DESIGN_BLOCK_LIB_TABLE : public LIB_TABLE
{
public:
    PROJECT::ELEM ProjectElementType() override { return PROJECT::ELEM::DESIGN_BLOCK_LIB_TABLE; }

    virtual void Parse( LIB_TABLE_LEXER* aLexer ) override;

    virtual void Format( OUTPUTFORMATTER* aOutput, int aIndentLevel ) const override;

    DESIGN_BLOCK_LIB_TABLE( DESIGN_BLOCK_LIB_TABLE* aFallBackTable = nullptr );

    bool operator==( const DESIGN_BLOCK_LIB_TABLE& aFpTable ) const;

    bool operator!=( const DESIGN_BLOCK_LIB_TABLE& r ) const { return !( *this == r ); }

    const DESIGN_BLOCK_LIB_TABLE_ROW* FindRow( const QString& aNickName,
                                               bool            aCheckIfEnabled = false );

    void DesignBlockEnumerate( QStringList& aDesignBlockNames, const QString& aNickname,
                               bool aBestEfforts, const LOCALE_IO* aLocale = nullptr );

    long long GenerateTimestamp( const QString* aNickname );

    DESIGN_BLOCK* DesignBlockLoad( const QString& aNickname, const QString& aDesignBlockName,
                                   bool aKeepUUID = false );

    bool DesignBlockExists( const QString& aNickname, const QString& aDesignBlockName );

    const DESIGN_BLOCK* GetEnumeratedDesignBlock( const QString& aNickname, const QString& aDesignBlockName,
                                                  const LOCALE_IO* aLocale = nullptr );
    enum SAVE_T
    {
        SAVE_OK,
        SAVE_SKIPPED,
    };

    SAVE_T DesignBlockSave( const QString& aNickname, const DESIGN_BLOCK* aDesignBlock,
                            bool aOverwrite = true );

    void DesignBlockDelete( const QString& aNickname, const QString& aDesignBlockName );

    bool IsDesignBlockLibWritable( const QString& aNickname );

    void DesignBlockLibDelete( const QString& aNickname );

    void DesignBlockLibCreate( const QString& aNickname );

    DESIGN_BLOCK* DesignBlockLoadWithOptionalNickname( const LIB_ID& aDesignBlockId,
                                                       bool          aKeepUUID = false );

    static bool LoadGlobalTable( DESIGN_BLOCK_LIB_TABLE& aTable );

    static DESIGN_BLOCK_LIB_TABLE& GetGlobalLibTable();

    static DESIGN_BLOCK_LIST_IMPL& GetGlobalList();

    static QString GetGlobalTableFileName();

    static const QString GlobalPathEnvVariableName();

private:
    friend class DESIGN_BLOCK_LIB_TABLE_GRID;
};

#endif // DESIGN_BLOCK_LIB_TABLE_H_
