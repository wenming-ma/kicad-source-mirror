#include <kiway_player.h>
#include <kiway_express.h>
#include <kiway.h>
#include <id.h>
#include <macros.h>
#include <typeinfo>
#include <QWidget>
#include <QEventLoop>
#include <QApplication>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <core/raii.h>


// Event table macros removed - Qt uses signal/slot mechanism instead


KIWAY_PLAYER::KIWAY_PLAYER( KIWAY* aKiway, QWidget* aParent, FRAME_T aFrameType,
                            const QString& aTitle, const QPoint& aPos, const QSize& aSize,
                            long aStyle, const QString& aFrameName,
                            const EDA_IU_SCALE& aIuScale ) :
        EDA_BASE_FRAME( aParent, aFrameType, aTitle, aPos, aSize, 
                        static_cast<Qt::WindowFlags>(aStyle), aFrameName, aKiway,
                        aIuScale ),
        m_modal( false ),
        m_modal_loop( nullptr ),
        m_modal_resultant_parent( nullptr ),
        m_modal_ret_val( false ),
        m_socketServer( nullptr )
{
}


KIWAY_PLAYER::~KIWAY_PLAYER() throw()
{
    // socket server must be destructed before we complete
    // destructing the frame or else we could crash
    // as the socket server holds a reference to this frame
    if( m_socketServer )
    {
        // ensure any event handling stops
        m_socketServer->pauseAccepting();

        delete m_socketServer;
        m_socketServer = nullptr;
    }

    // remove active sockets as well
    for( QTcpSocket* socket : m_sockets )
    {
        if( !socket )
            continue;

        // ensure any event handling stops
        socket->disconnectFromHost();

        delete socket;
    }

    m_sockets.clear();
}


void KIWAY_PLAYER::KiwayMailIn( KIWAY_EXPRESS& aEvent )
{
    // override this in derived classes.
}


bool KIWAY_PLAYER::ShowModal( QString* aResult, QWidget* aResultantFocusWindow )
{
    Q_ASSERT_X( IsModal(), "ShowModal", "ShowModal() shouldn't be called on non-modal frame" );

    NULLER raii_nuller( (void*&) m_modal_loop );

    m_modal_resultant_parent = aResultantFocusWindow;

    show();
    raise();    // Needed on some Window managers to always display the frame

    setFocus();

    {
        // Using QWidget disabling has two issues: it will disable top-level windows that are
        // our *children* (such as sub-frames), and it will disable all context menus we try to
        // put up.  Fortunatly we already had to cross this Rubicon for QuasiModal dialogs, so
        // we re-use that strategy.
        QWidget* parent = parentWidget();

        while( parent && !parent->isWindow() )
            parent = parent->parentWidget();

        WINDOW_DISABLER raii_parent_disabler( parent );

        QEventLoop event_loop;
        m_modal_loop = &event_loop;
        event_loop.exec();
    }

    if( aResult )
        *aResult = m_modal_string;

    if( aResultantFocusWindow )
    {
        aResultantFocusWindow->raise();

        // have the final say, after WINDOW_DISABLER reenables my parent and
        // the events settle down, set the focus
        QApplication::processEvents();
        aResultantFocusWindow->setFocus();
    }

    return m_modal_ret_val;
}


bool KIWAY_PLAYER::Destroy()
{
    Kiway().PlayerDidClose( GetFrameType() );

    // In Qt, we use close() and deleteLater() instead of wxWidgets Destroy()
    close();
    deleteLater();
    return true;
}


bool KIWAY_PLAYER::IsDismissed()
{
    return !m_modal_loop;
}


void KIWAY_PLAYER::DismissModal( bool aRetVal, const QString& aResult )
{
    m_modal_ret_val = aRetVal;
    m_modal_string  = aResult;

    if( m_modal_loop )
    {
        m_modal_loop->exit();
        m_modal_loop = nullptr;      // this marks it as dismissed.
    }

    hide();
}


void KIWAY_PLAYER::kiway_express( KIWAY_EXPRESS& aEvent )
{
    // logging support
    KiwayMailIn( aEvent );     // call the virtual, override in derived.
}


void KIWAY_PLAYER::language_change( QAction* action )
{
    int id = action->data().toInt();

    // tell all the KIWAY_PLAYERs about the language change.
    Kiway().SetLanguage( id );
}



