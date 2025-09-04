
#include "tool/tool_dispatcher.h"

#include <bit>
#include <optional>

#include <QApplication>
#include <QGuiApplication>
#include <QStyleHints>
#include <QTimer>
#include <QWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMenu>
#include <QMenuBar>
#include <QString>
#include <QLoggingCategory>
#include <QDateTime>

#include <core/ignore.h>
#include <macros.h>
#include <trace_helpers.h>
#include <tool/tool_manager.h>
#include <tool/actions.h>
#include <tool/action_manager.h>
#include <tool/action_menu.h>
#include <view/view.h>
#include <view/qt_view_controls.h>
#include <eda_draw_frame.h>
#include <core/kicad_algo.h>

#include <kiplatform/app.h>
#include <kiplatform/ui.h>


struct TOOL_DISPATCHER::BUTTON_STATE
{
    BUTTON_STATE( TOOL_MOUSE_BUTTONS aButton, const QEvent::Type& aDownEvent,
                 const QEvent::Type& aUpEvent, const QEvent::Type& aDblClickEvent ) :
        dragging( false ),
        pressed( false ),
        button( aButton ),
        downEvent( aDownEvent ),
        upEvent( aUpEvent ),
        dblClickEvent( aDblClickEvent )
    {};

    bool dragging;
    bool pressed;
    VECTOR2D dragOrigin;
    VECTOR2D dragOriginScreen;
    VECTOR2D downPosition;
    TOOL_MOUSE_BUTTONS button;

    QEvent::Type downEvent;
    QEvent::Type upEvent;
    QEvent::Type dblClickEvent;

    qint64 downTimestamp;
    void Reset()
    {
        dragging = false;
        pressed = false;
    }

    bool GetState() const
    {
        Qt::MouseButtons mouseButtons = QGuiApplication::mouseButtons();

        switch( button )
        {
        case BUT_LEFT:
            return mouseButtons & Qt::LeftButton;

        case BUT_MIDDLE:
            return mouseButtons & Qt::MiddleButton;

        case BUT_RIGHT:
            return mouseButtons & Qt::RightButton;

        case BUT_AUX1:
            return mouseButtons & Qt::BackButton;

        case BUT_AUX2:
            return mouseButtons & Qt::ForwardButton;

        default:
            Q_ASSERT_X( false, "BUTTON_STATE::GetState", "unknown button" );
            return false;
        }
    }
};


TOOL_DISPATCHER::TOOL_DISPATCHER( TOOL_MANAGER* aToolMgr ) :
    m_toolMgr( aToolMgr )
{
    m_sysDragMinX = QApplication::startDragDistance();
    m_sysDragMinY = QApplication::startDragDistance();

    m_sysDragMinX = m_sysDragMinX != -1 ? m_sysDragMinX : DragDistanceThreshold;
    m_sysDragMinY = m_sysDragMinY != -1 ? m_sysDragMinY : DragDistanceThreshold;

    m_buttons.push_back( new BUTTON_STATE( BUT_LEFT, QEvent::MouseButtonPress,
                         QEvent::MouseButtonRelease, QEvent::MouseButtonDblClick ) );
    m_buttons.push_back( new BUTTON_STATE( BUT_RIGHT, QEvent::MouseButtonPress,
                         QEvent::MouseButtonRelease, QEvent::MouseButtonDblClick ) );
    m_buttons.push_back( new BUTTON_STATE( BUT_MIDDLE, QEvent::MouseButtonPress,
                         QEvent::MouseButtonRelease, QEvent::MouseButtonDblClick ) );
    m_buttons.push_back( new BUTTON_STATE( BUT_AUX1, QEvent::MouseButtonPress,
                         QEvent::MouseButtonRelease, QEvent::MouseButtonDblClick ) );
    m_buttons.push_back( new BUTTON_STATE( BUT_AUX2, QEvent::MouseButtonPress,
                         QEvent::MouseButtonRelease, QEvent::MouseButtonDblClick ) );

    ResetState();
}


