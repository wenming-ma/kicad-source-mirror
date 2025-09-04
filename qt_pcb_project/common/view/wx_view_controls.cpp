
#include <pgm_base.h>
#include <core/profile.h>
#include <view/view.h>
#include <view/wx_view_controls.h>
#include <view/zoom_controller.h>
#include <gal/graphics_abstraction_layer.h>
#include <tool/tool_dispatcher.h>
#include <trace_helpers.h>
#include <settings/common_settings.h>
#include <math/util.h>      // for KiROUND
#include <geometry/geometry_utils.h>
#include <widgets/ui_common.h>
#include <class_draw_panel_gal.h>
#include <eda_draw_frame.h>
#include <kiway.h>
#include <kiplatform/ui.h>
#include <QDebug>
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollEvent>
#include <QEnterEvent>
#include <QPoint>
#include <QSize>
#include <QCoreApplication>
#include <QString>

#ifdef _WIN32
   #define USE_MOUSE_CAPTURE
#endif

using namespace KIGFX;

const QEvent::Type WX_VIEW_CONTROLS::EVT_REFRESH_MOUSE = static_cast<QEvent::Type>(QEvent::User + 1);


static std::unique_ptr<ZOOM_CONTROLLER> GetZoomControllerForPlatform( bool aAcceleration )
{
#ifdef __APPLE__
    return std::make_unique<CONSTANT_ZOOM_CONTROLLER>( CONSTANT_ZOOM_CONTROLLER::MAC_SCALE );
#elif defined(__linux__)
    return std::make_unique<CONSTANT_ZOOM_CONTROLLER>( CONSTANT_ZOOM_CONTROLLER::GTK3_SCALE );
#else
    if( aAcceleration )
        return std::make_unique<ACCELERATING_ZOOM_CONTROLLER>();
    else
        return std::make_unique<CONSTANT_ZOOM_CONTROLLER>( CONSTANT_ZOOM_CONTROLLER::MSW_SCALE );
#endif
}


WX_VIEW_CONTROLS::WX_VIEW_CONTROLS( VIEW* aView, EDA_DRAW_PANEL_GAL* aParentPanel ) :
        VIEW_CONTROLS( aView ), m_state( IDLE ), m_parentPanel( aParentPanel ),
        m_scrollScale( 1.0, 1.0 ), m_cursorPos( 0, 0 ), m_updateCursor( true ),
        m_infinitePanWorks( false ), m_gestureLastZoomFactor( 1.0 )
{
    LoadSettings();

    m_MotionEventCounter = std::make_unique<PROF_COUNTER>( "Mouse motion events" );

    // Qt event connections will be handled in the parent panel
    // Qt event connections will be handled in the parent panel
    // Qt event connections will be handled in the parent panel

    // Qt event connections will be handled in the parent panel
    // Qt mouse capture handling will be different

    // Qt gesture handling will be different

    m_cursorWarped = false;

    m_panTimer.setSingleShot(false);
    connect(&m_panTimer, &QTimer::timeout, this, &WX_VIEW_CONTROLS::onTimer);

    m_settings.m_lastKeyboardCursorPositionValid = false;
    m_settings.m_lastKeyboardCursorPosition = { 0.0, 0.0 };
    m_settings.m_lastKeyboardCursorCommand = 0;
}


WX_VIEW_CONTROLS::~WX_VIEW_CONTROLS()
{
#if defined USE_MOUSE_CAPTURE
    if( m_parentPanel->HasCapture() )
        m_parentPanel->ReleaseMouse();
#endif
}


