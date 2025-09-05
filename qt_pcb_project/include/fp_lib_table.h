
#ifndef FP_LIB_TABLE_H_
#define FP_LIB_TABLE_H_

#include <QString>
#include <vector>
#include <string>
#include <lib_table_base.h>
#include <pcb_io/pcb_io_mgr.h>

class FOOTPRINT;
class FP_LIB_TABLE_GRID;
class PCB_IO;
class LOCALE_IO;


class FP_LIB_TABLE_ROW : public LIB_TABLE_ROW
{
public:
    FP_LIB_TABLE_ROW( const QString& aNick, const QString& aURI, const QString& aType,
                      const QString& aOptions, const QString& aDescr = QString() ) :
        LIB_TABLE_ROW( aNick, aURI, aOptions, aDescr )
    {
        SetType( aType );
    }

    FP_LIB_TABLE_ROW() :
        type( PCB_IO_MGR::KICAD_SEXP )
    {
    }

    bool operator==( const FP_LIB_TABLE_ROW& aRow ) const;

    bool operator!=( const FP_LIB_TABLE_ROW& aRow ) const   { return !( *this == aRow ); }

    const QString GetType() const override         { return QString::fromStdString( PCB_IO_MGR::ShowType( type ).ToStdString() ); }

    void SetType( const QString& aType ) override;

    bool LibraryExists() const override;

    PCB_IO_MGR::PCB_FILE_T GetFileType() { return type; }

protected:
    FP_LIB_TABLE_ROW( const FP_LIB_TABLE_ROW& aRow ) :
        LIB_TABLE_ROW( aRow ),
        type( aRow.type )
    {
    }

private:
    virtual LIB_TABLE_ROW* do_clone() const override
    {
        return new FP_LIB_TABLE_ROW( *this );
    }

    void setPlugin( PCB_IO* aPlugin )
    {
        plugin.reset( aPlugin );
    }

    friend class FP_LIB_TABLE;

private:
    IO_RELEASER<PCB_IO>    plugin;
    PCB_IO_MGR::PCB_FILE_T type;
};


class FP_LIB_TABLE : public LIB_TABLE
{
public:
    PROJECT::ELEM ProjectElementType() override { return PROJECT::ELEM::FPTBL; }

    virtual void Parse( LIB_TABLE_LEXER* aLexer ) override;

    virtual void Format( OUTPUTFORMATTER* aOutput, int aIndentLevel ) const override;

    FP_LIB_TABLE( FP_LIB_TABLE* aFallBackTable = nullptr );

    bool operator==( const FP_LIB_TABLE& aFpTable ) const;

    bool operator!=( const FP_LIB_TABLE& r ) const  { return !( *this == r ); }

    const FP_LIB_TABLE_ROW* FindRow( const QString& aNickName, bool aCheckIfEnabled = false );

    void FootprintEnumerate( std::vector<std::string>& aFootprintNames, const QString& aNickname,
                             bool aBestEfforts, const LOCALE_IO* aLocale = nullptr );

    long long GenerateTimestamp( const QString* aNickname );

    FOOTPRINT* FootprintLoad( const QString& aNickname, const QString& aFootprintName,
                              bool aKeepUUID = false );

    bool FootprintExists( const QString& aNickname, const QString& aFootprintName );

    const FOOTPRINT* GetEnumeratedFootprint( const QString& aNickname,
                                             const QString& aFootprintName,
                                             const LOCALE_IO* aLocale = nullptr );
    enum SAVE_T
    {
        SAVE_OK,
        SAVE_SKIPPED,
    };

    SAVE_T FootprintSave( const QString& aNickname, const FOOTPRINT* aFootprint,
                          bool aOverwrite = true );

    void FootprintDelete( const QString& aNickname, const QString& aFootprintName );

    bool IsFootprintLibWritable( const QString& aNickname );

    void FootprintLibDelete( const QString& aNickname );

    void FootprintLibCreate( const QString& aNickname );

    FOOTPRINT* FootprintLoadWithOptionalNickname( const LIB_ID& aFootprintId,
                                                  bool aKeepUUID = false );

    static bool LoadGlobalTable( FP_LIB_TABLE& aTable );

    static QString GetGlobalTableFileName();

    static const QString GlobalPathEnvVariableName();

private:
    friend class FP_LIB_TABLE_GRID;
};


extern FP_LIB_TABLE GFootprintTable;        // KIFACE scope.

#endif  // FP_LIB_TABLE_H_
