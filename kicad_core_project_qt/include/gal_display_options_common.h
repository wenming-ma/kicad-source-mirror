
#ifndef GAL_DISPLAY_OPTIONS_COMMON_H__
#define GAL_DISPLAY_OPTIONS_COMMON_H__

#include <gal/gal_display_options.h>
#include <dpi_scaling_common.h>

class COMMON_SETTINGS;
struct WINDOW_SETTINGS;
class QString;
class QWidget;


class GAL_DISPLAY_OPTIONS_IMPL : public KIGFX::GAL_DISPLAY_OPTIONS
{
public:
    GAL_DISPLAY_OPTIONS_IMPL();

    void ReadWindowSettings( WINDOW_SETTINGS& aCfg );

    void ReadCommonConfig( COMMON_SETTINGS& aCommonSettings, QWidget* aWindow );

    void ReadConfig( COMMON_SETTINGS& aCommonConfig, WINDOW_SETTINGS& aWindowConfig,
                     QWidget* aWindow );

    void WriteConfig( WINDOW_SETTINGS& aCfg );

    void UpdateScaleFactor();

    DPI_SCALING_COMMON m_dpi;
};

#endif
