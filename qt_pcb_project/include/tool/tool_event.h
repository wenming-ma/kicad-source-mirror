
#ifndef __TOOL_EVENT_H
#define __TOOL_EVENT_H

#include <cstdio>
#include <deque>
#include <iterator>

#include <ki_any.h>

#include <math/vector2d.h>
#include <optional>
#include <atomic>

#include <tool/tool_action.h>
#include <QDebug>
#include <QString>

class COMMIT;
class TOOL_ACTION;
class TOOL_MANAGER;
class TOOL_BASE;
class TOOLS_HOLDER;

enum TOOL_EVENT_CATEGORY
{
    TC_NONE     = 0x00,
    TC_MOUSE    = 0x01,
    TC_KEYBOARD = 0x02,
    TC_COMMAND  = 0x04,
    TC_MESSAGE  = 0x08,
    TC_VIEW     = 0x10,
    TC_ANY      = 0xffffffff
};

enum TOOL_ACTIONS
{
    // UI input events
    TA_NONE                 = 0x0000,
    TA_MOUSE_CLICK          = 0x0001,
    TA_MOUSE_DBLCLICK       = 0x0002,
    TA_MOUSE_UP             = 0x0004,
    TA_MOUSE_DOWN           = 0x0008,
    TA_MOUSE_DRAG           = 0x0010,
    TA_MOUSE_MOTION         = 0x0020,
    TA_MOUSE_WHEEL          = 0x0040,
    TA_MOUSE                = 0x007f,

    TA_KEY_PRESSED          = 0x0080,
    TA_KEYBOARD             = TA_KEY_PRESSED,

    // View related events
    TA_VIEW_REFRESH         = 0x0100,
    TA_VIEW_ZOOM            = 0x0200,
    TA_VIEW_PAN             = 0x0400,
    TA_VIEW_DIRTY           = 0x0800,
    TA_VIEW                 = 0x0f00,

    TA_CHANGE_LAYER         = 0x1000,

    TA_CANCEL_TOOL          = 0x2000,

    TA_CHOICE_MENU_UPDATE   = 0x4000,

    TA_CHOICE_MENU_CHOICE   = 0x8000,

    TA_CHOICE_MENU_CLOSED   = 0x10000,

    TA_CHOICE_MENU = TA_CHOICE_MENU_UPDATE | TA_CHOICE_MENU_CHOICE | TA_CHOICE_MENU_CLOSED,

    TA_UNDO_REDO_PRE        = 0x20000,

    TA_UNDO_REDO_POST       = 0x40000,

    TA_ACTION               = 0x80000,

    TA_ACTIVATE             = 0x100000,

    TA_REACTIVATE           = 0x200000,

    TA_MODEL_CHANGE         = 0x400000,

    TA_PRIME                = 0x800001,

    TA_ANY = 0xffffffff
};

enum TOOL_MOUSE_BUTTONS
{
    BUT_NONE         = 0x0,
    BUT_LEFT         = 0x1,
    BUT_RIGHT        = 0x2,
    BUT_MIDDLE       = 0x4,
    BUT_AUX1         = 0x8,
    BUT_AUX2         = 0x10,
    BUT_BUTTON_MASK  = BUT_LEFT | BUT_RIGHT | BUT_MIDDLE | BUT_AUX1 | BUT_AUX2,
    BUT_ANY          = 0xffffffff
};

enum TOOL_MODIFIERS
{
    MD_SHIFT        = 0x1000,
    MD_CTRL         = 0x2000,
    MD_ALT          = 0x4000,
    MD_MODIFIER_MASK = MD_SHIFT | MD_CTRL | MD_ALT,
};

enum CONTEXT_MENU_TRIGGER
{
    CMENU_BUTTON = 0,
    CMENU_NOW,
    CMENU_OFF
};

enum SYNCRONOUS_TOOL_STATE
{
    STS_RUNNING,
    STS_FINISHED,
    STS_CANCELLED
};

class TOOL_EVENT
{
public:
    const std::string Format() const;

    TOOL_EVENT( TOOL_EVENT_CATEGORY aCategory = TC_NONE, TOOL_ACTIONS aAction = TA_NONE,
                TOOL_ACTION_SCOPE aScope = AS_GLOBAL ) :
        m_category( aCategory ),
        m_actions( aAction ),
        m_scope( aScope ),
        m_mouseButtons( 0 ),
        m_keyCode( 0 ),
        m_modifiers( 0 ),
        m_synchronousState( nullptr ),
        m_commit( nullptr ),
        m_firstResponder( nullptr )
    {
        init();
    }

