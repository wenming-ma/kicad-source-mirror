
#include "settings/kicad_settings.h"
#include <json_common.h>
#include <settings/aui_settings.h>
#include <settings/parameters.h>


///! Update the schema version whenever a migration is required
const int kicadSchemaVersion = 0;


const nlohmann::json PCM_DEFAULT_REPOSITORIES = nlohmann::json::array( {
    nlohmann::json( {
        { "name", "KiCad official repository" },
        { "url", PCM_DEFAULT_REPOSITORY_URL },
    } )
} );


KICAD_SETTINGS::KICAD_SETTINGS() :
        APP_SETTINGS_BASE( "kicad", kicadSchemaVersion ), m_LeftWinWidth( 200 )
{
    m_params.emplace_back( new PARAM<int>( "appearance.left_frame_width", &m_LeftWinWidth, 200 ) );

    m_params.emplace_back(
            new PARAM_LIST<QString>( "system.open_projects", &m_OpenProjects, {} ) );

    m_params.emplace_back( new PARAM<QString>( "system.last_design_block_lib_dir",
                                                &m_lastDesignBlockLibDir, "" ) );

    m_params.emplace_back(
            new PARAM<QString>( "system.last_update_check_time", &m_lastUpdateCheckTime, "" ) );

    m_params.emplace_back(
            new PARAM<QString>( "system.last_received_update", &m_lastReceivedUpdate, "" ) );

    m_params.emplace_back( new PARAM<bool>( "system.check_for_kicad_updates", &m_KiCadUpdateCheck,
                                            true ) );

    m_params.emplace_back( new PARAM<QPoint>( "template.window.pos", &m_TemplateWindowPos,
                                               QPoint() ) );

    m_params.emplace_back( new PARAM<QSize>( "template.window.size", &m_TemplateWindowSize,
                                              QSize() ) );

    m_params.emplace_back( new PARAM_LAMBDA<nlohmann::json>(
            "pcm.repositories",
            [&]() -> nlohmann::json
            {
                nlohmann::json js = nlohmann::json::array();

                for( const auto& pair : m_PcmRepositories )
                {
                    js.push_back( nlohmann::json( { { "name", pair.first.ToUTF8() },
                                                    { "url", pair.second.ToUTF8() } } ) );
                }

                return js;
            },
            [&]( const nlohmann::json aObj )
            {
                m_PcmRepositories.clear();

                if( !aObj.is_array() )
                    return;

                for( const auto& entry : aObj )
                {
                    if( entry.empty() || !entry.is_object() )
                        continue;

                    m_PcmRepositories.emplace_back(
                            std::make_pair( QString::fromStdString( entry["name"].get<std::string>() ),
                                            QString::fromStdString( entry["url"].get<std::string>() ) ) );
                }
            },
            PCM_DEFAULT_REPOSITORIES ) );

    m_params.emplace_back(
            new PARAM<QString>( "pcm.last_download_dir", &m_PcmLastDownloadDir, "" ) );

    m_params.emplace_back( new PARAM<bool>( "pcm.check_for_updates", &m_PcmUpdateCheck, true ) );

    m_params.emplace_back( new PARAM<bool>( "pcm.lib_auto_add", &m_PcmLibAutoAdd, true ) );

    m_params.emplace_back( new PARAM<bool>( "pcm.lib_auto_remove", &m_PcmLibAutoRemove, true ) );

    m_params.emplace_back( new PARAM<QString>( "pcm.lib_prefix", &m_PcmLibPrefix,
                                                "PCM_" ) );
}


bool KICAD_SETTINGS::MigrateFromLegacy( QSettings* aCfg )
{
    bool ret = APP_SETTINGS_BASE::MigrateFromLegacy( aCfg );

    ret &= fromLegacy<int>( aCfg, "LeftWinWidth", "appearance.left_frame_width" );

    // Override the size parameters to ensure the new PCM button is always shown.
    // This will make the window take the default size instead of the migrated one.
    Set( "window.size_x", 0 );
    Set( "window.size_y", 0 );

    return ret;
}
