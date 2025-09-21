// Qt transformation completed - wxWidgets to Qt framework conversion

#include <gerbview.h>
#include <gerbview_frame.h>
#include <gerbview_id.h>
#include <gerber_file_image.h>
#include <gerber_file_image_list.h>
#include <gal/graphics_abstraction_layer.h>
#include <tool/tool_manager.h>
#include <tool/selection.h>
#include <tools/gerbview_selection_tool.h>
#include <gerbview_painter.h>
#include <view/view.h>
#include "widgets/gerbview_layer_widget.h"
#include "widgets/dcode_selection_box.h"


// Qt signal/slot connections are handled in the header/constructor


void GERBVIEW_FRAME::OnSelectHighlightChoice( QActionEvent& event )
{
    auto settings = static_cast<KIGFX::GERBVIEW_PAINTER*>( GetCanvas()->GetView()->GetPainter() )->GetSettings();

    switch( event.GetId() )
    {
    case ID_GBR_AUX_TOOLBAR_PCB_CMP_CHOICE:
        settings->m_componentHighlightString = m_SelComponentBox->GetStringSelection();
        break;

    case ID_GBR_AUX_TOOLBAR_PCB_NET_CHOICE:
        settings->m_netHighlightString = m_SelNetnameBox->GetStringSelection();
        break;

    case ID_GBR_AUX_TOOLBAR_PCB_APERATTRIBUTES_CHOICE:
        settings->m_attributeHighlightString = m_SelAperAttributesBox->GetStringSelection();
        break;

    }

    GetCanvas()->GetView()->UpdateAllItems( KIGFX::COLOR );
    GetCanvas()->Refresh();
}


void GERBVIEW_FRAME::OnSelectActiveDCode( QActionEvent& event )
{
    GERBER_FILE_IMAGE* gerber_image = GetGbrImage( GetActiveLayer() );

    if( gerber_image )
    {
        int d_code = m_DCodeSelector->GetSelectedDCodeId();

        auto settings = static_cast<KIGFX::GERBVIEW_PAINTER*>(
                            GetCanvas()->GetView()->GetPainter() )->GetSettings();
        gerber_image->m_Selected_Tool = d_code;
        settings->m_dcodeHighlightValue = d_code;

        GetCanvas()->GetView()->UpdateAllItems( KIGFX::COLOR );
        GetCanvas()->Refresh();
    }
}


void GERBVIEW_FRAME::OnSelectActiveLayer( QActionEvent& event )
{
    SetActiveLayer( event.GetSelection(), true );

    // Rebuild the DCode list in toolbar (but not the Layer Box) after change
    syncLayerBox( false );

    // Reinit highlighted dcode
    auto settings = static_cast<KIGFX::GERBVIEW_PAINTER*>
                        ( GetCanvas()->GetView()->GetPainter() )->GetSettings();
    int dcodeSelected = m_DCodeSelector->GetSelectedDCodeId();
    settings->m_dcodeHighlightValue = dcodeSelected;
    GetCanvas()->GetView()->UpdateAllItems( KIGFX::COLOR );
    GetCanvas()->Refresh();
}


void GERBVIEW_FRAME::OnQuit( QActionEvent& event )
{
    Close( true );
}


void GERBVIEW_FRAME::ShowChangedLanguage()
{
    // call my base class
    EDA_DRAW_FRAME::ShowChangedLanguage();

    m_LayersManager->SetLayersManagerTabsText();

    QDockWidget* pane_info = m_auimgr.GetPane( m_LayersManager );
    pane_info->setWindowTitle( "Layers Manager" );
    m_auimgr.Update();

    ReFillLayerWidget();
}
