#include <mutex>
#include <macros.h>
#include <reporter.h>
#include <string_utils.h>
#include <QtCore/QDebug>
#include <QtCore/QLoggingCategory>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QStatusBar>

static const QString traceReporter = "KICAD_REPORTER";

static std::mutex g_logReporterMutex;


REPORTER& REPORTER::Report( const char* aText, SEVERITY aSeverity )
{
    Report( From_UTF8( aText ) );
    return *this;
}


REPORTER& QT_TEXT_EDIT_REPORTER::Report( const QString& aText, SEVERITY aSeverity )
{
    REPORTER::Report( aText, aSeverity );

    if( m_textEdit == nullptr )
    {
        return *this;
    }

    m_textEdit->append( aText + "\n" );
    return *this;
}


REPORTER& QT_STRING_REPORTER::Report( const QString& aText, SEVERITY aSeverity )
{
    REPORTER::Report( aText, aSeverity );

    m_string += aText + "\n";
    return *this;
}


const QString& QT_STRING_REPORTER::GetMessages() const
{
    return m_string;
}


void QT_STRING_REPORTER::Clear()
{
    REPORTER::Clear();
    m_string.clear();
}


REPORTER& NULL_REPORTER::Report( const QString& aText, SEVERITY aSeverity )
{
    return REPORTER::Report( aText, aSeverity );
}


REPORTER& NULL_REPORTER::GetInstance()
{
    static REPORTER* s_nullReporter = nullptr;

    if( !s_nullReporter )
        s_nullReporter = new NULL_REPORTER();

    return *s_nullReporter;
}


REPORTER& CLI_REPORTER::Report( const QString& aMsg, SEVERITY aSeverity )
{
    REPORTER::Report( aMsg, aSeverity );

    FILE* target = stdout;

    if( aSeverity == RPT_SEVERITY_ERROR )
        target = stderr;

    if( aMsg.endsWith( "\n" ) )
        fprintf( target, "%s", aMsg.toUtf8().constData() );
    else
        fprintf( target, "%s\n", aMsg.toUtf8().constData() );

    fflush( target );

    return *this;
}


REPORTER& CLI_REPORTER::GetInstance()
{
    static CLI_REPORTER s_cliReporter;

    return s_cliReporter;
}


REPORTER& STDOUT_REPORTER::Report( const QString& aMsg, SEVERITY aSeverity )
{
    REPORTER::Report( aMsg, aSeverity );

    switch( aSeverity )
    {
    case RPT_SEVERITY_UNDEFINED: std::cout << "SEVERITY_UNDEFINED: "; break;
    case RPT_SEVERITY_INFO:      std::cout << "SEVERITY_INFO: ";      break;
    case RPT_SEVERITY_WARNING:   std::cout << "SEVERITY_WARNING: ";   break;
    case RPT_SEVERITY_ERROR:     std::cout << "SEVERITY_ERROR: ";     break;
    case RPT_SEVERITY_ACTION:    std::cout << "SEVERITY_ACTION: ";    break;
    case RPT_SEVERITY_DEBUG:     std::cout << "SEVERITY_DEBUG: ";    break;
    case RPT_SEVERITY_EXCLUSION:
    case RPT_SEVERITY_IGNORE:    break;
    }

    std::cout << aMsg.toUtf8().constData() << std::endl;

    return *this;
}


REPORTER& STDOUT_REPORTER::GetInstance()
{
    static REPORTER* s_stdoutReporter = nullptr;

    if( !s_stdoutReporter )
        s_stdoutReporter = new STDOUT_REPORTER();

    return *s_stdoutReporter;
}


REPORTER& QTLOG_REPORTER::Report( const QString& aMsg, SEVERITY aSeverity )
{
    REPORTER::Report( aMsg, aSeverity );

    switch( aSeverity )
    {
    case RPT_SEVERITY_ERROR:     qCritical() << aMsg;                  break;
    case RPT_SEVERITY_WARNING:   qWarning() << aMsg;                   break;
    case RPT_SEVERITY_UNDEFINED: qInfo() << aMsg;                      break;
    case RPT_SEVERITY_INFO:      qInfo() << aMsg;                      break;
    case RPT_SEVERITY_ACTION:    qInfo() << aMsg;                      break;
    case RPT_SEVERITY_DEBUG:     qDebug().noquote() << traceReporter << aMsg; break;
    case RPT_SEVERITY_EXCLUSION:                                       break;
    case RPT_SEVERITY_IGNORE:                                          break;
    }

    return *this;
}


REPORTER& QTLOG_REPORTER::GetInstance()
{
    static REPORTER* s_qtLogReporter = nullptr;
    std::lock_guard lock( g_logReporterMutex );

    if( !s_qtLogReporter )
        s_qtLogReporter = new QTLOG_REPORTER();

    return *s_qtLogReporter;
}


REPORTER& REDIRECT_REPORTER::Report( const QString& aText, SEVERITY aSeverity )
{
    REPORTER::Report( aText, aSeverity );

    if( m_redirectTarget )
        m_redirectTarget->Report( aText, aSeverity );

    return *this;
}


REPORTER& STATUSBAR_REPORTER::Report( const QString& aText, SEVERITY aSeverity )
{
    REPORTER::Report( aText, aSeverity );

    if( m_statusBar )
        m_statusBar->showMessage( aText );

    return *this;
}
