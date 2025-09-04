
#include <dpi_scaling_common.h>

#include <optional>

#include <env_vars.h>
#include <settings/common_settings.h>
#include <kiplatform/ui.h>

#include <QDebug>
#include <QWidget>
#include <QGuiApplication>
#include <QScreen>


const QString traceHiDpi = "KICAD_TRACE_HIGH_DPI";


static std::optional<double> getKiCadConfiguredScale( const COMMON_SETTINGS& aConfig )
{
    std::optional<double> scale;
    double                canvas_scale = aConfig.m_Appearance.canvas_scale;

    if( canvas_scale > 0.0 )
    {
        scale = canvas_scale;
    }

    return scale;
}


static std::optional<double> getEnvironmentScale()
{
    std::optional<double> scale;

#ifdef Q_OS_LINUX
    // Under Linux/GTK, the user can use GDK_SCALE to force the scaling
    scale = ENV_VAR::GetEnvVar<double>( "GDK_SCALE" );
#endif

    return scale;
}


DPI_SCALING_COMMON::DPI_SCALING_COMMON( COMMON_SETTINGS* aConfig, const QWidget* aWindow ) :
        m_config( aConfig ),
        m_window( aWindow )
{
}


double DPI_SCALING_COMMON::GetScaleFactor() const
{
    std::optional<double> val;
    QString               src;

    if( m_config )
    {
        val = getKiCadConfiguredScale( *m_config );
        src = "config";
    }

    if( !val )
    {
        val = getEnvironmentScale();
        src = "env";
    }

    if( !val && m_window )
    {
        // Use Qt's native DPI reporting
        val = KIPLATFORM::UI::GetPixelScaleFactor( m_window );
        src = "platform";
    }

    if( !val )
    {
        // Nothing else we can do, give it a default value
        val = GetDefaultScaleFactor();
        src = "default";
    }

    qDebug() << traceHiDpi << QString( "Scale factor (%1): %2" ).arg( src ).arg( *val );

    return *val;
}


double DPI_SCALING_COMMON::GetContentScaleFactor() const
{
    std::optional<double> val;
    QString               src;

    if( m_config )
    {
        val = getKiCadConfiguredScale( *m_config );
        src = "config";
    }

    if( !val )
    {
        val = getEnvironmentScale();
        src = "env";
    }

    if( !val && m_window )
    {
        // Use Qt's native DPI reporting
        val = KIPLATFORM::UI::GetContentScaleFactor( m_window );
        src = "platform";
    }

    if( !val )
    {
        // Nothing else we can do, give it a default value
        val = GetDefaultScaleFactor();
        src = "default";
    }

    qDebug() << traceHiDpi << QString( "Content scale factor (%1): %2" ).arg( src ).arg( *val );

    return *val;
}


bool DPI_SCALING_COMMON::GetCanvasIsAutoScaled() const
{
    if( m_config == nullptr )
    {
        // No configuration given, so has to be automatic scaling
        return true;
    }

    const bool automatic = getKiCadConfiguredScale( *m_config ) == std::nullopt;
    qDebug() << traceHiDpi << QString( "Scale is automatic: %1" ).arg( automatic );
    return automatic;
}


void DPI_SCALING_COMMON::SetDpiConfig( bool aAuto, double aValue )
{
    Q_ASSERT_X( m_config != nullptr, "SetDpiConfig", "Setting DPI config without a config store." );

    const double value = aAuto ? 0.0 : aValue;

    m_config->m_Appearance.canvas_scale = value;
}
