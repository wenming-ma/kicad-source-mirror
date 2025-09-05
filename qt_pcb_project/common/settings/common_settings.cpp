
#include <set>
#include <fstream>
#include <sstream>

#include <env_vars.h>
#include <paths.h>
#include <search_stack.h>
#include <settings/settings_manager.h>
#include <settings/common_settings.h>
#include <settings/json_settings.h>
#include <settings/parameters.h>
#include <systemdirsappend.h>
#include <trace_helpers.h>
#include <QDebug>
#include <QRegularExpression>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QtCore/Qt>


///! The following environment variables will never be migrated from a previous version
const QRegularExpression versionedEnvVarRegex( "KICAD[0-9]+_[A-Z0-9_]+(_DIR)?" );

///! Update the schema version whenever a migration is required
const int commonSchemaVersion = 3;

COMMON_SETTINGS::COMMON_SETTINGS() :
        JSON_SETTINGS( "kicad_common", SETTINGS_LOC::USER, commonSchemaVersion ),
        m_Appearance(),
        m_Backup(),
        m_Env(),
        m_Input(),
        m_Graphics(),
        m_Session(),
        m_System(),
        m_DoNotShowAgain(),
        m_NetclassPanel(),
        m_PackageManager(),
        m_Api()
{
    /*
     * Automatic dark mode detection works fine on Mac.
     */
#if defined( Q_OS_LINUX ) || defined( Q_OS_WIN )
    m_params.emplace_back( new PARAM_ENUM<ICON_THEME>( "appearance.icon_theme",
            &m_Appearance.icon_theme, ICON_THEME::AUTO, ICON_THEME::LIGHT, ICON_THEME::AUTO ) );
#else
    m_Appearance.icon_theme = ICON_THEME::AUTO;
#endif

    /*
   	 * Automatic canvas scaling works fine on all supported platforms, so it's no longer exposed as
     * a configuration option.
     */
    m_Appearance.canvas_scale = 0.0;

    /*
     * Menu icons are off by default on OSX and on for all other platforms.
     */
#ifdef Q_OS_MACOS
    m_params.emplace_back( new PARAM<bool>( "appearance.use_icons_in_menus",
            &m_Appearance.use_icons_in_menus, false ) );
#else
    m_params.emplace_back( new PARAM<bool>( "appearance.use_icons_in_menus",
            &m_Appearance.use_icons_in_menus, true ) );
#endif

    /*
     * Font scaling hacks are only needed on GTK under wxWidgets 3.0.
     */
    m_Appearance.apply_icon_scale_to_fonts = false;

    m_params.emplace_back( new PARAM<bool>( "appearance.show_scrollbars",
            &m_Appearance.show_scrollbars, false ) );

    m_params.emplace_back( new PARAM<double>( "appearance.hicontrast_dimming_factor",
            &m_Appearance.hicontrast_dimming_factor, 0.8f ) );

    m_params.emplace_back( new PARAM<int>( "appearance.text_editor_zoom",
            &m_Appearance.text_editor_zoom, 0 ) );

    m_params.emplace_back( new PARAM<int>( "appearance.toolbar_icon_size",
            &m_Appearance.toolbar_icon_size, 24, 16, 64 ) );

    m_params.emplace_back( new PARAM<bool>( "appearance.grid_striping",
            &m_Appearance.grid_striping, false ) );

    m_params.emplace_back( new PARAM<bool>( "auto_backup.enabled", &m_Backup.enabled, true ) );

    m_params.emplace_back( new PARAM<bool>( "auto_backup.backup_on_autosave",
            &m_Backup.backup_on_autosave, false ) );

    m_params.emplace_back( new PARAM<int>( "auto_backup.limit_total_files",
            &m_Backup.limit_total_files, 25 ) );

    m_params.emplace_back( new PARAM<unsigned long long>( "auto_backup.limit_total_size",
            &m_Backup.limit_total_size, 104857600 ) );

    m_params.emplace_back( new PARAM<int>( "auto_backup.limit_daily_files",
            &m_Backup.limit_daily_files, 5 ) );

    m_params.emplace_back( new PARAM<int>( "auto_backup.min_interval",
            &m_Backup.min_interval, 300 ) );

    auto envVarsParam = m_params.emplace_back( new PARAM_LAMBDA<nlohmann::json>( "environment.vars",
            [&]() -> nlohmann::json
            {
                nlohmann::json ret = {};

                for( const std::pair<QString, ENV_VAR_ITEM> entry : m_Env.vars )
                {
                    const ENV_VAR_ITEM& var = entry.second;

                    Q_ASSERT( entry.first == var.GetKey() );

                    // Default values are never persisted
                    if( var.IsDefault() )
                    {
                        qDebug() << "COMMON_SETTINGS: Env var" << var.GetKey() << "skipping save (default)";
                        continue;
                    }

                    QString value = var.GetValue();

                    value = value.trimmed(); // Trim from both sides

                    // Vars that existed in JSON are persisted, but if they were overridden
                    // externally, we persist the old value (i.e. the one that was loaded from JSON)
                    if( var.GetDefinedExternally() )
                    {
                        if( var.GetDefinedInSettings() )
                        {
                            qDebug() << "COMMON_SETTINGS: Env var" << var.GetKey() << "was overridden"
                                     << "externally, saving previously-loaded value" << var.GetSettingsValue();
                            value = var.GetSettingsValue();
                        }
                        else
                        {
                            qDebug() << "COMMON_SETTINGS: Env var" << var.GetKey() << "skipping save (external)";
                            continue;
                        }
                    }

                    qDebug() << "COMMON_SETTINGS: Saving env var" << var.GetKey() << "=" << value;

                    std::string key( var.GetKey().trimmed().toUtf8().constData() );
                    ret[ std::move( key ) ] = value;
                }

                return ret;
            },
            [&]( const nlohmann::json& aJson )
            {
                if( !aJson.is_object() )
                    return;

                for( const auto& entry : aJson.items() )
                {
                    QString key = QString::fromUtf8( entry.key().c_str() ).trimmed();
                    QString val = entry.value().get<QString>().trimmed();

                    if( m_Env.vars.count( key ) )
                    {
                        if( m_Env.vars[key].GetDefinedExternally() )
                        {
                            qDebug() << "COMMON_SETTINGS:" << key << "is defined externally";
                            m_Env.vars[key].SetDefinedInSettings();
                            m_Env.vars[key].SetSettingsValue( val );
                            continue;
                        }
                        else
                        {
                            qDebug() << "COMMON_SETTINGS: Updating" << key << ":" 
                                     << m_Env.vars[key].GetValue() << "->" << val;
                            m_Env.vars[key].SetValue( val );
                        }
                    }
                    else
                    {
                        qDebug() << "COMMON_SETTINGS: Loaded new var:" << key << "=" << val;
                        m_Env.vars[key] = ENV_VAR_ITEM( key, val );
                    }

                    m_Env.vars[key].SetDefinedInSettings();
                    m_Env.vars[key].SetSettingsValue( val );
                }
            },
            {} ) );
    envVarsParam->SetClearUnknownKeys();

    m_params.emplace_back( new PARAM<bool>( "input.focus_follow_sch_pcb",
            &m_Input.focus_follow_sch_pcb, false ) );

    m_params.emplace_back( new PARAM<bool>( "input.auto_pan", &m_Input.auto_pan, false ) );

    m_params.emplace_back( new PARAM<int>( "input.auto_pan_acceleration",
            &m_Input.auto_pan_acceleration, 5 ) );

    m_params.emplace_back( new PARAM<bool>( "input.center_on_zoom",
            &m_Input.center_on_zoom, true ) );

    m_params.emplace_back( new PARAM<bool>( "input.immediate_actions",
            &m_Input.immediate_actions, true ) );

    m_params.emplace_back( new PARAM<bool>( "input.warp_mouse_on_move",
            &m_Input.warp_mouse_on_move, true ) );

    m_params.emplace_back( new PARAM<bool>( "input.horizontal_pan",
            &m_Input.horizontal_pan, false ) );

    m_params.emplace_back( new PARAM<bool>( "input.hotkey_feedback",
            &m_Input.hotkey_feedback, true ) );

    m_params.emplace_back( new PARAM<bool>( "input.zoom_acceleration",
            &m_Input.zoom_acceleration, false ) );

#ifdef Q_OS_MACOS
    int default_zoom_speed = 5;
#else
    int default_zoom_speed = 1;
#endif

    m_params.emplace_back( new PARAM<int>( "input.zoom_speed",
            &m_Input.zoom_speed, default_zoom_speed ) );

    m_params.emplace_back( new PARAM<bool>( "input.zoom_speed_auto",
            &m_Input.zoom_speed_auto, true ) );

    m_params.emplace_back( new PARAM<int>( "input.scroll_modifier_zoom",
            &m_Input.scroll_modifier_zoom, 0 ) );

    m_params.emplace_back( new PARAM<int>( "input.scroll_modifier_pan_h",
            &m_Input.scroll_modifier_pan_h, Qt::Key_Control ) );

    m_params.emplace_back( new PARAM<int>( "input.scroll_modifier_pan_v",
            &m_Input.scroll_modifier_pan_v, Qt::Key_Shift ) );

    m_params.emplace_back( new PARAM<bool>( "input.reverse_scroll_zoom",
            &m_Input.reverse_scroll_zoom, false ) );

    m_params.emplace_back( new PARAM<bool>( "input.reverse_scroll_pan_h",
            &m_Input.reverse_scroll_pan_h, false ) );

    m_params.emplace_back( new PARAM_ENUM<MOUSE_DRAG_ACTION>( "input.mouse_left",
            &m_Input.drag_left, MOUSE_DRAG_ACTION::DRAG_SELECTED, MOUSE_DRAG_ACTION::DRAG_ANY,
            MOUSE_DRAG_ACTION::SELECT ) );

    m_params.emplace_back( new PARAM_ENUM<MOUSE_DRAG_ACTION>( "input.mouse_middle",
            &m_Input.drag_middle, MOUSE_DRAG_ACTION::PAN, MOUSE_DRAG_ACTION::SELECT,
            MOUSE_DRAG_ACTION::NONE ) );

    m_params.emplace_back( new PARAM_ENUM<MOUSE_DRAG_ACTION>( "input.mouse_right",
            &m_Input.drag_right, MOUSE_DRAG_ACTION::PAN, MOUSE_DRAG_ACTION::SELECT,
            MOUSE_DRAG_ACTION::NONE ) );

    m_params.emplace_back( new PARAM<int>( "graphics.opengl_antialiasing_mode",
            &m_Graphics.opengl_aa_mode, 2, 0, 2 ) );

    m_params.emplace_back( new PARAM<int>( "graphics.cairo_antialiasing_mode",
            &m_Graphics.cairo_aa_mode, 0, 0, 2 ) );

    m_params.emplace_back( new PARAM<int>( "system.autosave_interval",
            &m_System.autosave_interval, 600 ) );

#ifdef __APPLE__
    m_params.emplace_back( new PARAM<QString>( "system.text_editor",
            &m_System.text_editor, "/usr/bin/open -e" ) );
#else
    m_params.emplace_back( new PARAM<QString>( "system.text_editor",
            &m_System.text_editor, "" ) );
#endif

#if defined( Q_OS_WIN )
    m_params.emplace_back( new PARAM<QString>( "system.file_explorer",
            &m_System.file_explorer, "explorer.exe /n,/select,%F" ) );
#else
    m_params.emplace_back( new PARAM<QString>( "system.file_explorer",
            &m_System.file_explorer, "" ) );
#endif

    m_params.emplace_back( new PARAM<int>( "system.file_history_size",
            &m_System.file_history_size, 9 ) );

    m_params.emplace_back( new PARAM<QString>( "system.language",
            &m_System.language, "Default" ) );

    m_params.emplace_back( new PARAM<QString>( "system.pdf_viewer_name",
            &m_System.pdf_viewer_name, "" ) );

    m_params.emplace_back( new PARAM<bool>( "system.use_system_pdf_viewer",
            &m_System.use_system_pdf_viewer, true ) );

    m_params.emplace_back( new PARAM<QString>( "system.working_dir",
            &m_System.working_dir, "" ) );

    m_params.emplace_back( new PARAM<int>( "system.clear_3d_cache_interval",
            &m_System.clear_3d_cache_interval, 30 ) );

    m_params.emplace_back( new PARAM<bool>( "do_not_show_again.zone_fill_warning",
            &m_DoNotShowAgain.zone_fill_warning, false ) );

    m_params.emplace_back( new PARAM<bool>( "do_not_show_again.env_var_overwrite_warning",
            &m_DoNotShowAgain.env_var_overwrite_warning, false ) );

    m_params.emplace_back( new PARAM<bool>( "do_not_show_again.scaled_3d_models_warning",
            &m_DoNotShowAgain.scaled_3d_models_warning, false ) );

    m_params.emplace_back( new PARAM<bool>( "do_not_show_again.data_collection_prompt",
            &m_DoNotShowAgain.data_collection_prompt, false ) );

    m_params.emplace_back( new PARAM<bool>( "do_not_show_again.update_check_prompt",
            &m_DoNotShowAgain.update_check_prompt, false ) );

    m_params.emplace_back( new PARAM<bool>( "session.remember_open_files",
            &m_Session.remember_open_files, false ) );

    m_params.emplace_back( new PARAM_LIST<QString>( "session.pinned_symbol_libs",
            &m_Session.pinned_symbol_libs, {} ) );

    m_params.emplace_back( new PARAM_LIST<QString>( "session.pinned_fp_libs",
            &m_Session.pinned_fp_libs, {} ) );

    m_params.emplace_back( new PARAM_LIST<QString>( "session.pinned_design_block_libs",
            &m_Session.pinned_design_block_libs, {} ) );

    m_params.emplace_back( new PARAM<int>( "netclass_panel.sash_pos",
            &m_NetclassPanel.sash_pos, 160 ) );

    m_params.emplace_back( new PARAM<QString>( "netclass_panel.eeschema_shown_columns",
            &m_NetclassPanel.eeschema_visible_columns, "0 10 11 12 13" ) );

    m_params.emplace_back( new PARAM<QString>( "netclass_panel.pcbnew_shown_columns",
            &m_NetclassPanel.pcbnew_visible_columns, "0 1 2 3 4 5 6 7 8 9" ) );

    m_params.emplace_back( new PARAM<int>( "package_manager.sash_pos",
            &m_PackageManager.sash_pos, 380 ) );

    m_params.emplace_back( new PARAM_LAMBDA<nlohmann::json>( "git.repositories",
            [&]() -> nlohmann::json
            {
                nlohmann::json ret = {};

                for( const GIT_REPOSITORY& repo : m_Git.repositories )
                {
                    nlohmann::json repoJson = {};

                    repoJson["name"] = repo.name;
                    repoJson["path"] = repo.path;
                    repoJson["authType"] = repo.authType;
                    repoJson["username"] = repo.username;
                    repoJson["ssh_path"] = repo.ssh_path;
                    repoJson["active"] = repo.active;

                    ret.push_back( repoJson );
                }

                return ret;
            },
            [&]( const nlohmann::json& aJson )
            {
                if( !aJson.is_array() )
                    return;

                m_Git.repositories.clear();

                for( const auto& repoJson : aJson )
                {
                    GIT_REPOSITORY repo;

                    repo.name = repoJson["name"].get<QString>();
                    repo.path = repoJson["path"].get<QString>();
                    repo.authType = repoJson["authType"].get<QString>();
                    repo.username = repoJson["username"].get<QString>();
                    repo.ssh_path = repoJson["ssh_path"].get<QString>();
                    repo.active = repoJson["active"].get<bool>();
                    repo.checkValid = true;

                    m_Git.repositories.push_back( repo );
                }
            },
            {} ) );

    m_params.emplace_back( new PARAM<QString>( "git.authorName",
            &m_Git.authorName, "" ) );

    m_params.emplace_back( new PARAM<QString>( "git.authorEmail",
            &m_Git.authorEmail, "" ) );

    m_params.emplace_back( new PARAM<bool>( "git.useDefaultAuthor",
            &m_Git.useDefaultAuthor, true ) );

    m_params.emplace_back( new PARAM<bool>( "git.enableGit",
            &m_Git.enableGit, true ) );

    m_params.emplace_back( new PARAM<int>( "git.updatInterval",
            &m_Git.updatInterval, 5 ) );

    m_params.emplace_back( new PARAM<QString>( "api.interpreter_path",
            &m_Api.python_interpreter, "" ) );

    m_params.emplace_back( new PARAM<bool>( "api.enable_server",
            &m_Api.enable_server, false ) );

    registerMigration( 0, 1, std::bind( &COMMON_SETTINGS::migrateSchema0to1, this ) );
    registerMigration( 1, 2, std::bind( &COMMON_SETTINGS::migrateSchema1to2, this ) );
    registerMigration( 2, 3, std::bind( &COMMON_SETTINGS::migrateSchema2to3, this ) );
}


