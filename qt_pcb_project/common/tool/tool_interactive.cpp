
#include <string>

#include <trace_helpers.h>

#include <tool/tool_event.h>
#include <tool/tool_manager.h>
#include <tool/tool_interactive.h>
#include <tool/action_menu.h>

#include <pgm_base.h>

#include <QLoggingCategory>


TOOL_INTERACTIVE::TOOL_INTERACTIVE( TOOL_ID aId, const std::string& aName ) :
    TOOL_BASE( INTERACTIVE, aId, aName )
{
    if( Pgm().IsGUI() )
    {
        m_menu.reset( new TOOL_MENU( *this ) );
    }
}


TOOL_INTERACTIVE::TOOL_INTERACTIVE( const std::string& aName ) :
    TOOL_BASE( INTERACTIVE, TOOL_MANAGER::MakeToolId( aName ), aName )

{
    if( Pgm().IsGUI() )
    {
        m_menu.reset( new TOOL_MENU( *this ) );
    }
}


TOOL_INTERACTIVE::~TOOL_INTERACTIVE()
{
}


void TOOL_INTERACTIVE::Activate()
{
    m_toolMgr->InvokeTool( m_toolId );
}


TOOL_EVENT* TOOL_INTERACTIVE::Wait( const TOOL_EVENT_LIST& aEventList )
{
    return m_toolMgr->ScheduleWait( this, aEventList );
}


void TOOL_INTERACTIVE::resetTransitions()
{
    m_toolMgr->ClearTransitions( this );
    setTransitions();
}


void TOOL_INTERACTIVE::goInternal( TOOL_STATE_FUNC& aState, const TOOL_EVENT_LIST& aConditions )
{
    qCDebug(kicadTraceToolStack,
            "TOOL_INTERACTIVE::goInternal: Tool '%s', Registering handler for "
            "actions '%s'",
            GetName(), aConditions.Names());

    m_toolMgr->ScheduleNextState( this, aState, aConditions );
}


void TOOL_INTERACTIVE::SetContextMenu( ACTION_MENU* aMenu, CONTEXT_MENU_TRIGGER aTrigger )
{
    if( aMenu )
        aMenu->SetTool( this );
    else
        aTrigger = CMENU_OFF;

    m_toolMgr->ScheduleContextMenu( this, aMenu, aTrigger );
}


void TOOL_INTERACTIVE::RunMainStack( std::function<void()> aFunc )
{
    m_toolMgr->RunMainStack( this, std::move( aFunc ) );
}


TOOL_MENU& TOOL_INTERACTIVE::GetToolMenu()
{
    return *m_menu.get();
}
