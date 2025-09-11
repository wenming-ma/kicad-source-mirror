#include <launch_ext.h>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <vector>
#include <string>

bool LaunchExternal( const QString& aPath )
{
#ifdef Q_OS_MAC

    QProcess process;
    std::vector<std::string> argsVec = { aPath.toStdString() };
    QStringList args;
    for( const std::string& arg : argsVec )
        args << QString::fromStdString( arg );
    return process.startDetached("open", args);

#else

    QString path( aPath );
    return QDesktopServices::openUrl(QUrl::fromLocalFile(path));

#endif
}
