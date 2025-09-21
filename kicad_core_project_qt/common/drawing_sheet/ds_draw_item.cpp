// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
// Transformed from wxWidgets to Qt

/*
 * the class DS_DATA_ITEM (and DS_DATA_ITEM_TEXT) defines
 * a basic shape of a drawing sheet (frame references and title block)
 * Basic shapes are line, rect and texts
 * the DS_DATA_ITEM coordinates units is the mm, and are relative to
 * one of 4 page corners.
 *
 * These items cannot be drawn or plot "as this". they should be converted
 * to a "draw list" (DS_DRAW_ITEM_BASE and derived items)

 * The list of these items is stored in a DS_DATA_MODEL instance.
 *
 * When building the draw list:
 * the DS_DATA_MODEL is used to create a DS_DRAW_ITEM_LIST
 *  coordinates are converted to draw/plot coordinates.
 *  texts are expanded if they contain format symbols.
 *  Items with m_RepeatCount > 1 are created m_RepeatCount times
 *
 * the DS_DATA_MODEL is created only once.
 * the DS_DRAW_ITEM_LIST is created each time the drawing sheet is plotted/drawn
 *
 * the DS_DATA_MODEL instance is created from a S expression which
 * describes the drawing sheet (can be the default drawing sheet or a custom file).
 */

#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <eda_draw_frame.h>
#include <drawing_sheet/ds_draw_item.h>
#include <drawing_sheet/ds_data_item.h>
#include <drawing_sheet/ds_data_model.h>
#include <base_units.h>
#include <page_info.h>
#include <layer_ids.h>
#include <gr_basic.h>
#include <trigo.h>
#include <render_settings.h>
#include <font/font.h>


// ============================ BASE CLASS ==============================

const KIFONT::METRICS& DS_DRAW_ITEM_BASE::GetFontMetrics() const
{
    return KIFONT::METRICS::Default();
}


std::vector<int> DS_DRAW_ITEM_BASE::ViewGetLayers() const
{
    std::vector<int> layers( 1 );

    if( m_peer == nullptr )
    {
        layers[0] = LAYER_DRAWINGSHEET;
        return layers;
    }

    if( m_peer->GetPage1Option() == FIRST_PAGE_ONLY )
        layers[0] = LAYER_DRAWINGSHEET_PAGE1;
    else if( m_peer->GetPage1Option() == SUBSEQUENT_PAGES )
        layers[0] = LAYER_DRAWINGSHEET_PAGEn;
    else
        layers[0] = LAYER_DRAWINGSHEET;

    return layers;
}


bool DS_DRAW_ITEM_BASE::HitTest( const BOX2I& aRect, bool aContained, int aAccuracy ) const
{
    BOX2I sel = aRect;

    if ( aAccuracy )
        sel.Inflate( aAccuracy );

    if( aContained )
        return sel.Contains( GetBoundingBox() );

    return sel.Intersects( GetBoundingBox() );
}


