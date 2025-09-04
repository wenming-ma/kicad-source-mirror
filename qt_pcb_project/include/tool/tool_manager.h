#ifndef TOOL_MANAGER_H
#define TOOL_MANAGER_H

#include <list>
#include <map>
#include <vector>
#include <typeinfo>
#include <type_traits>

#include <tool/tool_base.h>
#include <tool/tool_event.h>

namespace KIGFX
{
class VIEW_CONTROLS;
struct VC_SETTINGS;
}

class COMMIT;
class TOOLS_HOLDER;
class TOOL_ACTION;
class ACTION_MANAGER;
class ACTION_MENU;
class APP_SETTINGS_BASE;


// Master controller class:
// - registers editing tools
// - pumps UI events to tools requesting them
// - manages tool state machines (transitions and wait requests)
class TOOL_MANAGER
{
private:
    struct TOOL_STATE;

public:
    TOOL_MANAGER();

    ~TOOL_MANAGER();

    // Helper typedefs
    typedef std::map<TOOL_BASE*, TOOL_STATE*> TOOL_STATE_MAP;
    typedef std::map<std::string, TOOL_STATE*> NAME_STATE_MAP;
    typedef std::map<TOOL_ID, TOOL_STATE*> ID_STATE_MAP;
    typedef std::list<TOOL_ID> ID_LIST;

    static TOOL_ID MakeToolId( const std::string& aToolName );

    void RegisterTool( TOOL_BASE* aTool );

    bool InvokeTool( TOOL_ID aToolId );

    bool InvokeTool( const std::string& aToolName );

    void ShutdownAllTools();

    void ShutdownTool( TOOL_BASE* aTool );

    void ShutdownTool( TOOL_ID aToolId );

    void ShutdownTool( const std::string& aToolName );

    template<typename T, std::enable_if_t<!std::is_convertible_v<T*, COMMIT*>>* = nullptr>
    bool RunAction( const std::string& aActionName, T aParam )
    {
        // Use a cast to ensure the proper type is stored inside the parameter
        ki::any a( static_cast<T>( aParam ) );

        return doRunAction( aActionName, true, a, nullptr );
    }

    bool RunAction( const std::string& aActionName )
    {
        // Default initialize the parameter argument to an empty ki_any
        ki::any a;

        return doRunAction( aActionName, true, a, nullptr );
    }

    template<typename T, std::enable_if_t<!std::is_convertible_v<T, COMMIT*>>* = nullptr>
    bool RunAction( const TOOL_ACTION& aAction, T aParam )
    {
        // Use a cast to ensure the proper type is stored inside the parameter
        ki::any a( static_cast<T>( aParam ) );

        return doRunAction( aAction, true, a, nullptr );
    }

    template<typename T>
    bool RunSynchronousAction( const TOOL_ACTION& aAction, COMMIT* aCommit, T aParam )
    {
        // Use a cast to ensure the proper type is stored inside the parameter
        ki::any a( static_cast<T>( aParam ) );

        return doRunAction( aAction, true, a, aCommit );
    }

    bool RunSynchronousAction( const TOOL_ACTION& aAction, COMMIT* aCommit )
    {
        // Default initialize the parameter argument to an empty ki_any
        ki::any a;

        return doRunAction( aAction, true, a, aCommit );
    }

    bool RunAction( const TOOL_ACTION& aAction )
    {
        // Default initialize the parameter argument to an empty ki_any
        ki::any a;

        return doRunAction( aAction, true, a, nullptr );
    }

    template<typename T>
    bool PostAction( const std::string& aActionName, T aParam )
    {
        // Use a cast to ensure the proper type is stored inside the parameter
        ki::any a( static_cast<T>( aParam ) );

        return doRunAction( aActionName, false, a, nullptr );
    }

    bool PostAction( const std::string& aActionName )
    {
        // Default initialize the parameter argument to an empty ki_any
        ki::any a;

        return doRunAction( aActionName, false, a, nullptr );
    }

    template<typename T, std::enable_if_t<!std::is_convertible_v<T, COMMIT*>>* = nullptr>
    bool PostAction( const TOOL_ACTION& aAction, T aParam )
    {
        // Use a cast to ensure the proper type is stored inside the parameter
        ki::any a( static_cast<T>( aParam ) );

        return doRunAction( aAction, false, a, nullptr );
    }

    void PostAction( const TOOL_ACTION& aAction )
    {
        // Default initialize the parameter argument to an empty ki_any
        ki::any a;

        doRunAction( aAction, false, a, nullptr );
    }

    bool PostAPIAction( const TOOL_ACTION& aAction, COMMIT* aCommit )
    {
        // Default initialize the parameter argument to an empty ki_any
        ki::any a;

        return doRunAction( aAction, false, a, aCommit, true );
    }

    void CancelTool();

    void PrimeTool( const VECTOR2D& aPosition );

    /// @copydoc ACTION_MANAGER::GetHotKey()
    int GetHotKey( const TOOL_ACTION& aAction ) const;

    ACTION_MANAGER* GetActionManager() const { return m_actionMgr; }

    TOOL_BASE* FindTool( int aId ) const;

    TOOL_BASE* FindTool( const std::string& aName ) const;

    template<typename T>
    T* GetTool()
    {
        std::map<const char*, TOOL_BASE*>::iterator tool = m_toolTypes.find( typeid( T ).name() );

        if( tool != m_toolTypes.end() )
            return static_cast<T*>( tool->second );

        return nullptr;
    }

    std::vector<TOOL_BASE*> Tools() { return m_toolOrder; }

    void DeactivateTool();

    bool IsToolActive( TOOL_ID aId ) const;

    void ResetTools( TOOL_BASE::RESET_REASON aReason );

