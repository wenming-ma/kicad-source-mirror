
#include <QtWidgets/QWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>
#include <QtCore/QTimer>
#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtGui/QFocusEvent>
#include <QtGui/QCloseEvent>
#include <QtCore/QStandardPaths>

#include <paths.h>

#include <notifications_manager.h>
#include <widgets/kistatusbar.h>
#include <widgets/ui_common.h>
#include <json_common.h>
#include <kiplatform/ui.h>

#include <core/json_serializers.h>
#include <core/kicad_algo.h>

#include <algorithm>
#include <fstream>
#include <map>
#include <optional>
#include <tuple>
#include <vector>


static long long g_last_closed_timer = 0;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( NOTIFICATION, title, description, href, key, date )

class NOTIFICATION_PANEL : public QWidget
{
public:
    NOTIFICATION_PANEL( QWidget* aParent, NOTIFICATIONS_MANAGER* aManager, NOTIFICATION* aNoti ) :
            QWidget( aParent ),
            m_hlDetails( nullptr ),
            m_notification( aNoti ),
            m_manager( aManager )
    {
        setFixedHeight( 75 );
        setStyleSheet( "border: 1px solid gray;" );

        QVBoxLayout* mainSizer = new QVBoxLayout( this );

        QColor fg, bg;
        KIPLATFORM::UI::GetInfoBarColours( fg, bg );
        setStyleSheet( QString( "background-color: %1; color: %2;" ).arg( bg.name(), fg.name() ) );

        m_stTitle = new QLabel( aNoti->title, this );
        m_stTitle->setWordWrap( true );
        QFont boldFont = KIUI::GetControlFont( this );
        boldFont.setBold( true );
        m_stTitle->setFont( boldFont );
        mainSizer->addWidget( m_stTitle );

        m_stDescription = new QLabel( aNoti->description, this );
        m_stDescription->setWordWrap( true );
        mainSizer->addWidget( m_stDescription );

        QHBoxLayout* tailSizer = new QHBoxLayout();

        if( !aNoti->href.isEmpty() )
        {
            m_hlDetails = new QLabel( this );
            m_hlDetails->setText( QString( "<a href='%1'>View Details</a>" ).arg( aNoti->href ) );
            m_hlDetails->setOpenExternalLinks( false );
            connect( m_hlDetails, &QLabel::linkActivated, this, &NOTIFICATION_PANEL::onDetails );
            tailSizer->addWidget( m_hlDetails );
        }

        m_hlDismiss = new QLabel( this );
        m_hlDismiss->setText( "<a href='dismiss'>Dismiss</a>" );
        m_hlDismiss->setOpenExternalLinks( false );
        connect( m_hlDismiss, &QLabel::linkActivated, this, &NOTIFICATION_PANEL::onDismiss );
        tailSizer->addWidget( m_hlDismiss );

        mainSizer->addLayout( tailSizer );
        mainSizer->addStretch();
        setLayout( mainSizer );
    }

private:
    void onDetails( const QString& url )
    {
        QString processedUrl = url;

        if( processedUrl.startsWith( "kicad://" ) )
        {
            processedUrl.replace( "kicad://", "" );

            if( processedUrl == "pcm" )
            {
                
            }
        }
        else
        {
            QDesktopServices::openUrl( QUrl( url ) );
        }
    }

    void onDismiss( const QString& )
    {
        QTimer::singleShot( 0, this, [this]()
                {
                    // This will cause this panel to get deleted
                    m_manager->Remove( m_notification->key );
                } );
    }

private:
    QLabel*                m_stTitle;
    QLabel*                m_stDescription;
    QLabel*                m_hlDetails;
    QLabel*                m_hlDismiss;
    NOTIFICATION*          m_notification;
    NOTIFICATIONS_MANAGER* m_manager;
};


