// QT_TRANSFORMATION_COMPLETED

#include <QLoggingCategory>
#include <QDebug>
#include <QString>

class WX_LOG_TRACE_ONLY
{
public:
    WX_LOG_TRACE_ONLY()
    {
    }

protected:
    void DoLogRecord( QtMsgType aLevel, const QString& aMsg )
    {
        // Only forward the message if it is a trace message
        if( aLevel == QtDebugMsg )
            qDebug() << aMsg;
    }
};
