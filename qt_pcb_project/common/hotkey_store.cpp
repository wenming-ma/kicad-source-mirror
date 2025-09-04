
#include <hotkey_store.h>
#include <eda_base_frame.h>
#include <tool/tool_manager.h>
#include <tool/action_manager.h>
#include <tool/tool_event.h>
#include <tool/tool_action.h>
#include <advanced_config.h>
#include <map>
#include <vector>

class PSEUDO_ACTION : public TOOL_ACTION
{
public:
    PSEUDO_ACTION( const QString& aLabel, int aHotKey, int aHotKeyAlt = 0 )
    {
        m_friendlyName = aLabel;
        m_hotKey = aHotKey;
        m_hotKeyAlt = aHotKeyAlt;
    }
};

static PSEUDO_ACTION* g_gesturePseudoActions[] = {
    new PSEUDO_ACTION( _( "Accept Autocomplete" ), WXK_RETURN, WXK_NUMPAD_ENTER ),
    new PSEUDO_ACTION( _( "Cancel Autocomplete" ), WXK_ESCAPE ),
    new PSEUDO_ACTION( _( "Toggle Checkbox" ), WXK_SPACE ),
    new PSEUDO_ACTION( _( "Pan Left/Right" ), MD_CTRL + PSEUDO_WXK_WHEEL ),
    new PSEUDO_ACTION( _( "Pan Up/Down" ), MD_SHIFT + PSEUDO_WXK_WHEEL ),
    new PSEUDO_ACTION( _( "Finish Drawing" ), PSEUDO_WXK_DBLCLICK ),
    new PSEUDO_ACTION( _( "Add to Selection" ), MD_SHIFT + PSEUDO_WXK_CLICK ),
    new PSEUDO_ACTION( _( "Highlight Net" ), MD_CTRL + PSEUDO_WXK_CLICK ),
    new PSEUDO_ACTION( _( "Remove from Selection" ), MD_CTRL + MD_SHIFT + PSEUDO_WXK_CLICK ),
    new PSEUDO_ACTION( _( "Ignore Grid Snaps" ), MD_CTRL ),
    new PSEUDO_ACTION( _( "Ignore Other Snaps" ), MD_SHIFT ),
};

static PSEUDO_ACTION* g_standardPlatformCommands[] = {
#ifndef __WINDOWS__
    new PSEUDO_ACTION( _( "Close" ), MD_CTRL + 'W' ),
#endif
    new PSEUDO_ACTION( _( "Quit" ), MD_CTRL + 'Q' )
};


QString HOTKEY_STORE::GetAppName( TOOL_ACTION* aAction )
{
    QString name( aAction->GetName() );
    return name.left( name.indexOf( '.' ) );
}


QString HOTKEY_STORE::GetSectionName( TOOL_ACTION* aAction )
{
    std::map<QString, QString> s_AppNames = {
            { QStringLiteral( "common" ),   _( "Common" ) },
            { QStringLiteral( "kicad" ),    _( "Project Manager" ) },
            { QStringLiteral( "eeschema" ), _( "Schematic Editor" ) },
            { QStringLiteral( "pcbnew" ),   _( "PCB Editor" ) },
            { QStringLiteral( "plEditor" ), _( "Drawing Sheet Editor" ), },
            { QStringLiteral( "3DViewer" ), _( "3D Viewer" ) },
            { QStringLiteral( "gerbview" ), _( "Gerber Viewer" ) }
    };

    QString appName = GetAppName( aAction );

    if( s_AppNames.count( appName ) )
        return s_AppNames[ appName ];
    else
        return appName;
}


HOTKEY_STORE::HOTKEY_STORE()
{
}