void DS_DRAW_ITEM_BASE::GetMsgPanelInfo( EDA_DRAW_FRAME* aFrame,
                                         std::vector<MSG_PANEL_ITEM>& aList )
{
    QString      msg;
    DS_DATA_ITEM* dataItem = GetPeer();

    if( dataItem == nullptr )   // Is only a pure graphic item used in drawing sheet editor to
                                // handle the page limits
        return;

    switch( dataItem->GetType() )
    {
    case DS_DATA_ITEM::DS_SEGMENT:
        aList.emplace_back( "Line", QString() );
        break;

    case DS_DATA_ITEM::DS_RECT:
        aList.emplace_back( "Rectangle", QString() );
        break;

    case DS_DATA_ITEM::DS_TEXT:
    {
        DS_DRAW_ITEM_TEXT* textItem = static_cast<DS_DRAW_ITEM_TEXT*>( this );

        // Don't use GetShownText(); we want to see the variable references here
        aList.emplace_back( "Text", KIUI::EllipsizeStatusText( aFrame, textItem->GetText() ) );
        break;
    }

    case DS_DATA_ITEM::DS_POLYPOLYGON:
        aList.emplace_back( "Imported Shape", QString() );
        break;

    case DS_DATA_ITEM::DS_BITMAP:
        aList.emplace_back( "Image", QString() );
        break;
    }

    switch( dataItem->GetPage1Option() )
    {
    case FIRST_PAGE_ONLY:  msg = "First Page Only";  break;
    case SUBSEQUENT_PAGES: msg = "Subsequent Pages"; break;
    default:               msg = "All Pages";        break;
    }

    aList.emplace_back( "First Page Option", msg );

    msg = EDA_UNIT_UTILS::UI::MessageTextFromValue( unityScale, EDA_UNITS::UNSCALED,
                                                    dataItem->m_RepeatCount );
    aList.emplace_back( "Repeat Count", msg );

    msg = EDA_UNIT_UTILS::UI::MessageTextFromValue( unityScale, EDA_UNITS::UNSCALED,
                                                    dataItem->m_IncrementLabel );
    aList.emplace_back( "Repeat Label Increment", msg );

    msg = QString::asprintf( "(%s, %s)",
                aFrame->MessageTextFromValue( dataItem->m_IncrementVector.x ).toStdString().c_str(),
                aFrame->MessageTextFromValue( dataItem->m_IncrementVector.y ).toStdString().c_str() );

    aList.emplace_back( "Repeat Position Increment", msg );

    aList.emplace_back( "Comment", dataItem->m_Info );
}


// ============================ TEXT ==============================

void DS_DRAW_ITEM_TEXT::PrintWsItem( const RENDER_SETTINGS* aSettings, const VECTOR2I& aOffset )
{
    COLOR4D color = GetTextColor();

    if( color == COLOR4D::UNSPECIFIED )
        color = aSettings->GetLayerColor( LAYER_DRAWINGSHEET );

    Print( aSettings, aOffset, color, FILLED );
}


const BOX2I DS_DRAW_ITEM_TEXT::GetApproxBBox()
{
    // A really dumb over-approximation because doing it for real (even with the stroke font)
    // shows up large in profiles.

    const TEXT_ATTRIBUTES& attrs = GetAttributes();
    const QString         text = GetShownText( true );
    BOX2I                  bbox( GetTextPos() );

    bbox.SetWidth( KiROUND( (int) text.length() * attrs.m_Size.x * 1.3 ) );
    bbox.SetHeight( attrs.m_Size.y );

    switch( attrs.m_Halign )
    {
    case GR_TEXT_H_ALIGN_LEFT:                                                  break;
    case GR_TEXT_H_ALIGN_CENTER: bbox.Offset( - (int) bbox.GetWidth() / 2, 0 ); break;
    case GR_TEXT_H_ALIGN_RIGHT:  bbox.Offset( - (int) bbox.GetWidth(),     0 ); break;
    case GR_TEXT_H_ALIGN_INDETERMINATE:
        Q_ASSERT_X( false, "DS_DRAW_ITEM_TEXT", "Indeterminate state legal only in dialogs." );
        break;
    }

    switch( GetAttributes().m_Valign )
    {
    case GR_TEXT_V_ALIGN_TOP:                                                    break;
    case GR_TEXT_V_ALIGN_CENTER: bbox.Offset( 0, - (int) bbox.GetHeight() / 2 ); break;
    case GR_TEXT_V_ALIGN_BOTTOM: bbox.Offset( 0, - (int) bbox.GetHeight()     ); break;
    case GR_TEXT_V_ALIGN_INDETERMINATE:
        Q_ASSERT_X( false, "DS_DRAW_ITEM_TEXT", "Indeterminate state legal only in dialogs." );
        break;
    }

    bbox.Inflate( attrs.m_Size.x, attrs.m_Size.y / 2 );
    return bbox;
}


const BOX2I DS_DRAW_ITEM_TEXT::GetBoundingBox() const
{
    return EDA_TEXT::GetTextBox( nullptr );
}


bool DS_DRAW_ITEM_TEXT::HitTest( const VECTOR2I& aPosition, int aAccuracy ) const
{
    return EDA_TEXT::TextHitTest( aPosition, aAccuracy );
}


