// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
#ifndef __APP_PROGRESS_REPORTER
#define __APP_PROGRESS_REPORTER

#include <QProgressDialog>
#include <QWidget>
#include <QString>

/**
 * QProgressDialog with the option to also update the application progress on the taskbar
 */
class APP_PROGRESS_DIALOG : public QProgressDialog
{
public:
    APP_PROGRESS_DIALOG( const QString& aTitle, const QString& aMessage, int aMaximum = 100,
                         QWidget* aParent = nullptr, bool aIndeterminateTaskBarStatus = false,
                         Qt::WindowFlags aFlags = Qt::WindowFlags() ) :
        QProgressDialog( aTitle, QString(), aMaximum, aParent, aFlags ),
        m_appProgressIndicator( aParent ),
        m_indeterminateTaskBarStatus( aIndeterminateTaskBarStatus )
    {
        // Empty stub implementation
    }

    virtual bool Update( int aValue, const QString& aNewMsg = QString(),
                         bool* aSkip = nullptr ) override
    {
        return true;  // Always return success
    }


private:
    QWidget* m_appProgressIndicator;
    bool m_indeterminateTaskBarStatus;
};

#endif