bool COMMON_SETTINGS::migrateSchema0to1()
{
    /**
     * Schema version 0 to 1:
     *
     * mousewheel_pan is replaced by explicit settings for scroll wheel behavior
     */

    nlohmann::json::json_pointer mwp_pointer( "/input/mousewheel_pan"_json_pointer );

    bool mwp = false;

    try
    {
        mwp = m_internals->at( mwp_pointer );
        m_internals->At( "input" ).erase( "mousewheel_pan" );
    }
    catch( ... )
    {
        qDebug() << "COMMON_SETTINGS::Migrate 0->1: mousewheel_pan not found";
    }

    if( mwp )
    {
        ( *m_internals )[nlohmann::json::json_pointer( "/input/horizontal_pan" )] = true;

        ( *m_internals )[nlohmann::json::json_pointer( "/input/scroll_modifier_pan_h" )] =
                Qt::Key_Shift;
        ( *m_internals )[nlohmann::json::json_pointer( "/input/scroll_modifier_pan_v" )] = 0;
        ( *m_internals )[nlohmann::json::json_pointer( "/input/scroll_modifier_zoom" )] =
                Qt::Key_Control;
    }
    else
    {
        ( *m_internals )[nlohmann::json::json_pointer( "/input/horizontal_pan" )] = false;

        ( *m_internals )[nlohmann::json::json_pointer( "/input/scroll_modifier_pan_h" )] =
                Qt::Key_Control;
        ( *m_internals )[nlohmann::json::json_pointer( "/input/scroll_modifier_pan_v" )] =
                Qt::Key_Shift;
        ( *m_internals )[nlohmann::json::json_pointer( "/input/scroll_modifier_zoom" )] = 0;
    }

    return true;
}


