#include <eda_draw_frame.h>
#include <tool/action_manager.h>
#include <tool/tool_action.h>
#include <tool/tool_manager.h>
#include <trace_helpers.h>
#include <QDebug>

#include <hotkeys_basic.h>
#include <cctype>


ACTION_MANAGER::ACTION_MANAGER( TOOL_MANAGER* aToolManager ) :
    m_toolMgr( aToolManager )
{
    // Register known actions
    std::list<TOOL_ACTION*>& actionList = GetActionList();

    for( TOOL_ACTION* action : actionList )
    {
        if( action->m_id == -1 )
            action->m_id = MakeActionId( action->m_name );

        int         groupID   = 0;
        std::string groupName = "none";

        std::optional<TOOL_ACTION_GROUP> group = action->GetActionGroup();

        if( group.has_value() )
        {
            groupID   = group.value().GetGroupID();
            groupName = group.value().GetName();
        }

        qDebug() << QString("ACTION_MANAGER::ACTION_MANAGER: Registering action %1 with ID %2, UI ID %3, and group %4(%5)")
                        .arg(QString::fromStdString(action->m_name))
                        .arg(action->m_id)
                        .arg(action->GetUIId())
                        .arg(QString::fromStdString(groupName))
                        .arg(groupID);

        RegisterAction( action );
    }
}


ACTION_MANAGER::~ACTION_MANAGER()
{
}


void ACTION_MANAGER::RegisterAction( TOOL_ACTION* aAction )
{
    // TOOL_ACTIONs are supposed to be named [appName.]toolName.actionName (with dots between)
    // action name without specifying at least toolName is not valid
    Q_ASSERT( aAction->GetName().find( '.', 0 ) != std::string::npos );

    // TOOL_ACTIONs must have unique names & ids
    Q_ASSERT( m_actionNameIndex.find( aAction->m_name ) == m_actionNameIndex.end() );

    m_actionNameIndex[aAction->m_name] = aAction;

    if( aAction->HasCustomUIId() )
        m_customUIIdIndex[aAction->GetUIId()] = aAction;
}


void ACTION_MANAGER::SetConditions( const TOOL_ACTION& aAction,
                                    const ACTION_CONDITIONS& aConditions )
{
    // Remove any existing handlers with the old conditions to ensure the UI layer doesn't have
    // stale data.
    if( m_toolMgr )
        m_toolMgr->GetToolHolder()->UnregisterUIUpdateHandler( aAction );

    m_uiConditions[aAction.GetId()] = aConditions;

    qDebug() << QString("ACTION_MANAGER::SetConditions: Registering conditions for ID %1 - %2")
                    .arg(aAction.GetId())
                    .arg(QString::fromStdString(aAction.GetName()));

    // Register a new handler with the new conditions
    if( m_toolMgr )
        m_toolMgr->GetToolHolder()->RegisterUIUpdateHandler( aAction, aConditions );
}


const ACTION_CONDITIONS* ACTION_MANAGER::GetCondition( const TOOL_ACTION& aAction ) const
{
    const auto it = m_uiConditions.find( aAction.GetId() );

    // If the action doesn't have something registered, then return null
    if( it == m_uiConditions.end() )
        return nullptr;
    else
        return &it->second;
}


int ACTION_MANAGER::MakeActionId( const std::string& aActionName )
{
    static int currentActionId = 1;

    return currentActionId++;
}


TOOL_ACTION* ACTION_MANAGER::FindAction( const std::string& aActionName ) const
{
    std::map<std::string, TOOL_ACTION*>::const_iterator it = m_actionNameIndex.find( aActionName );

    if( it != m_actionNameIndex.end() )
        return it->second;

    return nullptr;
}


