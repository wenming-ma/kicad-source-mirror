#ifndef LIB_LOGGER_H
#define LIB_LOGGER_H

#include <QLoggingCategory>
#include <QObject>

class LIB_LOGGER : public QObject
{
    Q_OBJECT

public:
    LIB_LOGGER();

    ~LIB_LOGGER() override;

    void Activate();

    void Deactivate();

    void Flush();

    void SetHasMessages( bool hasMessages ) { m_bHasMessages = hasMessages; }

private:
    QtMessageHandler m_previousLogger;
    bool   m_activated;
    bool   m_bHasMessages;
};

#endif /* LIB_LOGGER_H */