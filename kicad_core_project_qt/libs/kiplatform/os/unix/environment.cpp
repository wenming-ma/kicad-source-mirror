
#include <glib.h>
#include <gio/gio.h>
#include <kiplatform/environment.h>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QProcess>


void KIPLATFORM::ENV::Init()
{
    // Disable proxy menu in Unity window manager. Only usual menubar works with
    // Qt.  When the proxy menu menubar is enable, some important things for us
    // do not work: menuitems UI events and shortcuts.
    QString wm = qgetenv("XDG_CURRENT_DESKTOP");

    if( !wm.isEmpty() && wm.compare("Unity", Qt::CaseInsensitive) == 0 )
        qputenv("UBUNTU_MENUPROXY", "0");

    // Force the use of X11 backend (or wayland-x11 compatibility layer).  This is
    // required until Qt supports the Wayland compositors properly
    qputenv("GDK_BACKEND", "x11");

    // Set GTK2-style input instead of xinput2.  This disables touchscreen and smooth
    // scrolling.  It's needed to ensure that we are not getting multiple mouse scroll
    // events.
    qputenv("GDK_CORE_DEVICE_EVENTS", "1");
}


bool KIPLATFORM::ENV::MoveToTrash( const QString& aPath, QString& aError )
{
    GError* err   = nullptr;
    GFile*  file  = g_file_new_for_path( aPath.toStdString().c_str() );

    bool retVal = g_file_trash( file, nullptr, &err );

    // Extract the error string if the operation failed
    if( !retVal && err )
        aError = QString::fromUtf8(err->message);

    g_clear_error( &err );
    g_object_unref( file );

    return retVal;
}


bool KIPLATFORM::ENV::IsNetworkPath( const QString& aPath )
{
    // placeholder, we "nerf" behavior if its a network path so return false by default
    return false;
}


QString KIPLATFORM::ENV::GetDocumentsPath()
{
    QString docsPath = QString::fromUtf8(g_get_user_data_dir());

    if( docsPath.isEmpty() )
    {
        QDir fallback(QString::fromUtf8(g_get_home_dir()));

        fallback.cd(".local");
        fallback.cd("share");

        docsPath = fallback.absolutePath();
    }

    return docsPath;
}


QString KIPLATFORM::ENV::GetUserConfigPath()
{
    return QString::fromUtf8(g_get_user_config_dir());
}


QString KIPLATFORM::ENV::GetUserDataPath()
{
    return QString::fromUtf8(g_get_user_data_dir());
}


QString KIPLATFORM::ENV::GetUserLocalDataPath()
{
    return QString::fromUtf8(g_get_user_data_dir());
}


QString KIPLATFORM::ENV::GetUserCachePath()
{
    return QString::fromUtf8(g_get_user_cache_dir());
}


bool KIPLATFORM::ENV::GetSystemProxyConfig( const QString& aURL, PROXY_CONFIG& aCfg )
{
    return false;
}


bool KIPLATFORM::ENV::VerifyFileSignature( const QString& aPath )
{
    return true;
}


QString KIPLATFORM::ENV::GetAppUserModelId()
{
    return QString();
}


void KIPLATFORM::ENV::SetAppDetailsForWindow( QWidget* aWindow, const QString& aRelaunchCommand,
                                              const QString& aRelaunchDisplayName )
{
}


QString KIPLATFORM::ENV::GetCommandLineStr()
{
    return QString();
}


void KIPLATFORM::ENV::AddToRecentDocs( const QString& aPath )
{
}