    TOOL_EVENT( TOOL_EVENT_CATEGORY aCategory, TOOL_ACTIONS aAction, int aExtraParam,
                TOOL_ACTION_SCOPE aScope = AS_GLOBAL ) :
        m_category( aCategory ),
        m_actions( aAction ),
        m_scope( aScope ),
        m_mouseButtons( 0 ),
        m_keyCode( 0 ),
        m_modifiers( 0 ),
        m_synchronousState( nullptr ),
        m_commit( nullptr ),
        m_firstResponder( nullptr )
    {
        if( aCategory == TC_MOUSE )
        {
            setMouseButtons( aExtraParam & BUT_BUTTON_MASK );
        }
        else if( aCategory == TC_KEYBOARD )
        {
            m_keyCode = aExtraParam & ~MD_MODIFIER_MASK;         // Filter out modifiers
        }
        else if( aCategory == TC_COMMAND )
        {
            m_commandId = aExtraParam;
        }

        if( aCategory & ( TC_MOUSE | TC_KEYBOARD ) )
        {
            m_modifiers = aExtraParam & MD_MODIFIER_MASK;
        }

        init();
    }

    TOOL_EVENT( TOOL_EVENT_CATEGORY aCategory, TOOL_ACTIONS aAction,
            const std::string& aExtraParam, TOOL_ACTION_SCOPE aScope = AS_GLOBAL ) :
        m_category( aCategory ),
        m_actions( aAction ),
        m_scope( aScope ),
        m_mouseButtons( 0 ),
        m_keyCode( 0 ),
        m_modifiers( 0 ),
        m_synchronousState( nullptr ),
        m_commit( nullptr ),
        m_firstResponder( nullptr )
    {
        if( aCategory == TC_COMMAND || aCategory == TC_MESSAGE )
            m_commandStr = aExtraParam;

        init();
    }

    TOOL_EVENT_CATEGORY Category() const { return m_category; }

    TOOL_ACTIONS Action() const { return m_actions; }

    bool PassEvent() const { return m_passEvent; }
    void SetPassEvent( bool aPass = true ) { m_passEvent = aPass; }

    bool HasPosition() const { return m_hasPosition; }
    void SetHasPosition( bool aHasPosition ) { m_hasPosition = aHasPosition; }

    bool ForceImmediate() const { return m_forceImmediate; }
    void SetForceImmediate( bool aForceImmediate = true ) { m_forceImmediate = aForceImmediate; }

    TOOL_BASE* FirstResponder() const { return m_firstResponder; }
    void SetFirstResponder( TOOL_BASE* aTool ) { m_firstResponder = aTool; }

    bool IsReactivate() const { return m_reactivate; }
    void SetReactivate( bool aReactivate = true ) { m_reactivate = aReactivate; }

    void SetSynchronous( std::atomic<SYNCRONOUS_TOOL_STATE>* aState )
    {
        m_synchronousState = aState;
    }
    std::atomic<SYNCRONOUS_TOOL_STATE>* SynchronousState() const { return m_synchronousState; }

    void SetCommit( COMMIT* aCommit ) { m_commit = aCommit; }
    COMMIT* Commit() const { return m_commit; }

    const VECTOR2D Delta() const
    {
        return returnCheckedPosition( m_mouseDelta );
    }

    const VECTOR2D Position() const
    {
        return returnCheckedPosition( m_mousePos );
    }

    const VECTOR2D DragOrigin() const
    {
        return returnCheckedPosition( m_mouseDragOrigin );
    }

    int Buttons() const
    {
        assert( m_category == TC_MOUSE );
        return m_mouseButtons;
    }

    bool IsClick( int aButtonMask = BUT_ANY ) const;

    bool IsDblClick( int aButtonMask = BUT_ANY ) const;

    bool IsDrag( int aButtonMask = BUT_ANY ) const
    {
        return m_actions == TA_MOUSE_DRAG && ( m_mouseButtons & aButtonMask ) == m_mouseButtons;
    }

    bool IsMouseDown( int aButtonMask = BUT_ANY ) const
    {
        return m_actions == TA_MOUSE_DOWN && ( m_mouseButtons & aButtonMask ) == m_mouseButtons;
    }

    bool IsMouseUp( int aButtonMask = BUT_ANY ) const
    {
        return m_actions == TA_MOUSE_UP && ( m_mouseButtons & aButtonMask ) == m_mouseButtons;
    }