    void InitTools();

    bool ProcessEvent( const TOOL_EVENT& aEvent );

    void PostEvent( const TOOL_EVENT& aEvent );

    void SetEnvironment( EDA_ITEM* aModel, KIGFX::VIEW* aView,
                         KIGFX::VIEW_CONTROLS* aViewControls, APP_SETTINGS_BASE* aSettings,
                         TOOLS_HOLDER* aFrame );

    KIGFX::VIEW* GetView() const { return m_view; }

    KIGFX::VIEW_CONTROLS* GetViewControls() const { return m_viewControls; }

    VECTOR2D GetMousePosition() const;
    VECTOR2D GetCursorPosition() const;

    EDA_ITEM* GetModel() const { return m_model; }

    APP_SETTINGS_BASE* GetSettings() const { return m_settings; }

    TOOLS_HOLDER* GetToolHolder() const { return m_frame; }

    inline int GetCurrentToolId() const
    {
        return m_activeTools.empty() ? -1 : m_activeTools.front();
    }

    inline TOOL_BASE* GetCurrentTool() const
    {
        return FindTool( GetCurrentToolId() );
    }

    TOOL_STATE* GetCurrentToolState() const
    {
        auto it = m_toolIdIndex.find( GetCurrentToolId() );
        return ( it != m_toolIdIndex.end() ) ? it->second : nullptr;
    }

    int GetPriority( int aToolId ) const;

    void ScheduleNextState( TOOL_BASE* aTool, TOOL_STATE_FUNC& aHandler,
                            const TOOL_EVENT_LIST& aConditions );

    void ClearTransitions( TOOL_BASE* aTool );

    void RunMainStack( TOOL_BASE* aTool, std::function<void()> aFunc );

    void UpdateUI( const TOOL_EVENT& aEvent );

    TOOL_EVENT* ScheduleWait( TOOL_BASE* aTool, const TOOL_EVENT_LIST& aConditions );

    void ScheduleContextMenu( TOOL_BASE* aTool, ACTION_MENU* aMenu, CONTEXT_MENU_TRIGGER aTrigger );

    const KIGFX::VC_SETTINGS& GetCurrentToolVC() const;

    bool IsContextMenuActive() const
    {
        return m_menuActive;
    }

    void VetoContextMenuMouseWarp()
    {
        m_warpMouseAfterContextMenu = false;
    }

    void WarpAfterContextMenu();

    void DispatchContextMenu( const TOOL_EVENT& aEvent );

    bool DispatchHotKey( const TOOL_EVENT& aEvent );

    VECTOR2D GetMenuCursorPos() const
    {
        return m_menuCursor;
    }

private:
    typedef std::pair<TOOL_EVENT_LIST, TOOL_STATE_FUNC> TRANSITION;

    bool doRunAction( const TOOL_ACTION& aAction, bool aNow, const ki::any& aParam,
                      COMMIT* aCommit, bool aFromAPI = false );
    bool doRunAction( const std::string& aActionName, bool aNow, const ki::any& aParam,
                      COMMIT* aCommit );

    bool dispatchInternal( TOOL_EVENT& aEvent );

    bool dispatchActivation( const TOOL_EVENT& aEvent );

    bool invokeTool( TOOL_BASE* aTool );

    bool runTool( TOOL_BASE* aTool );

    ID_LIST::iterator finishTool( TOOL_STATE* aState );

    bool isRegistered( TOOL_BASE* aTool ) const
    {
        return m_toolState.count( aTool ) > 0;
    }

    bool isActive( TOOL_BASE* aTool ) const;

    void saveViewControls( TOOL_STATE* aState );

    void applyViewControls( const TOOL_STATE* aState );

    bool processEvent( const TOOL_EVENT& aEvent );

    void setActiveState( TOOL_STATE* aState );

private:
    /// List of tools in the order they were registered.
    std::vector<TOOL_BASE*> m_toolOrder;

    /// Index of registered tools current states, associated by tools' objects.
    TOOL_STATE_MAP m_toolState;

    /// Index of the registered tools current states, associated by tools' names.
    NAME_STATE_MAP m_toolNameIndex;

    /// Index of the registered tools current states, associated by tools' ID numbers.
    ID_STATE_MAP m_toolIdIndex;

    /// Index of the registered tools to easily lookup by their type.
    std::map<const char*, TOOL_BASE*> m_toolTypes;

    /// Stack of the active tools.
    ID_LIST m_activeTools;

    /// Instance of ACTION_MANAGER that handles TOOL_ACTIONs.
    ACTION_MANAGER* m_actionMgr;

    /// Original cursor position, if overridden by the context menu handler.
    std::map<TOOL_ID, std::optional<VECTOR2D>> m_cursorSettings;

    EDA_ITEM*             m_model;
    KIGFX::VIEW*          m_view;
    KIGFX::VIEW_CONTROLS* m_viewControls;
    TOOLS_HOLDER*         m_frame;
    APP_SETTINGS_BASE*    m_settings;

    /// Queue that stores events to be processed at the end of the event processing cycle.
    std::list<TOOL_EVENT> m_eventQueue;

    /// Right click context menu position.
    VECTOR2D m_menuCursor;

    bool m_warpMouseAfterContextMenu;

    /// Flag indicating whether a context menu is currently displayed.
    bool m_menuActive;

    /// Tool currently displaying a popup menu. It is negative when there is no menu displayed.
    TOOL_ID m_menuOwner;

    /// Pointer to the state object corresponding to the currently executed tool.
    TOOL_STATE* m_activeState;

    /// True if the tool manager is shutting down (don't process additional events)
    bool m_shuttingDown;
};

#endif // TOOL_MANAGER_H