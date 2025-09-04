#include <unordered_map>

#include <QWidget>
#include <QFrame>
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QApplication>
#include <QFont>
#include <QPalette>
#include <QTimer>
#include <QString>
#include <QFocusEvent>
#include <QCloseEvent>
#include <QPoint>
#include <QSize>

#include <background_jobs_monitor.h>
#include <widgets/kistatusbar.h>


class BACKGROUND_JOB_PANEL : public QFrame
{
public:
    BACKGROUND_JOB_PANEL( QWidget* aParent, std::shared_ptr<BACKGROUND_JOB> aJob ) :
            QFrame( aParent ),
            m_job( aJob )
    {
        setFixedHeight( 75 );
        setFrameStyle( QFrame::Box );

        QVBoxLayout* mainLayout = new QVBoxLayout( this );
        mainLayout->setContentsMargins( 1, 1, 1, 1 );
        mainLayout->setSpacing( 1 );

        QPalette palette = this->palette();
        palette.setColor( QPalette::Window, QApplication::palette().color( QPalette::Light ) );
        setAutoFillBackground( true );
        setPalette( palette );

        m_stName = new QLabel( aJob->m_name, this );
        m_stName->setWordWrap( true );
        QFont font = m_stName->font();
        font.setBold( true );
        m_stName->setFont( font );
        mainLayout->addWidget( m_stName );

        m_stStatus = new QLabel( aJob->m_status, this );
        m_stStatus->setWordWrap( true );
        mainLayout->addWidget( m_stStatus );

        m_progress = new QProgressBar( this );
        m_progress->setMaximum( aJob->m_maxProgress );
        m_progress->setValue( 0 );
        mainLayout->addWidget( m_progress );

        setLayout( mainLayout );

        UpdateFromJob();
    }


    void UpdateFromJob()
    {
        m_stStatus->setText( m_job->m_status );
        m_progress->setValue( m_job->m_currentProgress );
        m_progress->setMaximum( m_job->m_maxProgress );
    }

private:
    QProgressBar* m_progress;
    QLabel* m_stName;
    QLabel* m_stStatus;
    std::shared_ptr<BACKGROUND_JOB> m_job;
};


class BACKGROUND_JOB_LIST : public QWidget
{
public:
    BACKGROUND_JOB_LIST( QWidget* parent, const QPoint& pos ) :
            QWidget( parent, Qt::Tool | Qt::FramelessWindowHint )
    {
        resize( 300, 150 );
        move( pos );
        setWindowTitle( "Background Jobs" );

        QVBoxLayout* mainLayout = new QVBoxLayout( this );
        mainLayout->setContentsMargins( 0, 0, 0, 0 );

        m_scrollArea = new QScrollArea( this );
        m_scrollArea->setWidgetResizable( true );
        m_scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
        m_scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        
        m_contentWidget = new QWidget();
        m_contentLayout = new QVBoxLayout( m_contentWidget );
        m_contentLayout->setContentsMargins( 0, 0, 0, 0 );
        m_contentLayout->setSpacing( 2 );
        
        m_scrollArea->setWidget( m_contentWidget );
        mainLayout->addWidget( m_scrollArea );

        setLayout( mainLayout );
        setFocus();
    }

protected:
    void focusOutEvent( QFocusEvent* aEvent ) override
    {
        close();
        QWidget::focusOutEvent( aEvent );
    }

public:
    void Add( std::shared_ptr<BACKGROUND_JOB> aJob )
    {
        BACKGROUND_JOB_PANEL* panel = new BACKGROUND_JOB_PANEL( m_contentWidget, aJob );
        m_contentLayout->addWidget( panel );
        m_contentWidget->adjustSize();
        
        m_jobPanels[aJob] = panel;
    }


    void Remove( std::shared_ptr<BACKGROUND_JOB> aJob )
    {
        auto it = m_jobPanels.find( aJob );
        if( it != m_jobPanels.end() )
        {
            BACKGROUND_JOB_PANEL* panel = m_jobPanels[aJob];
            m_contentLayout->removeWidget( panel );
            panel->deleteLater();

            m_jobPanels.erase( it );
        }
    }

    void UpdateJob( std::shared_ptr<BACKGROUND_JOB> aJob )
    {
        auto it = m_jobPanels.find( aJob );
        if( it != m_jobPanels.end() )
        {
            BACKGROUND_JOB_PANEL* panel = m_jobPanels[aJob];
            panel->UpdateFromJob();
        }
    }

private:
    QScrollArea* m_scrollArea;
    QWidget*     m_contentWidget;
    QVBoxLayout* m_contentLayout;
    std::unordered_map<std::shared_ptr<BACKGROUND_JOB>, BACKGROUND_JOB_PANEL*> m_jobPanels;
};


BACKGROUND_JOB_REPORTER::BACKGROUND_JOB_REPORTER( BACKGROUND_JOBS_MONITOR* aMonitor,
                                                  std::shared_ptr<BACKGROUND_JOB> aJob ) :
        PROGRESS_REPORTER_BASE( 1 ),
        m_monitor( aMonitor ), m_job( aJob )
{

}


bool BACKGROUND_JOB_REPORTER::updateUI()
{
    return !m_cancelled;
}


