
#include <fmt/format.h>
#include <QApplication>
#include <QDateTime>
#include <QEvent>
#include <QStandardPaths>
#include <QFileInfo>
#include <QFile>
#include <QCoreApplication>

#include <advanced_config.h>
#include <api/api_handler.h>
#include <api/api_server.h>
#include <kiid.h>
#include <kinng.h>
#include <paths.h>
#include <pgm_base.h>
#include <settings/common_settings.h>
#include <string_utils.h>

#ifdef __UNIX__
#include <sys/file.h>
#endif


QString KICAD_API_SERVER::s_logFileName = "api.log";


KICAD_API_SERVER::KICAD_API_SERVER() :
        QObject(),
        m_token( KIID().AsStdString() ),
        m_readyToReply( false )
{
    if( !Pgm().GetCommonSettings()->m_Api.enable_server )
    {
        // Server disabled by user preferences
        return;
    }

    Start();
}


KICAD_API_SERVER::~KICAD_API_SERVER()
{
    Stop();
}


void KICAD_API_SERVER::Start()
{
    if( Running() )
        return;

    QString socketPath;
#ifdef __APPLE__
    socketPath = "/tmp/kicad/api.sock";
#else
    socketPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/kicad/api.sock";
#endif

    QString socketDir = socketPath.left(socketPath.lastIndexOf('/'));
    if( !PATHS::EnsurePathExists( socketDir.toStdString() ) )
    {
        // Socket path could not be created
        return;
    }

#ifndef __WINDOWS__
    // We use non-abstract sockets because macOS and some other non-Linux platforms don't support
    // abstract sockets, which means there might be an old socket to unlink.  In order to try to
    // recover this, we lock a file (which will be unlocked on process exit) and if we get the lock,
    // we know the old socket is orphaned and can be removed.
    QString lockFilePath = socketDir + "/api.lock";

    int lockFile = open( lockFilePath.toLocal8Bit().constData(), O_RDONLY | O_CREAT, 0600 );

    if( lockFile >= 0 && flock( lockFile, LOCK_EX | LOCK_NB ) == 0 )
    {
        QFileInfo socketFileInfo(socketPath);
        if( socketFileInfo.exists() )
        {
            // Cleaning up stale socket path
            QFile::remove( socketPath );
        }
    }
#endif

    QFileInfo socketFileInfo(socketPath);
    if( socketFileInfo.exists() )
    {
        socketPath = socketDir + QString("/api-%1.sock").arg(QCoreApplication::applicationPid());
        
        QFileInfo pidSocketFileInfo(socketPath);
        if( pidSocketFileInfo.exists() )
        {
            // PID socket path already exists
            return;
        }
    }

    m_server = std::make_unique<KINNG_REQUEST_SERVER>(
            fmt::format( "ipc://{}", socketPath.toStdString() ) );
    m_server->SetCallback( [&]( std::string* aRequest ) { onApiRequest( aRequest ); } );

    m_logFilePath = QString::fromStdString(PATHS::GetLogsPath()) + "/" + s_logFileName;

    if( ADVANCED_CFG::GetCfg().m_EnableAPILogging )
    {
        PATHS::EnsurePathExists( PATHS::GetLogsPath() );
        log( fmt::format( "--- KiCad API server started at {} ---\n", SocketPath() ) );
    }
}


void KICAD_API_SERVER::Stop()
{
    if( !Running() )
        return;

    m_server->Stop();
    m_server.reset( nullptr );
}


bool KICAD_API_SERVER::Running() const
{
    return m_server && m_server->Running();
}


void KICAD_API_SERVER::RegisterHandler( API_HANDLER* aHandler )
{
    if( !aHandler ) return;
    m_handlers.insert( aHandler );
}


void KICAD_API_SERVER::DeregisterHandler( API_HANDLER* aHandler )
{
    m_handlers.erase( aHandler );
}


std::string KICAD_API_SERVER::SocketPath() const
{
    return m_server ? m_server->SocketPath() : "";
}


void KICAD_API_SERVER::onApiRequest( std::string* aRequest )
{
    if( !m_readyToReply )
    {
        // ApiResponse notHandled;
        // notHandled.mutable_status()->set_status( ApiStatusCode::AS_NOT_READY );
        // notHandled.mutable_status()->set_error_message( "KiCad is not ready to reply" );
        // m_server->Reply( notHandled.SerializeAsString() );
        log( "Got incoming request but was not yet ready to reply." );
        return;
    }

    std::string& requestString = *aRequest;
    handleApiRequest( requestString );
}


void KICAD_API_SERVER::handleApiRequest( std::string& requestString )
{
    
}


void KICAD_API_SERVER::log( const std::string& aOutput )
{
    FILE* fp = fopen( m_logFilePath.toLocal8Bit().constData(), "a" );

    if( !fp )
        return;

    QString out;
    QDateTime now = QDateTime::currentDateTime();
    
    QString logEntry = QString("%1: %2\n").arg(now.toString(Qt::ISODate), QString::fromStdString(aOutput));
    fprintf( fp, "%s", logEntry.toUtf8().constData() );
    fclose( fp );
}
