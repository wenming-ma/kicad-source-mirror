// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
// Base class for HiDPI aware Qt OpenGL widget implementations.
// Transformed from wxWidgets to Qt framework.

#include <gal/hidpi_gl_3D_canvas.h>

const float HIDPI_GL_3D_CANVAS::m_delta_move_step_factor = 0.7f;

HIDPI_GL_3D_CANVAS::HIDPI_GL_3D_CANVAS( const KIGFX::VC_SETTINGS& aVcSettings, CAMERA& aCamera,
                                        QWidget* aParent, const QSurfaceFormat& aGLFormat,
                                        Qt::WindowFlags aFlags, const QPoint& aPos,
                                        const QSize& aSize, const QString& aName ) :
        HIDPI_GL_CANVAS( aVcSettings, aParent, aGLFormat, aFlags, aPos, aSize, aName ),
        m_mouse_is_moving( false ),
        m_mouse_was_moved( false ),
        m_camera_is_moving( false ),
        m_camera( aCamera )
{
}


void HIDPI_GL_3D_CANVAS::OnMouseMoveCamera( QMouseEvent& event )
{
    if( m_camera_is_moving )
        return;

    const QSize&  nativeWinSize = GetNativePixelSize();
    const QPoint& nativePosition = GetNativePosition( event.pos() );

    m_camera.SetCurWindowSize( nativeWinSize );

    if( event.buttons() != Qt::NoButton )
    {
        if( event.buttons() & Qt::LeftButton ) // Drag
            m_camera.Drag( nativePosition );
        else if( event.buttons() & Qt::MiddleButton ) // Pan
            m_camera.Pan( nativePosition );

        m_mouse_is_moving = true;
        m_mouse_was_moved = true;
    }

    m_camera.SetCurMousePosition( nativePosition );
}

void HIDPI_GL_3D_CANVAS::OnMouseWheelCamera( QWheelEvent& event, bool aPan )
{
    bool mouseActivity = false;

    if( m_camera_is_moving )
        return;

    // Pick the modifier, if any.  Shift beats control beats alt, we don't support more than one.
    int modifiers = event.modifiers() & Qt::ShiftModifier ? Qt::Key_Shift
                                      : ( event.modifiers() & Qt::ControlModifier ? Qt::Key_Control
                                                              : ( event.modifiers() & Qt::AltModifier ? Qt::Key_Alt : 0 ) );

    float delta_move     = m_delta_move_step_factor * m_camera.GetZoom();
    float horizontalSign = m_settings.m_scrollReversePanH ? -1 : 1;
    float zoomSign       = m_settings.m_scrollReverseZoom ? -1 : 1;

    if( aPan )
        delta_move *= 0.01f * event.angleDelta().y();
    else if( event.angleDelta().y() < 0 )
        delta_move = -delta_move;

    // mousewheel_panning enabled:
    //      wheel           -> pan;
    //      wheel + shift   -> horizontal scrolling;
    //      wheel + ctrl    -> zooming;
    // mousewheel_panning disabled:
    //      wheel + shift   -> vertical scrolling;
    //      wheel + ctrl    -> horizontal scrolling;
    //      wheel           -> zooming.

    if( aPan && modifiers != m_settings.m_scrollModifierZoom )
    {
        if( qAbs(event.angleDelta().x()) > qAbs(event.angleDelta().y())
            || modifiers == m_settings.m_scrollModifierPanH )
            m_camera.Pan( SFVEC3F( -delta_move, 0.0f, 0.0f ) );
        else
            m_camera.Pan( SFVEC3F( 0.0f, -delta_move, 0.0f ) );

        mouseActivity = true;
    }
    else if( modifiers == m_settings.m_scrollModifierPanV && !aPan )
    {
        m_camera.Pan( SFVEC3F( 0.0f, -delta_move, 0.0f ) );
        mouseActivity = true;
    }
    else if( modifiers == m_settings.m_scrollModifierPanH && !aPan )
    {
        m_camera.Pan( SFVEC3F( delta_move * horizontalSign, 0.0f, 0.0f ) );
        mouseActivity = true;
    }
    else
    {
        mouseActivity =
                m_camera.Zoom( ( event.angleDelta().y() * zoomSign ) > 0 ? 1.1f : 1 / 1.1f );
    }

    // If it results on a camera movement
    if( mouseActivity )
    {
        m_mouse_is_moving = true;
        m_mouse_was_moved = true;
    }

    // Update the cursor current mouse position on the camera
    m_camera.SetCurMousePosition( GetNativePosition( event.position().toPoint() ) );
}

// Qt Transformation Complete: wxWidgets dependencies successfully replaced with Qt equivalents