class NOTIFICATIONS_LIST : public QFrame
{
public:
    NOTIFICATIONS_LIST( NOTIFICATIONS_MANAGER* aManager, QWidget* parent, const QPoint& pos ) :
            QFrame( parent, Qt::Popup | Qt::FramelessWindowHint ),
            m_manager( aManager )
    {
        setWindowTitle( "Notifications" );
        resize( 300, 150 );
        move( pos );

        QVBoxLayout* bSizer1 = new QVBoxLayout( this );
        bSizer1->setContentsMargins( 0, 0, 0, 0 );

        m_scrolledWindow = new QScrollArea( this );
        m_scrolledWindow->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
        m_scrolledWindow->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        m_scrolledWindow->setWidgetResizable( true );
        m_scrolledWindow->setFrameStyle( QFrame::StyledPanel );
        
        QColor fg, bg;
        KIPLATFORM::UI::GetInfoBarColours( fg, bg );
        m_scrolledWindow->setStyleSheet( QString( "background-color: %1; color: %2;" ).arg( bg.name(), fg.name() ) );

        QWidget* scrollWidget = new QWidget();
        m_contentSizer = new QVBoxLayout( scrollWidget );
        scrollWidget->setLayout( m_contentSizer );
        m_scrolledWindow->setWidget( scrollWidget );

        bSizer1->addWidget( m_scrolledWindow );

        m_noNotificationsText = new QLabel( "There are no notifications available", scrollWidget );
        m_noNotificationsText->setAlignment( Qt::AlignCenter );
        m_noNotificationsText->setWordWrap( true );
        m_contentSizer->addWidget( m_noNotificationsText );

        setLayout( bSizer1 );
        setFocus();
    }


protected:
    void focusOutEvent( QFocusEvent* aEvent ) override
    {
        if( !isAncestorOf( aEvent->widget() ) )
        {
            close();
            g_last_closed_timer = QDateTime::currentMSecsSinceEpoch();
        }
        QFrame::focusOutEvent( aEvent );
    }

public:


    void Add( NOTIFICATION* aNoti )
    {
        m_noNotificationsText->hide();

        NOTIFICATION_PANEL* panel = new NOTIFICATION_PANEL( m_scrolledWindow->widget(), m_manager, aNoti );
        m_contentSizer->addWidget( panel );
        m_scrolledWindow->widget()->updateGeometry();

        updateGeometry();

        m_panelMap[aNoti] = panel;
    }


    void Remove( NOTIFICATION* aNoti )
    {
        auto it = m_panelMap.find( aNoti );

        if( it != m_panelMap.end() )
        {
            NOTIFICATION_PANEL* panel = m_panelMap[aNoti];
            m_contentSizer->removeWidget( panel );
            panel->deleteLater();

            m_panelMap.erase( it );

            m_scrolledWindow->widget()->updateGeometry();
            updateGeometry();
        }

        if( m_panelMap.size() == 0 )
        {
            m_noNotificationsText->show();
        }
    }

private:
    QScrollArea*                                           m_scrolledWindow;

    QVBoxLayout*                                           m_contentSizer;
    std::unordered_map<NOTIFICATION*, NOTIFICATION_PANEL*> m_panelMap;
    NOTIFICATIONS_MANAGER*                                 m_manager;

    QLabel*                                                m_noNotificationsText;
};


NOTIFICATIONS_MANAGER::NOTIFICATIONS_MANAGER()
{
    m_destFileName = QDir( PATHS::GetUserCachePath() ).filePath( "notifications.json" );
}


void NOTIFICATIONS_MANAGER::Load()
{
    nlohmann::json saved_json;

    std::ifstream saved_json_stream( m_destFileName.toStdString() );

    try
    {
        saved_json_stream >> saved_json;

        m_notifications = saved_json.get<std::vector<NOTIFICATION>>();
    }
    catch( std::exception& )
    {
        // failed to load the json, which is fine, default to no notifications
    }

    if( !qEnvironmentVariable( "KICAD_TEST_NOTI" ).isEmpty() )
    {
        CreateOrUpdate( "test", "Test Notification", "Test please ignore",
                        "https://kicad.org" );
    }
}


