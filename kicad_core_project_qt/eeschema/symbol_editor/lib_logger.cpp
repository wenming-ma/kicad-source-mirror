

#include <QLoggingCategory>
#include <QDebug>
#include <lib_logger.h>

LIB_LOGGER::LIB_LOGGER() : QObject(), m_previousLogger( nullptr ), m_activated( false ), m_bHasMessages( false )
{
}

LIB_LOGGER::~LIB_LOGGER()
{
    Deactivate();
}

// Static pointer to current active instance for message handler
static LIB_LOGGER* s_activeInstance = nullptr;

void LIB_LOGGER::Activate()
{
    if( !m_activated )
    {
        m_previousLogger = qInstallMessageHandler( nullptr );
        s_activeInstance = this;
        qInstallMessageHandler( []( QtMsgType type, const QMessageLogContext& context, const QString& msg ) {
            if( s_activeInstance )
            {
                s_activeInstance->SetHasMessages( true );
            }
        });
        m_activated = true;
    }
}

void LIB_LOGGER::Deactivate()
{
    if( m_activated )
    {
        Flush();
        m_activated = false;
        qInstallMessageHandler( m_previousLogger );
        s_activeInstance = nullptr;
    }
}


void LIB_LOGGER::Flush()
{
    if( m_bHasMessages )
    {
        qWarning( "Not all symbol libraries could be loaded.  Use the Manage Symbol\n"
                 "Libraries dialog to adjust paths and add or remove libraries." );
        m_bHasMessages = false;
    }
}