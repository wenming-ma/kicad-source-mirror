
#include <class_draw_panel_gal.h>
#include <common.h>
#include <eda_units.h>
#include <layer_ids.h>
#include <pgm_base.h>
#include <settings/app_settings.h>
#include <settings/json_settings_internals.h>
#include <settings/color_settings.h>
#include <settings/common_settings.h>
#include <settings/grid_settings.h>
#include <settings/parameters.h>
#include <zoom_defines.h>

#include <QSettings>


APP_SETTINGS_BASE::APP_SETTINGS_BASE( const std::string& aFilename, int aSchemaVersion ) :
        JSON_SETTINGS( QString::fromStdString( aFilename ), SETTINGS_LOC::USER, aSchemaVersion ),
        m_CrossProbing(),
        m_FindReplace(),
        m_Graphics(),
        m_ColorPicker(),
        m_LibTree(),
        m_Printing(),
        m_SearchPane(),
        m_System(),
        m_Plugins(),
        m_Window(),
        m_appSettingsSchemaVersion( aSchemaVersion )
{
    // Make Coverity happy:
    m_Graphics.canvas_type = EDA_DRAW_PANEL_GAL::GAL_TYPE_OPENGL;

    // Build parameters list:
    m_params.emplace_back(
            new PARAM<int>( "find_replace.match_mode", &m_FindReplace.match_mode, 0 ) );

    m_params.emplace_back(
            new PARAM<bool>( "find_replace.match_case", &m_FindReplace.match_case, false ) );

    m_params.emplace_back( new PARAM<bool>( "find_replace.search_and_replace",
                                            &m_FindReplace.search_and_replace, false ) );

    m_params.emplace_back( new PARAM<QString>( "find_replace.find_string",
            &m_FindReplace.find_string, "" ) );

    m_params.emplace_back( new PARAM_LIST<QString>( "find_replace.find_history",
            &m_FindReplace.find_history, {} ) );

    m_params.emplace_back( new PARAM<QString>( "find_replace.replace_string",
            &m_FindReplace.replace_string, "" ) );

    m_params.emplace_back( new PARAM_LIST<QString>( "find_replace.replace_history",
            &m_FindReplace.replace_history, {} ) );

    m_params.emplace_back( new PARAM<int>( "graphics.canvas_type",
            &m_Graphics.canvas_type, EDA_DRAW_PANEL_GAL::GAL_TYPE_OPENGL ) );

    m_params.emplace_back( new PARAM<float>( "graphics.highlight_factor",
            &m_Graphics.highlight_factor, 0.5f, 0.0, 1.0f ) );

    m_params.emplace_back( new PARAM<float>( "graphics.select_factor",
            &m_Graphics.select_factor, 0.75f, 0.0, 1.0f ) );

    m_params.emplace_back( new PARAM<int>( "color_picker.default_tab",
            &m_ColorPicker.default_tab, 0 ) );

    m_params.emplace_back( new PARAM_LIST<QString>( "lib_tree.columns", &m_LibTree.columns, {} ) );

    m_params.emplace_back( new PARAM_LAMBDA<nlohmann::json>( "lib_tree.column_widths",
            [&]() -> nlohmann::json
            {
                nlohmann::json ret = {};

                for( const std::pair<const QString, int>& pair : m_LibTree.column_widths )
                    ret[pair.first.toStdString()] = pair.second;

                return ret;
            },
            [&]( const nlohmann::json& aJson )
            {
                if( !aJson.is_object() )
                    return;

                m_LibTree.column_widths.clear();

                for( const auto& entry : aJson.items() )
                {
                    if( !entry.value().is_number_integer() )
                        continue;

                    m_LibTree.column_widths[QString::fromStdString(entry.key())] = entry.value().get<int>();
                }
            },
            {} ) );

    m_params.emplace_back(
            new PARAM_LIST<QString>( "lib_tree.open_libs", &m_LibTree.open_libs, {} ) );

    m_params.emplace_back( new PARAM<bool>( "printing.background",
            &m_Printing.background, false ) );

    m_params.emplace_back( new PARAM<bool>( "printing.monochrome",
            &m_Printing.monochrome, true ) );

    m_params.emplace_back( new PARAM<double>( "printing.scale",
            &m_Printing.scale, 1.0 ) );

    m_params.emplace_back( new PARAM<bool>( "printing.use_theme",
            &m_Printing.use_theme, false ) );

    m_params.emplace_back( new PARAM<QString>( "printing.color_theme",
            &m_Printing.color_theme, "" ) );

    m_params.emplace_back( new PARAM<bool>( "printing.title_block",
            &m_Printing.title_block, false ) );

    m_params.emplace_back( new PARAM_LIST<int>( "printing.layers",
            &m_Printing.layers, {} ) );

    m_params.emplace_back( new PARAM<int>( "search_pane.selection_zoom",
            reinterpret_cast<int*>( &m_SearchPane.selection_zoom ),
            static_cast<int>( SEARCH_PANE::SELECTION_ZOOM::PAN ) ) );

    m_params.emplace_back( new PARAM<bool>( "search_pane.search_hidden_fields",
            &m_SearchPane.search_hidden_fields, true ) );

    m_params.emplace_back( new PARAM<bool>( "system.first_run_shown",
            &m_System.first_run_shown, false ) ); //@todo RFB remove? - not used

    m_params.emplace_back( new PARAM<int>( "system.max_undo_items",
            &m_System.max_undo_items, 0 ) );

    m_params.emplace_back( new PARAM_LIST<QString>( "system.file_history",
            &m_System.file_history, {} ) );

    if( m_filename == "pl_editor"
        || ( m_filename == "eeschema" || m_filename == "symbol_editor" ) )
    {
        m_params.emplace_back( new PARAM<int>( "system.units",
                &m_System.units, static_cast<int>( EDA_UNITS::MILS ) ) );
    }
    else
    {
        m_params.emplace_back( new PARAM<int>( "system.units",
                &m_System.units, static_cast<int>( EDA_UNITS::MM ) ) );
    }

    m_params.emplace_back( new PARAM<int>( "system.last_metric_units",
            &m_System.last_metric_units, static_cast<int>( EDA_UNITS::MM ) ) );

    m_params.emplace_back( new PARAM<int>( "system.last_imperial_units",
            &m_System.last_imperial_units, static_cast<int>( EDA_UNITS::MILS ) ) );

    m_params.emplace_back( new PARAM<bool>( "system.show_import_issues",
                                            &m_System.show_import_issues, true ) );

    m_params.emplace_back( new PARAM_LAMBDA<nlohmann::json>( "plugins.actions",
            [&]() -> nlohmann::json
            {
                nlohmann::json js = nlohmann::json::array();

                for( const auto& [identifier, visible] : m_Plugins.actions )
                    js.push_back( nlohmann::json( { { identifier.toStdString(), visible } } ) );

                return js;
            },
            [&]( const nlohmann::json& aObj )
            {
                m_Plugins.actions.clear();

                if( !aObj.is_array() )
                {
                    return;
                }

                for( const auto& entry : aObj )
                {
                    if( entry.empty() || !entry.is_object() )
                        continue;

                    for( const auto& pair : entry.items() )
                    {
                        m_Plugins.actions.emplace_back( std::make_pair(
                                QString::fromStdString(pair.key()), pair.value() ) );
                    }
                }
            },
            nlohmann::json::array() ) );

    m_params.emplace_back( new PARAM<QString>( "appearance.color_theme",
            &m_ColorTheme, COLOR_SETTINGS::COLOR_BUILTIN_DEFAULT ) );

    addParamsForWindow( &m_Window, "window" );

    m_params.emplace_back( new PARAM<bool>( "cross_probing.on_selection",
            &m_CrossProbing.on_selection, true ) );

    m_params.emplace_back( new PARAM<bool>( "cross_probing.center_on_items",
            &m_CrossProbing.center_on_items, true ) );

    m_params.emplace_back( new PARAM<bool>( "cross_probing.zoom_to_fit",
            &m_CrossProbing.zoom_to_fit, true ) );

    m_params.emplace_back( new PARAM<bool>( "cross_probing.auto_highlight",
            &m_CrossProbing.auto_highlight, true ) );
}


