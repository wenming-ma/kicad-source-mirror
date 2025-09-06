#include <QDebug>
#include <QFileInfo>
#include <set>
#include <common.h>
#include <kiface_base.h>
#include <lib_table_base.h>
#include <lib_table_lexer.h>
#include <macros.h>
#include <string_utils.h>
#include <build_version.h>

#define OPT_SEP     '|'


using namespace LIB_TABLE_T;


std::unique_ptr<LINE_READER> FILE_LIB_TABLE_IO::GetReader( const QString& aURI ) const
{
    const QFileInfo fn( aURI );

    if( !fn.exists() || !fn.isReadable() )
        return nullptr;

    return std::make_unique<FILE_LINE_READER>( aURI );
}


bool FILE_LIB_TABLE_IO::CanSaveToUri( const QString& aURI ) const
{
    const QFileInfo fn( aURI );

    if( !fn.exists() )
        return false;

    return fn.isWritable();
}


bool FILE_LIB_TABLE_IO::UrisAreEquivalent( const QString& aURI1, const QString& aURI2 ) const
{
    // Avoid comparing filenames as URIs
    if( aURI1.indexOf( "://" ) != -1 )
    {
        // found as full path
        return aURI1 == aURI2;
    }
    else
    {
        const QFileInfo fn1( aURI1 );
        const QFileInfo fn2( aURI2 );

        // This will also test if the file is a symlink so if we are comparing
        // a symlink to the same real file, the comparison will be true.
        // found as full path and file name
        return fn1 == fn2;
    }
}


std::unique_ptr<OUTPUTFORMATTER> FILE_LIB_TABLE_IO::GetWriter( const QString& aURI ) const
{
    const QFileInfo fn( aURI );
    return std::make_unique<FILE_OUTPUTFORMATTER>( aURI );
}


LIB_TABLE_ROW* new_clone( const LIB_TABLE_ROW& aRow )
{
    return aRow.clone();
}


void LIB_TABLE_ROW::setProperties( std::map<std::string, UTF8>* aProperties )
{
    properties.reset( aProperties );
}


void LIB_TABLE_ROW::SetFullURI( const QString& aFullURI )
{
    uri_user = aFullURI;
}


const QString LIB_TABLE_ROW::GetFullURI( bool aSubstituted ) const
{
    if( aSubstituted )
    {
        return ExpandEnvVarSubstitutions( uri_user, nullptr );
    }

    return uri_user;
}


void LIB_TABLE_ROW::Format( OUTPUTFORMATTER* out, int nestLevel ) const
{
    // In KiCad, we save path and file names using the Unix notation (separator = '/')
    // So ensure separator is always '/' is saved URI string
    QString uri = GetFullURI();
    uri.replace( '\\', '/' );

    QString extraOptions;

    if( !GetIsEnabled() )
        extraOptions += "(disabled)";

    if( !GetIsVisible() )
        extraOptions += "(hidden)";

    out->Print( nestLevel, "(lib (name %s)(type %s)(uri %s)(options %s)(descr %s)%s)\n",
                out->Quotew( GetNickName() ).c_str(),
                out->Quotew( GetType() ).c_str(),
                out->Quotew( uri ).c_str(),
                out->Quotew( GetOptions() ).c_str(),
                out->Quotew( GetDescr() ).c_str(),
                extraOptions.toStdString().c_str() );
}


bool LIB_TABLE_ROW::operator==( const LIB_TABLE_ROW& r ) const
{
    return nickName == r.nickName
        && uri_user == r.uri_user
        && options == r.options
        && description == r.description
        && enabled == r.enabled
        && visible == r.visible;
}


void LIB_TABLE_ROW::SetOptions( const QString& aOptions )
{
    options = aOptions;

    // set PROPERTIES* from options
    setProperties( LIB_TABLE::ParseOptions( TO_UTF8( aOptions ) ) );
}


