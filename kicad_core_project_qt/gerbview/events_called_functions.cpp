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
#include "widgets/gbr_layer_box_selector.h"
#include <i18n_utility.h>


// Qt signal/slot connections are handled in the header/constructor


void GERBVIEW_FRAME::OnSelectHighlightChoice( QEvent& event )
{
    auto settings = static_cast<KIGFX::GERBVIEW_PAINTER*>( this->GetCanvas()->GetView()->GetPainter() )->GetSettings();

    // Qt Note: In Qt signal/slot system, this would typically be handled by separate slots
    // For now, we'll use sender() to identify which widget triggered the event
    QObject* sender = this->sender();
    int senderId = sender ? sender->property("id").toInt() : 0;

    switch( senderId )
    {
    case ID_GBR_AUX_TOOLBAR_PCB_CMP_CHOICE:
        settings->m_componentHighlightString = m_SelComponentBox->currentText();
        break;

    case ID_GBR_AUX_TOOLBAR_PCB_NET_CHOICE:
        settings->m_netHighlightString = m_SelNetnameBox->currentText();
        break;

    case ID_GBR_AUX_TOOLBAR_PCB_APERATTRIBUTES_CHOICE:
        settings->m_attributeHighlightString = m_SelAperAttributesBox->currentText();
        break;

    }

    this->GetCanvas()->GetView()->UpdateAllItems( KIGFX::COLOR );
    this->GetCanvas()->Refresh();
}


void GERBVIEW_FRAME::OnSelectActiveDCode( QEvent& event )
{
    GERBER_FILE_IMAGE* gerber_image = this->GetGbrImage( this->GetActiveLayer() );

    if( gerber_image )
    {
        int d_code = m_DCodeSelector->GetSelectedDCodeId();

        auto settings = static_cast<KIGFX::GERBVIEW_PAINTER*>(
                            this->GetCanvas()->GetView()->GetPainter() )->GetSettings();
        gerber_image->m_Selected_Tool = d_code;
        settings->m_dcodeHighlightValue = d_code;

        this->GetCanvas()->GetView()->UpdateAllItems( KIGFX::COLOR );
        this->GetCanvas()->Refresh();
    }
}


void GERBVIEW_FRAME::OnSelectActiveLayer( QEvent& event )
{
    // Qt Note: In Qt, we would get selection from the specific widget that triggered this
    // For now, we'll get it from the layer box directly
    this->SetActiveLayer( m_SelLayerBox->currentIndex(), true );

    // Rebuild the DCode list in toolbar (but not the Layer Box) after change
    this->syncLayerBox( false );

    // Reinit highlighted dcode
    auto settings = static_cast<KIGFX::GERBVIEW_PAINTER*>
                        ( this->GetCanvas()->GetView()->GetPainter() )->GetSettings();
    int dcodeSelected = m_DCodeSelector->GetSelectedDCodeId();
    settings->m_dcodeHighlightValue = dcodeSelected;
    this->GetCanvas()->GetView()->UpdateAllItems( KIGFX::COLOR );
    this->GetCanvas()->Refresh();
}


void GERBVIEW_FRAME::OnQuit( QEvent& event )
{
    this->close();
}


void GERBVIEW_FRAME::ShowChangedLanguage()
{
    // call my base class
    EDA_DRAW_FRAME::ShowChangedLanguage();

    m_LayersManager->SetLayersManagerTabsText();

    // TODO: Qt dock system implementation needed
    // In Qt, dock widgets are managed differently - this needs to be implemented
    // when the proper Qt dock manager is added to the frame
    // QDockWidget* pane_info = findDockWidget( m_LayersManager );
    // if( pane_info )
    //     pane_info->setWindowTitle( _("Layers Manager") );

    ReFillLayerWidget();
}