bool APP_SETTINGS_BASE::MigrateFromLegacy( QSettings* aCfg )
{
    bool ret = true;

    const std::string f = getLegacyFrameName();

    ret &= fromLegacyString(   aCfg, "LastFindString",      "find_replace.find_string" );
    ret &= fromLegacyString(   aCfg, "LastReplaceString",   "find_replace.replace_string" );

    migrateFindReplace( aCfg );

    ret &= fromLegacy<int>(    aCfg, "canvas_type",         "graphics.canvas_type" );

    ret &= fromLegacy<int>(    aCfg, "P22LIB_TREE_MODEL_ADAPTERSelectorColumnWidth",
                                                            "lib_tree.column_width" );

    ret &= fromLegacy<bool>(   aCfg, "PrintMonochrome",     "printing.monochrome" );
    ret &= fromLegacy<double>( aCfg, "PrintScale",          "printing.scale" );
    ret &= fromLegacy<bool>(   aCfg, "PrintPageFrame",      "printing.title_block" );

    {
        nlohmann::json js = nlohmann::json::array();
        QString       key;

        for( unsigned i = 0; i < PCB_LAYER_ID_COUNT; ++i )
        {
            key = QString::asprintf( "PlotLayer_%d", i );

            if( aCfg->value( key, false ).toBool() )
                js.push_back( i );
        }

        Set( "printing.layers", js );
    }

    ret &= fromLegacy<bool>(   aCfg, f + "FirstRunShown",       "system.first_run_shown" );
    ret &= fromLegacy<int>(    aCfg, f + "DevelMaxUndoItems",   "system.max_undo_items" );
    ret &= fromLegacy<int>(    aCfg, f + "Units",               "system.units" );

    {
        int            max_history_size = Pgm().GetCommonSettings()->m_System.file_history_size;
        QString       file, key;
        nlohmann::json js = nlohmann::json::array();

        for( int i = 1; i <= max_history_size; i++ )
        {
            key = QString::asprintf( "file%d", i );
            file = aCfg->value( key, QString() ).toString();

            if( !file.isEmpty() )
                js.push_back( file );
        }

        Set( "system.file_history", js );
    }

    ret &= migrateWindowConfig( aCfg, f, "window" );

    return ret;
}