LIB_TABLE::LIB_TABLE( LIB_TABLE* aFallBackTable, std::unique_ptr<LIB_TABLE_IO> aTableIo ) :
        m_io( std::move( aTableIo ) ),
        m_fallBack( aFallBackTable ),
        m_version( 0 )
{
    // If not given, use the default file-based I/O.
    if( !m_io )
        m_io = std::make_unique<FILE_LIB_TABLE_IO>();

    // not copying fall back, simply search aFallBackTable separately
    // if "nickName not found".
}


LIB_TABLE::~LIB_TABLE()
{
    // fallBack is not owned here.
}


void LIB_TABLE::clear()
{
    m_rows.clear();
    m_rowsMap.clear();
}


bool LIB_TABLE::IsEmpty( bool aIncludeFallback )
{
    if( !aIncludeFallback || !m_fallBack )
        return m_rows.empty();

    return m_rows.empty() && m_fallBack->IsEmpty( true );
}


const QString LIB_TABLE::GetDescription( const QString& aNickname )
{
    // Use "no exception" form of find row and ignore disabled flag.
    const LIB_TABLE_ROW* row = findRow( aNickname );

    if( row )
        return row->GetDescr();
    else
        return QString();
}


bool LIB_TABLE::HasLibrary( const QString& aNickname, bool aCheckEnabled ) const
{
    return findRow( aNickname, aCheckEnabled ) != nullptr;
}


bool LIB_TABLE::HasLibraryWithPath( const QString& aPath ) const
{
    std::shared_lock<std::shared_mutex> lock( m_mutex );

    for( const LIB_TABLE_ROW& row : m_rows )
    {
        if( row.GetFullURI() == aPath )
            return true;
    }

    return false;
}


QString LIB_TABLE::GetFullURI( const QString& aNickname, bool aExpandEnvVars ) const
{
    const LIB_TABLE_ROW* row = findRow( aNickname, true );

    QString retv;

    if( row )
        retv = row->GetFullURI( aExpandEnvVars );

    return retv;
}


LIB_TABLE_ROW* LIB_TABLE::findRow( const QString& aNickName, bool aCheckIfEnabled ) const
{
    const LIB_TABLE* cur = this;

    do
    {
        std::shared_lock<std::shared_mutex> lock( cur->m_mutex );
        LIB_TABLE_ROW*                      row = nullptr;

        if( cur->m_rowsMap.count( aNickName ) )
            row = &*cur->m_rowsMap.at( aNickName );

        if( row )
        {
            if( !aCheckIfEnabled || row->GetIsEnabled() )
                return row;
            else
                return nullptr; // We found it, but it's disabled
        }

        // Repeat, this time looking for names that were "fixed" by legacy versions because
        // the old Eeschema file format didn't support spaces in tokens.
        for( const std::pair<const QString, LIB_TABLE_ROWS_ITER>& entry : cur->m_rowsMap )
        {
            QString legacyLibName = entry.first;
            legacyLibName.replace( " ", "_" );

            if( legacyLibName == aNickName )
            {
                row = &*entry.second;

                if( !aCheckIfEnabled || row->GetIsEnabled() )
                    return row;
            }
        }

        // not found, search fall back table(s), if any
    } while( ( cur = cur->m_fallBack ) != nullptr );

    return nullptr; // not found
}


const LIB_TABLE_ROW* LIB_TABLE::FindRowByURI( const QString& aURI )
{
    const LIB_TABLE* cur = this;

    do
    {
        std::shared_lock<std::shared_mutex> lock( cur->m_mutex );

        for( const LIB_TABLE_ROW& row : cur->m_rows)
        {
            const QString tmp = row.GetFullURI( true );

            if( m_io->UrisAreEquivalent( tmp, aURI ) )
                return &row;
        }

        // not found, search fall back table(s), if any
    } while( ( cur = cur->m_fallBack ) != nullptr );

    return nullptr; // not found
}


