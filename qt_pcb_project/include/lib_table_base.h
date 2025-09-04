
#ifndef _LIB_TABLE_BASE_H_
#define _LIB_TABLE_BASE_H_

#include <QHash>
#include <QVector>
#include <boost/ptr_container/ptr_vector.hpp>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <project.h>
#include <richio.h>
#include <kicommon.h>

class OUTPUTFORMATTER;
class LIB_TABLE_LEXER;
class LIB_ID;
class LIB_TABLE_ROW;
class LIB_TABLE_GRID;
class LIB_TABLE;
class IO_ERROR;
class QWidget;


typedef boost::ptr_vector< LIB_TABLE_ROW > LIB_TABLE_ROWS;
typedef LIB_TABLE_ROWS::iterator           LIB_TABLE_ROWS_ITER;
typedef LIB_TABLE_ROWS::const_iterator     LIB_TABLE_ROWS_CITER;


KICOMMON_API LIB_TABLE_ROW* new_clone( const LIB_TABLE_ROW& aRow );


class KICOMMON_API LIB_TABLE_IO
{
public:
    virtual ~LIB_TABLE_IO() = default;

    virtual std::unique_ptr<LINE_READER> GetReader( const QString& aURI ) const = 0;

    virtual bool CanSaveToUri( const QString& aURI ) const = 0;

    virtual bool UrisAreEquivalent( const QString& aURI1, const QString& aURI2 ) const = 0;

    virtual std::unique_ptr<OUTPUTFORMATTER> GetWriter( const QString& aURI ) const = 0;
};


class KICOMMON_API FILE_LIB_TABLE_IO : public LIB_TABLE_IO
{
public:
    FILE_LIB_TABLE_IO() = default;

    std::unique_ptr<LINE_READER> GetReader( const QString& aURI ) const override;

    bool CanSaveToUri( const QString& aURI ) const override;

    bool UrisAreEquivalent( const QString& aURI1, const QString& aURI2 ) const override;

    std::unique_ptr<OUTPUTFORMATTER> GetWriter( const QString& aURI ) const override;
};


class KICOMMON_API LIB_TABLE_ROW
{
public:
    LIB_TABLE_ROW() :
        enabled( true ),
        visible( true ),
        m_loaded( false ),
        m_parent( nullptr )
    {
    }

    virtual ~LIB_TABLE_ROW()
    {
    }

    LIB_TABLE_ROW( const QString& aNick, const QString& aURI, const QString& aOptions,
                   const QString& aDescr = QString(), LIB_TABLE* aParent = nullptr ) :
        nickName( aNick ),
        description( aDescr ),
        enabled( true ),
        visible( true ),
        m_loaded( false ),
        m_parent( aParent )
    {
        properties.reset();
        SetOptions( aOptions );
        SetFullURI( aURI );
    }

    bool operator==( const LIB_TABLE_ROW& r ) const;

    bool operator!=( const LIB_TABLE_ROW& r ) const { return !( *this == r ); }

    const QString& GetNickName() const { return nickName; }

    void SetNickName( const QString& aNickName ) { nickName = aNickName; }

    bool GetIsLoaded() const { return m_loaded; }

    void SetLoaded( bool aLoaded ) { m_loaded = aLoaded; };

    bool GetIsEnabled() const { return enabled; }

    void SetEnabled( bool aEnabled = true ) { enabled = aEnabled; }

    bool GetIsVisible() const { return visible; }

    void SetVisible( bool aVisible = true ) { visible = aVisible; }

    virtual bool LibraryExists() const = 0;

    virtual bool Refresh() { return false; }

    virtual const QString GetType() const = 0;

    virtual void SetType( const QString& aType ) = 0;

    virtual bool SupportsSettingsDialog() const { return false; }

    virtual void ShowSettingsDialog( QWidget* aParent ) const {}

    const QString GetFullURI( bool aSubstituted = false ) const;

    void SetFullURI( const QString& aFullURI );

    const QString& GetOptions() const          { return options; }

    void SetOptions( const QString& aOptions );

    const QString& GetDescr() const            { return description; }

    void SetDescr( const QString& aDescr )     { description = aDescr; }

    LIB_TABLE* GetParent() const { return m_parent; }

    void SetParent( LIB_TABLE* aParent ) { m_parent = aParent; }

    std::mutex& GetMutex() { return m_loadMutex; }

    const std::map<std::string, UTF8>* GetProperties() const     { return properties.get(); }

    void Format( OUTPUTFORMATTER* out, int nestLevel ) const;