void APP_SETTINGS_BASE::migrateFindReplace( QSettings* aCfg )
{
    const int find_replace_history_size = 10;
    nlohmann::json find_history         = nlohmann::json::array();
    nlohmann::json replace_history      = nlohmann::json::array();
    QString tmp, find_key, replace_key;

    for( int i = 0; i < find_replace_history_size; ++i )
    {
        find_key = QString::asprintf( "FindStringHistoryList%d", i );
        replace_key = QString::asprintf( "ReplaceStringHistoryList%d", i );

        tmp = aCfg->value( find_key, QString() ).toString();
        if( !tmp.isEmpty() )
            find_history.push_back( tmp );

        tmp = aCfg->value( replace_key, QString() ).toString();
        if( !tmp.isEmpty() )
            replace_history.push_back( tmp );
    }

    Set( "find_replace.find_history", find_history );
    Set( "find_replace.replace_history", replace_history );
}


bool APP_SETTINGS_BASE::migrateWindowConfig( QSettings* aCfg, const std::string& aFrame,
                                             const std::string& aJsonPath )
{
    bool ret = true;

    const std::string frameGDO = aFrame + "GalDisplayOptions";
    const std::string cursorPath = aJsonPath + ".cursor";
    const std::string gridPath = aJsonPath + ".grid";

    ret &= fromLegacy<bool>( aCfg, aFrame + "Maximized",            aJsonPath + ".maximized" );
    ret &= fromLegacyString( aCfg, aFrame + "MostRecentlyUsedPath", aJsonPath + ".mru_path" );
    ret &= fromLegacy<int>(  aCfg, aFrame + "Size_x",               aJsonPath + ".size_x" );
    ret &= fromLegacy<int>(  aCfg, aFrame + "Size_y",               aJsonPath + ".size_y" );
    ret &= fromLegacyString( aCfg, aFrame + "Perspective",          aJsonPath + ".perspective" );
    ret &= fromLegacy<int>(  aCfg, aFrame + "Pos_x",                aJsonPath + ".pos_x" );
    ret &= fromLegacy<int>(  aCfg, aFrame + "Pos_y",                aJsonPath + ".pos_y" );

    ret &= fromLegacy<bool>( aCfg, frameGDO + "ForceDisplayCursor",
                             cursorPath + ".always_show_cursor" );
    ret &= fromLegacy<bool>( aCfg, frameGDO + "CursorFullscreen",
                             cursorPath + ".fullscreen_cursor" );

    ret &= fromLegacy<int>(  aCfg, aFrame + "_LastGridSize",        gridPath + ".last_size" );

    ret &= fromLegacy<int>(  aCfg, aFrame + "FastGrid1",            gridPath + ".fast_grid_1" );
    ret &= fromLegacy<int>(  aCfg, aFrame + "FastGrid2",            gridPath + ".fast_grid_2" );

    ret &= fromLegacy<bool>(   aCfg, frameGDO + "GridAxesEnabled",  gridPath + ".axes_enabled" );
    ret &= fromLegacy<double>( aCfg, frameGDO + "GridLineWidth",    gridPath + ".line_width" );
    ret &= fromLegacy<double>( aCfg, frameGDO + "GridMaxDensity",   gridPath + ".min_spacing" );
    ret &= fromLegacy<bool>(   aCfg, frameGDO + "ShowGrid",         gridPath + ".show" );
    ret &= fromLegacy<int>(    aCfg, frameGDO + "GridStyle",        gridPath + ".style" );
    ret &= fromLegacyColor(    aCfg, frameGDO + "GridColor",        gridPath + ".color" );

    return ret;
}


