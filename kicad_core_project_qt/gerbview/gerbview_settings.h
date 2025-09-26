
#ifndef _GERBVIEW_SETTINGS_H
#define _GERBVIEW_SETTINGS_H

#include <settings/app_settings.h>
#include <excellon_defaults.h>
#include <gbr_display_options.h>
#include <QString>
#include <QWidget>

class GERBVIEW_SETTINGS : public APP_SETTINGS_BASE
{
public:
    struct APPEARANCE
    {
        bool show_border_and_titleblock;
        bool show_dcodes;
        bool show_negative_objects;
        QString page_type;
    };

    GERBVIEW_SETTINGS();

    virtual ~GERBVIEW_SETTINGS() {}

    virtual bool MigrateFromLegacy( QSettings* aCfg ) override;

    /**
     * return the Excellon default values to read a drill file
     * @param aNCDefaults is the EXCELLON_DEFAULTS to store these prms
     */
    void GetExcellonDefaults( EXCELLON_DEFAULTS& aNCDefaults )
    {
        aNCDefaults = m_ExcellonDefaults;
    }

public:

    APPEARANCE m_Appearance;

    GBR_DISPLAY_OPTIONS m_Display;

    int m_BoardLayersCount;

    std::vector<std::string> m_DrillFileHistory;

    std::vector<std::string> m_ZipFileHistory;

    std::vector<std::string> m_JobFileHistory;

    /**
     * A list of GERBER_DRAWLAYERS_COUNT length containing a mapping of gerber layers
     * to PCB layers, used when exporting gerbers to a PCB
     */
    std::vector<int> m_GerberToPcbLayerMapping;

    EXCELLON_DEFAULTS m_ExcellonDefaults;

protected:

    virtual std::string getLegacyFrameName() const override { return "GerberFrame"; }

};

#endif
