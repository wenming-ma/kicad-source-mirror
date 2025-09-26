
/**
 * @file clear_gbr_drawlayers.cpp
 * @brief erase a given or all draw layers, an free memory relative to the cleared layer(s)
 */

#include <confirm.h>
#include <gerbview_frame.h>
#include <gerber_file_image.h>
#include <gerber_file_image_list.h>
#include <view/view.h>
#include <base_screen.h>
#include "widgets/gerbview_layer_widget.h"
#include <i18n_utility.h>

#include <tool/tool_manager.h>

bool GERBVIEW_FRAME::Clear_DrawLayers( bool query )
{
    if( GetGerberLayout() == nullptr )
        return false;

    if( query && GetScreen()->IsContentModified() )
    {
        if( !IsOK( this, _( "Current data will be lost?" ) ) )
            return false;
    }

    if( GetCanvas() )
    {
        if( m_toolManager )
            m_toolManager->ResetTools( TOOL_BASE::MODEL_RELOAD );

        GetCanvas()->GetView()->Clear();

        // Reinit the drawing-sheet view, cleared by GetView()->Clear():
        SetPageSettings( GetPageSettings() );
    }

    GetImagesList()->DeleteAllImages();

    GetGerberLayout()->SetBoundingBox( BOX2I() );

    SetActiveLayer( 0 );
    ReFillLayerWidget();
    syncLayerBox();
    return true;
}


void GERBVIEW_FRAME::Erase_Current_DrawLayer( bool query )
{
    int layer = GetActiveLayer();
    QString msg;

    msg = _( "Clear layer %d?" ).arg( layer + 1 );

    if( query && !IsOK( this, msg ) )
        return;

    if( m_toolManager )
        m_toolManager->ResetTools( TOOL_BASE::MODEL_RELOAD );

    RemapLayers( GetImagesList()->RemoveImage( layer ) );

    ReFillLayerWidget();
    syncLayerBox();
    GetCanvas()->Refresh();
}
