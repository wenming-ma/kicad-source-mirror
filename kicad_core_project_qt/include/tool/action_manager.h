// QT_TRANSFORMATION_COMPLETED

#ifndef ACTION_MANAGER_H_
#define ACTION_MANAGER_H_

#include <list>
#include <map>
#include <string>
#include <set>

#include <tool/selection_conditions.h>

class TOOL_BASE;
class TOOL_MANAGER;
class TOOL_ACTION;

struct ACTION_CONDITIONS
{
    ACTION_CONDITIONS()
    {
        checkCondition  = SELECTION_CONDITIONS::ShowNever;      // Never check by default
        enableCondition = SELECTION_CONDITIONS::ShowAlways;     // Always enable by default
        showCondition   = SELECTION_CONDITIONS::ShowAlways;     // Always show by default
    }

    ACTION_CONDITIONS& Check( const SELECTION_CONDITION& aCondition )
    {
        checkCondition = aCondition;
        return *this;
    }

    ACTION_CONDITIONS& Enable( const SELECTION_CONDITION& aCondition )
    {
        enableCondition = aCondition;
        return *this;
    }

    ACTION_CONDITIONS& Show( const SELECTION_CONDITION& aCondition )
    {
        showCondition = aCondition;
        return *this;
    }

    SELECTION_CONDITION checkCondition;
    SELECTION_CONDITION enableCondition;
    SELECTION_CONDITION showCondition;
};

class ACTION_MANAGER
{
public:
    ACTION_MANAGER( TOOL_MANAGER* aToolManager );

    ~ACTION_MANAGER();

    void RegisterAction( TOOL_ACTION* aAction );

    static int MakeActionId( const std::string& aActionName );

    const std::map<std::string, TOOL_ACTION*>& GetActions() const;

    bool IsActionUIId( int aId ) const;

    TOOL_ACTION* FindAction( const std::string& aActionName ) const;

    bool RunHotKey( int aHotKey ) const;

    int GetHotKey( const TOOL_ACTION& aAction ) const;

    void UpdateHotKeys( bool aFullUpdate );
    static std::list<TOOL_ACTION*>& GetActionList()
    {
        static std::list<TOOL_ACTION*> actionList;

        return actionList;
    }

    void SetConditions( const TOOL_ACTION& aAction, const ACTION_CONDITIONS& aConditions );

    const ACTION_CONDITIONS* GetCondition( const TOOL_ACTION& aAction ) const;

private:
    // Resolve a hotkey by applying legacy and current settings over the action's
    // default hotkey.
    void processHotKey( TOOL_ACTION* aAction, const std::map<std::string, int>& aLegacyMap,
                        const std::map<std::string, std::pair<int, int>>& aHotKeyMap );

    TOOL_MANAGER* m_toolMgr;

    std::map<std::string, TOOL_ACTION*> m_actionNameIndex;

    std::map<int, TOOL_ACTION*> m_customUIIdIndex;

    typedef std::map<int, std::list<TOOL_ACTION*> > HOTKEY_LIST;
    HOTKEY_LIST m_actionHotKeys;

    std::map<int, int> m_hotkeys;

    std::map<int, ACTION_CONDITIONS> m_uiConditions;
};

#endif /* ACTION_MANAGER_H_ */
