
#include <dialog_shim.h>
#include <core/ignore.h>
#include <kiway_player.h>
#include <kiway.h>
#include <pgm_base.h>
#include <tool/tool_manager.h>
#include <kiplatform/ui.h>
#include <map>
#include <unordered_map>

#include <QApplication>
#include <QWidget>
#include <QCoreApplication>
#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QEventLoop>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QFocusEvent>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QHash>
#include <QLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QTextCursor>
#include <QScreen>
#include <QGuiApplication>
#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QList>
#include <QtGlobal>
#include <QObject>

#include <algorithm>

// Internationalization macro
#define _(s) QCoreApplication::translate("", (s))


DIALOG_SHIM::DIALOG_SHIM( QWidget* aParent, int id, const QString& title,
                          const QPoint& pos, const QSize& size, long style,
                          const QString& name ) :
        QDialog( aParent ),
        KIWAY_HOLDER( nullptr, KIWAY_HOLDER::DIALOG ),
        m_units( EDA_UNITS::MM ),
        m_useCalculatedSize( false ),
        m_firstPaintEvent( true ),
        m_initialFocusTarget( nullptr ),
        m_isClosing( false ),
        m_qmodal_loop( nullptr ),
        m_qmodal_showing( false ),
        m_qmodal_parent_disabler( nullptr ),
        m_parentFrame( nullptr ),
        m_userPositioned( false ),
        m_userResized( false )
{
    KIWAY_HOLDER* kiwayHolder = nullptr;
    m_initialSize = size;
    setWindowTitle( title );
    if( pos != QPoint() )
        move( pos );
    if( size != QSize() )
        resize( size );

    if( aParent )
    {
        kiwayHolder = dynamic_cast<KIWAY_HOLDER*>( aParent );

        while( !kiwayHolder && aParent->parentWidget() )
        {
            aParent = aParent->parentWidget();
            kiwayHolder = dynamic_cast<KIWAY_HOLDER*>( aParent );
        }
    }

    // Inherit units from parent
    if( kiwayHolder && kiwayHolder->GetType() == KIWAY_HOLDER::FRAME )
        m_units = static_cast<EDA_BASE_FRAME*>( kiwayHolder )->GetUserUnits();
    else if( kiwayHolder && kiwayHolder->GetType() == KIWAY_HOLDER::DIALOG )
        m_units = static_cast<DIALOG_SHIM*>( kiwayHolder )->GetUserUnits();

    // Don't mouse-warp after a dialog run from the context menu
    if( kiwayHolder && kiwayHolder->GetType() == KIWAY_HOLDER::FRAME )
    {
        m_parentFrame = static_cast<EDA_BASE_FRAME*>( kiwayHolder );
        TOOL_MANAGER* toolMgr = m_parentFrame->GetToolManager();

        if( toolMgr && toolMgr->IsContextMenuActive() )
            toolMgr->VetoContextMenuMouseWarp();
    }

    // Set up the message bus
    if( kiwayHolder )
        SetKiway( this, &kiwayHolder->Kiway() );

    if( HasKiway() )
        Kiway().SetBlockingDialog( this );


#ifdef __WINDOWS__
    // On Windows, the app top windows can be brought to the foreground (at least temporarily)
    // in certain circumstances such as when calling an external tool in Eeschema BOM generation.
    // So set the parent frame (if exists) to top window to avoid this annoying behavior.
    if( kiwayHolder && kiwayHolder->GetType() == KIWAY_HOLDER::FRAME )
        Pgm().App().SetTopWindow( (EDA_BASE_FRAME*) kiwayHolder );
#endif

}