void APP_SETTINGS_BASE::addParamsForWindow( WINDOW_SETTINGS* aWindow, const std::string& aJsonPath,
                                            int aDefaultWidth, int aDefaultHeight )
{
    m_params.emplace_back( new PARAM<bool>( aJsonPath + ".maximized",
            &aWindow->state.maximized, false ) );

    m_params.emplace_back( new PARAM<QString>( aJsonPath + ".mru_path",
            &aWindow->mru_path, "" ) );

    m_params.emplace_back( new PARAM<int>( aJsonPath + ".size_x", &aWindow->state.size_x, aDefaultWidth ) );

    m_params.emplace_back( new PARAM<int>( aJsonPath + ".size_y", &aWindow->state.size_y, aDefaultHeight ) );

    m_params.emplace_back( new PARAM<QString>( aJsonPath + ".perspective",
            &aWindow->perspective, "" ) );

    m_params.emplace_back( new PARAM<int>( aJsonPath + ".pos_x", &aWindow->state.pos_x, 0 ) );

    m_params.emplace_back( new PARAM<int>( aJsonPath + ".pos_y", &aWindow->state.pos_y, 0 ) );

    m_params.emplace_back( new PARAM<unsigned int>( aJsonPath + ".display",
            &aWindow->state.display, 0 ) );

    m_params.emplace_back( new PARAM_LIST<double>( aJsonPath + ".zoom_factors",
            &aWindow->zoom_factors, DefaultZoomList(), true /* resetIfEmpty */ ) );

    m_params.emplace_back( new PARAM<bool>( aJsonPath + ".grid.axes_enabled",
            &aWindow->grid.axes_enabled, false ) );

    int defaultGridIdx;

    if( ( m_filename == "eeschema" || m_filename == "symbol_editor" ) )
    {
        defaultGridIdx = 1;
    }
    else if( m_filename == "pl_editor" )
    {
        defaultGridIdx = 4;
    }
    else
    {
        defaultGridIdx = 15;
    }

    m_params.emplace_back( new PARAM_LIST<GRID>( aJsonPath + ".grid.sizes", &aWindow->grid.grids,
                                                 DefaultGridSizeList(), true /* resetIfEmpty */ ) );

    m_params.emplace_back( new PARAM<int>( aJsonPath + ".grid.last_size",
            &aWindow->grid.last_size_idx, defaultGridIdx ) );

    m_params.emplace_back( new PARAM<int>( aJsonPath + ".grid.fast_grid_1",
            &aWindow->grid.fast_grid_1, defaultGridIdx ) );

    m_params.emplace_back( new PARAM<int>( aJsonPath + ".grid.fast_grid_2",
            &aWindow->grid.fast_grid_2, defaultGridIdx + 1 ) );

    // legacy values, leave blank by default so we don't convert them
    m_params.emplace_back( new PARAM<QString>( aJsonPath + ".grid.user_grid_x",
            &aWindow->grid.user_grid_x, QString() ) );
    m_params.emplace_back( new PARAM<QString>( aJsonPath + ".grid.user_grid_y",
            &aWindow->grid.user_grid_y, QString() ) );

    // for grid overrides, give just the schematic and symbol editors sane values
    if( m_filename == "eeschema" || m_filename == "symbol_editor" )
    {
        m_params.emplace_back( new PARAM<bool>( aJsonPath + ".grid.overrides_enabled",
                                                &aWindow->grid.overrides_enabled, true ) );
        m_params.emplace_back( new PARAM<bool>( aJsonPath + ".grid.override_connected",
                                                &aWindow->grid.override_connected, true ) );
        m_params.emplace_back( new PARAM<bool>( aJsonPath + ".grid.override_wires",
                                                &aWindow->grid.override_wires, true ) );
        m_params.emplace_back( new PARAM<bool>( aJsonPath + ".grid.override_vias",
                                                &aWindow->grid.override_vias, false ) );
        m_params.emplace_back( new PARAM<bool>( aJsonPath + ".grid.override_text",
                                                &aWindow->grid.override_text, true ) );
        m_params.emplace_back( new PARAM<bool>( aJsonPath + ".grid.override_graphics",
                                                &aWindow->grid.override_graphics, false ) );

        m_params.emplace_back( new PARAM<int>( aJsonPath + ".grid.override_connected_idx",
                                               &aWindow->grid.override_connected_idx, 1 ) );
        m_params.emplace_back( new PARAM<int>( aJsonPath + ".grid.override_wires_idx",
                                               &aWindow->grid.override_wires_idx, 1 ) );
        m_params.emplace_back( new PARAM<int>( aJsonPath + ".grid.override_vias_idx",
                                               &aWindow->grid.override_vias_idx, 0 ) );
        m_params.emplace_back( new PARAM<int>( aJsonPath + ".grid.override_text_idx",
                                               &aWindow->grid.override_text_idx, 3 ) );
        m_params.emplace_back( new PARAM<int>( aJsonPath + ".grid.override_graphics_idx",
                                               &aWindow->grid.override_graphics_idx, 2 ) );
    }
    else
    {
        m_params.emplace_back( new PARAM<bool>( aJsonPath + ".grid.overrides_enabled",
                                                &aWindow->grid.overrides_enabled, true ) );
        m_params.emplace_back( new PARAM<bool>( aJsonPath + ".grid.override_connected",
                                                &aWindow->grid.override_connected, false ) );
        m_params.emplace_back( new PARAM<bool>( aJsonPath + ".grid.override_wires",
                                                &aWindow->grid.override_wires, false ) );
        m_params.emplace_back( new PARAM<bool>( aJsonPath + ".grid.override_vias",
                                                &aWindow->grid.override_vias, false ) );
        m_params.emplace_back( new PARAM<bool>( aJsonPath + ".grid.override_text",
                                                &aWindow->grid.override_text, false ) );
        m_params.emplace_back( new PARAM<bool>( aJsonPath + ".grid.override_graphics",
                                                &aWindow->grid.override_graphics, false ) );

        m_params.emplace_back( new PARAM<int>( aJsonPath + ".grid.override_connected_idx",
                                               &aWindow->grid.override_connected_idx, 16 ) );
        m_params.emplace_back( new PARAM<int>( aJsonPath + ".grid.override_text_idx",
                                               &aWindow->grid.override_text_idx, 18 ) );
        m_params.emplace_back( new PARAM<int>( aJsonPath + ".grid.override_wires_idx",
                                               &aWindow->grid.override_wires_idx, 19 ) );
        m_params.emplace_back( new PARAM<int>( aJsonPath + ".grid.override_vias_idx",
                                               &aWindow->grid.override_vias_idx, 18 ) );
        m_params.emplace_back( new PARAM<int>( aJsonPath + ".grid.override_graphics_idx",
                                               &aWindow->grid.override_graphics_idx, 15 ) );
    }

    m_params.emplace_back( new PARAM<double>( aJsonPath + ".grid.line_width",
            &aWindow->grid.line_width, 1.0 ) );

    m_params.emplace_back( new PARAM<double>( aJsonPath + ".grid.min_spacing",
            &aWindow->grid.min_spacing, 10 ) );

    m_params.emplace_back( new PARAM<bool>( aJsonPath + ".grid.show",
            &aWindow->grid.show, true ) );

    m_params.emplace_back( new PARAM<int>( aJsonPath + ".grid.style",
            &aWindow->grid.style, 0 ) );

    m_params.emplace_back( new PARAM<int>( aJsonPath + ".grid.snap",
            &aWindow->grid.snap, 0 ) );

    m_params.emplace_back( new PARAM<bool>( aJsonPath + ".cursor.always_show_cursor",
            &aWindow->cursor.always_show_cursor, true ) );

    m_params.emplace_back( new PARAM<bool>( aJsonPath + ".cursor.fullscreen_cursor",
            &aWindow->cursor.fullscreen_cursor, false ) );
}


