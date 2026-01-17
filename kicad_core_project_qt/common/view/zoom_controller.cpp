#include <view/zoom_controller.h>

#include <trace_helpers.h>

#include <QtCore/QDebug>

#include <algorithm>

using namespace KIGFX;


class SIMPLE_TIMESTAMPER : public ACCELERATING_ZOOM_CONTROLLER::TIMESTAMP_PROVIDER
{
public:
    ACCELERATING_ZOOM_CONTROLLER::TIME_PT GetTimestamp() override
    {
        return ACCELERATING_ZOOM_CONTROLLER::CLOCK::now();
    }
};


ACCELERATING_ZOOM_CONTROLLER::ACCELERATING_ZOOM_CONTROLLER(
        double aScale, const TIMEOUT& aAccTimeout, TIMESTAMP_PROVIDER* aTimestampProv ) :
        m_accTimeout( aAccTimeout ),
        m_scale( aScale )
{
    if( aTimestampProv )
    {
        m_timestampProv = aTimestampProv;
    }
    else
    {
        m_ownTimestampProv = std::make_unique<SIMPLE_TIMESTAMPER>();
        m_timestampProv = m_ownTimestampProv.get();
    }

    m_prevTimestamp = m_timestampProv->GetTimestamp();
}


double ACCELERATING_ZOOM_CONTROLLER::GetScaleForRotation( int aRotation )
{
    // The minimal step value when changing the current zoom level
    const double minStep = 1.05;

    const auto timestamp = m_timestampProv->GetTimestamp();
    auto       timeDiff = std::chrono::duration_cast<TIMEOUT>( timestamp - m_prevTimestamp );

    m_prevTimestamp = timestamp;

    qDebug() << QString("Rot %1, time diff: %2ms").arg(aRotation).arg((long)timeDiff.count());

    double zoomScale;

    // Set scaling speed depending on scroll wheel event interval
    if( timeDiff < m_accTimeout
        && ( (aRotation > 0) == m_prevRotationPositive ) )
    {
        zoomScale = ( 2.05 * m_scale / 5.0 ) - timeDiff / m_accTimeout;

        // be sure zoomScale value is significant
        zoomScale = std::max( zoomScale, minStep );

        if( aRotation < 0 )
            zoomScale = 1.0 / zoomScale;
    }
    else
    {
        zoomScale = ( aRotation > 0 ) ? minStep : 1 / minStep;
    }
    m_prevRotationPositive = aRotation > 0;

    qDebug() << QString("  Zoom factor: %1").arg(zoomScale);

    return zoomScale;
}

#ifdef __MINGW32__
// For some reason, this is needed to avoid a link issue
// (undefined reference to ACCELERATING_ZOOM_CONTROLLER::DEFAULT_TIMEOUT
// and GAL_API CONSTANT_ZOOM_CONTROLLER::MSW_SCALE)
constexpr ACCELERATING_ZOOM_CONTROLLER::TIMEOUT ACCELERATING_ZOOM_CONTROLLER::DEFAULT_TIMEOUT;
constexpr double GAL_API CONSTANT_ZOOM_CONTROLLER::MSW_SCALE;
#endif


CONSTANT_ZOOM_CONTROLLER::CONSTANT_ZOOM_CONTROLLER( double aScale ) : m_scale( aScale )
{
}


double CONSTANT_ZOOM_CONTROLLER::GetScaleForRotation( int aRotation )
{
    qDebug() << QString("Rot %1").arg(aRotation);

    aRotation = ( aRotation > 0 ) ? std::min( aRotation, 100 ) : std::max( aRotation, -100 );

    double dscale = aRotation * m_scale;

    double zoom_scale = ( aRotation > 0 ) ? ( 1 + dscale ) : 1 / ( 1 - dscale );

    qDebug() << QString("  Zoom factor: %1").arg(zoom_scale);

    return zoom_scale;
}
