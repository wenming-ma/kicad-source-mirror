#ifndef TOOL_DISPATCHER_H
#define TOOL_DISPATCHER_H

#include <vector>
#include <QObject>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <tool/tool_event.h>

class TOOL_MANAGER;
class PCB_BASE_FRAME;
class ACTIONS;
class ACTION_MENU;

namespace KIGFX
{
class VIEW;
}

// Takes Qt events, fixes input quirks, translates coordinates to world space,
// handles low-level input conditioning, and issues TOOL_EVENTS to the tool manager
class TOOL_DISPATCHER : public QObject
{
public:
    TOOL_DISPATCHER( TOOL_MANAGER* aToolMgr );

    virtual ~TOOL_DISPATCHER();

    // Bring the dispatcher to its initial state
    virtual void ResetState();

    // Process Qt events, translate them to TOOL_EVENTS, and make tools handle those
    virtual void DispatchQtEvent( QEvent& aEvent );

    // Map a Qt key event to a TOOL_EVENT
    std::optional<TOOL_EVENT> GetToolEvent( QKeyEvent* aKeyEvent, bool* aSpecialKeyFlag );

private:
    // Handles mouse related events (click, motion, dragging)
    bool handleMouseButton( QEvent& aEvent, int aIndex, bool aMotion );

    // Returns the instance of VIEW, used by the application
    KIGFX::VIEW* getView();

    // Saves the state of key modifiers (Alt, Ctrl and so on)
    static int decodeModifiers( const QKeyEvent* aState )
    {
        int mods = 0;

        if( aState->modifiers() & Qt::ControlModifier )
            mods |= MD_CTRL;

        if( aState->modifiers() & Qt::AltModifier )
            mods |= MD_ALT;

        if( aState->modifiers() & Qt::ShiftModifier )
            mods |= MD_SHIFT;

        return mods;
    }

private:
    // Time threshold for distinguishing click vs drag (milliseconds)
    static const int DragTimeThreshold = 300;

    // Distance threshold for distinguishing click vs drag (screen pixels)
    // System drag preferences take precedence if available
    static const int DragDistanceThreshold = 8;

    int      m_sysDragMinX;          // Minimum distance before drag is activated in the X axis
    int      m_sysDragMinY;          // Maximum distance before drag is activated in the Y axis

    VECTOR2D m_lastMousePos;         // The last mouse cursor position (in world coordinates)
    VECTOR2D m_lastMousePosScreen;   // The last mouse cursor position (in screen coordinates)

    // State of mouse buttons
    struct BUTTON_STATE;
    std::vector<BUTTON_STATE*> m_buttons;

    // Instance of tool manager that cooperates with the dispatcher
    TOOL_MANAGER* m_toolMgr;
};

#endif  // TOOL_DISPATCHER_H
