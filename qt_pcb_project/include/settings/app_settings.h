// QT_TRANSFORMATION_COMPLETED
#ifndef _APP_SETTINGS_H
#define _APP_SETTINGS_H

#include <gal/color4d.h>
#include <settings/json_settings.h>
#include <settings/grid_settings.h>
#include <QString>
#include <QVector>
#include <QHash>
#include <QSettings>

struct KICOMMON_API CROSS_PROBING_SETTINGS
{
    bool on_selection;
    bool center_on_items;
    bool zoom_to_fit;
    bool auto_highlight;
};

struct KICOMMON_API CURSOR_SETTINGS
{
    bool always_show_cursor;
    bool fullscreen_cursor;
};

enum class ARC_EDIT_MODE
{
    KEEP_CENTER_ADJUST_ANGLE_RADIUS,
    KEEP_ENDPOINTS_OR_START_DIRECTION
};

struct KICOMMON_API WINDOW_STATE
{
    bool maximized;
    int size_x;
    int size_y;
    int pos_x;
    int pos_y;
    unsigned int display;
};

struct KICOMMON_API WINDOW_SETTINGS
{
    WINDOW_STATE state;
    QString mru_path;
    QString perspective;
    QVector<double> zoom_factors;

    CURSOR_SETTINGS cursor;
    GRID_SETTINGS grid;
};

class KICOMMON_API APP_SETTINGS_BASE : public JSON_SETTINGS
{
public:
    struct FIND_REPLACE
    {
        QString         find_string;
        QVector<QString> find_history;
        QString         replace_string;
        QVector<QString> replace_history;

        bool search_and_replace;

        bool match_case;
        int match_mode;
    };

    struct SEARCH_PANE
    {
        enum class SELECTION_ZOOM
        {
            NONE,
            PAN,
            ZOOM,
        };

        SELECTION_ZOOM selection_zoom;
        bool           search_hidden_fields;
    };

    struct GRAPHICS
    {
        int   canvas_type;
        float highlight_factor;
        float select_factor;
    };

    struct COLOR_PICKER
    {
        int default_tab;
    };

    struct LIB_TREE
    {
        QVector<QString> columns;
        QHash<QString, int> column_widths;
        QVector<QString>   open_libs;
    };

    struct PRINTING
    {
        bool             background;
        bool             monochrome;
        double           scale;
        bool             use_theme;
        QString         color_theme;
        bool             title_block;
        QVector<int> layers;
    };

    struct SYSTEM
    {
        bool                  first_run_shown;
        int                   max_undo_items;
        QVector<QString> file_history;
        int                   units;
        int                   last_metric_units;
        int                   last_imperial_units;

        bool                  show_import_issues;
    };

    struct PLUGINS
    {
        QVector<std::pair<QString, bool>> actions;
    };

    APP_SETTINGS_BASE( const std::string& aFilename, int aSchemaVersion );

    virtual ~APP_SETTINGS_BASE() {}

    virtual bool MigrateFromLegacy( QSettings* aCfg ) override;

    const QVector<GRID> DefaultGridSizeList() const;

    const QVector<double> DefaultZoomList() const;

public:
    CROSS_PROBING_SETTINGS m_CrossProbing;

    FIND_REPLACE m_FindReplace;

    GRAPHICS m_Graphics;

    COLOR_PICKER m_ColorPicker;

    LIB_TREE m_LibTree;

    PRINTING m_Printing;

    SEARCH_PANE m_SearchPane;

    SYSTEM m_System;

    PLUGINS m_Plugins;

    WINDOW_SETTINGS m_Window;

    QString m_ColorTheme;

    int m_appSettingsSchemaVersion;

protected:

    virtual std::string getLegacyFrameName() const { return std::string(); }

    void migrateFindReplace( QSettings* aCfg );

    bool migrateWindowConfig( QSettings* aCfg, const std::string& aFrameName,
                              const std::string& aJsonPath );

    void addParamsForWindow( WINDOW_SETTINGS* aWindow, const std::string& aJsonPath,
                             int aDefaultWidth = 0, int aDefaultHeight = 0 );

    bool migrateLibTreeWidth();
};

#endif
