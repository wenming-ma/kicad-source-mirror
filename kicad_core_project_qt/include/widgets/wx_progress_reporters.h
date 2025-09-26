
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

#ifndef WX_PROGRESS_REPORTERS_H
#define WX_PROGRESS_REPORTERS_H

#include <QProgressDialog>
#include <QProgressBar>
#include <QWidget>
#include <QString>

#include <widgets/progress_reporter_base.h>

/**
 * Multi-thread safe progress reporter dialog, intended for use of tasks that parallel reporting
 * back of work status.
 *
 * @see PROGRESS_REPORTER.
 */
class WX_PROGRESS_REPORTER : public PROGRESS_REPORTER_BASE, public QProgressDialog
{
public:
    /**
     * The #PROGRESS_REPORTER will stay on top of \a aParent.
     *
     * The style includes auto-hide, abort capability, and elapsed time display.
     *
     * @param aParent is the QWidget of Frame that manage this.
     * @param aTitle is the dialog progress title
     * @param aNumPhases is the number of "virtual sections" of the progress bar
     *   aNumPhases = 1 is the usual progress bar
     *   aNumPhases = n creates n virtual progress bar zones: a 0 to 100 percent width
     *   of a virtual zone fills 0 to 1/n progress bar full size of the nth virtual zone index
     * @param aCanAbort is true if the abort button should be shown
     * @param aReserveSpaceForMessage will ensure that the dialog is laid out for status messages,
     *        preventing layout issues on Windows when reporting a message after the initial layout
     */
    WX_PROGRESS_REPORTER( QWidget* aParent, const QString& aTitle, int aNumPhases,
                          bool aCanAbort = true, bool aReserveSpaceForMessage = true );
    ~WX_PROGRESS_REPORTER();

    /**
     * Change the title displayed on the window caption.
     */
    void SetTitle( const QString& aTitle ) override
    {
        QProgressDialog::setWindowTitle( aTitle );
    }

private:
    bool updateUI() override;

private:
    int                    m_messageWidth;
};


class GAUGE_PROGRESS_REPORTER : public PROGRESS_REPORTER_BASE, public QProgressBar
{
public:
    /**
     * @param aParent is the parent of the QProgressBar control
     * @param aNumPhases is the number of "virtual sections" of the progress bar
     *   aNumPhases = 1 is the usual progress bar
     *   aNumPhases = n creates n virtual progress bar zones: a 0 to 100 percent width
     *   of a virtual zone fills 0 to 1/n progress bar full size of the nth virtual zone index
     */
    GAUGE_PROGRESS_REPORTER( QWidget* aParent, int aNumPhases );

private:
    bool updateUI() override;
};

#endif
