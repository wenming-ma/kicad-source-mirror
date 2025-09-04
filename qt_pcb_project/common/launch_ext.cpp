#include <launch_ext.h>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>

bool LaunchExternal( const QString& aPath )
{
#ifdef Q_OS_MAC

    QProcess process;
    QStringList args;
    args << aPath;
    return process.startDetached("open", args);

#else

    QString path( aPath );
    return QDesktopServices::openUrl(QUrl::fromLocalFile(path));

#endif
}