bool COMMON_SETTINGS::migrateSchema1to2()
{
    nlohmann::json::json_pointer v1_pointer( "/input/prefer_select_to_drag"_json_pointer );

    bool prefer_selection = false;

    try
    {
        prefer_selection = m_internals->at( v1_pointer );
        m_internals->at( nlohmann::json::json_pointer( "/input"_json_pointer ) )
                .erase( "prefer_select_to_drag" );
    }
    catch( ... )
    {
        qDebug() << "COMMON_SETTINGS::Migrate 1->2: prefer_select_to_drag not found";
    }

    if( prefer_selection )
        ( *m_internals )[nlohmann::json::json_pointer( "/input/mouse_left" )] =
                MOUSE_DRAG_ACTION::SELECT;
    else
        ( *m_internals )[nlohmann::json::json_pointer( "/input/mouse_left" )] =
                MOUSE_DRAG_ACTION::DRAG_ANY;

    return true;
}


bool COMMON_SETTINGS::migrateSchema2to3()
{
    QDir cfgdir( PATHS::GetUserSettingsPath() );
    cfgdir.cd( "3d" );
    QString cfgpath = cfgdir.absoluteFilePath( "3Dresolver.cfg" );

    std::vector<LEGACY_3D_SEARCH_PATH> legacyPaths;
    readLegacy3DResolverCfg( cfgpath, legacyPaths );

    // env variables have a limited allowed character set for names
    QRegularExpression nonValidCharsRegex( "[^A-Z0-9_]+" );

    for( const LEGACY_3D_SEARCH_PATH& path : legacyPaths )
    {
        QString key = path.m_Alias;
        const QString& val = path.m_Pathvar;

        // The 3d alias config didn't use the same naming restrictions as real env variables
        // We need to sanitize them

        // upper case only
        key = key.toUpper();

        // logically swap - with _
        key.replace( "-", "_" );

        // remove any other chars
        key.remove( nonValidCharsRegex );

        if( !m_Env.vars.count( key ) )
        {
            qDebug() << "COMMON_SETTINGS: Loaded new var:" << key << "=" << val;
            m_Env.vars[key] = ENV_VAR_ITEM( key, val );
        }
    }

    if( QFile::exists( cfgpath ) )
    {
        QFile::remove( cfgpath );
    }

    return true;
}



