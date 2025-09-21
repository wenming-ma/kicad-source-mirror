
#ifndef HIDPI_GL_CANVAS_H
#define HIDPI_GL_CANVAS_H

#include <view/view_controls.h>
#include <QtOpenGL/QOpenGLWidget>
#include <QtOpenGL/QSurfaceFormat>
#include <QtCore/QString>
#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <gal/gal.h>


// QOpenGLWidget wrapper for HiDPI/Retina support.
// This is a small wrapper class to enable HiDPI/Retina support for QOpenGLWidget.
class GAL_API HIDPI_GL_CANVAS : public QOpenGLWidget
{
public:
    // QOpenGLWidget constructor
    HIDPI_GL_CANVAS( const KIGFX::VC_SETTINGS& aSettings, QWidget* aParent,
                     const QSurfaceFormat& aGLFormat, int aId = -1,
                     const QPoint& aPos = QPoint(), const QSize& aSize = QSize(),
                     Qt::WindowFlags aFlags = Qt::WindowFlags(), const QString& aName = QString() );

    virtual QSize GetNativePixelSize() const;

    // Convert the given point from client coordinates to native pixel coordinates.
    QPoint GetNativePosition( const QPoint& aPoint ) const;

    // Get the current scale factor
    double GetScaleFactor() const;

    void SetVcSettings( const KIGFX::VC_SETTINGS& aVcSettings ) { m_settings = aVcSettings; }

protected:
    // Current VIEW_CONTROLS settings.
    KIGFX::VC_SETTINGS m_settings;
};

#endif // HIDPI_GL_CANVAS_H