std::vector<QString> LIB_TABLE::GetLogicalLibs()
{
    // Only return unique logical library names.  Use std::set::insert() to quietly reject any
    // duplicates (usually due to encountering a duplicate nickname in a fallback table).

    std::set<QString>    unique;
    std::vector<QString> ret;
    const LIB_TABLE*      cur = this;

    do
    {
        std::shared_lock<std::shared_mutex> lock( cur->m_mutex );

        for( const LIB_TABLE_ROW& row : cur->m_rows )
        {
            if( row.GetIsEnabled() )
                unique.insert( row.GetNickName() );
        }

    } while( ( cur = cur->m_fallBack ) != nullptr );

    ret.reserve( unique.size() );

    // return a sorted, unique set of nicknames in a std::vector<QString> to caller
    for( const QString& nickname : unique )
        ret.push_back( nickname );

    // We want to allow case-sensitive duplicates but sort by case-insensitive ordering
    std::sort( ret.begin(), ret.end(),
            []( const QString& lhs, const QString& rhs )
            {
                return StrNumCmp( lhs, rhs, true ) < 0;
            } );

    return ret;
}


bool LIB_TABLE::InsertRow( LIB_TABLE_ROW* aRow, bool doReplace )
{
    std::lock_guard<std::shared_mutex> lock( m_mutex );

    doInsertRow( aRow, doReplace );

    return true;
}


bool LIB_TABLE::doInsertRow( LIB_TABLE_ROW* aRow, bool doReplace )
{
    auto it = m_rowsMap.find( aRow->GetNickName() );

    if( it != m_rowsMap.end() )
    {
        if( !doReplace )
            return false;

        m_rows.replace( it->second, aRow );
    }
    else
    {
        m_rows.push_back( aRow );
    }

    aRow->SetParent( this );
    reindex();
    return true;
}


bool LIB_TABLE::RemoveRow( const LIB_TABLE_ROW* aRow )
{
    std::lock_guard<std::shared_mutex> lock( m_mutex );

    bool found = false;
    auto it = m_rowsMap.find( aRow->GetNickName() );

    if( it != m_rowsMap.end() )
    {
        if( &*it->second == aRow )
        {
            found = true;
            m_rows.erase( it->second );
        }
    }

    if( !found )
    {
        // Bookkeeping got messed up...
        for( int i = (int)m_rows.size() - 1; i >= 0; --i )
        {
            if( &m_rows[i] == aRow )
            {
                m_rows.erase( m_rows.begin() + i );
                found = true;
                break;
            }
        }
    }

    if( found )
        reindex();

    return found;
}


bool LIB_TABLE::ReplaceRow( size_t aIndex, LIB_TABLE_ROW* aRow )
{
    std::lock_guard<std::shared_mutex> lock( m_mutex );

    if( aIndex >= m_rows.size() )
        return false;

    m_rowsMap.erase( m_rows[aIndex].GetNickName() );

    m_rows.replace( aIndex, aRow );
    reindex();
    return true;
}


bool LIB_TABLE::ChangeRowOrder( size_t aIndex, int aOffset )
{
    std::lock_guard<std::shared_mutex> lock( m_mutex );

    if( aIndex >= m_rows.size() )
        return false;

    int newPos = static_cast<int>( aIndex ) + aOffset;

    if( newPos < 0 || newPos > static_cast<int>( m_rows.size() ) - 1 )
        return false;

    auto element = m_rows.release( m_rows.begin() + aIndex );

    m_rows.insert( m_rows.begin() + newPos, element.release() );
    reindex();

    return true;
}


void LIB_TABLE::TransferRows( LIB_TABLE_ROWS& aRowsList )
{
    std::lock_guard<std::shared_mutex> lock( m_mutex );

    clear();
    m_rows.transfer( m_rows.end(), aRowsList.begin(), aRowsList.end(), aRowsList );

    reindex();
}


void LIB_TABLE::reindex()
{
    m_rowsMap.clear();

    for( LIB_TABLE_ROWS_ITER it = m_rows.begin(); it != m_rows.end(); ++it )
    {
        it->SetParent( this );
        m_rowsMap[it->GetNickName()] = it;
    }
}