void COMMON_SETTINGS::InitializeEnvironment()
{
    auto addVar =
        [&]( const QString& aKey, const QString& aDefault )
        {
            m_Env.vars[aKey] = ENV_VAR_ITEM( aKey, aDefault, aDefault );

            QString envValue;

            envValue = QString::fromLocal8Bit( qgetenv( aKey.toLocal8Bit() ) );
            if( !envValue.isEmpty() )
            {
                m_Env.vars[aKey].SetValue( envValue );
                m_Env.vars[aKey].SetDefinedExternally();
                qDebug() << "InitializeEnvironment: Entry" << aKey << "defined externally as" << envValue;
            }
            else
            {
                qDebug() << "InitializeEnvironment: Setting entry" << aKey << "to default" << aDefault;
            }
        };

    QString basePath = PATHS::GetStockEDALibraryPath();

    QDir footprintDir( basePath );
    footprintDir.cd( "footprints" );
    addVar( ENV_VAR::GetVersionedEnvVarName( "FOOTPRINT_DIR" ), footprintDir.absolutePath() );

    QDir modelsDir( basePath );
    modelsDir.cd( "3dmodels" );
    addVar( ENV_VAR::GetVersionedEnvVarName( "3DMODEL_DIR" ), modelsDir.absolutePath() );

    addVar( ENV_VAR::GetVersionedEnvVarName( "TEMPLATE_DIR" ),
            PATHS::GetStockTemplatesPath() );

    addVar( "KICAD_USER_TEMPLATE_DIR", PATHS::GetUserTemplatesPath() );

    addVar( ENV_VAR::GetVersionedEnvVarName( "3RD_PARTY" ),
            PATHS::GetDefault3rdPartyPath() );

    QDir symbolDir( basePath );
    symbolDir.cd( "symbols" );
    addVar( ENV_VAR::GetVersionedEnvVarName( "SYMBOL_DIR" ), symbolDir.absolutePath() );

    QDir blocksDir( basePath );
    blocksDir.cd( "blocks" );
    addVar( ENV_VAR::GetVersionedEnvVarName( "DESIGN_BLOCK_DIR" ), blocksDir.absolutePath() );
}


