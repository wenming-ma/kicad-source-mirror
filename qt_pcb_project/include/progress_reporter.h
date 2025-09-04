
#ifndef PROGRESS_REPORTER_H
#define PROGRESS_REPORTER_H

#include <kicommon.h>
#include <QString>

/**
 * A progress reporter interface for use in multi-threaded environments.  The various advancement
 * and message methods can be called from sub-threads.  The KeepRefreshing method *MUST* be called
 * only from the main thread (primarily a MSW requirement, which won't allow access to UI objects
 * allocated from a separate thread).
 */
class KICOMMON_API PROGRESS_REPORTER
{
public:

    PROGRESS_REPORTER()
    { }

    PROGRESS_REPORTER( const PROGRESS_REPORTER& ) = delete;

    virtual ~PROGRESS_REPORTER()
    { }

    /**
     * Set the number of phases.
     */
    virtual void SetNumPhases( int aNumPhases ) = 0;
    virtual void AddPhases( int aNumPhases ) = 0;

    /**
     * Initialize the \a aPhase virtual zone of the dialog progress bar.
     */
    virtual void BeginPhase( int aPhase ) = 0;

    /**
     * Use the next available virtual zone of the dialog progress bar.
     */
    virtual void AdvancePhase() = 0;

    /**
     * Use the next available virtual zone of the dialog progress bar and updates the message.
     */
    virtual void AdvancePhase( const QString& aMessage ) = 0;

    /**
     * Display \a aMessage in the progress bar dialog.
     */
    virtual void Report( const QString& aMessage ) = 0;

    /**
     * Set the progress value to aProgress (0..1).
     */
    virtual void SetCurrentProgress( double aProgress ) = 0;

    /**
     * Fix the value that gives the 100 percent progress bar length
     * (inside the current virtual zone).
     */
    virtual void SetMaxProgress( int aMaxProgress ) = 0;

    /**
     * Increment the progress bar length (inside the current virtual zone).
     */
    virtual void AdvanceProgress() = 0;

    /**
     * Update the UI (if any).
     *
     * @warning This should  only be called from the main thread.
     *
     * @return false if the user cancelled.
     */
    virtual bool KeepRefreshing( bool aWait = false ) = 0;

    /**
     * Change the title displayed on the window caption.
     *
     * Has meaning only for some reporters.  Does nothing for others.
     *
     * @warning This should only be called from the main thread.
     */
    virtual void SetTitle( const QString& aTitle ) = 0;

    virtual bool IsCancelled() const = 0;
};


#endif
