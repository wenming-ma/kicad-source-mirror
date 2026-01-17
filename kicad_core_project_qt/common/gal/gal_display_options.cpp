
#include <gal/gal_display_options.h>
#include <settings/app_settings.h>
#include <settings/common_settings.h>

#include <QDebug>

#include <config_map.h>
#include <dpi_scaling.h>

using namespace KIGFX;


static const char* traceGalDispOpts = "KICAD_GAL_DISPLAY_OPTIONS";


GAL_DISPLAY_OPTIONS::GAL_DISPLAY_OPTIONS()
    : gl_antialiasing_mode( OPENGL_ANTIALIASING_MODE::NONE ),
      cairo_antialiasing_mode( CAIRO_ANTIALIASING_MODE::NONE ),
      m_gridStyle( GRID_STYLE::DOTS ),
      m_gridSnapping( GRID_SNAPPING::ALWAYS ),
      m_gridLineWidth( 1.0 ),
      m_gridMinSpacing( 10.0 ),
      m_axesEnabled( false ),
      m_fullscreenCursor( false ),
      m_forceDisplayCursor( false ),
      m_scaleFactor( DPI_SCALING::GetDefaultScaleFactor() )
{
}


void GAL_DISPLAY_OPTIONS::NotifyChanged()
{
    qDebug() << traceGalDispOpts << QString( "Change notification" );

    Notify( &GAL_DISPLAY_OPTIONS_OBSERVER::OnGalDisplayOptionsChanged, *this );
}