void WX_VIEW_CONTROLS::LoadSettings()
{
    COMMON_SETTINGS* cfg = Pgm().GetCommonSettings();

    m_settings.m_warpCursor            = cfg->m_Input.center_on_zoom;
    m_settings.m_focusFollowSchPcb     = cfg->m_Input.focus_follow_sch_pcb;
    m_settings.m_autoPanSettingEnabled = cfg->m_Input.auto_pan;
    m_settings.m_autoPanAcceleration   = cfg->m_Input.auto_pan_acceleration;
    m_settings.m_horizontalPan         = cfg->m_Input.horizontal_pan;
    m_settings.m_zoomAcceleration      = cfg->m_Input.zoom_acceleration;
    m_settings.m_zoomSpeed             = cfg->m_Input.zoom_speed;
    m_settings.m_zoomSpeedAuto         = cfg->m_Input.zoom_speed_auto;
    m_settings.m_scrollModifierZoom    = cfg->m_Input.scroll_modifier_zoom;
    m_settings.m_scrollModifierPanH    = cfg->m_Input.scroll_modifier_pan_h;
    m_settings.m_scrollModifierPanV    = cfg->m_Input.scroll_modifier_pan_v;
    m_settings.m_dragLeft              = cfg->m_Input.drag_left;
    m_settings.m_dragMiddle            = cfg->m_Input.drag_middle;
    m_settings.m_dragRight             = cfg->m_Input.drag_right;
    m_settings.m_scrollReverseZoom     = cfg->m_Input.reverse_scroll_zoom;
    m_settings.m_scrollReversePanH     = cfg->m_Input.reverse_scroll_pan_h;

    m_zoomController.reset();

    if( cfg->m_Input.zoom_speed_auto )
    {
        m_zoomController = GetZoomControllerForPlatform( cfg->m_Input.zoom_acceleration );
    }
    else
    {
        if( cfg->m_Input.zoom_acceleration )
        {
            m_zoomController =
                    std::make_unique<ACCELERATING_ZOOM_CONTROLLER>( cfg->m_Input.zoom_speed );
        }
        else
        {
            double scale = CONSTANT_ZOOM_CONTROLLER::MANUAL_SCALE_FACTOR * cfg->m_Input.zoom_speed;

            m_zoomController = std::make_unique<CONSTANT_ZOOM_CONTROLLER>( scale );
        }
    }
}