TOOL_DISPATCHER::~TOOL_DISPATCHER()
{
    for( BUTTON_STATE* st : m_buttons )
        delete st;
}


void TOOL_DISPATCHER::ResetState()
{
    for( BUTTON_STATE* st : m_buttons )
        st->Reset();
}


KIGFX::VIEW* TOOL_DISPATCHER::getView()
{
    return m_toolMgr->GetView();
}


bool TOOL_DISPATCHER::handleMouseButton( QEvent& aEvent, int aIndex, bool aMotion )
{
    BUTTON_STATE* st = m_buttons[aIndex];
    QEvent::Type type = aEvent.type();
    std::optional<TOOL_EVENT> evt;
    bool isClick = false;

//    bool up = type == st->upEvent;
//    bool down = type == st->downEvent;
    bool up = false, down = false;
    bool dblClick = type == st->dblClickEvent;
    bool state = st->GetState();

    if( !dblClick )
    {
        // Sometimes the dispatcher does not receive mouse button up event, so it stays
        // in the dragging mode even if the mouse button is not held anymore
        if( st->pressed && !state )
            up = true;
        // Don't apply same logic to down events as it kills touchpad tapping
        else if( !st->pressed && type == st->downEvent )
            down = true;
    }

    int mods = decodeModifiers( static_cast<QMouseEvent*>( &aEvent ) );
    int args = st->button | mods;

    if( down )      // Handle mouse button press
    {
        st->downTimestamp = QDateTime::currentMSecsSinceEpoch();

        if( !st->pressed )      // save the drag origin on the first click only
        {
            st->dragOrigin = m_lastMousePos;
            st->dragOriginScreen = m_lastMousePosScreen;
        }

        st->downPosition = m_lastMousePos;
        st->pressed = true;
        evt = TOOL_EVENT( TC_MOUSE, TA_MOUSE_DOWN, args );
    }
    else if( up )   // Handle mouse button release
    {
        st->pressed = false;

        if( st->dragging )
            evt = TOOL_EVENT( TC_MOUSE, TA_MOUSE_UP, args );
        else
            isClick = true;

        if( isClick )
            evt = TOOL_EVENT( TC_MOUSE, TA_MOUSE_CLICK, args );

        st->dragging = false;
    }
    else if( dblClick )
    {
        evt = TOOL_EVENT( TC_MOUSE, TA_MOUSE_DBLCLICK, args );
    }

    if( st->pressed && aMotion )
    {
        if( !st->dragging )
        {
#ifdef Q_OS_MACOS
            if( QDateTime::currentMSecsSinceEpoch() - st->downTimestamp > DragTimeThreshold )
                st->dragging = true;
#endif
            VECTOR2D offset = m_lastMousePosScreen - st->dragOriginScreen;

            if( abs( offset.x ) > m_sysDragMinX || abs( offset.y ) > m_sysDragMinY )
                st->dragging = true;

        }

        if( st->dragging )
        {
            evt = TOOL_EVENT( TC_MOUSE, TA_MOUSE_DRAG, args );
            evt->setMouseDragOrigin( st->dragOrigin );
            evt->setMouseDelta( m_lastMousePos - st->dragOrigin );
        }
    }

    if( evt )
    {
        evt->SetMousePosition( isClick ? st->downPosition : m_lastMousePos );
        m_toolMgr->ProcessEvent( *evt );

        return true;
    }

    return false;
}


bool isKeySpecialCode( int aKeyCode )
{
    // These keys have predefined actions (like move thumbtrack cursor),
    // and we do not want these actions executed
    const std::vector<int> special_keys =
    {
        Qt::Key_PageUp, Qt::Key_PageDown,
        Qt::Key_PageUp, Qt::Key_PageDown
    };

    return alg::contains( special_keys, aKeyCode );
}


static bool isKeyModifierOnly( int aKeyCode )
{
    static std::vector<int> special_keys =
    {
        Qt::Key_Control, Qt::Key_Control, Qt::Key_Shift, Qt::Key_Alt
    };

    return alg::contains( special_keys, aKeyCode );
}


