// QT_TRANSFORMATION_COMPLETED

#ifndef GENERIC_PROGRESS_REPORTER_H
#define GENERIC_PROGRESS_REPORTER_H

#include <mutex>
#include <atomic>
#include <QString>
#include <progress_reporter.h>
class KICOMMON_API PROGRESS_REPORTER_BASE : public PROGRESS_REPORTER
{
public:

    PROGRESS_REPORTER_BASE( int aNumPhases );
    PROGRESS_REPORTER_BASE( const PROGRESS_REPORTER_BASE& ) = delete;

    virtual ~PROGRESS_REPORTER_BASE()
    {
    }

    void SetNumPhases( int aNumPhases ) override;
    void AddPhases( int aNumPhases ) override;

    virtual void BeginPhase( int aPhase ) override;

    virtual void AdvancePhase() override;

    virtual void AdvancePhase( const QString& aMessage ) override;

    virtual void Report( const QString& aMessage ) override;

    virtual void SetCurrentProgress( double aProgress ) override;

    void SetMaxProgress( int aMaxProgress ) override;

    void AdvanceProgress() override;

    bool KeepRefreshing( bool aWait = false ) override;

    void SetTitle( const QString& aTitle ) override { }

    bool IsCancelled() const override { return m_cancelled; }

    int CurrentProgress() const;

protected:

    virtual bool updateUI() = 0;

    QString           m_rptMessage;

    mutable std::mutex m_mutex;
    std::atomic_int    m_phase;
    std::atomic_int    m_numPhases;
    std::atomic_int    m_progress;
    std::atomic_int    m_maxProgress;
    std::atomic_bool   m_cancelled;

    std::atomic_bool   m_messageChanged;
};


#endif