DIALOG_SHIM::~DIALOG_SHIM()
{
    m_isClosing = true;


    std::function<void( const QList<QWidget*>& )> disconnectFocusHandlers =
            [&]( const QList<QWidget*>& children )
            {
                for( QWidget* child : children )
                {
                    if( QLineEdit* textCtrl = qobject_cast<QLineEdit*>( child ) )
                    {
                        // Qt handles focus events through event filters, not signals
                        textCtrl->removeEventFilter( this );
                    }
                    else if( QTextEdit* textEdit = qobject_cast<QTextEdit*>( child ) )
                    {
                        // Qt handles focus events through event filters, not signals
                        textEdit->removeEventFilter( this );
                    }
                    else
                    {
                        disconnectFocusHandlers( child->findChildren<QWidget*>() );
                    }
                }
            };

    disconnectFocusHandlers( findChildren<QWidget*>() );

    // if the dialog is quasi-modal, this will end its event loop
    if( IsQuasiModal() )
        EndQuasiModal( QDialog::Rejected );

    if( HasKiway() )
        Kiway().SetBlockingDialog( nullptr );

    delete m_qmodal_parent_disabler;
}


void DIALOG_SHIM::finishDialogSettings()
{
    // must be called from the constructor of derived classes,
    // when all widgets are initialized, and therefore their size fixed

    // SetSizeHints fixes the minimal size of sizers in the dialog
    // (SetSizeHints calls Fit(), so no need to call it)
    if( layout() )
        layout()->setSizeConstraint( QLayout::SetMinimumSize );
}


void DIALOG_SHIM::setSizeInDU( int x, int y )
{
    QSize sz( x, y );
    resize( sz );
}


int DIALOG_SHIM::horizPixelsFromDU( int x ) const
{
    return x;
}


int DIALOG_SHIM::vertPixelsFromDU( int y ) const
{
    return y;
}


// our hashtable is an implementation secret, don't need or want it in a header file
#include <hashtables.h>
#include <typeinfo>

static std::unordered_map<std::string, QRect> class_map;


void DIALOG_SHIM::SetPosition( const QPoint& aNewPosition )
{
    move( aNewPosition );

    // Now update the stored position:
    std::string hash_key;

    if( m_hash_key.size() )
    {
        // a special case like EDA_LIST_DIALOG, which has multiple uses.
        hash_key = m_hash_key;
    }
    else
    {
        hash_key = typeid(*this).name();
    }

    auto it = class_map.find( hash_key );

    if( it == class_map.end() )
        return;

    QRect rect = it->second;
    rect.moveTopLeft( aNewPosition );

    class_map[ hash_key ] = rect;
}


bool DIALOG_SHIM::Show( bool show )
{
    bool        ret;
    std::string hash_key;

    if( m_hash_key.size() )
    {
        // a special case like EDA_LIST_DIALOG, which has multiple uses.
        hash_key = m_hash_key;
    }
    else
    {
        hash_key = typeid(*this).name();
    }

    // Show or hide the window.  If hiding, save current position and size.
    // If showing, use previous position and size.
    if( show )
    {
#ifndef Q_OS_WIN
        raise();
#endif
        this->setVisible( show );
        ret = this->isVisible();

        // classname is key, returns a zeroed-out default QRect if none existed before.
        QRect savedDialogRect = class_map[ hash_key ];

        if( savedDialogRect.size().width() != 0 && savedDialogRect.size().height() != 0 )
        {
            if( m_useCalculatedSize )
            {
                setGeometry( savedDialogRect.topLeft().x(), savedDialogRect.topLeft().y(),
                           size().width(), size().height() );
            }
            else
            {
                setGeometry( savedDialogRect.topLeft().x(), savedDialogRect.topLeft().y(),
                           std::max( size().width(), savedDialogRect.size().width() ),
                           std::max( size().height(), savedDialogRect.size().height() ) );
            }
#ifdef Q_OS_MAC
            if( parentWidget() != nullptr )
            {
                QRect parentRect = parentWidget()->geometry();
                if( QGuiApplication::screenAt( parentRect.center() )
                    != QGuiApplication::screenAt( savedDialogRect.center() ) )
                {
                    move( QGuiApplication::primaryScreen()->availableGeometry().center() - rect().center() );
                }
            }
#endif
        }
        else if( m_initialSize != QSize() )
        {
            resize( m_initialSize );
            move( QGuiApplication::primaryScreen()->availableGeometry().center() - rect().center() );
        }

        // Be sure that the dialog appears in a visible area
        // (the dialog position might have been stored at the time when it was
        // shown on another display)
        if( !QGuiApplication::screenAt( mapToGlobal( rect().center() ) ) )
            move( QGuiApplication::primaryScreen()->availableGeometry().center() - rect().center() );

        m_userPositioned = false;
        m_userResized = false;
    }
    else
    {
        // Save the dialog's position & size before hiding, using classname as key.
        // Be careful of rounding errors: only re-save if the user modified the value or
        // it has not yet been saved.
        QRect rect = class_map[ hash_key ];

        if( m_userPositioned || rect.topLeft() == QPoint() )
            rect.moveTopLeft( pos() );

        if( m_userResized || rect.size() == QSize() )
            rect.setSize( size() );

        class_map[ hash_key ] = rect;

#ifdef Q_OS_MAC
        if ( m_eventLoop )
            m_eventLoop->exit( result() );
#endif

        this->setVisible( show );
        ret = this->isVisible();

        if( parentWidget() )
            parentWidget()->setFocus();
    }

    return ret;
}


