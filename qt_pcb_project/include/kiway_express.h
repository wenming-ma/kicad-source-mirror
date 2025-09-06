
#ifndef KIWAY_EXPRESS_H_
#define KIWAY_EXPRESS_H_

#include <frame_type.h>
#include <mail_type.h>
#include <kicommon.h>
#include <QEvent>
#include <QString>


class KICOMMON_API KIWAY_EXPRESS : public QEvent
{
public:
    FRAME_T Dest() { return m_destination; }

    MAIL_T Command()
    {
        return m_command;
    }

    std::string&  GetPayload() { return m_payload; }
    void SetPayload( const std::string& aPayload ) { m_payload = aPayload; }

    KIWAY_EXPRESS* Clone() const { return new KIWAY_EXPRESS( *this ); }

    //KIWAY_EXPRESS() {}

    KIWAY_EXPRESS( FRAME_T aDestination, MAIL_T aCommand, std::string& aPayload,
                   QWidget* aSource = nullptr );

    KIWAY_EXPRESS( const KIWAY_EXPRESS& anOther );

    /// The QEvent::Type argument to QEvent() and identifies an event class
    static const int QT_EVENT_ID;


private:
    FRAME_T         m_destination;    // could have been a bitmap indicating multiple recipients.
    MAIL_T          m_command;        // command associated with this message.
    std::string&    m_payload;        // very often s-expression text, but not always.
};



#endif  // KIWAY_EXPRESS_H_