bool COMMON_SETTINGS::readLegacy3DResolverCfg( const QString&                   path,
                                               std::vector<LEGACY_3D_SEARCH_PATH>& aSearchPaths )
{
    QFileInfo cfgpath( path );

    // Normalize path - Qt handles this automatically
    QString cfgname = cfgpath.absoluteFilePath();

    std::ifstream cfgFile;
    std::string   cfgLine;

    if( !QFile::exists( cfgname ) )
    {
        std::ostringstream ostr;
        ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        QString errmsg = "no 3D configuration file";
        ostr << " * " << errmsg.toUtf8().constData() << " '";
        ostr << cfgname.toUtf8().constData() << "'";
        qDebug() << QString::fromStdString( ostr.str() );
        return false;
    }

    cfgFile.open( cfgname.toUtf8().constData() );

    if( !cfgFile.is_open() )
    {
        std::ostringstream ostr;
        ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        QString errmsg = "Could not open configuration file";
        ostr << " * " << errmsg.toUtf8().constData() << " '" << cfgname.toUtf8().constData() << "'";
        qDebug() << QString::fromStdString( ostr.str() );
        return false;
    }

    int         lineno = 0;
    LEGACY_3D_SEARCH_PATH al;
    size_t      idx;
    int         vnum = 0; // version number

    while( cfgFile.good() )
    {
        cfgLine.clear();
        std::getline( cfgFile, cfgLine );
        ++lineno;

        if( cfgLine.empty() )
        {
            if( cfgFile.eof() )
                break;

            continue;
        }

        if( 1 == lineno && cfgLine.compare( 0, 2, "#V" ) == 0 )
        {
            // extract the version number and parse accordingly
            if( cfgLine.size() > 2 )
            {
                std::istringstream istr;
                istr.str( cfgLine.substr( 2 ) );
                istr >> vnum;
            }

            continue;
        }

        idx = 0;

        if( !getLegacy3DHollerith( cfgLine, idx, al.m_Alias ) )
            continue;

        // Don't add KICADn_3DMODEL_DIR, one of its legacy equivalents, or KIPRJMOD from a
        // config file.  They're system variables which are defined at runtime.
        QString versionedPath = QString( "${%1}" )
                                       .arg( ENV_VAR::GetVersionedEnvVarName( "3DMODEL_DIR" ) );

        if( al.m_Alias == versionedPath || al.m_Alias == "${KIPRJMOD}"
            || al.m_Alias == "$(KIPRJMOD)" || al.m_Alias == "${KISYS3DMOD}"
            || al.m_Alias == "$(KISYS3DMOD)" )
        {
            continue;
        }

        if( !getLegacy3DHollerith( cfgLine, idx, al.m_Pathvar ) )
            continue;

        if( !getLegacy3DHollerith( cfgLine, idx, al.m_Description ) )
            continue;

        aSearchPaths.push_back( al );
    }

    cfgFile.close();

    return true;
}