void DIALOG_SHIM::resetSize()
{
    std::string hash_key;

    if( m_hash_key.size() )
    {
        // a special case like EDA_LIST_DIALOG, which has multiple uses.
        hash_key = m_hash_key;
    }
    else
    {
        hash_key = typeid(*this).name();
    }

    auto it = class_map.find( hash_key );

    if( it == class_map.end() )
        return;

    QRect rect = it->second;
    rect.setSize( QSize( 0, 0 ) );
    class_map[ hash_key ] = rect;
}


void DIALOG_SHIM::resizeEvent( QResizeEvent* aEvent )
{
    m_userResized = true;
    aEvent->accept();
}


void DIALOG_SHIM::moveEvent( QMoveEvent* aEvent )
{
    m_userPositioned = true;
    aEvent->accept();
}


bool DIALOG_SHIM::Enable( bool enable )
{
    // so we can do logging of this state change:
    this->setEnabled( enable );
    return this->isEnabled();
}


// Recursive descent doing a SelectAll() in text controls.
// MacOS User Interface Guidelines state that when tabbing to a text control all its
// text should be selected.
void DIALOG_SHIM::SelectAllInTextCtrls( const QList<QWidget*>& children )
{
    for( QWidget* child : children )
    {
        if( QLineEdit* textCtrl = qobject_cast<QLineEdit*>( child ) )
        {
            m_beforeEditValues[ textCtrl ] = textCtrl->text();
            // Qt handles focus events through event filters
            textCtrl->installEventFilter( this );

            // We don't currently run this on GTK because some window managers don't hide the
            // selection in non-active controls, and other window managers do the selection
            // automatically anyway.
#if defined( Q_OS_MAC ) || defined( Q_OS_WIN )
            if( !textCtrl->selectedText().isEmpty() )
            {
                // Respect an existing selection
            }
            else if( !textCtrl->isReadOnly() )
            {
                textCtrl->selectAll();
            }
#else
            ignore_unused( textCtrl );
#endif
        }
        else if( QTextEdit* textEdit = qobject_cast<QTextEdit*>( child ) )
        {
            m_beforeEditValues[ textEdit ] = textEdit->toPlainText();
            // Qt handles focus events through event filters
            textEdit->installEventFilter( this );

            if( !textEdit->textCursor().selectedText().isEmpty() )
            {
                // Respect an existing selection
            }
            else if( !textEdit->isReadOnly() )
            {
                textEdit->selectAll();
            }
        }
#ifdef Q_OS_MAC
        else if( QPushButton* button = qobject_cast<QPushButton*>( child ) )
        {
            QSize minSize( 29, 27 );
            QRect rect = child->geometry();

            rect = rect.adjusted( -std::max( 0, minSize.width() - rect.width() ) / 2,
                                  -std::max( 0, minSize.height() - rect.height() ) / 2,
                                  std::max( 0, minSize.width() - rect.width() ) / 2,
                                  std::max( 0, minSize.height() - rect.height() ) / 2 );

            child->setMinimumSize( rect.size() );
            child->setGeometry( rect );
        }
#endif
        else
        {
            SelectAllInTextCtrls( child->findChildren<QWidget*>() );
        }
    }
}


