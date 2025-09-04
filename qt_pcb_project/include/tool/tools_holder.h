
#ifndef  TOOL_HOLDER_H
#define  TOOL_HOLDER_H

#include <QVector>
#include <QString>
#include <QWidget>
#include <vector>
#include <tool/selection.h>
#include <settings/common_settings.h>

struct ACTION_CONDITIONS;

class ACTIONS;
class TOOL_ACTION;
class TOOL_DISPATCHER;
class TOOL_EVENT;
class TOOL_MANAGER;

// A mix-in class which allows its owner to hold a set of tools from the tool framework.
// This is just the framework; the owner is responsible for registering individual tools,
// creating the dispatcher, etc.
class TOOLS_HOLDER
{
public:
    TOOLS_HOLDER();

    virtual ~TOOLS_HOLDER() { }

    // Return the MVC controller.
    TOOL_MANAGER* GetToolManager() const { return m_toolManager; }

    TOOL_DISPATCHER* GetToolDispatcher() const { return m_toolDispatcher; }

    // Register an action's update conditions with the UI layer to allow the UI to appropriately
    // display the state of its controls.
    virtual void RegisterUIUpdateHandler( const TOOL_ACTION& aAction,
                                          const ACTION_CONDITIONS& aConditions );

    // Register a UI update handler for the control with ID aID.
    virtual void RegisterUIUpdateHandler( int aID, const ACTION_CONDITIONS& aConditions )
    {}

    // Unregister a UI handler for an action that was registered using RegisterUIUpdateHandler.
    virtual void UnregisterUIUpdateHandler( const TOOL_ACTION& aAction );

    // Unregister a UI handler for a given ID that was registered using RegisterUIUpdateHandler.
    virtual void UnregisterUIUpdateHandler( int aID )
    {}

    // Get the current selection from the canvas area.
    virtual SELECTION& GetCurrentSelection()
    {
        return m_dummySelection;
    }

    // NB: the definition of "tool" is different at the user level.
    // The implementation uses a single TOOL_BASE derived class to implement several user
    // "tools", such as rectangle and circle, or wire and bus.  So each user-level tool is
    // actually a TOOL_ACTION.

    // Pushes a tool to the stack.
    virtual void PushTool( const TOOL_EVENT& aEvent );

    // Pops a tool from the stack.
    virtual void PopTool( const TOOL_EVENT& aEvent );

    bool ToolStackIsEmpty() { return m_toolStack.empty(); }

    std::string CurrentToolName() const;
    bool IsCurrentTool( const TOOL_ACTION& aAction ) const;

    virtual void DisplayToolMsg( const QString& msg ) {};

    virtual void ShowChangedLanguage();

    // Indicate that hotkeys should perform an immediate action even if another tool is
    // currently active.  If false, the first hotkey should select the relevant tool.
    bool GetDoImmediateActions() const { return m_immediateActions; }

    // Indicates whether a drag should draw a selection rectangle or drag selected (or unselected)
    // objects.
    MOUSE_DRAG_ACTION GetDragAction() const { return m_dragAction; }

    // Indicate that a move operation should warp the mouse pointer to the origin of the
    // move object.  This improves snapping, but some users are allergic to mouse warping.
    bool GetMoveWarpsCursor() const { return m_moveWarpsCursor; }

#define ENVVARS_CHANGED  0x0001
#define TEXTVARS_CHANGED 0x0002
#define HOTKEYS_CHANGED  0x0004

    // Notification event that some of the common (suite-wide) settings have changed.
    // Update hotkeys, preferences, etc.
    virtual void CommonSettingsChanged( int aFlags = 0 );

    // Canvas access.
    virtual QWidget* GetToolCanvas() const = 0;
    virtual void RefreshCanvas() { }

    virtual QString ConfigBaseName() { return QString(); }

protected:
    TOOL_MANAGER*     m_toolManager;
    ACTIONS*          m_actions;
    TOOL_DISPATCHER*  m_toolDispatcher;

    SELECTION         m_dummySelection;     // Empty dummy selection

    std::vector<std::string> m_toolStack;   // Stack of user-level "tools".  This is NOT a
                                            // stack of TOOL instances, because somewhat
                                            // confusingly most TOOLs implement more than one
                                            // user-level tool.  A user-level tool actually
                                            // equates to an ACTION handler, so this stack
                                            // stores ACTION names.

    bool              m_immediateActions;   // Preference for immediate actions.  If false,
                                            // the first invocation of a hotkey will just
                                            // select the relevant tool rather than executing
                                            // the tool's action.
    MOUSE_DRAG_ACTION m_dragAction;         // DRAG_ANY/DRAG_SELECTED/SELECT.

    bool              m_moveWarpsCursor;    // cursor is warped to move/drag origin
};

#endif  // TOOL_HOLDER_H
