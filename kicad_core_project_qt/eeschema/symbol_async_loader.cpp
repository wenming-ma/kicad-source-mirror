
#include <thread>

#include <QtCore/QDebug>
#include <symbol_async_loader.h>
#include <symbol_lib_table.h>
#include <progress_reporter.h>


SYMBOL_ASYNC_LOADER::SYMBOL_ASYNC_LOADER( const std::vector<QString>& aNicknames,
                                          SYMBOL_LIB_TABLE* aTable, bool aOnlyPowerSymbols,
                                          std::unordered_map<QString, std::vector<LIB_SYMBOL*>>* aOutput,
                                          PROGRESS_REPORTER* aReporter ) :
        m_nicknames( aNicknames ),
        m_table( aTable ),
        m_onlyPowerSymbols( aOnlyPowerSymbols ),
        m_output( aOutput ),
        m_reporter( aReporter ),
        m_nextLibrary( 0 )
{
    Q_ASSERT( m_table );
    m_threadCount = std::max<size_t>( 1, std::thread::hardware_concurrency() );

    m_returns.resize( m_threadCount );
}



SYMBOL_ASYNC_LOADER::~SYMBOL_ASYNC_LOADER()
{
    Join();
}


void SYMBOL_ASYNC_LOADER::Start()
{
    for( size_t ii = 0; ii < m_threadCount; ++ii )
        m_returns[ii] = std::async( std::launch::async, &SYMBOL_ASYNC_LOADER::worker, this );
}


bool SYMBOL_ASYNC_LOADER::Join()
{
    for( size_t ii = 0; ii < m_threadCount; ++ii )
    {
        if( !m_returns[ii].valid() )
            continue;

        m_returns[ii].wait();

        const std::vector<LOADED_PAIR>& ret = m_returns[ii].get();

        if( m_output && !ret.empty() )
        {
            for( const LOADED_PAIR& pair : ret )
            {
                // Don't show libraries that had no power symbols
                if( m_onlyPowerSymbols && pair.second.empty() )
                    continue;

                // *Do* show empty libraries in the normal case
                m_output->insert( pair );
            }
        }
    }

    return true;
}


bool SYMBOL_ASYNC_LOADER::Done()
{
    return m_nextLibrary.load() >= m_nicknames.size();
}


std::vector<SYMBOL_ASYNC_LOADER::LOADED_PAIR> SYMBOL_ASYNC_LOADER::worker()
{
    std::vector<LOADED_PAIR> ret;

    bool onlyPower = m_onlyPowerSymbols;

    for( size_t libraryIndex = m_nextLibrary++; libraryIndex < m_nicknames.size();
         libraryIndex = m_nextLibrary++ )
    {
        const QString& nickname = m_nicknames[libraryIndex];

        if( m_reporter )
            m_reporter->AdvancePhase( QString::asprintf( _( "Loading library %s..." ), qPrintable(nickname) ) );

        if( m_reporter && m_reporter->IsCancelled() )
            break;

        LOADED_PAIR pair( nickname, {} );

        try
        {
            m_table->LoadSymbolLib( pair.second, nickname, onlyPower );
            ret.emplace_back( std::move( pair ) );
        }
        catch( const IO_ERROR& ioe )
        {
            QString msg = QString::asprintf( _( "Error loading symbol library %s.\n\n%s\n" ),
                                             qPrintable(nickname), qPrintable(ioe.What()) );

            std::lock_guard<std::mutex> lock( m_errorMutex );
            m_errors += msg;
        }
        catch( std::exception& e )
        {
            QString msg = QString::asprintf( _( "Error loading symbol library %s.\n\n%s\n" ),
                                             qPrintable(nickname), e.what() );

            std::lock_guard<std::mutex> lock( m_errorMutex );
            m_errors += msg;
        }
    }

    return ret;
}