void WX_VIEW_CONTROLS::onMotion( QMouseEvent& aEvent )
{
    ( *m_MotionEventCounter )++;

    QPoint mouseRel = m_parentPanel->mapFromGlobal( KIPLATFORM::UI::GetMousePosition() );

    bool     isAutoPanning = false;
    int      x = mouseRel.x();
    int      y = mouseRel.y();
    VECTOR2D mousePos( x, y );

    if( m_settings.m_focusFollowSchPcb )
    {
        if( EDA_DRAW_FRAME* frame = m_parentPanel->GetParentEDAFrame() )
        {
            KIWAY_PLAYER* otherFrame = nullptr;

            if( frame->IsType( FRAME_PCB_EDITOR ) )
            {
                otherFrame = frame->Kiway().Player( FRAME_SCH, false );
            }
            else if( frame->IsType( FRAME_SCH ) )
            {
                otherFrame = frame->Kiway().Player( FRAME_PCB_EDITOR, false );
            }

            if( otherFrame && KIPLATFORM::UI::IsWindowActive( otherFrame )
                && !KIPLATFORM::UI::IsWindowActive( frame ) )
            {
                frame->Raise();
            }
        }
    }

    if( m_state != DRAG_PANNING && m_state != DRAG_ZOOMING )
        handleCursorCapture( x, y );

    if( m_settings.m_autoPanEnabled && m_settings.m_autoPanSettingEnabled )
        isAutoPanning = handleAutoPanning( aEvent );

    if( !isAutoPanning && aEvent.Dragging() )
    {
        if( m_state == DRAG_PANNING )
        {
            static bool justWarped = false;
            int warpX = 0;
            int warpY = 0;
            QSize parentSize = m_parentPanel->size();

            if( x < 0 )
            {
                warpX = parentSize.x;
            }
            else if(x >= parentSize.width() )
            {
                warpX = -parentSize.width();
            }

            if( y < 0 )
            {
                warpY = parentSize.height();
            }
            else if( y >= parentSize.height() )
            {
                warpY = -parentSize.height();
            }

            if( !justWarped )
            {
                VECTOR2D d = m_dragStartPoint - mousePos;
                m_dragStartPoint = mousePos;
                VECTOR2D delta = m_view->ToWorld( d, false );
                m_view->SetCenter( m_view->GetCenter() + delta );
                aEvent.accept();
            }

            if( warpX || warpY )
            {
                if( !justWarped )
                {
                    if( m_infinitePanWorks
                        && KIPLATFORM::UI::WarpPointer( m_parentPanel, x + warpX, y + warpY ) )
                    {
                        m_dragStartPoint += VECTOR2D( warpX, warpY );
                        justWarped = true;
                    }
                }
                else
                {
                    justWarped = false;
                }
            }
            else
            {
                justWarped = false;
            }
        }
        else if( m_state == DRAG_ZOOMING )
        {
            static bool justWarped = false;
            int warpY = 0;
            QSize parentSize = m_parentPanel->size();

            if( y < 0 )
            {
                warpY = parentSize.height();
            }
            else if( y >= parentSize.height() )
            {
                warpY = -parentSize.height();
            }

            if( !justWarped )
            {
                VECTOR2D d = m_dragStartPoint - mousePos;
                m_dragStartPoint = mousePos;

                double scale = exp( d.y * m_settings.m_zoomSpeed * 0.001 );


                m_view->SetScale( m_view->GetScale() * scale, m_view->ToWorld( m_zoomStartPoint ) );
                aEvent.accept();
            }

            if( warpY )
            {
                if( !justWarped )
                {
                    KIPLATFORM::UI::WarpPointer( m_parentPanel, x, y + warpY );
                    m_dragStartPoint += VECTOR2D( 0, warpY );
                    justWarped = true;
                }
                else
                    justWarped = false;
            }
            else
            {
                justWarped = false;
            }
        }
    }

    if( m_updateCursor )        // do not update the cursor position if it was explicitly set
        m_cursorPos = GetClampedCoords( m_view->ToWorld( mousePos ) );
    else
        m_updateCursor = true;

    aEvent.ignore();
}


void WX_VIEW_CONTROLS::onWheel( QWheelEvent& aEvent )
{
    const double wheelPanSpeed = 0.001;
    const int    axis = aEvent.orientation();

    if( axis == Qt::Horizontal && !m_settings.m_horizontalPan )
        return;

    int nMods = 0;
    int modifiers = 0;
    Qt::KeyboardModifiers qtModifiers = aEvent.modifiers();

    if( qtModifiers & Qt::ShiftModifier )
    {
        nMods += 1;
        modifiers = Qt::Key_Shift;
    }

    if( qtModifiers & Qt::ControlModifier )
    {
        nMods += 1;
        modifiers = modifiers == 0 ? Qt::Key_Control : modifiers;
    }

    if( qtModifiers & Qt::AltModifier )
    {
        nMods += 1;
        modifiers = modifiers == 0 ? Qt::Key_Alt : modifiers;
    }

    // Zero or one modifier is view control
    if( nMods <= 1 )
    {
        if( modifiers == m_settings.m_scrollModifierZoom && axis == Qt::Vertical )
        {
            const int rotation =
                    aEvent.angleDelta().y() * ( m_settings.m_scrollReverseZoom ? -1 : 1 );
            const double zoomScale = m_zoomController->GetScaleForRotation( rotation );

            if( IsCursorWarpingEnabled() )
            {
                CenterOnCursor();
                m_view->SetScale( m_view->GetScale() * zoomScale );
            }
            else
            {
                const VECTOR2D anchor = m_view->ToWorld( VECTOR2D( aEvent.position().x(), aEvent.position().y() ) );
                m_view->SetScale( m_view->GetScale() * zoomScale, anchor );
            }

            // Refresh the zoom level and mouse position on message panel
            // (mouse position has not changed, only the zoom level has changed):
            refreshMouse( true );
        }
        else
        {
            // Scrolling
            VECTOR2D scrollVec = m_view->ToWorld( m_view->GetScreenPixelSize(), false )
                                 * ( (double) aEvent.angleDelta().y() * wheelPanSpeed );
            double scrollX = 0.0;
            double scrollY = 0.0;
            bool   hReverse = false;

            if( axis != Qt::Horizontal )
                hReverse = m_settings.m_scrollReversePanH;

            if( axis == Qt::Horizontal || modifiers == m_settings.m_scrollModifierPanH )
            {
                if( hReverse )
                    scrollX = scrollVec.x;
                else
                    scrollX = ( axis == Qt::Horizontal ) ? scrollVec.x : -scrollVec.x;
            }
            else
            {
                scrollY = -scrollVec.y;
            }

            VECTOR2D delta( scrollX, scrollY );

            m_view->SetCenter( m_view->GetCenter() + delta );
            refreshMouse( true );
        }

    }
    else
    {
        // When we have multiple mods, forward it for tool handling
        aEvent.ignore();
    }
}


