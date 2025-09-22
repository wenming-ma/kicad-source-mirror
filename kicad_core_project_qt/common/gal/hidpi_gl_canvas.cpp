// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
#include <gal/hidpi_gl_canvas.h>

#include <dpi_scaling.h>


HIDPI_GL_CANVAS::HIDPI_GL_CANVAS( const KIGFX::VC_SETTINGS& aSettings, QWidget* aParent,
                                  const QSurfaceFormat& aGLFormat, int aId,
                                  const QPoint& aPos, const QSize& aSize,
                                  Qt::WindowFlags aFlags, const QString& aName ) :
        QOpenGLWidget( aParent ),
        m_settings( aSettings )
{
    Q_UNUSED( aId );
    Q_UNUSED( aPos );
    Q_UNUSED( aSize );
    Q_UNUSED( aFlags );
    Q_UNUSED( aName );

    setFormat( aGLFormat );
}


QSize HIDPI_GL_CANVAS::GetNativePixelSize() const
{
    QSize size = QOpenGLWidget::size();

    const double scaleFactor = GetScaleFactor();
    size.setWidth(size.width() * scaleFactor);
    size.setHeight(size.height() * scaleFactor);

    return size;
}


QPoint HIDPI_GL_CANVAS::GetNativePosition( const QPoint& aPoint ) const
{
    QPoint nativePoint = aPoint;

    const double scaleFactor = GetScaleFactor();
    nativePoint.setX(nativePoint.x() * scaleFactor);
    nativePoint.setY(nativePoint.y() * scaleFactor);

    return nativePoint;
}


double HIDPI_GL_CANVAS::GetScaleFactor() const
{
    return GetContentScaleFactor();
}