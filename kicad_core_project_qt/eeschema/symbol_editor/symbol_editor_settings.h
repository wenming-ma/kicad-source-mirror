
#pragma once

#include <settings/app_settings.h>
#include <project/sch_project_settings.h>
#include <QString>
#include <QSettings>


class SYMBOL_EDITOR_SETTINGS : public APP_SETTINGS_BASE
{
public:

    struct AUI_PANELS
    {
        int  properties_panel_width;
        float properties_splitter;
        bool show_properties;
    };

    struct DEFAULTS
    {
        int line_width;
        int text_size;
        int pin_length;
        int pin_name_size;
        int pin_num_size;
    };

    struct REPEAT
    {
        int label_delta;
        int pin_step;
    };

    struct DIALOG_IMPORT_GRAPHICS
    {
        bool     interactive_placement;
        QString last_file;
        double   dxf_line_width;
        int      dxf_line_width_units;
        int      origin_units;
        double   origin_x;
        double   origin_y;
        int      dxf_units;
    };

    SYMBOL_EDITOR_SETTINGS();

    virtual ~SYMBOL_EDITOR_SETTINGS() {}

    virtual bool MigrateFromLegacy( QSettings* aLegacyConfig ) override;

    AUI_PANELS m_AuiPanels;

    DEFAULTS m_Defaults;

    REPEAT m_Repeat;

    DIALOG_IMPORT_GRAPHICS m_ImportGraphics;

    bool m_ShowPinElectricalType;
    bool m_ShowHiddenPins;
    bool m_ShowHiddenFields;
    bool m_ShowPinAltIcons;

    ///< When true, dragging an outline edge will drag pins rooted on it
    bool m_dragPinsAlongWithEdges;

    int m_LibWidth;

    int m_LibrarySortMode;

    QString m_EditSymbolVisibleColumns;

    QString m_PinTableVisibleColumns;

    bool m_UseEeschemaColorSettings;

    SCH_SELECTION_FILTER_OPTIONS m_SelectionFilter;

protected:

    virtual std::string getLegacyFrameName() const override { return "LibeditFrame"; }
};