bool LIB_TABLE::migrate()
{
    bool table_updated = false;

    for( LIB_TABLE_ROW& row : m_rows )
    {
        bool     row_updated = false;
        QString uri = row.GetFullURI( true );

        // If the uri still has a variable in it, that means that the user does not have
        // these vars defined.  We update the old vars to the current versions on load

        static QString fmtStr = "${KICAD%1_";
        int version = 0;
        std::tie(version, std::ignore, std::ignore) = GetMajorMinorPatchTuple();

        for( int ii = 5; ii < version - 1; ++ii )
        {
            QString searchStr = fmtStr.arg( ii );
            if( uri.contains( searchStr ) )
            {
                uri.replace( searchStr, fmtStr.arg( version ) );
                row_updated = true;
            }
        }

        if( row_updated )
        {
            row.SetFullURI( uri );
            table_updated = true;
        }
    }

    return table_updated;
}


void LIB_TABLE::Load( const QString& aFileName )
{
    std::lock_guard<std::shared_mutex> lock( m_mutex );
    clear();

    std::unique_ptr<LINE_READER> reader = m_io->GetReader( aFileName );

    // It's OK if footprint library tables are missing.
    if( reader )
    {
        LIB_TABLE_LEXER lexer( reader.get() );

        Parse( &lexer );

        if( m_version != 7 && migrate() && m_io->CanSaveToUri( aFileName ) )
            Save( aFileName );

        reindex();
    }
}


void LIB_TABLE::Save( const QString& aFileName ) const
{
    std::unique_ptr<OUTPUTFORMATTER> sf = m_io->GetWriter( aFileName );

    if( !sf )
        THROW_IO_ERROR( QString( "Failed to get writer for %1" ).arg( aFileName ) );

    // Force the lib table version to 7 before saving
    m_version = 7;
    Format( sf.get(), 0 );
}


std::map<std::string, UTF8>* LIB_TABLE::ParseOptions( const std::string& aOptionsList )
{
    if( aOptionsList.size() )
    {
        const char* cp  = &aOptionsList[0];
        const char* end = cp + aOptionsList.size();

        std::map<std::string, UTF8> props;
        std::string pair;

        // Parse all name=value pairs
        while( cp < end )
        {
            pair.clear();

            // Skip leading white space.
            while( cp < end && isspace( *cp )  )
                ++cp;

            // Find the end of pair/field
            while( cp < end )
            {
                if( *cp == '\\'  &&  cp + 1 < end  &&  cp[1] == OPT_SEP  )
                {
                    ++cp;           // skip the escape
                    pair += *cp++;  // add the separator
                }
                else if( *cp == OPT_SEP )
                {
                    ++cp;           // skip the separator
                    break;          // process the pair
                }
                else
                {
                    pair += *cp++;
                }
            }

            // stash the pair
            if( pair.size() )
            {
                // first equals sign separates 'name' and 'value'.
                size_t  eqNdx = pair.find( '=' );

                if( eqNdx != pair.npos )
                {
                    std::string name  = pair.substr( 0, eqNdx );
                    std::string value = pair.substr( eqNdx + 1 );
                    props[name] = value;
                }
                else
                {
                    props[pair] = "";       // property is present, but with no value.
                }
            }
        }

        if( props.size() )
            return new std::map<std::string, UTF8>( props );
    }

    return nullptr;
}


UTF8 LIB_TABLE::FormatOptions( const std::map<std::string, UTF8>* aProperties )
{
    UTF8 ret;

    if( aProperties )
    {
        for( std::map<std::string, UTF8>::const_iterator it = aProperties->begin();
             it != aProperties->end(); ++it )
        {
            const std::string& name = it->first;

            const UTF8& value = it->second;

            if( ret.size() )
                ret += OPT_SEP;

            ret += name;

            // the separation between name and value is '='
            if( value.size() )
            {
                ret += '=';

                for( std::string::const_iterator si = value.begin();  si != value.end();  ++si )
                {
                    // escape any separator in the value.
                    if( *si == OPT_SEP )
                        ret += '\\';

                    ret += *si;
                }
            }
        }
    }

    return ret;
}