const std::vector<double> APP_SETTINGS_BASE::DefaultZoomList() const
{
    if( m_filename == "eeschema" || m_filename == "symbol_editor" )
    {
        return { ZOOM_LIST_EESCHEMA };
    }
    else if( m_filename == "pl_editor" )
    {
        return { ZOOM_LIST_PL_EDITOR };
    }
    else if( m_filename == "gerbview" )
    {
        return { ZOOM_LIST_GERBVIEW };
    }
    else
    {
        if( ADVANCED_CFG::GetCfg().m_HyperZoom )
            return { ZOOM_LIST_PCBNEW_HYPER };
        else
            return { ZOOM_LIST_PCBNEW };
    }
}


const std::vector<GRID> APP_SETTINGS_BASE::DefaultGridSizeList() const
{
    if( m_filename == "eeschema" || m_filename == "symbol_editor" )
    {
        return { GRID{ QString(), "100 mil", "100 mil" },
                 GRID{ QString(), "50 mil", "50 mil" },
                 GRID{ QString(), "25 mil", "25 mil" },
                 GRID{ QString(), "10 mil", "10 mil" } };
    }
    else if( m_filename == "pl_editor" )
    {
        return { GRID{ QString(), "5.00 mm", "5.00 mm" },
                 GRID{ QString(), "2.50 mm", "2.50 mm" },
                 GRID{ QString(), "2.00 mm", "2.00 mm" },
                 GRID{ QString(), "1.00 mm", "1.00 mm" },
                 GRID{ QString(), "0.50 mm", "0.50 mm" },
                 GRID{ QString(), "0.25 mm", "0.25 mm" },
                 GRID{ QString(), "0.20 mm", "0.20 mm" },
                 GRID{ QString(), "0.10 mm", "0.10 mm" } };
    }
    else if( m_filename == "gerbview" )
    {
        return { GRID{ QString(), "100 mil", "100 mil" },
                 GRID{ QString(), "50 mil", "50 mil" },
                 GRID{ QString(), "25 mil", "25 mil" },
                 GRID{ QString(), "20 mil", "20 mil" },
                 GRID{ QString(), "10 mil", "10 mil" },
                 GRID{ QString(), "5 mil", "5 mil" },
                 GRID{ QString(), "2.5 mil", "2.5 mil" },
                 GRID{ QString(), "2 mil", "2 mil" },
                 GRID{ QString(), "1 mil", "1 mil" },
                 GRID{ QString(), "0.5 mil", "0.5 mil" },
                 GRID{ QString(), "0.2 mil", "0.2 mil" },
                 GRID{ QString(), "0.1 mil", "0.1 mil" },
                 GRID{ QString(), "5.0 mm", "5.0 mm" },
                 GRID{ QString(), "1.5 mm", "2.5 mm" },
                 GRID{ QString(), "1.0 mm", "1.0 mm" },
                 GRID{ QString(), "0.5 mm", "0.5 mm" },
                 GRID{ QString(), "0.25 mm", "0.25 mm" },
                 GRID{ QString(), "0.2 mm", "0.2 mm" },
                 GRID{ QString(), "0.1 mm", "0.1 mm" },
                 GRID{ QString(), "0.05 mm", "0.0 mm" },
                 GRID{ QString(), "0.025 mm", "0.0 mm" },
                 GRID{ QString(), "0.01 mm", "0.0 mm" } };
    }
    else
    {
        return { GRID{ QString(), "1000 mil", "1000 mil" },
                 GRID{ QString(), "500 mil", "500 mil" },
                 GRID{ QString(), "250 mil", "250 mil" },
                 GRID{ QString(), "200 mil", "200 mil" },
                 GRID{ QString(), "100 mil", "100 mil" },
                 GRID{ QString(), "50 mil", "50 mil" },
                 GRID{ QString(), "25 mil", "25 mil" },
                 GRID{ QString(), "20 mil", "20 mil" },
                 GRID{ QString(), "10 mil", "10 mil" },
                 GRID{ QString(), "5 mil", "5 mil" },
                 GRID{ QString(), "2 mil", "2 mil" },
                 GRID{ QString(), "1 mil", "1 mil" },
                 GRID{ QString(), "5.0 mm", "5.0 mm" },
                 GRID{ QString(), "2.5 mm", "2.5 mm" },
                 GRID{ QString(), "1.0 mm", "1.0 mm" },
                 GRID{ QString(), "0.5 mm", "0.5 mm" },
                 GRID{ QString(), "0.25 mm", "0.25 mm" },
                 GRID{ QString(), "0.2 mm", "0.2 mm" },
                 GRID{ QString(), "0.1 mm", "0.1 mm" },
                 GRID{ QString(), "0.05 mm", "0.05 mm" },
                 GRID{ QString(), "0.025 mm", "0.025 mm" },
                 GRID{ QString(), "0.01 mm", "0.01 mm" } };
    }
}


bool APP_SETTINGS_BASE::migrateLibTreeWidth()
{
    // We used to store only the width of the first column, because there were only
    // two possible columns.
    if( std::optional<int> optWidth = Get<int>( "lib_tree.column_width" ) )
    {
        Set<nlohmann::json>( "lib_tree.column_widths", { { "Item", *optWidth } } );
        At( "lib_tree" ).erase( "column_width" );
    }

    return true;
}