static bool isMouseClick( QEvent::Type type )
{
    return type == QEvent::MouseButtonPress || type == QEvent::MouseButtonRelease || type == QEvent::MouseButtonDblClick;
}


int translateSpecialCode( int aKeyCode )
{
    switch( aKeyCode )
    {
    case Qt::Key_Up: return Qt::Key_Up;
    case Qt::Key_Down: return Qt::Key_Down;
    case Qt::Key_Left: return Qt::Key_Left;
    case Qt::Key_Right: return Qt::Key_Right;
    case Qt::Key_PageUp: return Qt::Key_PageUp;
    case Qt::Key_PageDown: return Qt::Key_PageDown;
    default: break;
    };

    return aKeyCode;
}


std::optional<TOOL_EVENT> TOOL_DISPATCHER::GetToolEvent( QKeyEvent* aKeyEvent, bool* keyIsSpecial )
{
    std::optional<TOOL_EVENT> evt;
    int             key = aKeyEvent->key();
    int             unicode_key = aKeyEvent->text().isEmpty() ? 0 : aKeyEvent->text().at(0).unicode();

    // This wxEVT_CHAR_HOOK event can be ignored: not useful in KiCad
    if( isKeyModifierOnly( key ) )
    {
        aKeyEvent->ignore();
        return evt;
    }

    // Qt logging equivalent removed for simplicity

    // if the key event must be skipped, skip it here if the event is a wxEVT_CHAR_HOOK
    // and do nothing.
    *keyIsSpecial = isKeySpecialCode( key );

    if( aKeyEvent->type() == QEvent::KeyPress )
        key = translateSpecialCode( key );

    int mods = decodeModifiers( aKeyEvent );

    if( mods & MD_CTRL )
    {
        // wxWidgets maps key codes related to Ctrl+letter handled by CHAR_EVT
        // (http://docs.wxwidgets.org/trunk/classwx_key_event.html):
        // char events for ASCII letters in this case carry codes corresponding to the ASCII
        // value of Ctrl-Latter, i.e. 1 for Ctrl-A, 2 for Ctrl-B and so on until 26 for Ctrl-Z.
        // They are remapped here to be more easy to handle in code
        // Note also on OSX wxWidgets has a different behavior and the mapping is made
        // only for ctrl+'A' to ctlr+'Z' (unicode code return 'A' to 'Z').
        // Others OS return WXK_CONTROL_A to WXK_CONTROL_Z, and Ctrl+'M' returns the same code as
        // the return key, so the remapping does not use the unicode key value.
#ifdef Q_OS_MACOS
        if( unicode_key >= 'A' && unicode_key <= 'Z' && key >= Qt::Key_A && key <= Qt::Key_Z )
#else
        ignore_unused( unicode_key );

        if( key >= Qt::Key_A && key <= Qt::Key_Z )
#endif
            key += 'A' - Qt::Key_A;
    }

#ifdef Q_OS_MACOS
    if( mods & MD_ALT )
    {
        switch( aKeyEvent->nativeScanCode() )
        {
        case 0x12: key = '1'; break;
        case 0x13: key = '2'; break;
        case 0x14: key = '3'; break;
        case 0x15: key = '4'; break;
        case 0x16: key = '6'; break;
        case 0x17: key = '5'; break;
        case 0x18: key = '='; break;
        case 0x19: key = '9'; break;
        case 0x1A: key = '7'; break;
        case 0x1B: key = '-'; break;
        case 0x1C: key = '8'; break;
        case 0x1D: key = '0'; break;
        default: ;
        }
    }
#endif

    if( key == Qt::Key_Escape )
        evt = TOOL_EVENT( TC_COMMAND, TA_CANCEL_TOOL, Qt::Key_Escape );
    else
        evt = TOOL_EVENT( TC_KEYBOARD, TA_KEY_PRESSED, key | mods );

    return evt;
}