    LIB_TABLE_ROW* clone() const
    {
        return do_clone();
    }

protected:
    LIB_TABLE_ROW( const LIB_TABLE_ROW& aRow ) :
        nickName( aRow.nickName ),
        uri_user( aRow.uri_user ),
        options( aRow.options ),
        description( aRow.description ),
        enabled( aRow.enabled ),
        visible( aRow.visible ),
        m_loaded( aRow.m_loaded ),
        m_parent( aRow.m_parent )
    {
        if( aRow.properties )
            properties = std::make_unique<std::map<std::string, UTF8>>( *aRow.properties.get() );
        else
            properties.reset();
    }

    void operator=( const LIB_TABLE_ROW& aRow );

private:
    virtual LIB_TABLE_ROW* do_clone() const = 0;

    void setProperties( std::map<std::string, UTF8>* aProperties );

private:
    QString          nickName;
    QString          uri_user;           // what user entered from UI or loaded from disk
    QString          options;
    QString          description;

    bool              enabled  = true;    ///< Whether the LIB_TABLE_ROW is enabled
    bool              visible  = true;    ///< Whether the LIB_TABLE_ROW is visible in choosers
    bool              m_loaded = false;   ///< Whether the LIB_TABLE_ROW is loaded
    LIB_TABLE*        m_parent;           ///< Pointer to the table this row lives in (maybe null)

    std::unique_ptr<std::map<std::string, UTF8>> properties;

    std::mutex        m_loadMutex;
};


class KICOMMON_API LIB_TABLE : public PROJECT::_ELEM
{
public:
    virtual void Parse( LIB_TABLE_LEXER* aLexer ) = 0;

    virtual void Format( OUTPUTFORMATTER* aOutput, int aIndentLevel ) const = 0;

    LIB_TABLE( LIB_TABLE*                    aFallBackTable = nullptr,
               std::unique_ptr<LIB_TABLE_IO> aTableIo = nullptr );

    virtual ~LIB_TABLE();

    bool operator==( const LIB_TABLE& r ) const
    {
        if( m_rows.size() == r.m_rows.size() )
        {
            unsigned i;

            for( i = 0; i < m_rows.size() && m_rows[i] == r.m_rows[i];  ++i )
                ;

            if( i == m_rows.size() )
                return true;
        }

        return false;
    }

    bool operator!=( const LIB_TABLE& r ) const  { return !( *this == r ); }

    unsigned GetCount() const
    {
        return m_rows.size();
    }

    LIB_TABLE_ROW& At( unsigned aIndex )
    {
        return m_rows[aIndex];
    }

    const LIB_TABLE_ROW& At( unsigned aIndex ) const
    {
        return m_rows[aIndex];
    }

    bool IsEmpty( bool aIncludeFallback = true );

    const QString GetDescription( const QString& aNickname );

    bool HasLibrary( const QString& aNickname, bool aCheckEnabled = false ) const;

    bool HasLibraryWithPath( const QString& aPath ) const;

    QVector<QString> GetLogicalLibs();

    QString GetFullURI( const QString& aLibNickname, bool aExpandEnvVars = true ) const;

    bool InsertRow( LIB_TABLE_ROW* aRow, bool doReplace = false );

    bool RemoveRow( const LIB_TABLE_ROW* aRow );

    bool ReplaceRow( size_t aIndex, LIB_TABLE_ROW* aRow );

    bool ChangeRowOrder( size_t aIndex, int aOffset );

    void TransferRows( LIB_TABLE_ROWS& aRowsList );

    const LIB_TABLE_ROW* FindRowByURI( const QString& aURI );

    void Load( const QString& aFileName );

    void Save( const QString& aFileName ) const;

    static std::map<std::string, UTF8>* ParseOptions( const std::string& aOptionsList );

    static UTF8 FormatOptions( const std::map<std::string, UTF8>* aProperties );

    int GetVersion() const
    {
        return m_version;
    }

protected:
    void clear();

    LIB_TABLE_ROW* findRow( const QString& aNickname, bool aCheckIfEnabled = false ) const;

    bool doInsertRow( LIB_TABLE_ROW* aRow, bool doReplace = false );

    bool migrate();

    void reindex();

protected:
    // Injected I/O interface
    std::unique_ptr<LIB_TABLE_IO> m_io;

    LIB_TABLE* m_fallBack;

    // Versioning to handle importing old tables
    mutable int m_version;

    // Owning set of rows.
    LIB_TABLE_ROWS m_rows;

    // this is a non-owning index into the LIB_TABLE_ROWS table
    QHash<QString, LIB_TABLE_ROWS_ITER> m_rowsMap;

    // Mutex to protect access to the rows vector
    mutable std::shared_mutex m_mutex;
};

#endif  // _LIB_TABLE_BASE_H_
