
#include <condition_variable>
#include <mutex>
#include <thread>

#include <eda_dde.h>
#include <kiway_player.h>
#include <id.h>

#include <QString>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QHostAddress>

static const QString HOSTNAME( "localhost" );

// buffer for read and write data in socket connections
#define IPC_BUF_SIZE 4096
static char client_ipc_buffer[IPC_BUF_SIZE];


void KIWAY_PLAYER::CreateServer( int service, bool local )
{
    if( m_socketServer )
    {
        m_socketServer->close();
        delete m_socketServer;
    }

    m_socketServer = new QTcpServer( this );

    QHostAddress address = local ? QHostAddress::LocalHost : QHostAddress::Any;
    m_socketServer->listen( address, service );

    connect( m_socketServer, &QTcpServer::newConnection, this, &KIWAY_PLAYER::OnSockRequestServer );
}


void KIWAY_PLAYER::OnSockRequest()
{
    QTcpSocket* sock = qobject_cast<QTcpSocket*>( sender() );
    if( !sock )
        return;

    if( sock->bytesAvailable() > 0 )
    {
        QByteArray data = sock->readAll();
        if( data.size() > 0 && data.size() < IPC_BUF_SIZE )
        {
            memcpy( client_ipc_buffer, data.constData(), data.size() );
            client_ipc_buffer[data.size()] = 0;
            ExecuteRemoteCommand( client_ipc_buffer );
        }
    }
}


void KIWAY_PLAYER::OnSockRequestServer()
{
    QTcpServer* server = qobject_cast<QTcpServer*>( sender() );
    if( !server )
        return;

    while( server->hasPendingConnections() )
    {
        QTcpSocket* socket = server->nextPendingConnection();
        if( socket )
        {
            m_sockets.push_back( socket );
            connect( socket, &QTcpSocket::readyRead, this, &KIWAY_PLAYER::OnSockRequest );
            connect( socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater );
        }
    }
}


// Spin up a thread to send messages via a socket.
// No message queuing, if a message is in flight when another is posted with Send(), the
// second is just dropped.  This is a workaround for "non-blocking" sockets not always being
// non-blocking, especially on Windows.  It is kept fairly simple and not exposed to the
// outside world because it should be replaced in a future KiCad version with a real message
// queue of some sort, and unified with the Kiway messaging system.
class ASYNC_SOCKET_HOLDER
{
public:
    ASYNC_SOCKET_HOLDER() :
            m_messageReady( false ),
            m_shutdown( false )
    {
        // Do a dummy Connect so that Qt will set up the socket stuff on the main thread, which is
        // required even if you later make socket connections on another thread.
        QTcpSocket* client = new QTcpSocket;
        client->connectToHost( HOSTNAME, KICAD_PCB_PORT_SERVICE_NUMBER );
        client->waitForConnected( 100 );
        client->close();
        delete client;

        m_thread = std::thread( &ASYNC_SOCKET_HOLDER::worker, this );
    }

    ~ASYNC_SOCKET_HOLDER()
    {
        {
            std::lock_guard<std::mutex> lock( m_mutex );
            m_shutdown = true;
        }

        m_cv.notify_one();

        try
        {
            if( m_thread.joinable() )
                m_thread.join();
        }
        catch( ... )
        {
        }
    }

    // Attempt to send a message if the thread is available.
    // @param aService is the port number (i.e. service) to send to.
    // @param aMessage is the message to send.
    // @return true if the message was queued.
    bool Send( int aService, const std::string& aMessage )
    {
        if( m_messageReady )
            return false;

        std::lock_guard<std::mutex> lock( m_mutex );

        m_message      = std::make_pair( aService, aMessage );
        m_messageReady = true;
        m_cv.notify_one();

        return true;
    }

private:
    // Actual task that sends data to the socket server
    void worker()
    {
        int         port;
        std::string message;

        std::unique_lock<std::mutex> lock( m_mutex );

        while( !m_shutdown )
        {
            m_cv.wait( lock, [&]() { return m_messageReady || m_shutdown; } );

            if( m_shutdown )
                break;

            port    = m_message.first;
            message = m_message.second;

            lock.unlock();

            QTcpSocket* sock_client = new QTcpSocket;

            // Create a connection

            // Mini-tutorial for connectToHost()
            // Qt sockets are asynchronous by default, but we can wait for connection

            sock_client->connectToHost( HOSTNAME, port );
            
            if( sock_client->waitForConnected( 1000 ) )
            {
                sock_client->write( message.c_str(), message.length() );
                sock_client->waitForBytesWritten( 1000 );
            }

            sock_client->close();
            delete sock_client;

            m_messageReady = false;

            lock.lock();
        }
    }

    std::thread                 m_thread;
    std::pair<int, std::string> m_message;
    bool                        m_messageReady;
    mutable std::mutex          m_mutex;
    std::condition_variable     m_cv;
    bool                        m_shutdown;
};


std::unique_ptr<ASYNC_SOCKET_HOLDER> socketHolder = nullptr;


// Used by a client to sent (by a socket connection) a data to a server.
// - Open a Socket Client connection.
// - Send the buffer cmdline.
// - Close the socket connection.
//
// @param aService is the service number for the TC/IP connection.
// @param aMessage is the message to send.
bool SendCommand( int aService, const std::string& aMessage )
{
    if( !socketHolder )
        socketHolder.reset( new ASYNC_SOCKET_HOLDER() );

    return socketHolder->Send( aService, aMessage );
}


void SocketCleanup()
{
    if( socketHolder )
        socketHolder.reset();
}
