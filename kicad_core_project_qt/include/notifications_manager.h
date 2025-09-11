#ifndef NOTIFICATIONS_MANAGER_H
#define NOTIFICATIONS_MANAGER_H

#include <kicommon.h>
#include <functional>
#include <vector>

class QString;
class KISTATUSBAR;
struct NOTIFICATION;
class NOTIFICATIONS_LIST;
class QWidget;
class QCloseEvent;
class QPoint;


struct KICOMMON_API NOTIFICATION
{
public:
    QString title;
    QString description;
    QString href;
    QString key;
    QString date;
};


class KICOMMON_API NOTIFICATIONS_MANAGER
{
public:
    NOTIFICATIONS_MANAGER();

    void CreateOrUpdate( const QString& aKey, const QString& aTitle, const QString& aDescription,
                         const QString& aHref = QString() );

    void Remove( const QString& aKey );

    void Load();

    void Save();

    void ShowList( QWidget* aParent, QPoint aPos );

    void RegisterStatusBar( KISTATUSBAR* aStatusBar );

    void UnregisterStatusBar( KISTATUSBAR* aStatusBar );

private:
    void onListWindowClosed( QCloseEvent* aEvent );

private:
    std::vector<NOTIFICATION>        m_notifications;
    std::vector<NOTIFICATIONS_LIST*> m_shownDialogs;
    std::vector<KISTATUSBAR*>        m_statusBars;
    QString                      m_destFileName;
};

#endif