void DIALOG_SHIM::paintEvent( QPaintEvent* event )
{
    if( m_firstPaintEvent )
    {
        KIPLATFORM::UI::FixupCancelButtonCmdKeyCollision( this );

        SelectAllInTextCtrls( findChildren<QWidget*>() );

        if( m_initialFocusTarget )
            KIPLATFORM::UI::ForceFocus( m_initialFocusTarget );
        else
            KIPLATFORM::UI::ForceFocus( this );     // Focus the dialog itself

        m_firstPaintEvent = false;
    }

    event->accept();
}


void DIALOG_SHIM::OnModify()
{
    if( !windowTitle().startsWith( "*" ) )
        setWindowTitle( "*" + windowTitle() );
}


void DIALOG_SHIM::ClearModify()
{
    if( windowTitle().startsWith( "*" ) )
        setWindowTitle( windowTitle().mid( 1 ) );
}

int DIALOG_SHIM::ShowModal()
{
    // Apple in its infinite wisdom will raise a disabled window before even passing
    // us the event, so we have no way to stop it.  Instead, we must set an order on
    // the windows so that the modal will be pushed in front of the disabled
    // window when it is raised.
    KIPLATFORM::UI::ReparentModal( this );

    // Call the base class exec() method
    return exec();
}

/*
    QuasiModal Mode Explained:

    QuasiModal mode is our own almost modal mode which disables only the parent
    of the DIALOG_SHIM, leaving other frames operable and while staying captured in the
    nested event loop. When using ShowQuasiModal() you have to use EndQuasiModal() 
    in your dialogs and not EndModal(). There is also IsQuasiModal() but its value 
    can only be true when the nested event loop is active. Do not mix the modal 
    and quasi-modal functions. Use one set or the other.

    You CAN use it anywhere for any dialog. But you MUST use it when
    you want to use KIWAY_PLAYER::ShowModal() from a dialog event.
*/

int DIALOG_SHIM::ShowQuasiModal()
{
    NULLER raii_nuller( (void*&) m_qmodal_loop );

    // release the mouse if it's currently captured as the window having it
    // will be disabled when this dialog is shown -- but will still keep the
    // capture making it impossible to do anything in the modal dialog itself
    QWidget* win = QWidget::mouseGrabber();
    if( win )
        win->releaseMouse();

    // Get the optimal parent
    QWidget* parent = parentWidget();

    Q_ASSERT_X( !m_qmodal_parent_disabler, "ShowQuasiModal", "Caller using ShowQuasiModal() twice on same window?" );

    // quasi-modal: disable only my "optimal" parent
    m_qmodal_parent_disabler = new WINDOW_DISABLER( parent );

    // Apple in its infinite wisdom will raise a disabled window before even passing
    // us the event, so we have no way to stop it.  Instead, we must set an order on
    // the windows so that the quasi-modal will be pushed in front of the disabled
    // window when it is raised.
    KIPLATFORM::UI::ReparentModal( this );

    setVisible( true );

    m_qmodal_showing = true;

    QEventLoop event_loop;

    m_qmodal_loop = &event_loop;

    event_loop.exec();

    m_qmodal_showing = false;

    if( parent )
        parent->setFocus();

    return result();
}


void DIALOG_SHIM::PrepareForModalSubDialog()
{
    if( m_qmodal_parent_disabler )
        m_qmodal_parent_disabler->SuspendForTrueModal();
}


void DIALOG_SHIM::CleanupAfterModalSubDialog()
{
    if( m_qmodal_parent_disabler )
        m_qmodal_parent_disabler->ResumeAfterTrueModal();
}


void DIALOG_SHIM::EndQuasiModal( int retCode )
{
    // Hook up validator and transfer data from controls handling so quasi-modal dialogs
    // handle validation in the same way as other dialogs.
    if( ( retCode == QDialog::Accepted ) && ( !Validate() || !TransferDataFromWindow() ) )
        return;

    setResult( retCode );

    if( !IsQuasiModal() )
    {
        Q_ASSERT_X( false, "EndQuasiModal", "Either DIALOG_SHIM::EndQuasiModal was called twice, or ShowQuasiModal wasn't called" );
        return;
    }

    TearDownQuasiModal();

    if( m_qmodal_loop )
    {
        if( m_qmodal_loop->isRunning() )
            m_qmodal_loop->exit( 0 );
        else
            m_qmodal_loop->exit( 0 );
    }

    delete m_qmodal_parent_disabler;
    m_qmodal_parent_disabler = nullptr;

    setVisible( false );
}


