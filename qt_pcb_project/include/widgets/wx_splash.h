// QT_TRANSFORMATION_COMPLETED
#ifndef KICAD_WX_SPLASH_H
#define KICAD_WX_SPLASH_H

#include <QSplashScreen>
#include <QPixmap>
#include <QWidget>
#include <QEvent>
#include <Qt>

class WX_SPLASH : public QSplashScreen
{
    Q_OBJECT

public:
    WX_SPLASH() : QSplashScreen() {}

    WX_SPLASH( const QPixmap& aBitmap, Qt::WindowFlags aSplashStyle, int aMilliseconds, QWidget* aParent = nullptr ) :
            QSplashScreen( aParent, aBitmap, aSplashStyle )
    {
        if( aMilliseconds > 0 )
            showMessage( QString(), Qt::AlignCenter, Qt::white );
    }

    // Nullify the virtual in the parent which attempts to close the splash on any input
    virtual bool event( QEvent* event ) override 
    { 
        // Filter out mouse and key events to prevent auto-close
        if( event->type() == QEvent::MouseButtonPress || 
            event->type() == QEvent::KeyPress )
            return true;
        
        return QSplashScreen::event( event ); 
    }
};

#endif