bool ACTION_MANAGER::RunHotKey( int aHotKey ) const
{
    int key = aHotKey & ~MD_MODIFIER_MASK;
    int mod = aHotKey & MD_MODIFIER_MASK;

    if( key >= 'a' && key <= 'z' )
        key = std::toupper( key );

    qDebug() << QString("ACTION_MANAGER::RunHotKey Key: %1")
                    .arg(QString::fromStdString(KeyNameFromKeyCode( aHotKey )));

    HOTKEY_LIST::const_iterator it = m_actionHotKeys.find( key | mod );

    // If no luck, try without Shift, to handle keys that require it
    // e.g. to get ? you need to press Shift+/ without US keyboard layout
    // Hardcoding ? as Shift+/ is a bad idea, as on another layout you may need to press a
    // different combination.
    // This doesn't apply for letters, as we already handled case normalisation.
    if( it == m_actionHotKeys.end() && !std::isalpha( key ) )
    {
        qDebug() << QString("ACTION_MANAGER::RunHotKey No actions found, searching with key: %1")
                        .arg(QString::fromStdString(KeyNameFromKeyCode( key | ( mod & ~MD_SHIFT ) )));

        it = m_actionHotKeys.find( key | ( mod & ~MD_SHIFT ) );
    }

    // Still no luck, we're done without a match
    if( it == m_actionHotKeys.end() )
        return false; // no appropriate action found for the hotkey

    const std::list<TOOL_ACTION*>& actions = it->second;

    // Choose the action that has the highest priority on the active tools stack
    // If there is none, run the global action associated with the hot key
    int highestPriority = -1, priority = -1;
    const TOOL_ACTION* context = nullptr; // pointer to context action of the highest priority tool
    std::vector<const TOOL_ACTION*> global; // pointers to global actions
                                            // if there is no context action

    for( const TOOL_ACTION* action : actions )
    {
        if( action->GetScope() == AS_GLOBAL )
        {
            // Store the global action in case there are no context actions to run
            global.emplace_back( action );
            continue;
        }

        TOOL_BASE* tool = m_toolMgr->FindTool( action->GetToolName() );

        if( tool )
        {
            // Choose the action that goes to the tool with highest priority
            // (i.e. is on the top of active tools stack)
            priority = m_toolMgr->GetPriority( tool->GetId() );

            if( priority >= 0 && priority > highestPriority )
            {
                highestPriority = priority;
                context = action;
            }
        }
    }

    // Get the selection to use to test if the action is enabled
    SELECTION& sel = m_toolMgr->GetToolHolder()->GetCurrentSelection();

    if( context )
    {
        bool runAction = true;

        if( const ACTION_CONDITIONS* aCond = GetCondition( *context ) )
            runAction = aCond->enableCondition( sel );

        qDebug() << QString("ACTION_MANAGER::RunHotKey %1 context action: %2 for hotkey %3")
                        .arg(runAction ? "Running" : "Not running")
                        .arg(QString::fromStdString(context->GetName()))
                        .arg(QString::fromStdString(KeyNameFromKeyCode( aHotKey )));

        if( runAction )
            return m_toolMgr->RunAction( *context );
    }
    else if( !global.empty() )
    {
        for( const TOOL_ACTION* act : global )
        {
            bool runAction = true;

            if( const ACTION_CONDITIONS* aCond = GetCondition( *act ) )
                runAction = aCond->enableCondition( sel );

            qDebug() << QString("ACTION_MANAGER::RunHotKey %1 global action: %2 for hotkey %3")
                            .arg(runAction ? "Running" : "Not running")
                            .arg(QString::fromStdString(act->GetName()))
                            .arg(QString::fromStdString(KeyNameFromKeyCode( aHotKey )));

            if( runAction && m_toolMgr->RunAction( *act ) )
                return true;
        }
    }

    qDebug() << QString("ACTION_MANAGER::RunHotKey No action found for key %1")
                    .arg(QString::fromStdString(KeyNameFromKeyCode( aHotKey )));

    return false;
}


bool ACTION_MANAGER::IsActionUIId( int aId ) const
{
    // Automatically assigned IDs are always in this range
    if( aId >= TOOL_ACTION::GetBaseUIId() )
        return true;

    // Search the custom assigned UI IDs
    auto it = m_customUIIdIndex.find( aId );

    return ( it != m_customUIIdIndex.end() );
}


const std::map<std::string, TOOL_ACTION*>& ACTION_MANAGER::GetActions() const
{
    return m_actionNameIndex;
}


int ACTION_MANAGER::GetHotKey( const TOOL_ACTION& aAction ) const
{
    std::map<int, int>::const_iterator it = m_hotkeys.find( aAction.GetId() );

    if( it == m_hotkeys.end() )
        return 0;

    return it->second;
}


void ACTION_MANAGER::UpdateHotKeys( bool aFullUpdate )
{
    static std::map<std::string, int>                 legacyHotKeyMap;
    static std::map<std::string, std::pair<int, int>> userHotKeyMap;
    static bool                                       mapsInitialized = false;

    m_actionHotKeys.clear();
    m_hotkeys.clear();

    if( m_toolMgr->GetToolHolder() && ( aFullUpdate || !mapsInitialized ) )
    {
        ReadLegacyHotkeyConfig( m_toolMgr->GetToolHolder()->ConfigBaseName(), legacyHotKeyMap );
        ReadHotKeyConfig( QString(), userHotKeyMap );
        mapsInitialized = true;
    }

    for( const auto& ii : m_actionNameIndex )
    {
        TOOL_ACTION* action = ii.second;
        int          hotkey = 0;
        int          alt    = 0;

        if( aFullUpdate )
            processHotKey( action, legacyHotKeyMap, userHotKeyMap );

        hotkey = action->GetHotKey();
        alt    = action->GetHotKeyAlt();

        if( hotkey > 0 )
            m_actionHotKeys[hotkey].push_back( action );

        if( alt > 0 )
            m_actionHotKeys[alt].push_back( action );

        m_hotkeys[action->GetId()] = hotkey;
    }
}


void ACTION_MANAGER::processHotKey( TOOL_ACTION*                                      aAction,
                                    const std::map<std::string, int>&                 aLegacyMap,
                                    const std::map<std::string, std::pair<int, int>>& aHotKeyMap )
{
    aAction->m_hotKey = aAction->m_defaultHotKey;

    if( !aAction->m_legacyName.empty() && aLegacyMap.count( aAction->m_legacyName ) )
        aAction->SetHotKey( aLegacyMap.at( aAction->m_legacyName ) );

    if( aHotKeyMap.count( aAction->m_name ) )
    {
        std::pair<int, int> keys = aHotKeyMap.at( aAction->m_name );
        aAction->SetHotKey( keys.first, keys.second );
    }
}
