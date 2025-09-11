
#include <tool/tool_event.h>
#include <tool/tool_manager.h>

#include <eda_draw_frame.h>
#include <eda_base_frame.h>


bool TOOL_BASE::IsToolActive() const
{
    return m_toolMgr->IsToolActive( m_toolId );
}


KIGFX::VIEW* TOOL_BASE::getView() const
{
    return m_toolMgr->GetView();
}


KIGFX::VIEW_CONTROLS* TOOL_BASE::getViewControls() const
{
    return m_toolMgr->GetViewControls();
}


TOOLS_HOLDER* TOOL_BASE::getToolHolderInternal() const
{
    return m_toolMgr->GetToolHolder();
}


EDA_ITEM* TOOL_BASE::getModelInternal() const
{
    return m_toolMgr->GetModel();
}


void TOOL_BASE::attachManager( TOOL_MANAGER* aManager )
{
    m_toolMgr = aManager;
}