void WX_VIEW_CONTROLS::onMagnify( QMouseEvent& aEvent )
{
    VECTOR2D anchor = m_view->ToWorld( VECTOR2D( aEvent.position().x(), aEvent.position().y() ) );
    m_view->SetScale( m_view->GetScale() * 1.1f, anchor );

    aEvent.ignore();
}


void WX_VIEW_CONTROLS::setState( STATE aNewState )
{
    m_state = aNewState;
}


void WX_VIEW_CONTROLS::onButton( QMouseEvent& aEvent )
{
    switch( m_state )
    {
    case IDLE:
    case AUTO_PANNING:
        if( ( aEvent.button() == Qt::MiddleButton && aEvent.type() == QEvent::MouseButtonPress && m_settings.m_dragMiddle == MOUSE_DRAG_ACTION::PAN ) ||
            ( aEvent.button() == Qt::RightButton && aEvent.type() == QEvent::MouseButtonPress && m_settings.m_dragRight == MOUSE_DRAG_ACTION::PAN ) )
        {
            m_dragStartPoint = VECTOR2D( aEvent.position().x(), aEvent.position().y() );
            setState( DRAG_PANNING );
            m_infinitePanWorks = KIPLATFORM::UI::InfiniteDragPrepareWindow( m_parentPanel );

            // Qt mouse capture handling
        }
        else if( ( aEvent.button() == Qt::MiddleButton && aEvent.type() == QEvent::MouseButtonPress && m_settings.m_dragMiddle == MOUSE_DRAG_ACTION::ZOOM ) ||
                 ( aEvent.button() == Qt::RightButton && aEvent.type() == QEvent::MouseButtonPress && m_settings.m_dragRight == MOUSE_DRAG_ACTION::ZOOM ) )
        {
            m_dragStartPoint   = VECTOR2D( aEvent.position().x(), aEvent.position().y() );
            m_zoomStartPoint = m_dragStartPoint;
            setState( DRAG_ZOOMING );

            // Qt mouse capture handling
        }

        if( aEvent.button() == Qt::LeftButton && aEvent.type() == QEvent::MouseButtonRelease )
            setState( IDLE );     // Stop autopanning when user release left mouse button

        break;

    case DRAG_ZOOMING:
    case DRAG_PANNING:
        if( aEvent.type() == QEvent::MouseButtonRelease && 
             (aEvent.button() == Qt::MiddleButton || aEvent.button() == Qt::LeftButton || aEvent.button() == Qt::RightButton) )
        {
            setState( IDLE );
            KIPLATFORM::UI::InfiniteDragReleaseWindow();

            // Qt mouse capture release
        }

        break;
    }

    aEvent.ignore();
}


