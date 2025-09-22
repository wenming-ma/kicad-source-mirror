

#include <footprint_info_impl.h>

#include <dialogs/html_message_box.h>
#include <footprint.h>
#include <footprint_info.h>
#include <fp_lib_table.h>
#include <kiway.h>
#include <locale_io.h>
#include <lib_id.h>
#include <progress_reporter.h>
#include <string_utils.h>
#include <thread_pool.h>
#include <wildcards_and_files_ext.h>

#include <kiplatform/io.h>

#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryFile>


void FOOTPRINT_INFO_IMPL::load( const LOCALE_IO* aLocale )
{
    FP_LIB_TABLE* fptable = m_owner->GetTable();

    Q_ASSERT( fptable );

    const FOOTPRINT* footprint = fptable->GetEnumeratedFootprint( m_nickname, m_fpname, aLocale );

    if( footprint == nullptr ) // Should happen only with malformed/broken libraries
    {
        m_pad_count = 0;
        m_unique_pad_count = 0;
    }
    else
    {
        m_pad_count = footprint->GetPadCount( DO_NOT_INCLUDE_NPTH );
        m_unique_pad_count = footprint->GetUniquePadCount( DO_NOT_INCLUDE_NPTH );
        m_keywords = footprint->GetKeywords();
        m_doc = footprint->GetLibDescription();
    }

    m_loaded = true;
}


void FOOTPRINT_LIST_IMPL::Clear()
{
    m_list.clear();
    m_list_timestamp = 0;
}


bool FOOTPRINT_LIST_IMPL::CatchErrors( const std::function<void()>& aFunc )
{
    try
    {
        aFunc();
    }
    catch( const IO_ERROR& ioe )
    {
        m_errors.move_push( std::make_unique<IO_ERROR>( ioe ) );
        return false;
    }
    catch( const std::exception& se )
    {
        // This is a round about way to do this, but who knows what THROW_IO_ERROR()
        // may be tricked out to do someday, keep it in the game.
        try
        {
            THROW_IO_ERROR( se.what() );
        }
        catch( const IO_ERROR& ioe )
        {
            m_errors.move_push( std::make_unique<IO_ERROR>( ioe ) );
        }

        return false;
    }

    return true;
}


bool FOOTPRINT_LIST_IMPL::ReadFootprintFiles( FP_LIB_TABLE* aTable, const QString* aNickname,
                                              PROGRESS_REPORTER* aProgressReporter )
{
    long long int generatedTimestamp = 0;

    if( !CatchErrors( [&]()
                 {
                     generatedTimestamp = aTable->GenerateTimestamp( aNickname );
                 } ) )
    {
        return false;
    }

    if( generatedTimestamp == m_list_timestamp )
        return true;

    // Disable KIID generation: not needed for library parts; sometimes very slow
    KIID_NIL_SET_RESET reset_kiid;

    m_progress_reporter = aProgressReporter;

    m_cancelled = false;
    m_lib_table = aTable;

    // Clear data before reading files
    m_errors.clear();
    m_list.clear();
    m_queue.clear();

    if( aNickname )
    {
        m_queue.push( *aNickname );
    }
    else
    {
        for( const QString& nickname : aTable->GetLogicalLibs() )
            m_queue.push( nickname );
    }

    if( m_progress_reporter )
    {
        m_progress_reporter->SetMaxProgress( (int) m_queue.size() );
        m_progress_reporter->Report( _( "Loading footprints..." ) );
    }

    loadFootprints();

    if( m_progress_reporter )
        m_progress_reporter->AdvancePhase();

    if( m_cancelled )
        m_list_timestamp = 0;       // God knows what we got before we were canceled
    else
        m_list_timestamp = generatedTimestamp;

    return m_errors.empty();
}


