// QT_TRANSFORMATION_COMPLETED

#ifndef KISTATUSBAR_H
#define KISTATUSBAR_H

#include <kicommon.h>
#include <QStatusBar>
#include <QObject>

class QProgressBar;
class QPushButton;
class QLabel;
class BITMAP_BUTTON;

class KICOMMON_API KISTATUSBAR : public QStatusBar
{
    Q_OBJECT

public:
    KISTATUSBAR( int aNumberFields, QWidget* parent, int id );

    ~KISTATUSBAR();

    void SetEllipsedTextField( const QString& aText, int aFieldId );

    void ShowBackgroundProgressBar( bool aCancellable = false );

    void HideBackgroundProgressBar();

    void SetBackgroundProgress( int aAmount );

    void SetBackgroundProgressMax( int aAmount );

    void SetBackgroundStatusText( const QString& aTxt );

    void SetNotificationCount( int aCount );

private slots:
    void onSize();
    void onBackgroundProgressClick();
    void onNotificationsIconClick();

private:
    QProgressBar*  m_backgroundProgressBar;
    QPushButton*   m_backgroundStopButton;
    QLabel*        m_backgroundTxt;
    BITMAP_BUTTON* m_notificationsButton;
    int            m_normalFieldsCount;
};

#endif