    bool IsMotion() const
    {
        return m_actions == TA_MOUSE_MOTION;
    }

    bool IsMouseAction() const
    {
        return ( m_actions & TA_MOUSE );
    }

    bool IsCancel() const
    {
        return m_actions == TA_CANCEL_TOOL;
    }

    bool IsActivate() const
    {
        return m_actions == TA_ACTIVATE;
    }

    bool IsUndoRedo() const
    {
        return m_actions & ( TA_UNDO_REDO_PRE | TA_UNDO_REDO_POST );
    }

    bool IsChoiceMenu() const
    {
        return m_actions & TA_CHOICE_MENU;
    }

    bool IsPrime() const
    {
        return m_actions == TA_PRIME;
    }

    int Modifier( int aMask = MD_MODIFIER_MASK ) const
    {
        return m_modifiers & aMask;
    }

    bool DisableGridSnapping() const
    {
        return Modifier( MD_CTRL );
    }

    int KeyCode() const
    {
        return m_keyCode;
    }

    bool IsKeyPressed() const
    {
        return m_actions == TA_KEY_PRESSED;
    }

    bool Matches( const TOOL_EVENT& aEvent ) const
    {
        if( !( m_category & aEvent.m_category ) )
            return false;

        if( m_category == TC_COMMAND || m_category == TC_MESSAGE )
        {
            if( !m_commandStr.empty() && !aEvent.getCommandStr().empty() )
                return m_commandStr == aEvent.m_commandStr;

            if( (bool) m_commandId && (bool) aEvent.m_commandId )
                return *m_commandId == *aEvent.m_commandId;
        }

        if( m_actions == TA_ANY && aEvent.m_actions == TA_NONE && aEvent.m_category == TC_MESSAGE )
            return true;
        if( !( m_actions & aEvent.m_actions ) )
            return false;

        return true;
    }

    bool IsAction( const TOOL_ACTION* aAction ) const;

    bool IsCancelInteractive() const;

    bool IsSelectionEvent() const;

    bool IsPointEditor() const;

    bool IsMoveTool() const;

    bool IsEditorTool() const;

    bool IsSimulator() const;
    template<typename T, std::enable_if_t<!std::is_pointer<T>::value>* = nullptr >
    T Parameter() const
    {
        T param;

        Q_ASSERT_X( m_param.has_value(), "Parameter", "Attempted to get a parameter from an event with "
                                               "no parameter." );
        if( !m_param.has_value() ) return T();

        try
        {
            param = ki::any_cast<T>( m_param );
        }
        catch( const ki::bad_any_cast& )
        {
            Q_ASSERT_X( false, "Parameter", QString( "Requested parameter type %1 from event "
                                                       "with parameter type %2." )
                                                       .arg( typeid(T).name() )
                                                       .arg( m_param.type().name() ).toLocal8Bit().constData() );
            return T();
        }

        return param;
    }

    template<typename T, std::enable_if_t<std::is_pointer<T>::value>* = nullptr>
    T Parameter() const
    {
        T param = nullptr;

        Q_ASSERT_X( m_param.has_value(), "Parameter", "Attempted to get a parameter from an event with "
                                                 "no parameter." );
        if( !m_param.has_value() ) return param;

        try
        {
            param = ki::any_cast<T>( m_param );
        }
        catch( const ki::bad_any_cast& )
        {
            Q_ASSERT_X( false, "Parameter", QString( "Requested parameter type %1 from event "
                                                         "with parameter type %2." )
                                                         .arg( typeid(T).name() )
                                                         .arg( m_param.type().name() ).toLocal8Bit().constData() );
            return param;
        }

        return param;
    }

    template<typename T>
    void SetParameter(T aParam)
    {
        m_param = aParam;
    }

    std::optional<int> GetCommandId() const
    {
        return m_commandId;
    }

    void SetMousePosition( const VECTOR2D& aP )
    {
        m_mousePos = aP;
    }

    void SetActionGroup( const TOOL_ACTION_GROUP& aGroup )
    {
        m_actionGroup = aGroup;
    }

    bool IsActionInGroup( const TOOL_ACTION_GROUP& aGroup ) const;

private:
    friend class TOOL_EVENT_LIST;
    friend class TOOL_DISPATCHER;
    friend class TOOL_MANAGER;
    friend class TOOLS_HOLDER;

    void init();

    const std::string& getCommandStr() const { return m_commandStr; }