void WX_VIEW_CONTROLS::onEnter( QEnterEvent& aEvent )
{
    if( KIUI::IsInputControlFocused() )
    {
        return;
    }

#if defined( _WIN32 ) || defined( __linux__ )
    // Win32 and some *nix WMs transmit mouse move and wheel events to all controls below the
    // mouse regardless of focus.  Forcing the focus here will cause the EDA FRAMES to immediately
    // become the top level active window.
    if( m_parentPanel->GetParent() != nullptr )
    {
        // this assumes the parent panel's parent is the eda window
        if( KIPLATFORM::UI::IsWindowActive( m_parentPanel->GetParent() ) )
        {
            m_parentPanel->setFocus();
        }
    }
#else
    m_parentPanel->setFocus();
#endif
}


void WX_VIEW_CONTROLS::onLeave( QEvent& aEvent )
{
#if !defined USE_MOUSE_CAPTURE
    // onMotion( aEvent );  // Qt event handling differs
#endif
}


void WX_VIEW_CONTROLS::onCaptureLost( QEvent& aEvent )
{
    // Qt mouse capture flag handling
}


void WX_VIEW_CONTROLS::onTimer()
{
    switch( m_state )
    {
    case AUTO_PANNING:
    {
        if( !m_settings.m_autoPanEnabled )
        {
            setState( IDLE );
            return;
        }

        // Qt handles mouse tracking differently

        if( !m_parentPanel->hasFocus() && !m_parentPanel->StatusPopupHasFocus() )
        {
            setState( IDLE );
            return;
        }

        double borderSize = std::min( m_settings.m_autoPanMargin * m_view->GetScreenPixelSize().x,
                                      m_settings.m_autoPanMargin * m_view->GetScreenPixelSize().y );

        VECTOR2D dir( m_panDirection );

        float accel = 0.5f + ( m_settings.m_autoPanAcceleration / 5.0f );


        if( dir.EuclideanNorm() >= borderSize )
            dir = dir.Resize( borderSize * accel );
        else if( dir.EuclideanNorm() > borderSize / 2 )
            dir = dir.Resize( borderSize );

        dir = m_view->ToWorld( dir, false );
        m_view->SetCenter( m_view->GetCenter() + dir );

        refreshMouse( true );

        m_panTimer.start(static_cast<int>(250.0 / 60.0));
    }
    break;

    case IDLE:
    case DRAG_PANNING:
    case DRAG_ZOOMING:
        break;
    }
}


void WX_VIEW_CONTROLS::onZoomGesture( QEvent& aEvent )
{
    // Qt gesture handling implementation needed

    refreshMouse( true );
}


void WX_VIEW_CONTROLS::onPanGesture( QEvent& aEvent )
{
    // Qt pan gesture handling implementation needed

    refreshMouse( true );
}


void WX_VIEW_CONTROLS::onScroll( QScrollEvent& aEvent )
{
    const double linePanDelta = 0.05;
    const double pagePanDelta = 0.5;

    int type = aEvent.type();
    int dir = aEvent.orientation();

    if( type == QEvent::ScrollPrepare )
    {
        auto center = m_view->GetCenter();
        const auto& boundary = m_view->GetBoundary();

        const double xstart = ( m_view->IsMirroredX() ?
                                boundary.GetRight() : boundary.GetLeft() );
        const double xdelta = ( m_view->IsMirroredX() ? -1 : 1 );

        if( dir == Qt::Horizontal )
            center.x = xstart + xdelta * ( aEvent.contentPos().x() / m_scrollScale.x );
        else
            center.y = boundary.GetTop() + aEvent.contentPos().y() / m_scrollScale.y;

        m_view->SetCenter( center );
    }
    else if( type == QEvent::Scroll )
    {
    }
    else
    {
        double dist = 0;

        if( type == QEvent::ScrollPrepare )
        {
            dist = pagePanDelta;
        }
        else if( type == QEvent::Scroll )
        {
            dist = -pagePanDelta;
        }
        else if( false )
        {
            dist = linePanDelta;
        }
        else if( false )
        {
            dist = -linePanDelta;
        }
        else
        {
            Q_ASSERT( false );  // Unhandled event type
        }

        VECTOR2D scroll = m_view->ToWorld( m_view->GetScreenPixelSize(), false ) * dist;

        double scrollX = 0.0;
        double scrollY = 0.0;

        if ( dir == Qt::Horizontal )
            scrollX = -scroll.x;
        else
            scrollY = -scroll.y;

        VECTOR2D delta( scrollX, scrollY );

        m_view->SetCenter( m_view->GetCenter() + delta );
    }

    m_parentPanel->update();
}


