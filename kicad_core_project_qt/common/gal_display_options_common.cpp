
#include <gal_display_options_common.h>
#include <settings/app_settings.h>
#include <settings/common_settings.h>

#include <QLoggingCategory>
#include <QDebug>

#include <config_map.h>
#include <dpi_scaling_common.h>

using namespace KIGFX;

/**
 * Flag to enable GAL_DISPLAY_OPTIONS logging
 *
 * Use "KICAD_GAL_DISPLAY_OPTIONS" to enable.
 *
 * @ingroup trace_env_vars
 */
Q_LOGGING_CATEGORY(traceGalDispOpts, "KICAD_GAL_DISPLAY_OPTIONS")


static const UTIL::CFG_MAP<KIGFX::GRID_STYLE> gridStyleConfigVals = {
    { KIGFX::GRID_STYLE::DOTS, 0 },
    { KIGFX::GRID_STYLE::LINES, 1 },
    { KIGFX::GRID_STYLE::SMALL_CROSS, 2 },
};

static const UTIL::CFG_MAP<KIGFX::GRID_SNAPPING> gridSnapConfigVals = {
    { KIGFX::GRID_SNAPPING::ALWAYS, 0 },
    { KIGFX::GRID_SNAPPING::WITH_GRID, 1 },
    { KIGFX::GRID_SNAPPING::NEVER, 2 }
};

GAL_DISPLAY_OPTIONS_IMPL::GAL_DISPLAY_OPTIONS_IMPL() :
    GAL_DISPLAY_OPTIONS(),
    m_dpi( { nullptr, nullptr } )
{
}


void GAL_DISPLAY_OPTIONS_IMPL::ReadWindowSettings( WINDOW_SETTINGS& aCfg )
{
    qCDebug(traceGalDispOpts) << "Reading app-specific options";

    m_gridStyle = UTIL::GetValFromConfig( gridStyleConfigVals, aCfg.grid.style );
    m_gridSnapping = UTIL::GetValFromConfig( gridSnapConfigVals, aCfg.grid.snap );
    m_gridLineWidth = aCfg.grid.line_width;
    m_gridMinSpacing = aCfg.grid.min_spacing;
    m_axesEnabled = aCfg.grid.axes_enabled;

    m_fullscreenCursor = aCfg.cursor.fullscreen_cursor;
    m_forceDisplayCursor = aCfg.cursor.always_show_cursor;

    NotifyChanged();
}


void GAL_DISPLAY_OPTIONS_IMPL::ReadCommonConfig( COMMON_SETTINGS& aSettings, QWidget* aWindow )
{
    qCDebug(traceGalDispOpts) << "Reading common config";

    gl_antialiasing_mode =
            static_cast<KIGFX::OPENGL_ANTIALIASING_MODE>( aSettings.m_Graphics.opengl_aa_mode );

    cairo_antialiasing_mode =
            static_cast<KIGFX::CAIRO_ANTIALIASING_MODE>( aSettings.m_Graphics.cairo_aa_mode );

    m_dpi = DPI_SCALING_COMMON( &aSettings, aWindow );
    UpdateScaleFactor();

    NotifyChanged();
}


void GAL_DISPLAY_OPTIONS_IMPL::ReadConfig( COMMON_SETTINGS& aCommonConfig,
                                           WINDOW_SETTINGS& aWindowConfig, QWidget* aWindow )
{
    qCDebug(traceGalDispOpts) << "Reading common and app config";

    ReadWindowSettings( aWindowConfig );

    ReadCommonConfig( aCommonConfig, aWindow );
}


void GAL_DISPLAY_OPTIONS_IMPL::WriteConfig( WINDOW_SETTINGS& aCfg )
{
    qCDebug(traceGalDispOpts) << "Writing window settings";

    aCfg.grid.style = UTIL::GetConfigForVal( gridStyleConfigVals, m_gridStyle );
    aCfg.grid.snap = UTIL::GetConfigForVal( gridSnapConfigVals, m_gridSnapping );
    aCfg.grid.line_width = m_gridLineWidth;
    aCfg.grid.min_spacing = m_gridMinSpacing;
    aCfg.grid.axes_enabled = m_axesEnabled;
    aCfg.cursor.fullscreen_cursor = m_fullscreenCursor;
    aCfg.cursor.always_show_cursor = m_forceDisplayCursor;
}


void GAL_DISPLAY_OPTIONS_IMPL::UpdateScaleFactor()
{
    if( m_scaleFactor != m_dpi.GetScaleFactor() )
    {
        m_scaleFactor = m_dpi.GetScaleFactor();
        NotifyChanged();
    }
}
