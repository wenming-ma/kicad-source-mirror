// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24
// Qt Framework Transformation Complete
// Transformed from wxWidgets to Qt framework

#include <layer_ids.h>
#include <page_info.h>
#include <drawing_sheet/ds_proxy_view_item.h>
#include <drawing_sheet/ds_draw_item.h>
#include <drawing_sheet/ds_data_item.h>
#include <drawing_sheet/ds_painter.h>
#include <gal/graphics_abstraction_layer.h>
#include <project.h>
#include <view/view.h>

using namespace KIGFX;

DS_PROXY_VIEW_ITEM::DS_PROXY_VIEW_ITEM( const EDA_IU_SCALE& aIuScale, const PAGE_INFO* aPageInfo,
                                        const PROJECT* aProject, const TITLE_BLOCK* aTitleBlock,
                                        const std::map<QString, QString>* aProperties ) :
        EDA_ITEM( NOT_USED ), // this item is never added to a BOARD so it needs no type
        m_iuScale( aIuScale ),
        m_titleBlock( aTitleBlock ),
        m_pageInfo( aPageInfo ),
        m_pageNumber( "1" ),
        m_sheetCount( 1 ),
        m_isFirstPage( false ),
        m_project( aProject ),
        m_properties( aProperties ),
        m_colorLayer( LAYER_DRAWINGSHEET ),
        m_pageBorderColorLayer( LAYER_PAGE_LIMITS )
{
}


const BOX2I DS_PROXY_VIEW_ITEM::ViewBBox() const
{
    BOX2I bbox;

    if( m_pageInfo )
    {
        bbox.SetOrigin( VECTOR2I( 0, 0 ) );
        bbox.SetEnd( VECTOR2I( m_iuScale.MilsToIU( m_pageInfo->GetWidthMils() ),
                               m_iuScale.MilsToIU( m_pageInfo->GetHeightMils() ) ) );
    }
    else
    {
        bbox.SetMaximum();
    }

    return bbox;
}


void DS_PROXY_VIEW_ITEM::buildDrawList( VIEW* aView,
                                        const std::map<QString, QString>* aProperties,
                                        DS_DRAW_ITEM_LIST* aDrawList ) const
{
    RENDER_SETTINGS* settings = aView->GetPainter()->GetSettings();
    QString          fileName = QString::fromStdString( m_fileName );
    QString          sheetName = QString::fromStdString( m_sheetName );
    QString          sheetPath = QString::fromStdString( m_sheetPath );

    aDrawList->SetDefaultPenSize( (int) settings->GetDrawingSheetLineWidth() );
    aDrawList->SetIsFirstPage( m_isFirstPage );
    aDrawList->SetPageNumber( m_pageNumber );
    aDrawList->SetSheetCount( m_sheetCount );
    aDrawList->SetFileName( fileName );
    aDrawList->SetSheetName( sheetName );
    aDrawList->SetSheetPath( sheetPath );
    aDrawList->SetSheetLayer( settings->GetLayerName() );
    aDrawList->SetProject( m_project );
    aDrawList->SetProperties( aProperties );

    aDrawList->BuildDrawItemsList( *m_pageInfo, *m_titleBlock );
}


void DS_PROXY_VIEW_ITEM::ViewDraw( int aLayer, VIEW* aView ) const
{
    GAL*              gal = aView->GetGAL();
    RENDER_SETTINGS*  settings = aView->GetPainter()->GetSettings();
    DS_DRAW_ITEM_LIST drawList( m_iuScale );

    buildDrawList( aView, m_properties, &drawList );

    BOX2I viewport = BOX2ISafe( aView->GetViewport() );

    // Draw the title block normally even if the view is flipped
    bool flipped = gal->IsFlippedX();

    if( flipped )
    {
        int pageWidth = m_iuScale.MilsToIU( m_pageInfo->GetWidthMils() );

        gal->Save();
        gal->Translate( VECTOR2D( pageWidth, 0 ) );
        gal->Scale( VECTOR2D( -1.0, 1.0 ) );

        int right = pageWidth - viewport.GetLeft();
        int left = right - viewport.GetWidth();
        viewport.SetOrigin( left, viewport.GetTop() );
    }

    DS_PAINTER ws_painter( gal );
    auto       ws_settings = static_cast<DS_RENDER_SETTINGS*>( ws_painter.GetSettings() );

    ws_settings->SetNormalColor( settings->GetLayerColor( m_colorLayer ) );
    ws_settings->SetSelectedColor( settings->GetLayerColor( LAYER_SELECT_OVERLAY ) );
    ws_settings->SetBrightenedColor( settings->GetLayerColor( LAYER_BRIGHTENED ) );
    ws_settings->SetPageBorderColor( settings->GetLayerColor( m_pageBorderColorLayer ) );
    ws_settings->SetDefaultFont( settings->GetDefaultFont() );

    // Draw all the components that make the drawing sheet
    for( DS_DRAW_ITEM_BASE* item = drawList.GetFirst(); item; item = drawList.GetNext() )
    {
        if( viewport.Intersects( item->GetApproxBBox() ) )
            ws_painter.Draw( item, LAYER_DRAWINGSHEET );
    }

    // Draw gray line that outlines the sheet size
    if( settings->GetShowPageLimits() )
        ws_painter.DrawBorder( m_pageInfo, m_iuScale.IU_PER_MILS );

    if( flipped )
        gal->Restore();
}


std::vector<int> DS_PROXY_VIEW_ITEM::ViewGetLayers() const
{
    std::vector<int> layer{ LAYER_DRAWINGSHEET };
    return layer;
}


bool DS_PROXY_VIEW_ITEM::HitTestDrawingSheetItems( VIEW* aView, const VECTOR2I& aPosition )
{
    int               accuracy = (int) aView->ToWorld( 5.0 );   // five pixels at current zoom
    DS_DRAW_ITEM_LIST drawList( m_iuScale );

    buildDrawList( aView, m_properties, &drawList );

    for( DS_DRAW_ITEM_BASE* item = drawList.GetFirst(); item; item = drawList.GetNext() )
    {
        if( item->HitTest( aPosition, accuracy ) )
            return true;
    }

    return false;
}