void TOOL_DISPATCHER::DispatchQtEvent( QEvent& aEvent )
{
    bool            motion = false;
    bool            buttonEvents = false;
    VECTOR2D        pos;
    std::optional<TOOL_EVENT> evt;
    bool            keyIsEscape  = false;  // True if the keypress was the escape key
    bool            keyIsSpecial = false;  // True if the key is a special key code
    QWidget*        focus = QApplication::focusWidget();

    QApplication::processEvents();

    QEvent::Type type = aEvent.type();

    // Sometimes there is no window that has the focus (it happens when another PCB_BASE_FRAME
    // is opened and is iconized on Windows).
    // In this case, give the focus to the parent frame (GAL canvas itself does not accept the
    // focus when iconized for some obscure reason)
    if( focus == nullptr )
    {
        QWidget* holderWindow = dynamic_cast<QWidget*>( m_toolMgr->GetToolHolder() );

#if defined( Q_OS_WIN ) || defined( Q_OS_LINUX )
        if( holderWindow && holderWindow->isActiveWindow() )
#else
        if( holderWindow )
#endif
        {
            holderWindow->setFocus();
        }
    }

    if( isMouseClick( type ) )
    {
        if( m_toolMgr->GetToolHolder() && m_toolMgr->GetToolHolder()->GetToolCanvas() &&
            !m_toolMgr->GetToolHolder()->GetToolCanvas()->hasFocus() )
        {
            m_toolMgr->GetToolHolder()->GetToolCanvas()->setFocus();
        }
    }

    // Mouse handling
    // Note: wxEVT_LEFT_DOWN event must always be skipped.
    if( type == QEvent::MouseMove || type == QEvent::Wheel ||
        type == QEvent::Gesture ||
        isMouseClick( type ) ||
        type == KIGFX::QT_VIEW_CONTROLS::EVT_REFRESH_MOUSE )
    {
        QMouseEvent* me = static_cast<QMouseEvent*>( &aEvent );
        int mods = decodeModifiers( me );

        if( m_toolMgr->GetViewControls() )
        {
            pos = m_toolMgr->GetViewControls()->GetMousePosition();
            m_lastMousePosScreen = m_toolMgr->GetViewControls()->GetMousePosition( false );

            if( pos != m_lastMousePos )
            {
                motion = true;
                m_lastMousePos = pos;
            }
        }

        for( unsigned int i = 0; i < m_buttons.size(); i++ )
            buttonEvents |= handleMouseButton( aEvent, i, motion );

        if( m_toolMgr->GetViewControls() )
        {
            if( !buttonEvents && motion )
            {
                evt = TOOL_EVENT( TC_MOUSE, TA_MOUSE_MOTION, mods );
                evt->SetMousePosition( pos );
            }
        }

        // We only handle wheel events that aren't for the view control.
        // Events with zero or one modifier are reserved for view control.
        // When using WX_VIEW_CONTROLS, these will already be handled, but
        // we still shouldn't consume such events if we get them (e.g. for
        // when WX_VIEW_CONTROLS is not in use, like in the 3D viewer)
        if( !evt && type == QEvent::Wheel )
        {
            const unsigned modBits =
                    static_cast<unsigned>( mods ) & ( MD_CTRL | MD_ALT | MD_SHIFT );
            const bool shouldHandle = std::popcount( modBits ) > 1;

            if( shouldHandle )
            {
                QWheelEvent* we = static_cast<QWheelEvent*>( &aEvent );
                evt = TOOL_EVENT( TC_MOUSE, TA_MOUSE_WHEEL, mods );
                evt->SetParameter<int>( we->angleDelta().y() );
            }
        }
    }
    else if( type == QEvent::KeyPress || type == QEvent::KeyRelease )
    {
        QKeyEvent* ke = static_cast<QKeyEvent*>( &aEvent );

        // Qt logging removed for simplicity

        // Do not process wxEVT_CHAR_HOOK for a shift-modified key, as ACTION_MANAGER::RunHotKey
        // will run the un-shifted key and that's not what we want.  Wait to get the translated
        // key from wxEVT_CHAR.
        // See https://gitlab.com/kicad/code/kicad/-/issues/1809
        if( type == QEvent::KeyPress && ke->modifiers() == Qt::ShiftModifier )
        {
            aEvent.ignore();
            return;
        }

        keyIsEscape = ( ke->key() == Qt::Key_Escape );

        if( KIUI::IsInputControlFocused( focus ) )
        {
            bool enabled = KIUI::IsInputControlEditable( focus );

            // Never process key events for tools when a text entry has focus
            if( enabled )
            {
                aEvent.ignore();
                return;
            }
            else if( ke->modifiers() == Qt::ControlModifier && ke->key() == Qt::Key_C )
            {
                aEvent.ignore();
                return;
            }
        }

        evt = GetToolEvent( ke, &keyIsSpecial );
    }
    else if( type == QEvent::MenubarUpdated || type == QEvent::Show || type == QEvent::Hide )
    {
        QEvent* tmp = &aEvent;

        if( !tmp )
        {
            aEvent.ignore();
            return;
        }

        QEvent& menuEvent = *tmp;

        static ACTION_MENU* currentMenu;

        if( type == QEvent::Show )
        {
            currentMenu = nullptr;

            if( currentMenu )
                currentMenu->OnMenuEvent( menuEvent );
        }
        else if( type == QEvent::MenubarUpdated )
        {
            if( currentMenu )
                currentMenu->OnMenuEvent( menuEvent );
        }
        else if( type == QEvent::Hide )
        {
            if( currentMenu )
                currentMenu->OnMenuEvent( menuEvent );

            currentMenu = nullptr;
        }

        aEvent.ignore();
    }

    bool handled = false;

    if( evt )
    {
        // Qt logging removed for simplicity

        handled = m_toolMgr->ProcessEvent( *evt );

        // Qt logging removed for simplicity
    }

    // pass the event to the GUI, it might still be interested in it
    // Note wxEVT_CHAR_HOOK event is already skipped for special keys not used by KiCad
    // and wxEVT_LEFT_DOWN must be always Skipped.
    //
    // On OS X, key events are always meant to be caught.  An uncaught key event is assumed
    // to be a user input error by OS X (as they are pressing keys in a context where nothing
    // is there to catch the event).  This annoyingly makes OS X beep and/or flash the screen
    // in Pcbnew and the footprint editor any time a hotkey is used.  The correct procedure is
    // to NOT pass wxEVT_CHAR events to the GUI under OS X.
    //
    // On Windows, avoid to call wxEvent::Skip for special keys because some keys
    // (PAGE_UP, PAGE_DOWN) have predefined actions (like move thumbtrack cursor), and we do
    // not want these actions executed (most are handled by KiCad)

    if( !evt || type == QEvent::MouseButtonPress )
        aEvent.ignore();

    // Not handled wxEVT_CHAR must be Skipped (sent to GUI).
    // Otherwise accelerators and shortcuts in main menu or toolbars are not seen.
    // Escape key presses are never skipped by the handler since they correspond to tool cancel
    // events, and if they aren't skipped then they are propagated to other frames (which we
    // don't want).
    if( ( type == QEvent::KeyPress || type == QEvent::KeyRelease )
             && !keyIsSpecial
             && !handled
             && !keyIsEscape )
    {
        aEvent.ignore();
    }

    // Qt logging removed for simplicity
}

//  LocalWords:  EDA CERN CHANGELOG txt Tomasz Wlostowski wxEvent WXK
//  LocalWords:  MERCHANTABILITY bool upEvent downEvent touchpad Ctrl
//  LocalWords:  wxEVENT isKeySpecialCode thumbtrack DispatchWxEvent
//  LocalWords:  NUMPAD PAGEUP PAGEDOWN wxEVT EVT OSX ctrl ctlr kVK
//  LocalWords:  unicode ESC keypress wxTimer iconized autopanning WX
//  LocalWords:  un Wx