void BACKGROUND_JOB_REPORTER::Report( const QString& aMessage )
{
    m_job->m_status = aMessage;
    m_monitor->jobUpdated( m_job );
}


void BACKGROUND_JOB_REPORTER::SetNumPhases( int aNumPhases )
{
    PROGRESS_REPORTER_BASE::SetNumPhases( aNumPhases );
    m_job->m_maxProgress = m_numPhases;
    m_monitor->jobUpdated( m_job );
}


void BACKGROUND_JOB_REPORTER::AdvancePhase()
{
    PROGRESS_REPORTER_BASE::AdvancePhase();
    m_job->m_currentProgress = m_phase;
    m_monitor->jobUpdated( m_job );
}


BACKGROUND_JOBS_MONITOR::BACKGROUND_JOBS_MONITOR()
{

}


std::shared_ptr<BACKGROUND_JOB> BACKGROUND_JOBS_MONITOR::Create( const QString& aName )
{
    std::shared_ptr<BACKGROUND_JOB> job = std::make_shared<BACKGROUND_JOB>();

    job->m_name = aName;
    job->m_reporter = std::make_shared<BACKGROUND_JOB_REPORTER>( this, job );

    std::lock_guard<std::shared_mutex> lock( m_mutex );
    m_jobs.push_back( job );

    if( m_shownDialogs.size() > 0 )
    {
        for( BACKGROUND_JOB_LIST* list : m_shownDialogs )
        {
            QTimer::singleShot( 0, [list, job]()
                    {
                        list->Add( job );
                    } );
        }
    }

    return job;
}


void BACKGROUND_JOBS_MONITOR::Remove( std::shared_ptr<BACKGROUND_JOB> aJob )
{
    if( m_shownDialogs.size() > 0 )
    {
        for( BACKGROUND_JOB_LIST* list : m_shownDialogs )
        {
            QTimer::singleShot( 0, [list, aJob]()
                    {
                        list->Remove( aJob );
                    } );
        }
    }

    std::lock_guard<std::shared_mutex> lock( m_mutex );
    m_jobs.erase( std::remove_if( m_jobs.begin(), m_jobs.end(),
                                  [&]( std::shared_ptr<BACKGROUND_JOB> job )
                                  {
                                      return job == aJob;
                                  } ) );

    if( m_jobs.size() > 0 )
    {
        jobUpdated( m_jobs.front() );
    }
    else
    {
        for( KISTATUSBAR* statusBar : m_statusBars )
        {
            QTimer::singleShot( 0, [statusBar]()
                    {
                        statusBar->HideBackgroundProgressBar();
                        statusBar->SetBackgroundStatusText( QString( "" ) );
                    } );
        }
    }
}


void BACKGROUND_JOBS_MONITOR::ShowList( QWidget* aParent, QPoint aPos )
{
    BACKGROUND_JOB_LIST* list = new BACKGROUND_JOB_LIST( aParent, aPos );

    std::shared_lock<std::shared_mutex> lock( m_mutex, std::try_to_lock );

    for( std::shared_ptr<BACKGROUND_JOB> job : m_jobs )
    {
        list->Add( job );
    }

    lock.unlock();

    m_shownDialogs.push_back( list );

    connect( list, &QWidget::destroyed, this, [this, list]()
            {
                m_shownDialogs.erase( std::remove_if( m_shownDialogs.begin(), m_shownDialogs.end(),
                                              [&]( BACKGROUND_JOB_LIST* dialog )
                                              {
                                                  return dialog == list;
                                              } ), m_shownDialogs.end() );
            } );

    QSize windowSize = list->size();
    list->move( aPos - QPoint( windowSize.width(), windowSize.height() ) );

    list->show();
}


void BACKGROUND_JOBS_MONITOR::jobUpdated( std::shared_ptr<BACKGROUND_JOB> aJob )
{
    std::shared_lock<std::shared_mutex> lock( m_mutex, std::try_to_lock );

    if( m_jobs.size() > 0 )
    {
        if( m_jobs.front() == aJob )
        {
            for( KISTATUSBAR* statusBar : m_statusBars )
            {
                QTimer::singleShot( 0, [statusBar, aJob]()
                        {
                            statusBar->ShowBackgroundProgressBar();
                            statusBar->SetBackgroundProgress( aJob->m_currentProgress );
                            statusBar->SetBackgroundProgressMax( aJob->m_maxProgress );
                            statusBar->SetBackgroundStatusText( aJob->m_status );
                        } );
            }
        }
    }


    for( BACKGROUND_JOB_LIST* list : m_shownDialogs )
    {
        QTimer::singleShot( 0, [list, aJob]()
                {
                    list->UpdateJob( aJob );
                } );
    }
}


void BACKGROUND_JOBS_MONITOR::RegisterStatusBar( KISTATUSBAR* aStatusBar )
{
    m_statusBars.push_back( aStatusBar );
}


void BACKGROUND_JOBS_MONITOR::UnregisterStatusBar( KISTATUSBAR* aStatusBar )
{
    m_statusBars.erase( std::remove_if( m_statusBars.begin(), m_statusBars.end(),
                                        [&]( KISTATUSBAR* statusBar )
                                        {
                                            return statusBar == aStatusBar;
                                        } ) );
}