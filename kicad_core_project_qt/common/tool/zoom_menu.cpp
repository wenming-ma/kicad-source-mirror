// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
// Qt transformation completed - wxWidgets to Qt framework conversion

#include <tool/zoom_menu.h>
#include <id.h>
#include <eda_draw_frame.h>
#include <settings/app_settings.h>
#include <tool/actions.h>
#include <gal/graphics_abstraction_layer.h>
#include <bitmaps.h>
#include <functional>

using namespace std::placeholders;

ZOOM_MENU::ZOOM_MENU( EDA_DRAW_FRAME* aParent ) :
        ACTION_MENU( true ),
        m_parent( aParent )
{
    UpdateTitle();
    SetIcon( BITMAPS::zoom_selection );
}


OPT_TOOL_EVENT ZOOM_MENU::eventHandler( QAction* aAction )
{
    OPT_TOOL_EVENT event( ACTIONS::zoomPreset.MakeEvent() );
    event->SetParameter<int>( aAction->data().toInt() - ID_POPUP_ZOOM_LEVEL_START );
    return event;
}


void ZOOM_MENU::UpdateTitle()
{
    SetTitle( "Zoom" );
}


void ZOOM_MENU::update()
{
    Clear();

    int ii = ID_POPUP_ZOOM_LEVEL_START + 1;  // 0 reserved for menus which support auto-zoom

    for( double factor : m_parent->config()->m_Window.zoom_factors )
        Append( ii++, QString::asprintf( "Zoom: %.2f", factor ), "", true );

    double zoom = m_parent->GetCanvas()->GetGAL()->GetZoomFactor();

    const std::vector<double>& zoomList = m_parent->config()->m_Window.zoom_factors;

    for( size_t jj = 0; jj < zoomList.size(); ++jj )
    {
        // Search for a value near the current zoom setting:
        double rel_error = std::fabs( zoomList[jj] - zoom ) / zoom;

        // IDs start with 1 (leaving 0 for auto-zoom)
        Check( ID_POPUP_ZOOM_LEVEL_START + jj + 1, rel_error < 0.1 );
    }
}