bool DS_DRAW_ITEM_TEXT::HitTest( const BOX2I& aRect, bool aContains, int aAccuracy ) const
{
    return EDA_TEXT::TextHitTest( aRect, aContains, aAccuracy );
}


QString DS_DRAW_ITEM_TEXT::GetItemDescription( UNITS_PROVIDER* aUnitsProvider, bool aFull ) const
{
    return QString::asprintf( "Text '%s'",
                             (aFull ? GetShownText( false ) : KIUI::EllipsizeMenuText( GetText() )).toStdString().c_str() );
}


// ============================ POLYGON =================================

void DS_DRAW_ITEM_POLYPOLYGONS::PrintWsItem( const RENDER_SETTINGS* aSettings,
                                             const VECTOR2I&        aOffset )
{
    QPainter*   painter = aSettings->GetPrintDC();
    COLOR4D color = aSettings->GetLayerColor( LAYER_DRAWINGSHEET );
    int     penWidth = std::max( GetPenWidth(), aSettings->GetDefaultPenWidth() );

    std::vector<VECTOR2I> points_moved;

    for( int idx = 0; idx < m_Polygons.OutlineCount(); ++idx )
    {
        points_moved.clear();
        SHAPE_LINE_CHAIN& outline = m_Polygons.Outline( idx );

        for( int ii = 0; ii < outline.PointCount(); ii++ )
        {
            points_moved.emplace_back( outline.CPoint( ii ).x + aOffset.x,
                                       outline.CPoint( ii ).y + aOffset.y );
        }

        GRPoly( painter, (int) points_moved.size(), &points_moved[0], true, penWidth, color, color );
    }
}


void DS_DRAW_ITEM_POLYPOLYGONS::SetPosition( const VECTOR2I& aPos )
{
    // Note: m_pos is the anchor point of the shape.
    VECTOR2I move_vect = aPos - m_pos;
    m_pos = aPos;

    // Move polygon corners to the new position:
    m_Polygons.Move( move_vect );
}


const BOX2I DS_DRAW_ITEM_POLYPOLYGONS::GetBoundingBox() const
{
    return m_Polygons.BBox();
}


bool DS_DRAW_ITEM_POLYPOLYGONS::HitTest( const VECTOR2I& aPosition, int aAccuracy ) const
{
    return m_Polygons.Collide( aPosition, aAccuracy );
}


bool DS_DRAW_ITEM_POLYPOLYGONS::HitTest( const BOX2I& aRect, bool aContained, int aAccuracy ) const
{
    BOX2I sel = aRect;

    if ( aAccuracy )
        sel.Inflate( aAccuracy );

    if( aContained )
        return sel.Contains( GetBoundingBox() );

    // Fast test: if rect is outside the polygon bounding box, then they cannot intersect
    if( !sel.Intersects( GetBoundingBox() ) )
        return false;

    for( int idx = 0; idx < m_Polygons.OutlineCount(); ++idx )
    {
        const SHAPE_LINE_CHAIN& outline = m_Polygons.COutline( idx );

        for( int ii = 0; ii < outline.PointCount(); ii++ )
        {
            VECTOR2I corner( outline.CPoint( ii ).x, outline.CPoint( ii ).y );

            // Test if the point is within aRect
            if( sel.Contains( corner ) )
                return true;

            // Test if this edge intersects aRect
            int ii_next = (ii+1) % outline.PointCount();
            VECTOR2I next_corner( outline.CPoint( ii_next ).x, outline.CPoint( ii_next ).y );

            if( sel.Intersects( corner, next_corner ) )
                return true;
        }
    }

    return false;
}


QString DS_DRAW_ITEM_POLYPOLYGONS::GetItemDescription( UNITS_PROVIDER* aUnitsProvider,
                                                        bool aFull ) const
{
    return "Imported Shape";
}


// ============================ RECT ==============================