void HOTKEY_STORE::Init( std::vector<TOOL_ACTION*> aActionsList, bool aIncludeReadOnlyCmds )
{
    std::map<std::string, HOTKEY> masterMap;

    for( TOOL_ACTION* action : aActionsList )
    {
        // Internal actions probably shouldn't be allowed hotkeys
        if( action->GetFriendlyName().IsEmpty() )
            continue;

        if( !ADVANCED_CFG::GetCfg().m_ExtraZoneDisplayModes )
        {
            if( action->GetName() == "pcbnew.Control.zoneDisplayOutlines"
                    || action->GetName() == "pcbnew.Control.zoneDisplayTesselation" )
            {
                continue;
            }
        }

        HOTKEY& hotkey = masterMap[ action->GetName() ];
        hotkey.m_Actions.push_back( action );

        if( !hotkey.m_EditKeycode )
        {
            hotkey.m_EditKeycode = action->GetHotKey();
            hotkey.m_EditKeycodeAlt = action->GetHotKeyAlt();
        }
    }

    QString        currentApp;
    HOTKEY_SECTION* currentSection = nullptr;

    // If a previous list was built, ensure this previous list is cleared:
    m_hk_sections.clear();

    for( const std::pair<const std::string, HOTKEY>& entry : masterMap )
    {
        TOOL_ACTION* entryAction = entry.second.m_Actions[ 0 ];
        QString     entryApp = GetAppName( entryAction );

        if( !currentSection || entryApp != currentApp )
        {
            m_hk_sections.emplace_back( HOTKEY_SECTION() );
            currentApp = entryApp;
            currentSection = &m_hk_sections.back();
            currentSection->m_SectionName = GetSectionName( entryAction );

            if( aIncludeReadOnlyCmds && currentApp == "common" )
            {
                for( TOOL_ACTION* command : g_standardPlatformCommands )
                    currentSection->m_HotKeys.emplace_back( HOTKEY( command ) );
            }
        }

        currentSection->m_HotKeys.emplace_back( HOTKEY( entry.second ) );
    }

    if( aIncludeReadOnlyCmds )
    {
        m_hk_sections.emplace_back( HOTKEY_SECTION() );
        currentSection = &m_hk_sections.back();
        currentSection->m_SectionName = _( "Gestures" );

        for( TOOL_ACTION* gesture : g_gesturePseudoActions )
            currentSection->m_HotKeys.emplace_back( HOTKEY( gesture ) );
    }
}


std::vector<HOTKEY_SECTION>& HOTKEY_STORE::GetSections()
{
    return m_hk_sections;
}


void HOTKEY_STORE::SaveAllHotkeys()
{
    for( HOTKEY_SECTION& section : m_hk_sections )
    {
        for( HOTKEY& hotkey : section.m_HotKeys )
        {
            for( TOOL_ACTION* action : hotkey.m_Actions )
                action->SetHotKey( hotkey.m_EditKeycode, hotkey.m_EditKeycodeAlt );
        }
    }
}


void HOTKEY_STORE::ResetAllHotkeysToDefault()
{
    for( HOTKEY_SECTION& section : m_hk_sections )
    {
        for( HOTKEY& hotkey : section.m_HotKeys )
        {
            hotkey.m_EditKeycode    = hotkey.m_Actions[ 0 ]->GetDefaultHotKey();
            hotkey.m_EditKeycodeAlt = hotkey.m_Actions[ 0 ]->GetDefaultHotKeyAlt();
        }
    }
}


void HOTKEY_STORE::ResetAllHotkeysToOriginal()
{
    for( HOTKEY_SECTION& section : m_hk_sections )
    {
        for( HOTKEY& hotkey : section.m_HotKeys )
        {
            hotkey.m_EditKeycode    = hotkey.m_Actions[ 0 ]->GetHotKey();
            hotkey.m_EditKeycodeAlt = hotkey.m_Actions[ 0 ]->GetHotKeyAlt();
        }
    }
}


bool HOTKEY_STORE::CheckKeyConflicts( TOOL_ACTION* aAction, long aKey, HOTKEY** aConflict )
{
    QString sectionName = GetSectionName( aAction );

    // Create a fake "TOOL_ACTION" so we can get the section name for "Common" through the API.
    // Simply declaring a QString with the value "Common" works, but the goal is to futureproof
    // the code here as much as possible.
    TOOL_ACTION commonAction( TOOL_ACTION_ARGS().Name( "common.Control.Fake" ).Scope( AS_GLOBAL ) );
    QString    commonName = GetSectionName( &commonAction );

    for( HOTKEY_SECTION& section : m_hk_sections )
    {
        // We can have the same hotkey in multiple sections (i.e. Kicad programs), but if a hotkey
        // is in "Common" it can't be in any other section and vice versa.

        if( !( section.m_SectionName == sectionName || section.m_SectionName == commonName ) )
            continue;

        for( HOTKEY& hotkey : section.m_HotKeys )
        {
            if( hotkey.m_Actions[0] == aAction )
                continue;

            if( hotkey.m_EditKeycode == aKey || hotkey.m_EditKeycodeAlt == aKey )
            {
                // We can use the same key for a different action if both actions are contextual and
                // for different tools.
                if( hotkey.m_Actions[0]->GetScope() == AS_CONTEXT &&
                    aAction->GetScope() == AS_CONTEXT &&
                    hotkey.m_Actions[0]->GetToolName() != aAction->GetToolName() )
                {
                    continue;
                }

                *aConflict = &hotkey;
                return true;
            }
        }
    }

    return false;
}