void DIALOG_SHIM::closeEvent( QCloseEvent* aEvent )
{
    if( IsQuasiModal() )
    {
        EndQuasiModal( QDialog::Rejected );
        return;
    }

    // This is mandatory to allow QDialog::closeEvent() to be called.
    aEvent->accept();
}


void DIALOG_SHIM::OnButton( int id )
{
    if( IsQuasiModal() )
    {
        if( id == QDialog::Accepted )
        {
            EndQuasiModal( id );
        }
        else if( id == 1 )
        {
            // Dialogs that provide Apply buttons should make sure data is valid before
            // allowing a transfer, as there is no other way to indicate failure
            // (i.e. the dialog can't refuse to close as it might with OK, because it
            // isn't closing anyway)
            if( Validate() )
            {
                ignore_unused( TransferDataFromWindow() );
            }
        }
        else if( id == QDialog::Rejected )
        {
            EndQuasiModal( QDialog::Rejected );
        }
        else // not a standard button
        {
            return;
        }

        return;
    }

}


bool DIALOG_SHIM::eventFilter( QObject* watched, QEvent* event )
{
    if( event->type() == QEvent::FocusIn && !m_isClosing )
    {
        if( QLineEdit* textCtrl = qobject_cast<QLineEdit*>( watched ) )
            m_beforeEditValues[ textCtrl ] = textCtrl->text();
        else if( QTextEdit* textEdit = qobject_cast<QTextEdit*>( watched ) )
            m_beforeEditValues[ textEdit ] = textEdit->toPlainText();
    }
    return QDialog::eventFilter( watched, event );
}

void DIALOG_SHIM::onChildSetFocus( QFocusEvent* aEvent )
{
    // When setting focus to a text control reset the before-edit value.

    if( !m_isClosing )
    {
        QWidget* widget = qobject_cast<QWidget*>( sender() );
        if( QLineEdit* textCtrl = qobject_cast<QLineEdit*>( widget ) )
            m_beforeEditValues[ textCtrl ] = textCtrl->text();
        else if( QTextEdit* textEdit = qobject_cast<QTextEdit*>( widget ) )
            m_beforeEditValues[ textEdit ] = textEdit->toPlainText();
    }

    aEvent->accept();
}