void DS_DRAW_ITEM_RECT::PrintWsItem( const RENDER_SETTINGS* aSettings, const VECTOR2I& aOffset )
{
    QPainter*   painter = aSettings->GetPrintDC();
    COLOR4D color = aSettings->GetLayerColor( LAYER_DRAWINGSHEET );
    int     penWidth = std::max( GetPenWidth(), aSettings->GetDefaultPenWidth() );

    GRRect( painter, GetStart() + aOffset, GetEnd() + aOffset, penWidth, color );
}


const BOX2I DS_DRAW_ITEM_RECT::GetBoundingBox() const
{
    return BOX2I( GetStart(), GetEnd() - GetStart() );
}


bool DS_DRAW_ITEM_RECT::HitTest( const VECTOR2I& aPosition, int aAccuracy ) const
{
    int dist = aAccuracy + ( GetPenWidth() / 2 );
    VECTOR2I start = GetStart();
    VECTOR2I end;
    end.x = GetEnd().x;
    end.y = start.y;

    // Upper line
    if( TestSegmentHit( aPosition, start, end, dist ) )
        return true;

    // Right line
    start = end;
    end.y = GetEnd().y;

    if( TestSegmentHit( aPosition, start, end, dist ) )
        return true;

    // lower line
    start = end;
    end.x = GetStart().x;

    if( TestSegmentHit( aPosition, start, end, dist ) )
        return true;

    // left line
    start = end;
    end = GetStart();

    if( TestSegmentHit( aPosition, start, end, dist ) )
        return true;

    return false;
}


bool DS_DRAW_ITEM_RECT::HitTest( const BOX2I& aRect, bool aContained, int aAccuracy ) const
{
    BOX2I sel = aRect;

    if ( aAccuracy )
        sel.Inflate( aAccuracy );

    if( aContained )
        return sel.Contains( GetBoundingBox() );

    // For greedy we need to check each side of the rect as we're pretty much always inside the
    // rect which defines the drawing-sheet frame.
    BOX2I side = GetBoundingBox();
    side.SetHeight( 0 );

    if( sel.Intersects( side ) )
        return true;

    side.SetY( GetBoundingBox().GetBottom() );

    if( sel.Intersects( side ) )
        return true;

    side = GetBoundingBox();
    side.SetWidth( 0 );

    if( sel.Intersects( side ) )
        return true;

    side.SetX( GetBoundingBox().GetRight() );

    if( sel.Intersects( side ) )
        return true;

    return false;
}


QString DS_DRAW_ITEM_RECT::GetItemDescription( UNITS_PROVIDER* aUnitsProvider, bool aFull ) const
{
    return QString::asprintf(
            "Rectangle, width %s height %s",
            aUnitsProvider->MessageTextFromValue( std::abs( GetStart().x - GetEnd().x ) ).toStdString().c_str(),
            aUnitsProvider->MessageTextFromValue( std::abs( GetStart().y - GetEnd().y ) ).toStdString().c_str() );
}


// ============================ LINE ==============================

void DS_DRAW_ITEM_LINE::PrintWsItem( const RENDER_SETTINGS* aSettings, const VECTOR2I& aOffset )
{
    QPainter*   painter = aSettings->GetPrintDC();
    COLOR4D color = aSettings->GetLayerColor( LAYER_DRAWINGSHEET );
    int     penWidth = std::max( GetPenWidth(), aSettings->GetDefaultPenWidth() );

    GRLine( painter, GetStart() + aOffset, GetEnd() + aOffset, penWidth, color );
}


const BOX2I DS_DRAW_ITEM_LINE::GetBoundingBox() const
{
    return BOX2I( GetStart(), GetEnd() - GetStart() );
}


bool DS_DRAW_ITEM_LINE::HitTest( const VECTOR2I& aPosition, int aAccuracy ) const
{
    int mindist = aAccuracy + ( GetPenWidth() / 2 ) + 1;
    return TestSegmentHit( aPosition, GetStart(), GetEnd(), mindist );
}


QString DS_DRAW_ITEM_LINE::GetItemDescription( UNITS_PROVIDER* aUnitsProvider, bool aFull ) const
{
    return QString::asprintf( "Line, length %s",
                             aUnitsProvider->MessageTextFromValue( GetStart().Distance( GetEnd() ) ).toStdString().c_str() );
}