    void setMouseDragOrigin( const VECTOR2D& aP )
    {
        m_mouseDragOrigin = aP;
     }

    void setMouseDelta( const VECTOR2D& aP )
    {
        m_mouseDelta = aP;
    }

    void setMouseButtons( int aButtons )
    {
        assert( ( aButtons & ~BUT_BUTTON_MASK ) == 0 );
        m_mouseButtons = aButtons;
    }

    void setModifiers( int aMods )
    {
        assert( ( aMods & ~MD_MODIFIER_MASK ) == 0 );
        m_modifiers = aMods;
    }

    VECTOR2D returnCheckedPosition( const VECTOR2D& aPos ) const;

    TOOL_EVENT_CATEGORY m_category;
    TOOL_ACTIONS m_actions;
    TOOL_ACTION_SCOPE m_scope;
    bool m_passEvent;
    bool m_hasPosition;
    bool m_forceImmediate;


    std::optional<TOOL_ACTION_GROUP> m_actionGroup;

    bool m_reactivate;

    VECTOR2D m_mouseDelta;

    VECTOR2D m_mousePos;

    VECTOR2D m_mouseDragOrigin;

    int m_mouseButtons;

    int m_keyCode;

    int m_modifiers;

    std::atomic<SYNCRONOUS_TOOL_STATE>* m_synchronousState;

    COMMIT* m_commit;

    ki::any m_param;

    TOOL_BASE* m_firstResponder;

    std::optional<int> m_commandId;
    std::string        m_commandStr;
};

typedef std::optional<TOOL_EVENT> OPT_TOOL_EVENT;

class TOOL_EVENT_LIST
{
public:
    typedef TOOL_EVENT value_type;
    typedef std::deque<TOOL_EVENT>::iterator iterator;
    typedef std::deque<TOOL_EVENT>::const_iterator const_iterator;

    TOOL_EVENT_LIST()
    {}

    TOOL_EVENT_LIST( const TOOL_EVENT& aSingleEvent )
    {
        m_events.push_back( aSingleEvent );
    }

    TOOL_EVENT_LIST( const TOOL_EVENT_LIST& aEventList )
    {
        m_events.clear();

        for( const TOOL_EVENT& event : aEventList.m_events )
            m_events.push_back( event );
    }

    const std::string Format() const;

    const std::string Names() const;

    OPT_TOOL_EVENT Matches( const TOOL_EVENT& aEvent ) const
    {
        for( const TOOL_EVENT& event : m_events )
        {
            if( event.Matches( aEvent ) )
                return event;
        }

        return OPT_TOOL_EVENT();
    }

    void Add( const TOOL_EVENT& aEvent )
    {
        m_events.push_back( aEvent );
    }

    iterator begin()
    {
        return m_events.begin();
    }

    iterator end()
    {
        return m_events.end();
    }

    const_iterator cbegin() const
    {
        return m_events.begin();
    }

    const_iterator cend() const
    {
        return m_events.end();
    }

    int size() const
    {
        return m_events.size();
    }

    void clear()
    {
        m_events.clear();
    }

    TOOL_EVENT_LIST& operator=( const TOOL_EVENT_LIST& aEventList )
    {
        m_events.clear();

        for( const TOOL_EVENT& event : aEventList.m_events )
            m_events.push_back( event );

        return *this;
    }

    TOOL_EVENT_LIST& operator=( const TOOL_EVENT& aEvent )
    {
        m_events.clear();
        m_events.push_back( aEvent );
        return *this;
    }

    TOOL_EVENT_LIST& operator||( const TOOL_EVENT& aEvent )
    {
        Add( aEvent );
        return *this;
    }

    TOOL_EVENT_LIST& operator||( const TOOL_EVENT_LIST& aEvent )
    {
        std::copy( aEvent.m_events.begin(), aEvent.m_events.end(), std::back_inserter( m_events ) );
        return *this;
    }

private:
    std::deque<TOOL_EVENT> m_events;
};


inline const TOOL_EVENT_LIST operator||( const TOOL_EVENT& aEventA, const TOOL_EVENT& aEventB )
{
    TOOL_EVENT_LIST l;

    l.Add( aEventA );
    l.Add( aEventB );

    return l;
}


inline const TOOL_EVENT_LIST operator||( const TOOL_EVENT& aEvent,
                                         const TOOL_EVENT_LIST& aEventList )
{
    TOOL_EVENT_LIST l( aEventList );

    l.Add( aEvent );
    return l;
}


#endif
