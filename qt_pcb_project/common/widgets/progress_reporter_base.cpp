#include <QThread>
#include <thread>
#include <widgets/progress_reporter_base.h>

PROGRESS_REPORTER_BASE::PROGRESS_REPORTER_BASE( int aNumPhases ) :
    PROGRESS_REPORTER(),
    m_phase( 0 ),
    m_numPhases( aNumPhases ),
    m_progress( 0 ),
    m_maxProgress( 1000 ),
    m_cancelled( false ),
    m_messageChanged( false )
{
}


void PROGRESS_REPORTER_BASE::BeginPhase( int aPhase )
{
    m_phase.store( aPhase );
    m_progress.store( 0 );
}


void PROGRESS_REPORTER_BASE::AdvancePhase()
{
    m_phase.fetch_add( 1 );
    m_progress.store( 0 );
}


void PROGRESS_REPORTER_BASE::AdvancePhase( const QString& aMessage )
{
    AdvancePhase();
    Report( aMessage );
}


void PROGRESS_REPORTER_BASE::Report( const QString& aMessage )
{
    std::lock_guard<std::mutex> guard( m_mutex );

    m_messageChanged = m_rptMessage != aMessage;
    m_rptMessage = aMessage;
}


void PROGRESS_REPORTER_BASE::SetMaxProgress( int aMaxProgress )
{
    m_maxProgress.store( aMaxProgress );
}


void PROGRESS_REPORTER_BASE::SetCurrentProgress( double aProgress )
{
    m_maxProgress.store( 1000 );
    m_progress.store( (int) ( aProgress * 1000.0 ) );
}


void PROGRESS_REPORTER_BASE::AdvanceProgress()
{
    m_progress.fetch_add( 1 );
}


void PROGRESS_REPORTER_BASE::SetNumPhases( int aNumPhases )
{
    m_numPhases = aNumPhases;
}


void PROGRESS_REPORTER_BASE::AddPhases( int aNumPhases )
{
    m_numPhases += aNumPhases;
}


int PROGRESS_REPORTER_BASE::CurrentProgress() const
{
    double current = ( 1.0 / (double) m_numPhases ) *
                     ( (double) m_phase + ( (double) m_progress.load() / (double) m_maxProgress ) );

    return (int)( current * 1000 );
}


bool PROGRESS_REPORTER_BASE::KeepRefreshing( bool aWait )
{
    if( aWait )
    {
        while( m_progress.load() < m_maxProgress && m_maxProgress > 0 )
        {
            if( !updateUI() )
            {
                m_cancelled.store( true );
                return false;
            }

            QThread::msleep( 33 /* 30 FPS refresh rate */ );
        }

        return true;
    }
    else
    {
        if( !updateUI() )
        {
            m_cancelled.store( true );
            return false;
        }

        return true;
    }
}