bool COMMON_SETTINGS::getLegacy3DHollerith( const std::string& aString, size_t& aIndex,
                                            QString& aResult )
{
    aResult.clear();

    if( aIndex >= aString.size() )
    {
        std::ostringstream ostr;
        ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        QString errmsg = "bad Hollerith string on line";
        ostr << " * " << errmsg.toUtf8().constData() << "\n'" << aString << "'";
        qDebug() << QString::fromStdString( ostr.str() );

        return false;
    }

    size_t i2 = aString.find( '"', aIndex );

    if( std::string::npos == i2 )
    {
        std::ostringstream ostr;
        ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        QString errmsg = "missing opening quote mark in config file";
        ostr << " * " << errmsg.toUtf8().constData() << "\n'" << aString << "'";
        qDebug() << QString::fromStdString( ostr.str() );

        return false;
    }

    ++i2;

    if( i2 >= aString.size() )
    {
        std::ostringstream ostr;
        ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        QString errmsg = "invalid entry (unexpected end of line)";
        ostr << " * " << errmsg.toUtf8().constData() << "\n'" << aString << "'";
        qDebug() << QString::fromStdString( ostr.str() );

        return false;
    }

    std::string tnum;

    while( aString[i2] >= '0' && aString[i2] <= '9' )
        tnum.append( 1, aString[i2++] );

    if( tnum.empty() || aString[i2++] != ':' )
    {
        std::ostringstream ostr;
        ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        QString errmsg = "bad Hollerith string on line";
        ostr << " * " << errmsg.toUtf8().constData() << "\n'" << aString << "'";
        qDebug() << QString::fromStdString( ostr.str() );

        return false;
    }

    std::istringstream istr;
    istr.str( tnum );
    size_t nchars;
    istr >> nchars;

    if( ( i2 + nchars ) >= aString.size() )
    {
        std::ostringstream ostr;
        ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        QString errmsg = "invalid entry (unexpected end of line)";
        ostr << " * " << errmsg.toUtf8().constData() << "\n'" << aString << "'";
        qDebug() << QString::fromStdString( ostr.str() );

        return false;
    }

    if( nchars > 0 )
    {
        aResult = QString::fromUtf8( aString.substr( i2, nchars ).c_str() );
        i2 += nchars;
    }

    if( i2 >= aString.size() || aString[i2] != '"' )
    {
        std::ostringstream ostr;
        ostr << __FILE__ << ": " << __FUNCTION__ << ": " << __LINE__ << "\n";
        QString errmsg = "missing closing quote mark in config file";
        ostr << " * " << errmsg.toUtf8().constData() << "\n'" << aString << "'";
        qDebug() << QString::fromStdString( ostr.str() );

        return false;
    }

    aIndex = i2 + 1;
    return true;
}