void FOOTPRINT_LIST_IMPL::loadFootprints()
{
    LOCALE_IO toggle_locale;

    // Parse the footprints in parallel. WARNING! This requires changing the locale, which is
    // GLOBAL. It is only thread safe to construct the LOCALE_IO before the threads are created,
    // destroy it after they finish, and block the main (GUI) thread while they work. Any deviation
    // from this will cause nasal demons.
    //
    // TODO: blast LOCALE_IO into the sun

    SYNC_QUEUE<std::unique_ptr<FOOTPRINT_INFO>> queue_parsed;
    thread_pool&                                tp = GetKiCadThreadPool();
    size_t                                      num_elements = m_queue.size();
    std::vector<std::future<size_t>>            returns( num_elements );

    auto fp_thread =
            [ this, &queue_parsed, &toggle_locale ]() -> size_t
            {
                QString nickname;

                if( m_cancelled || !m_queue.pop( nickname ) )
                    return 0;

                QStringList fpnames;

                CatchErrors(
                        [&]()
                        {
                            m_lib_table->FootprintEnumerate( fpnames, nickname, false, &toggle_locale );
                        } );

                for( QString fpname : fpnames )
                {
                    CatchErrors(
                            [&]()
                            {
                                auto* fpinfo = new FOOTPRINT_INFO_IMPL( this, nickname, fpname, &toggle_locale );
                                queue_parsed.move_push( std::unique_ptr<FOOTPRINT_INFO>( fpinfo ) );
                            } );

                    if( m_cancelled )
                        return 0;
                }

                if( m_progress_reporter )
                    m_progress_reporter->AdvanceProgress();

                return 1;
            };

    for( size_t ii = 0; ii < num_elements; ++ii )
        returns[ii] = tp.submit( fp_thread );

    for( const std::future<size_t>& ret : returns )
    {
        std::future_status status = ret.wait_for( std::chrono::milliseconds( 250 ) );

        while( status != std::future_status::ready )
        {
            if( m_progress_reporter )
                m_progress_reporter->KeepRefreshing();

            status = ret.wait_for( std::chrono::milliseconds( 250 ) );
        }
    }

    std::unique_ptr<FOOTPRINT_INFO> fpi;

    while( queue_parsed.pop( fpi ) )
        m_list.push_back( std::move( fpi ) );

    std::sort( m_list.begin(), m_list.end(),
               []( std::unique_ptr<FOOTPRINT_INFO> const& lhs,
                   std::unique_ptr<FOOTPRINT_INFO> const& rhs ) -> bool
               {
                   return *lhs < *rhs;
               } );
}


FOOTPRINT_LIST_IMPL::FOOTPRINT_LIST_IMPL() :
    m_list_timestamp( 0 ),
    m_progress_reporter( nullptr ),
    m_cancelled( false )
{
}


void FOOTPRINT_LIST_IMPL::WriteCacheToFile( const QString& aFilePath )
{
    QTemporaryFile      tmpFile;
    tmpFile.setFileTemplate( aFilePath + ".XXXXXX" );
    tmpFile.open();
    QTextStream         txtStream( &tmpFile );

    if( !tmpFile.isOpen() )
    {
        return;
    }

    txtStream << QString::asprintf( "%lld", m_list_timestamp ) << Qt::endl;

    for( std::unique_ptr<FOOTPRINT_INFO>& fpinfo : m_list )
    {
        txtStream << fpinfo->GetLibNickname() << Qt::endl;
        txtStream << fpinfo->GetName() << Qt::endl;
        txtStream << EscapeString( fpinfo->GetDesc(), CTX_LINE ) << Qt::endl;
        txtStream << EscapeString( fpinfo->GetKeywords(), CTX_LINE ) << Qt::endl;
        txtStream << QString::asprintf( "%d", fpinfo->GetOrderNum() ) << Qt::endl;
        txtStream << QString::asprintf( "%u", fpinfo->GetPadCount() ) << Qt::endl;
        txtStream << QString::asprintf( "%u", fpinfo->GetUniquePadCount() ) << Qt::endl;
    }

    txtStream.flush();
    tmpFile.close();

    // Preserve the permissions of the current file
    KIPLATFORM::IO::DuplicatePermissions( aFilePath, tmpFile.fileName() );

    if( !QFile::rename( tmpFile.fileName(), aFilePath ) )
    {
        // cleanup in case rename failed
        // its also not the end of the world since this is just a cache file
        QFile::remove( tmpFile.fileName() );
    }
}


void FOOTPRINT_LIST_IMPL::ReadCacheFromFile( const QString& aFilePath )
{
    QFile cacheFile( aFilePath );

    m_list_timestamp = 0;
    m_list.clear();

    try
    {
        if( cacheFile.exists() && cacheFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            QTextStream stream( &cacheFile );
            QStringList lines;

            while( !stream.atEnd() )
                lines.append( stream.readLine() );

            if( lines.size() > 0 )
                m_list_timestamp = lines[0].toLongLong();

            int lineIndex = 1;
            while( lineIndex + 6 < lines.size() )
            {
                QString              libNickname    = lines[lineIndex++];
                QString              name           = lines[lineIndex++];
                QString              desc           = UnescapeString( lines[lineIndex++] );
                QString              keywords       = UnescapeString( lines[lineIndex++] );
                int                  orderNum       = lines[lineIndex++].toInt();
                unsigned int         padCount       = (unsigned) lines[lineIndex++].toInt();
                unsigned int         uniquePadCount = (unsigned) lines[lineIndex++].toInt();

                FOOTPRINT_INFO_IMPL* fpinfo = new FOOTPRINT_INFO_IMPL( libNickname, name, desc,
                                                                       keywords, orderNum,
                                                                       padCount,  uniquePadCount );

                m_list.emplace_back( std::unique_ptr<FOOTPRINT_INFO>( fpinfo ) );
            }
        }
    }
    catch( ... )
    {
        // whatever went wrong, invalidate the cache
        m_list_timestamp = 0;
    }

    // Sanity check: an empty list is very unlikely to be correct.
    if( m_list.size() == 0 )
        m_list_timestamp = 0;

    if( cacheFile.isOpen() )
        cacheFile.close();
}