void WX_VIEW_CONTROLS::CaptureCursor( bool aEnabled )
{
    // Qt mouse capture implementation differs
    VIEW_CONTROLS::CaptureCursor( aEnabled );
}


void WX_VIEW_CONTROLS::CancelDrag()
{
    if( m_state == DRAG_PANNING || m_state == DRAG_ZOOMING )
    {
        setState( IDLE );

        // Qt mouse capture release
    }
}


VECTOR2D WX_VIEW_CONTROLS::GetMousePosition( bool aWorldCoordinates ) const
{
    QPoint msp = getMouseScreenPosition();
    VECTOR2D screenPos( msp.x(), msp.y() );

    return aWorldCoordinates ? GetClampedCoords( m_view->ToWorld( screenPos ) ) : screenPos;
}


VECTOR2D WX_VIEW_CONTROLS::GetRawCursorPosition( bool aEnableSnapping ) const
{
    GAL* gal = m_view->GetGAL();

    if( aEnableSnapping && gal->GetGridSnapping() )
    {
        return gal->GetGridPoint( m_cursorPos );
    }
    else
    {
        return m_cursorPos;
    }
}


VECTOR2D WX_VIEW_CONTROLS::GetCursorPosition( bool aEnableSnapping ) const
{
    if( m_settings.m_forceCursorPosition )
    {
        return m_settings.m_forcedPosition;
    }
    else
    {
        return GetClampedCoords( GetRawCursorPosition( aEnableSnapping ) );
    }
}


void WX_VIEW_CONTROLS::SetCursorPosition( const VECTOR2D& aPosition, bool aWarpView,
                                          bool aTriggeredByArrows, long aArrowCommand )
{
    m_updateCursor = false;

    VECTOR2D clampedPosition = GetClampedCoords( aPosition );

    if( aTriggeredByArrows )
    {
        m_settings.m_lastKeyboardCursorPositionValid = true;
        m_settings.m_lastKeyboardCursorPosition = clampedPosition;
        m_settings.m_lastKeyboardCursorCommand = aArrowCommand;
        m_cursorWarped = false;
    }
    else
    {
        m_settings.m_lastKeyboardCursorPositionValid = false;
        m_settings.m_lastKeyboardCursorPosition = { 0.0, 0.0 };
        m_settings.m_lastKeyboardCursorCommand = 0;
        m_cursorWarped = true;
    }

    WarpMouseCursor( clampedPosition, true, aWarpView );
    m_cursorPos = clampedPosition;
}


void WX_VIEW_CONTROLS::SetCrossHairCursorPosition( const VECTOR2D& aPosition,
                                                   bool aWarpView = true )
{
    m_updateCursor = false;

    VECTOR2D clampedPosition = GetClampedCoords( aPosition );

    const VECTOR2I& screenSize = m_view->GetGAL()->GetScreenPixelSize();
    BOX2I           screen( VECTOR2I( 0, 0 ), screenSize );
    VECTOR2D        screenPos = m_view->ToScreen( clampedPosition );

    if( aWarpView && !screen.Contains( screenPos ) )
        m_view->SetCenter( clampedPosition );

    m_cursorPos = clampedPosition;
}


