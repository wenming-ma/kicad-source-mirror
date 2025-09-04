
#include <windows.h>

#include <kiplatform/ui.h>

#include <QWidget>
#include <QWindow>
#include <QApplication>
#include <QPalette>
#include <QSettings>
#include <QCursor>
#include <QPoint>
#include <QSize>
#include <QColor>
#include <QComboBox>


bool KIPLATFORM::UI::IsDarkTheme()
{
    // NOTE: Disabled for now because we can't yet react to dark mode in Windows reasonably:
    // Windows 10 dark mode does not change the values returned by QApplication::palette()
    // so our window backgrounds, text colors, etc will stay in "light mode" until either Qt
    // implements something or we apply a custom theme ourselves.
#ifdef NOTYET
    const QString lightModeKey = "AppsUseLightTheme";

    // Note: registry used because there is not yet an official API for this yet.
    // This may stop working on future Windows versions
    QSettings themeKey( "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                        QSettings::NativeFormat );

    if( !themeKey.contains( lightModeKey ) )
        return false;

    bool ok = false;
    long val = themeKey.value( lightModeKey, 1 ).toLongLong( &ok );

    if( !ok )
        return false;

    return ( val == 0 );
#else
    QColor bg = QApplication::palette().color( QPalette::Window );

    // Weighted W3C formula
    double brightness = ( bg.red() / 255.0 ) * 0.299 +
        ( bg.green() / 255.0 ) * 0.587 +
        ( bg.blue() / 255.0 ) * 0.117;

    return brightness < 0.5;
#endif
}


QColor KIPLATFORM::UI::GetDialogBGColour()
{
    return QApplication::palette().color( QPalette::Button );
}


void KIPLATFORM::UI::GetInfoBarColours( QColor& aFGColour, QColor& aBGColour )
{
    aBGColour = QApplication::palette().color( QPalette::ToolTipBase );
    aFGColour = QApplication::palette().color( QPalette::ToolTipText );
}


void KIPLATFORM::UI::ForceFocus( QWidget* aWindow )
{
    aWindow->setFocus();
}


bool KIPLATFORM::UI::IsWindowActive( QWidget* aWindow )
{
    if(! aWindow )
    {
	    return false;
    }

    return ( (HWND)aWindow->winId() == GetForegroundWindow() );
}


void KIPLATFORM::UI::ReparentModal( QWidget* aWindow )
{
    // Not needed on this platform
}


void KIPLATFORM::UI::FixupCancelButtonCmdKeyCollision( QWidget *aWindow )
{
    // Not needed on this platform
}


bool KIPLATFORM::UI::IsStockCursorOk( Qt::CursorShape aCursor )
{
    switch( aCursor )
    {
    case Qt::CrossCursor:
    case Qt::PointingHandCursor:
    case Qt::ArrowCursor:
        return true;
    default:
        return false;
    }
}


void KIPLATFORM::UI::LargeChoiceBoxHack( QComboBox* aChoice )
{
    // Not implemented
}


void KIPLATFORM::UI::EllipsizeChoiceBox( QComboBox* aChoice )
{
    // Not implemented
}


double KIPLATFORM::UI::GetPixelScaleFactor( const QWidget* aWindow )
{
    return aWindow->devicePixelRatioF();
}


double KIPLATFORM::UI::GetContentScaleFactor( const QWidget* aWindow )
{
    return aWindow->logicalDpiX() / 96.0;
}


QSize KIPLATFORM::UI::GetUnobscuredSize( const QWidget* aWindow )
{
    return aWindow->size();
}


void KIPLATFORM::UI::SetOverlayScrolling( const QWidget* aWindow, bool overlay )
{
    // Not implemented
}


bool KIPLATFORM::UI::AllowIconsInMenus()
{
    return true;
}


QPoint KIPLATFORM::UI::GetMousePosition()
{
    return QCursor::pos();
}


bool KIPLATFORM::UI::WarpPointer( QWidget* aWindow, int aX, int aY )
{
    QPoint global = aWindow->mapToGlobal( QPoint( aX, aY ) );
    QCursor::setPos( global );
    return true;
}


void KIPLATFORM::UI::ImmControl( QWidget* aWindow, bool aEnable )
{
    if ( !aEnable )
    {
        ImmAssociateContext( (HWND)aWindow->winId(), NULL );
    }
    else
    {
        ImmAssociateContextEx( (HWND)aWindow->winId(), 0, IACE_DEFAULT );
    }
}


void KIPLATFORM::UI::ImeNotifyCancelComposition( QWidget* aWindow )
{
    const HIMC himc = ImmGetContext( (HWND)aWindow->winId() );
    ImmNotifyIME( himc, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
    ImmReleaseContext( (HWND)aWindow->winId(), himc );
}


bool KIPLATFORM::UI::InfiniteDragPrepareWindow( QWidget* aWindow )
{
    return true;
}


void KIPLATFORM::UI::InfiniteDragReleaseWindow()
{
    // Not needed on this platform
}


void KIPLATFORM::UI::SetFloatLevel( QWidget* aWindow )
{
}