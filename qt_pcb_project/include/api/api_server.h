
#ifndef KICAD_API_SERVER_H
#define KICAD_API_SERVER_H

#include <memory>
#include <set>
#include <string>

#include <QObject>
#include <QString>

#include <kicommon.h>

class API_HANDLER;
class KINNG_REQUEST_SERVER;


// Qt-based event handling


class KICOMMON_API KICAD_API_SERVER : public QObject
{
public:
    KICAD_API_SERVER();

    ~KICAD_API_SERVER();

    void Start();

    void Stop();

    bool Running() const;

    void RegisterHandler( API_HANDLER* aHandler );

    void DeregisterHandler( API_HANDLER* aHandler );

    void SetReadyToReply( bool aReady = true ) { m_readyToReply = aReady; }

    std::string SocketPath() const;

    const std::string& Token() const { return m_token; }

private:

    void onApiRequest( std::string* aRequest );

    void handleApiRequest( std::string& requestString );

    void log( const std::string& aOutput );

    std::unique_ptr<KINNG_REQUEST_SERVER> m_server;

    std::set<API_HANDLER*> m_handlers;

    std::string m_token;

    bool m_readyToReply;

    static QString s_logFileName;

    QString m_logFilePath;
};

#endif //KICAD_API_SERVER_H