void WX_VIEW_CONTROLS::WarpMouseCursor( const VECTOR2D& aPosition, bool aWorldCoordinates,
                                        bool aWarpView )
{
    if( aWorldCoordinates )
    {
        const VECTOR2I& screenSize = m_view->GetGAL()->GetScreenPixelSize();
        BOX2I           screen( VECTOR2I( 0, 0 ), screenSize );
        VECTOR2D        clampedPosition = GetClampedCoords( aPosition );
        VECTOR2D        screenPos = m_view->ToScreen( clampedPosition );

        if( !screen.Contains( screenPos ) )
        {
            if( aWarpView )
            {
                m_view->SetCenter( clampedPosition );
                KIPLATFORM::UI::WarpPointer( m_parentPanel, screenSize.x / 2, screenSize.y / 2 );
            }
        }
        else
        {
            KIPLATFORM::UI::WarpPointer( m_parentPanel, screenPos.x, screenPos.y );
        }
    }
    else
    {
        KIPLATFORM::UI::WarpPointer( m_parentPanel, aPosition.x, aPosition.y );
    }

    // If we are not refreshing because of mouse movement, don't set the modifiers because we
    // are refreshing for keyboard movement, which uses the same modifiers for other actions
    refreshMouse( m_updateCursor );
}


void WX_VIEW_CONTROLS::CenterOnCursor()
{
    const VECTOR2I& screenSize = m_view->GetGAL()->GetScreenPixelSize();
    VECTOR2D screenCenter( screenSize / 2 );

    if( GetMousePosition( false ) != screenCenter )
    {
        VECTOR2D newCenter = GetCursorPosition();

        if( KIPLATFORM::UI::WarpPointer( m_parentPanel, screenCenter.x, screenCenter.y ) )
        {
            m_view->SetCenter( newCenter );
            m_dragStartPoint = screenCenter;
        }
    }
}


void WX_VIEW_CONTROLS::PinCursorInsideNonAutoscrollArea( bool aWarpMouseCursor )
{
    int border = std::min( m_settings.m_autoPanMargin * m_view->GetScreenPixelSize().x,
                           m_settings.m_autoPanMargin * m_view->GetScreenPixelSize().y );
    border += 2;

    VECTOR2D topLeft( border, border );
    VECTOR2D botRight( m_view->GetScreenPixelSize().x - border,
                       m_view->GetScreenPixelSize().y - border );

    topLeft = m_view->ToWorld( topLeft );
    botRight = m_view->ToWorld( botRight );

    VECTOR2D pos = GetMousePosition( true );

    if( pos.x < topLeft.x )
        pos.x = topLeft.x;
    else if( pos.x > botRight.x )
        pos.x = botRight.x;

    if( pos.y < topLeft.y )
        pos.y = topLeft.y;
    else if( pos.y > botRight.y )
        pos.y = botRight.y;

    SetCursorPosition( pos, false, false, 0 );

    if( aWarpMouseCursor )
        WarpMouseCursor( pos, true );
}


bool WX_VIEW_CONTROLS::handleAutoPanning( const QMouseEvent& aEvent )
{
    VECTOR2I p( aEvent.position().x(), aEvent.position().y() );
    VECTOR2I pKey( m_view->ToScreen(m_settings.m_lastKeyboardCursorPosition ) );

    if( m_cursorWarped || ( m_settings.m_lastKeyboardCursorPositionValid && p == pKey ) )
    {

        m_cursorWarped = false;
        return true;
    }

    m_cursorWarped = false;

    // Compute areas where autopanning is active
    int borderStart = std::min( m_settings.m_autoPanMargin * m_view->GetScreenPixelSize().x,
                                m_settings.m_autoPanMargin * m_view->GetScreenPixelSize().y );
    borderStart = std::max( borderStart, 2 );
    int borderEndX = m_view->GetScreenPixelSize().x - borderStart;
    int borderEndY = m_view->GetScreenPixelSize().y - borderStart;

    if( p.x < borderStart )
        m_panDirection.x = -( borderStart - p.x );
    else if( p.x > borderEndX )
        m_panDirection.x = ( p.x - borderEndX );
    else
        m_panDirection.x = 0;

    if( p.y < borderStart )
        m_panDirection.y = -( borderStart - p.y );
    else if( p.y > borderEndY )
        m_panDirection.y = ( p.y - borderEndY );
    else
        m_panDirection.y = 0;

    bool borderHit = ( m_panDirection.x != 0 || m_panDirection.y != 0 );

    switch( m_state )
    {
    case AUTO_PANNING:
        if( !borderHit )
        {
            m_panTimer.stop();
            setState( IDLE );

            return false;
        }

        return true;

    case IDLE:
        if( borderHit )
        {
            setState( AUTO_PANNING );
            m_panTimer.start( static_cast<int>( 250.0 / 60.0 ) );

            return true;
        }

        return false;

    case DRAG_PANNING:
    case DRAG_ZOOMING:
        return false;
    }

    Q_ASSERT( false );  // This line should never be reached

    return false;
}


