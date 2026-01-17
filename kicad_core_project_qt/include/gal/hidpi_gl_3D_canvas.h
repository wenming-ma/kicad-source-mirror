
#ifndef HIDPI_GL_3D_CANVAS_H
#define HIDPI_GL_3D_CANVAS_H

#include <gal/gal.h>
#include <atomic>
#include <gal/3d/camera.h>
#include <gal/hidpi_gl_canvas.h>
#include <QImage>
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QWidget>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QSurfaceFormat>

// Provides basic 3D controls ( zoom, rotate, translate, ... )
class GAL_API HIDPI_GL_3D_CANVAS : public HIDPI_GL_CANVAS
{
public:
    // Qt OpenGL widget constructor

    HIDPI_GL_3D_CANVAS( const KIGFX::VC_SETTINGS& aVcSettings, CAMERA& aCamera,
                        QWidget* aParent, const QSurfaceFormat& aGLFormat,
                        Qt::WindowFlags aFlags = Qt::WindowFlags(), const QPoint& aPos = QPoint(),
                        const QSize& aSize = QSize(), const QString& aName = QString() );

    bool m_mouse_is_moving; // Mouse activity is in progress
    bool m_mouse_was_moved;
    bool m_camera_is_moving; // Camera animation is ongoing

    CAMERA&            m_camera;
    static const float m_delta_move_step_factor; // Step factor to used with cursor on
                                                 // relation to the current zoom

    /**
     * Get the canvas camera.
     */
    CAMERA* GetCamera() { return &m_camera; }

    void OnMouseMoveCamera( QMouseEvent& event );
    void OnMouseWheelCamera( QWheelEvent& event, bool aPan );
};


#endif // HIDPI_GL_3D_CANVAS_H
