// Qt transformation completed - wxWidgets to Qt framework migration

#include <config.h>
#include <gestfich.h>
#include <QtCore/QProcess>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStandardPaths>
#include <QtCore/QDebug>

#include <future>
#include <utility>

// #include <api/api_utils.h>
#include <paths.h>
#include <pgm_base.h>
#include <python_manager.h>
#include <thread_pool.h>


class PYTHON_PROCESS : public QProcess
{
public:
    PYTHON_PROCESS( std::function<void(int, const QString&, const QString&)> aCallback ) :
            QProcess(),
            m_callback( std::move( aCallback ) )
    {}

    void OnTerminate( int aPid, int aStatus )
    {
        // Print stdout trace info from the monitor thread
        qDebug().flush();

        if( m_callback )
        {
            QString output, error;
            QByteArray stdoutData = readAllStandardOutput();
            QByteArray stderrData = readAllStandardError();

            if( stdoutData.size() > MAX_OUTPUT_LEN )
                stdoutData = stdoutData.left( MAX_OUTPUT_LEN );
            if( stderrData.size() > MAX_OUTPUT_LEN )
                stderrData = stderrData.left( MAX_OUTPUT_LEN );

            output = QString::fromUtf8( stdoutData );
            error = QString::fromUtf8( stderrData );

            m_callback( aStatus, output, error );
        }
    }

    static constexpr size_t MAX_OUTPUT_LEN = 1024L * 1024L;

private:
    std::function<void(int, const QString&, const QString&)> m_callback;
};


PYTHON_MANAGER::PYTHON_MANAGER( const QString& aInterpreterPath )
{
    QFileInfo path( aInterpreterPath );
    m_interpreterPath = path.absoluteFilePath();
}


long PYTHON_MANAGER::Execute( const std::vector<QString>& aArgs,
        const std::function<void(int, const QString&, const QString&)>& aCallback,
        const QProcessEnvironment* aEnv, bool aSaveOutput )
{
    PYTHON_PROCESS* process = new PYTHON_PROCESS( aCallback );
    // Qt QProcess automatically redirects output

    auto monitor =
        []( PYTHON_PROCESS* aProcess )
        {
            while( aProcess->state() == QProcess::Running )
            {
                if( aProcess->canReadLine() )
                {
                    QByteArray data = aProcess->readLine();
                    QString stdOut = QString::fromUtf8( data ).trimmed();
                    qDebug() << QString("Python: %1").arg( stdOut );
                }
            }
        };

    QString argsStr;
    QStringList args;

    for( const QString& arg : aArgs )
    {
        args.append( arg );
        argsStr += arg + " ";
    }

    qDebug() << QString("Execute: %1 %2").arg( m_interpreterPath, argsStr );

    if( aEnv )
        process->setProcessEnvironment( *aEnv );

    process->start( m_interpreterPath, args );
    long pid = process->processId();

    if( pid == 0 )
    {
        delete process;
        aCallback( -1, QString(), "Process could not be created" );
    }
    else
    {
        // On Windows, if there is a lot of stdout written by the process, this can
        // hang up the wxProcess such that it will never call OnTerminate.  To work
        // around this, we use this monitor thread to just dump the stdout to the
        // trace log, which prevents the hangup.  This flag is provided to keep the
        // old behavior for commands where we need to read the output directly,
        // which is currently only used for detecting the interpreter version.
        // If we need to use the async monitor thread approach and preserve the stdout
        // contents in the future, a more complicated hack might be necessary.
        if( !aSaveOutput )
        {
            thread_pool& tp = GetKiCadThreadPool();
            auto ret = tp.submit( monitor, process );
        }
    }

    return pid;
}


QString PYTHON_MANAGER::FindPythonInterpreter()
{
    // First, attempt to use a Python we distribute with KiCad
#if defined( __WINDOWS__ )
    QString pythonPath = FindKicadFile( "pythonw.exe" );
    QFileInfo pythonExe( pythonPath );

    if( pythonExe.isExecutable() )
        return pythonExe.absoluteFilePath();
#elif defined( __WXMAC__ )
    QDir pythonDir( PATHS::GetOSXKicadDataDir() );
    pythonDir.cdUp();
    QString pythonPath = pythonDir.absolutePath() + "/Frameworks/Python.framework/Versions/Current/bin/python3";
    QFileInfo pythonExe( pythonPath );

    if( pythonExe.isExecutable() )
        return pythonExe.absoluteFilePath();
#else
    QFileInfo pythonExe;
#endif

    // In case one is forced with cmake
    pythonExe = QFileInfo( QString::fromUtf8( PYTHON_EXECUTABLE ) );

    if( pythonExe.isExecutable() )
        return pythonExe.absoluteFilePath();

    // Fall back on finding any Python in the user's path

#ifdef _WIN32
    QProcess process;
    process.start( "where", QStringList() << "pythonw.exe" );
    process.waitForFinished();

    if( process.exitCode() == 0 )
    {
        QStringList output = QString::fromUtf8( process.readAllStandardOutput() ).split( '\n', Qt::SkipEmptyParts );
        if( !output.isEmpty() )
            return output[0];
    }
#else
    QProcess process;
    process.start( "which", QStringList() << "-a" << "python3" );
    process.waitForFinished();

    if( process.exitCode() == 0 )
    {
        QStringList output = QString::fromUtf8( process.readAllStandardOutput() ).split( '\n', Qt::SkipEmptyParts );
        if( !output.isEmpty() )
            return output[0];
    }

    process.start( "which", QStringList() << "-a" << "python" );
    process.waitForFinished();

    if( process.exitCode() == 0 )
    {
        QStringList output = QString::fromUtf8( process.readAllStandardOutput() ).split( '\n', Qt::SkipEmptyParts );
        if( !output.isEmpty() )
            return output[0];
    }
#endif

    return QString();
}


std::optional<QString> PYTHON_MANAGER::GetPythonEnvironment( const QString& aNamespace )
{
    QDir path( PATHS::GetUserCachePath() );
    path.mkdir( "python-environments" );
    path.cd( "python-environments" );
    path.mkdir( aNamespace );
    path.cd( aNamespace );

    if( !PATHS::EnsurePathExists( path.absolutePath() ) )
        return std::nullopt;

    return path.absolutePath();
}


std::optional<QString> PYTHON_MANAGER::GetVirtualPython( const QString& aNamespace )
{
    std::optional<QString> envPath = GetPythonEnvironment( aNamespace );

    if( !envPath )
        return std::nullopt;

    QDir python( *envPath );

#ifdef _WIN32
    QFileInfo pythonExe( python.absoluteFilePath( "Scripts/pythonw.exe" ) );
#else
    QFileInfo pythonExe( python.absoluteFilePath( "bin/python" ) );
#endif

    if( !pythonExe.isExecutable() )
        return std::nullopt;

    return pythonExe.absoluteFilePath();
}
