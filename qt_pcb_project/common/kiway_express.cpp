#include <kiway_express.h>

#include <QWidget>

#if 0
const int KIWAY_EXPRESS::QT_EVENT_ID = QEvent::registerEventType();
#else
const int KIWAY_EXPRESS::QT_EVENT_ID = 30000;
#endif


KIWAY_EXPRESS::KIWAY_EXPRESS( const KIWAY_EXPRESS& anOther ) :
    QEvent( static_cast<QEvent::Type>(QT_EVENT_ID) ),
    m_destination( anOther.m_destination ),
    m_command( anOther.m_command ),
    m_payload( anOther.m_payload )
{
}


KIWAY_EXPRESS::KIWAY_EXPRESS( FRAME_T aDestination, MAIL_T aCommand, std::string& aPayload,
                              QWidget* aSource ) :
    QEvent( static_cast<QEvent::Type>(QT_EVENT_ID) ),
    m_destination( aDestination ),
    m_command( aCommand ),
    m_payload( aPayload )
{
    // Qt events don't have SetEventObject equivalent, source tracking handled differently
}

