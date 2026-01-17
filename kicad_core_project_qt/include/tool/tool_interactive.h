// QT_TRANSFORMATION_COMPLETED

#ifndef __TOOL_INTERACTIVE_H
#define __TOOL_INTERACTIVE_H

#include <string>
#include <tool/tool_menu.h>
#include <tool/tool_event.h>    // Needed for CONTEXT_MENU_TRIGGER
#include <tool/tool_base.h>

class ACTION_MENU;


struct REENTRANCY_GUARD
{
    REENTRANCY_GUARD( bool* aFlag ) :
            m_flag( aFlag )
    {
        *m_flag = true;
    }

    ~REENTRANCY_GUARD()
    {
        *m_flag = false;
    }

private:
    bool* m_flag;
};


class TOOL_INTERACTIVE : public TOOL_BASE
{
public:
    TOOL_INTERACTIVE( TOOL_ID aId, const std::string& aName );

    TOOL_INTERACTIVE( const std::string& aName );
    virtual ~TOOL_INTERACTIVE();

    void Activate();

    TOOL_MENU& GetToolMenu();

    void SetContextMenu( ACTION_MENU* aMenu, CONTEXT_MENU_TRIGGER aTrigger = CMENU_BUTTON );

    void RunMainStack( std::function<void()> aFunc );

    template <class T>
    void Go( int (T::* aStateFunc)( const TOOL_EVENT& ),
             const TOOL_EVENT_LIST& aConditions = TOOL_EVENT( TC_ANY, TA_ANY ) );

    TOOL_EVENT* Wait( const TOOL_EVENT_LIST& aEventList = TOOL_EVENT( TC_ANY, TA_ANY ) );

    /*template <class Parameters, class ReturnValue>
        bool InvokeTool( const std::string& aToolName, const Parameters& parameters,
                         ReturnValue& returnValue );

    template <class Parameters, class ReturnValue>
        bool InvokeWindow( const std::string& aWindowName, const Parameters& parameters,
                           ReturnValue& returnValue );

    template <class T>
        void Yield( const T& returnValue );*/

protected:
    std::unique_ptr<TOOL_MENU> m_menu;

private:
    virtual void setTransitions() = 0;

    void resetTransitions();

    void goInternal( TOOL_STATE_FUNC& aState, const TOOL_EVENT_LIST& aConditions );

    friend class TOOL_MANAGER;
};

// hide TOOL_MANAGER implementation
template <class T>
void TOOL_INTERACTIVE::Go( int (T::* aStateFunc)( const TOOL_EVENT& ),
                           const TOOL_EVENT_LIST& aConditions )
{
    TOOL_STATE_FUNC sptr = std::bind( aStateFunc, static_cast<T*>( this ), std::placeholders::_1 );

    goInternal( sptr, aConditions );
}

#endif
