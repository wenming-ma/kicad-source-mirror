#include <QElapsedTimer>
#include <QDateTime>

#include "time_limit.h"

namespace PNS {

TIME_LIMIT::TIME_LIMIT( int aMilliseconds ) :
    m_limitMs( aMilliseconds )
{
    Restart();
}


TIME_LIMIT::~TIME_LIMIT()
{}


bool TIME_LIMIT::Expired() const
{
    return ( QDateTime::currentMSecsSinceEpoch() - m_startTics ) >= m_limitMs;
}


void TIME_LIMIT::Restart()
{
    m_startTics = QDateTime::currentMSecsSinceEpoch();
}


void TIME_LIMIT::Set( int aMilliseconds )
{
    m_limitMs = aMilliseconds;
}

}