void WX_VIEW_CONTROLS::handleCursorCapture( int x, int y )
{
    if( m_settings.m_cursorCaptured )
    {
        bool warp = false;
        QSize parentSize = m_parentPanel->size();

        if( x < 0 )
        {
            x = 0;
            warp = true;
        }
        else if( x >= parentSize.width() )
        {
            x = parentSize.width() - 1;
            warp = true;
        }

        if( y < 0 )
        {
            y = 0;
            warp = true;
        }
        else if( y >= parentSize.height() )
        {
            y = parentSize.height() - 1;
            warp = true;
        }

        if( warp )
            KIPLATFORM::UI::WarpPointer( m_parentPanel, x, y );
    }
}


void WX_VIEW_CONTROLS::refreshMouse( bool aSetModifiers )
{
    QMouseEvent* moveEvent = new QMouseEvent( static_cast<QEvent::Type>(EVT_REFRESH_MOUSE), 
                                              QPointF(0,0), Qt::NoButton, Qt::NoButton, Qt::NoModifier );
    QPoint msp = getMouseScreenPosition();

    // Qt modifier state will be handled differently

    m_cursorPos = GetClampedCoords( m_view->ToWorld( VECTOR2D( msp.x(), msp.y() ) ) );
    QCoreApplication::postEvent( m_parentPanel, moveEvent );
}


QPoint WX_VIEW_CONTROLS::getMouseScreenPosition() const
{
    QPoint msp = KIPLATFORM::UI::GetMousePosition();
    msp = m_parentPanel->mapFromGlobal( msp );
    return msp;
}


void WX_VIEW_CONTROLS::UpdateScrollbars()
{
    const BOX2D viewport = m_view->GetViewport();
    const BOX2D& boundary = m_view->GetBoundary();

    m_scrollScale.x = 2e3 / viewport.GetWidth();    // TODO it does not have to be updated so often
    m_scrollScale.y = 2e3 / viewport.GetHeight();
    VECTOR2I newScroll( ( viewport.Centre().x - boundary.GetLeft() ) * m_scrollScale.x,
                        ( viewport.Centre().y - boundary.GetTop() ) * m_scrollScale.y );

    // We add the width of the scroll bar thumb to the range because the scroll range is given by
    // the full bar while the position is given by the left/top position of the thumb
    VECTOR2I newRange( m_scrollScale.x * boundary.GetWidth() +
                       20,  // Default thumb size
                       m_scrollScale.y * boundary.GetHeight() +
                       20 );  // Default thumb size

    // Flip scroll direction in flipped view
    if( m_view->IsMirroredX() )
        newScroll.x = ( boundary.GetRight() - viewport.Centre().x ) * m_scrollScale.x;

    // Adjust scrollbars only if it is needed. Otherwise there are cases when canvas is continuously
    // refreshed (Windows)
    if( m_scrollPos != newScroll )
    {
        // Qt scrollbar implementation
        m_scrollPos = newScroll;

        refreshMouse( false );
    }
}


void WX_VIEW_CONTROLS::ForceCursorPosition( bool aEnabled, const VECTOR2D& aPosition )
{
    VECTOR2D clampedPosition = GetClampedCoords( aPosition );

    m_settings.m_forceCursorPosition = aEnabled;
    m_settings.m_forcedPosition = clampedPosition;
}
