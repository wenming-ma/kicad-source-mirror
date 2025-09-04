#ifndef BACKGROUND_JOBS_MONITOR_H
#define BACKGROUND_JOBS_MONITOR_H

#include <kicommon.h>
#include <widgets/progress_reporter_base.h>
#include <functional>
#include <memory>
#include <shared_mutex>
#include <vector>

#include <QString>
#include <QWidget>
#include <QPoint>
#include <QCloseEvent>

class PROGRESS_REPORTER;
class KISTATUSBAR;
struct BACKGROUND_JOB;
class BACKGROUND_JOB_REPORTER;
class BACKGROUND_JOB_LIST;
class BACKGROUND_JOBS_MONITOR;

class KICOMMON_API BACKGROUND_JOB_REPORTER : public PROGRESS_REPORTER_BASE
{
public:
    BACKGROUND_JOB_REPORTER( BACKGROUND_JOBS_MONITOR*        aMonitor,
                             std::shared_ptr<BACKGROUND_JOB> aJob );

    void SetTitle( const QString& aTitle ) override
    {
    }

    void Report( const QString& aMessage ) override;

    void Cancel() { m_cancelled.store( true ); }

    void AdvancePhase() override;

    void SetNumPhases( int aNumPhases ) override;

private:
    bool updateUI() override;

    BACKGROUND_JOBS_MONITOR* m_monitor;
    std::shared_ptr<BACKGROUND_JOB> m_job;
    QString m_title;
    QString m_report;
};


struct KICOMMON_API BACKGROUND_JOB
{
public:
    QString m_name;
    QString m_status;
    std::shared_ptr<BACKGROUND_JOB_REPORTER> m_reporter;

    int m_maxProgress;
    int m_currentProgress;
};


class KICOMMON_API BACKGROUND_JOBS_MONITOR
{
    friend class BACKGROUND_JOB_REPORTER;
    friend class BACKGROUND_JOB_LIST;

public:
    BACKGROUND_JOBS_MONITOR();

    std::shared_ptr<BACKGROUND_JOB> Create( const QString& aName );

    void Remove( std::shared_ptr<BACKGROUND_JOB> job );

    void ShowList( QWidget* aParent, QPoint aPos );

    void RegisterStatusBar( KISTATUSBAR* aStatusBar );

    void UnregisterStatusBar( KISTATUSBAR* aStatusBar );

private:
    void jobUpdated( std::shared_ptr<BACKGROUND_JOB> aJob );

    std::vector<std::shared_ptr<BACKGROUND_JOB>> m_jobs;
    std::vector<BACKGROUND_JOB_LIST*> m_shownDialogs;

    std::vector<KISTATUSBAR*> m_statusBars;

    mutable std::shared_mutex m_mutex;
};

#endif