void DIALOG_SHIM::keyPressEvent( QKeyEvent* aEvt )
{
    if( aEvt->key() == Qt::Key_U && aEvt->modifiers() == Qt::ControlModifier )
    {
        if( m_parentFrame )
        {
            m_parentFrame->ToggleUserUnits();
            return;
        }
    }
    // shift-return (Mac default) or Ctrl-Return (GTK) for new line input
    else if( ( aEvt->key() == Qt::Key_Return || aEvt->key() == Qt::Key_Enter ) && aEvt->modifiers() & Qt::ShiftModifier )
    {
        QWidget* eventSource = qobject_cast<QWidget*>( focusWidget() );

        if( QLineEdit* textCtrl = qobject_cast<QLineEdit*>( eventSource ) )
        {
#if defined( Q_OS_MAC ) || defined( Q_OS_WIN )
            QString eol = "\r\n";
#else
            QString eol = "\n";
#endif

            int pos = textCtrl->cursorPosition();
            textCtrl->insert( eol );
            textCtrl->setCursorPosition( pos + eol.length() );
            return;
        }
        else if( QTextEdit* textEdit = qobject_cast<QTextEdit*>( eventSource ) )
        {
            QString eol = "\n";

            QTextCursor cursor = textEdit->textCursor();
            int pos = cursor.position();
            cursor.insertText( eol );
            cursor.setPosition( pos + eol.length() );
            textEdit->setTextCursor( cursor );
            return;
        }
        return;
    }
    // command-return (Mac default) or Ctrl-Return (GTK) for OK
    else if( ( aEvt->key() == Qt::Key_Return || aEvt->key() == Qt::Key_Enter ) && aEvt->modifiers() & Qt::ControlModifier )
    {
        OnButton( QDialog::Accepted );
        return;
    }
    else if( aEvt->key() == Qt::Key_Tab && !(aEvt->modifiers() & Qt::ControlModifier) )
    {
        QWidget* currentWindow = focusWidget();
        int       currentIdx = -1;
        int       delta = aEvt->modifiers() & Qt::ShiftModifier ? -1 : 1;

        auto advance =
                [&]( int& idx )
                {
                    // Wrap-around modulus
                    int size = (int) m_tabOrder.size();
                    idx = ( ( idx + delta ) % size + size ) % size;
                };

        for( size_t i = 0; i < m_tabOrder.size(); ++i )
        {
            if( m_tabOrder[i] == currentWindow )
            {
                currentIdx = (int) i;
                break;
            }
        }

        if( currentIdx >= 0 )
        {
            advance( currentIdx );

#ifdef __APPLE__
            while( qobject_cast<QLineEdit*>( m_tabOrder[ currentIdx ] ) == nullptr &&
                   qobject_cast<QTextEdit*>( m_tabOrder[ currentIdx ] ) == nullptr )
                advance( currentIdx );
#endif

            m_tabOrder[ currentIdx ]->setFocus();
            return;
        }
    }
    else if( aEvt->key() == Qt::Key_Escape )
    {
        QWidget* eventSource = qobject_cast<QWidget*>( focusWidget() );

        if( QLineEdit* textCtrl = qobject_cast<QLineEdit*>( eventSource ) )
        {
            // First escape after an edit cancels edit
            if( textCtrl->text() != m_beforeEditValues[ textCtrl ] )
            {
                textCtrl->setText( m_beforeEditValues[ textCtrl ] );
                textCtrl->selectAll();
                return;
            }
        }
        else if( QTextEdit* textEdit = qobject_cast<QTextEdit*>( eventSource ) )
        {
            // First escape after an edit cancels edit
            if( textEdit->toPlainText() != m_beforeEditValues[ textEdit ] )
            {
                textEdit->setPlainText( m_beforeEditValues[ textEdit ] );
                textEdit->selectAll();
                return;
            }
        }
    }

    QDialog::keyPressEvent( aEvt );
}


static void recursiveDescent( QLayout* aLayout, std::map<int, QString>& aLabels )
{
    QDialogButtonBox* buttonBox = qobject_cast<QDialogButtonBox*>( aLayout->parentWidget() );

    auto setupButton =
            [&]( QPushButton* aButton, int buttonId )
            {
                if( aLabels.find( buttonId ) != aLabels.end() )
                {
                    aButton->setText( aLabels[ buttonId ] );
                }
                else
                {
                    switch( buttonId )
                    {
                    case 0:  aButton->setText( _( "&OK" ) );     break;
                    case 1:  aButton->setText( _( "&Cancel" ) ); break;
                    case 2:  aButton->setText( _( "&Yes" ) );    break;
                    case 3:  aButton->setText( _( "&No" ) );     break;
                    case 4:  aButton->setText( _( "&Apply" ) );  break;
                    case 5:  aButton->setText( _( "&Save" ) );   break;
                    case 6:  aButton->setText( _( "&Help" ) );   break;
                    case 7:  aButton->setText( _( "&Help" ) );   break;
                    }
                }
            };

    if( buttonBox )
    {
        QList<QAbstractButton*> buttons = buttonBox->buttons();
        for( QAbstractButton* button : buttons )
        {
            QDialogButtonBox::StandardButton role = buttonBox->standardButton( button );
            if( QPushButton* pushButton = qobject_cast<QPushButton*>( button ) )
            {
                setupButton( pushButton, static_cast<int>( role ) );
                if( role == QDialogButtonBox::Ok )
                    pushButton->setDefault( true );
            }
        }
    }

    for( int i = 0; i < aLayout->count(); ++i )
    {
        QLayoutItem* item = aLayout->itemAt( i );
        if( item && item->layout() )
            recursiveDescent( item->layout(), aLabels );
    }
}


void DIALOG_SHIM::SetupStandardButtons( std::map<int, QString> aLabels )
{
    if( layout() )
        recursiveDescent( layout(), aLabels );
}
