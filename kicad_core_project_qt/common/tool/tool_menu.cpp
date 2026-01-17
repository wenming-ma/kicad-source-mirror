
#include <eda_draw_frame.h>
#include <tool/tool_menu.h>
#include <tool/tool_interactive.h>
#include <tool/action_menu.h>
#include <tool/actions.h>
#include <tool/selection.h>


TOOL_MENU::TOOL_MENU( TOOL_INTERACTIVE& aTool ) :
    m_menu( &aTool ),
    m_tool( aTool )
{
}


TOOL_MENU::~TOOL_MENU()
{
}


CONDITIONAL_MENU& TOOL_MENU::GetMenu()
{
    return m_menu;
}


void TOOL_MENU::RegisterSubMenu( std::shared_ptr<ACTION_MENU> aSubMenu )
{
    // store a copy of the menu (keeps a reference)
    m_subMenus.push_back( std::move( aSubMenu ) );
}


void TOOL_MENU::ShowContextMenu( SELECTION& aSelection )
{
    m_menu.Evaluate( aSelection );
    m_menu.UpdateAll();
    m_menu.ClearDirty();
    m_tool.SetContextMenu( &m_menu, CMENU_NOW );
}


void TOOL_MENU::ShowContextMenu()
{
    m_menu.SetDirty();
    m_tool.SetContextMenu( &m_menu, CMENU_NOW );
}