// ============== BITMAP ================

void DS_DRAW_ITEM_BITMAP::PrintWsItem( const RENDER_SETTINGS* aSettings, const VECTOR2I& aOffset )
{
    DS_DATA_ITEM_BITMAP* bitmap = (DS_DATA_ITEM_BITMAP*) GetPeer();

    if( !bitmap->m_ImageBitmap )
        return;

    bitmap->m_ImageBitmap->DrawBitmap( aSettings->GetPrintDC(), m_pos + aOffset,
                                       aSettings->GetBackgroundColor() );
}


const BOX2I DS_DRAW_ITEM_BITMAP::GetBoundingBox() const
{
    const DS_DATA_ITEM_BITMAP* bitmap = static_cast<const DS_DATA_ITEM_BITMAP*>( m_peer );
    BOX2I                      bbox;

    if( bitmap && bitmap->m_ImageBitmap )
    {
        VECTOR2I bm_size = bitmap->m_ImageBitmap->GetSize();
        bbox.SetSize( bm_size );
        bbox.SetOrigin( m_pos.x - bm_size.x / 2, m_pos.y - bm_size.y / 2 );
    }

    return bbox;
}


bool DS_DRAW_ITEM_BITMAP::HitTest( const VECTOR2I& aPosition, int aAccuracy ) const
{
    BOX2I bbox = GetBoundingBox();
    bbox.Inflate( aAccuracy );

    return bbox.Contains( aPosition );
}


bool DS_DRAW_ITEM_BITMAP::HitTest( const BOX2I& aRect, bool aContains, int aAccuracy ) const
{
    return DS_DRAW_ITEM_BASE::HitTest( aRect, aContains, aAccuracy );
}


QString DS_DRAW_ITEM_BITMAP::GetItemDescription( UNITS_PROVIDER* aUnitsProvider, bool aFull ) const
{
    return "Image";
}


QString DS_DRAW_ITEM_PAGE::GetItemDescription( UNITS_PROVIDER* aUnitsProvider, bool aFull ) const
{
    return "Page Limits";
}


const BOX2I DS_DRAW_ITEM_PAGE::GetBoundingBox() const
{
    BOX2I dummy;

    // We want this graphic item always visible. So gives the max size to the
    // bounding box to avoid any clamping:
    dummy.SetMaximum();

    return dummy;
}


// ====================== DS_DRAW_ITEM_LIST ==============================

void DS_DRAW_ITEM_LIST::BuildDrawItemsList( const PAGE_INFO& aPageInfo,
                                            const TITLE_BLOCK& aTitleBlock )
{
    DS_DATA_MODEL& model = DS_DATA_MODEL::GetTheInstance();

    m_titleBlock = &aTitleBlock;
    m_paperFormat = aPageInfo.GetType();

    // Build the basic layout shape, if the layout list is empty
    if( model.GetCount() == 0 && !model.VoidListAllowed() )
        model.LoadDrawingSheet( QString(), nullptr );

    model.SetupDrawEnvironment( aPageInfo, GetMilsToIUfactor() );

    for( DS_DATA_ITEM* wsItem : model.GetItems() )
    {
        // Generate it only if the page option allows this
        if( wsItem->GetPage1Option() == FIRST_PAGE_ONLY && !m_isFirstPage )
            continue;
        else if( wsItem->GetPage1Option() == SUBSEQUENT_PAGES && m_isFirstPage )
            continue;

        wsItem->SyncDrawItems( this, nullptr );
    }
}


void DS_DRAW_ITEM_LIST::Print( const RENDER_SETTINGS* aSettings )
{
    std::vector<DS_DRAW_ITEM_BASE*> second_items;

    for( DS_DRAW_ITEM_BASE* item = GetFirst(); item; item = GetNext() )
    {
        if( item->Type() == WSG_BITMAP_T )
            item->PrintWsItem( aSettings );
        else
            second_items.push_back( item );
    }

    for( DS_DRAW_ITEM_BASE* item : second_items )
        item->PrintWsItem( aSettings );
}

// Qt transformation completed