void NOTIFICATIONS_MANAGER::Save()
{
    std::ofstream jsonFileStream( m_destFileName.toStdString() );

    nlohmann::json saveJson = nlohmann::json( m_notifications );
    jsonFileStream << std::setw( 4 ) << saveJson << std::endl;
    jsonFileStream.flush();
    jsonFileStream.close();
}


void NOTIFICATIONS_MANAGER::CreateOrUpdate( const QString& aKey,
                                            const QString& aTitle,
                                            const QString& aDescription,
                                            const QString& aHref )
{
    Q_ASSERT( !aKey.isEmpty() );

    auto it = std::find_if( m_notifications.begin(), m_notifications.end(),
                            [&]( const NOTIFICATION& noti )
                            {
                                return noti.key == aKey;
                            } );

    if( it != m_notifications.end() )
    {
        NOTIFICATION& noti = *it;

        noti.title = aTitle;
        noti.description = aDescription;
        noti.href = aHref;
    }
    else
    {
        m_notifications.emplace_back( NOTIFICATION{ aTitle, aDescription, aHref,
                                                    aKey, QString() } );
    }

    if( m_shownDialogs.size() > 0 )
    {
        // update dialogs
        for( NOTIFICATIONS_LIST* list : m_shownDialogs )
            list->Add( &m_notifications.back() );
    }

    for( KISTATUSBAR* statusBar : m_statusBars )
        statusBar->SetNotificationCount( m_notifications.size() );

    Save();
}


void NOTIFICATIONS_MANAGER::Remove( const QString& aKey )
{
    auto it = std::find_if( m_notifications.begin(), m_notifications.end(),
                            [&]( const NOTIFICATION& noti )
                            {
                                return noti.key == aKey;
                            } );

    if( it == m_notifications.end() )
        return;

    if( m_shownDialogs.size() > 0 )
    {
        // update dialogs

        for( NOTIFICATIONS_LIST* list : m_shownDialogs )
            list->Remove( &(*it) );
    }

    m_notifications.erase( it );

    Save();

    for( KISTATUSBAR* statusBar : m_statusBars )
        statusBar->SetNotificationCount( m_notifications.size() );
}


void NOTIFICATIONS_MANAGER::onListWindowClosed( QCloseEvent* aEvent )
{
    NOTIFICATIONS_LIST* evtWindow = qobject_cast<NOTIFICATIONS_LIST*>( sender() );

    alg::delete_if( m_shownDialogs, [&]( NOTIFICATIONS_LIST* dialog )
                                    {
                                        return dialog == evtWindow;
                                    } );

    aEvent->accept();
}


void NOTIFICATIONS_MANAGER::ShowList( QWidget* aParent, QPoint aPos )
{
    if( QDateTime::currentMSecsSinceEpoch() - g_last_closed_timer < 300 )
    {
        g_last_closed_timer = 0;
        return;
    }

    NOTIFICATIONS_LIST* list = new NOTIFICATIONS_LIST( this, aParent, aPos );

    for( NOTIFICATION& job : m_notifications )
        list->Add( &job );

    m_shownDialogs.push_back( list );

    connect( list, &QWidget::destroyed, this, &NOTIFICATIONS_MANAGER::onListWindowClosed );

    QSize windowSize = list->size();
    list->move( aPos - QPoint( windowSize.width(), windowSize.height() ) );

    list->show();
    KIPLATFORM::UI::ForceFocus( list );
}


void NOTIFICATIONS_MANAGER::RegisterStatusBar( KISTATUSBAR* aStatusBar )
{
    m_statusBars.push_back( aStatusBar );

    // notifications should already be loaded so set the initial notification count
    aStatusBar->SetNotificationCount( m_notifications.size() );
}


void NOTIFICATIONS_MANAGER::UnregisterStatusBar( KISTATUSBAR* aStatusBar )
{
    alg::delete_if( m_statusBars, [&]( KISTATUSBAR* statusBar )
                                  {
                                      return statusBar == aStatusBar;
                                  } );
}
