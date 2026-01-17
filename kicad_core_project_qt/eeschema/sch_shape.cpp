
#include <sch_draw_panel.h>
#include <macros.h>
#include <plotters/plotter.h>
#include <base_units.h>
#include <widgets/msgpanel.h>
#include <bitmaps.h>
#include <eda_draw_frame.h>
#include <gr_basic.h>
#include <schematic.h>
#include <sch_shape.h>


SCH_SHAPE::SCH_SHAPE( SHAPE_T aShape, SCH_LAYER_ID aLayer, int aLineWidth, FILL_T aFillType,
                      KICAD_T aType ) :
    SCH_ITEM( nullptr, aType ),
    EDA_SHAPE( aShape, aLineWidth, aFillType )
{
    SetLayer( aLayer );
}


EDA_ITEM* SCH_SHAPE::Clone() const
{
    return new SCH_SHAPE( *this );
}


void SCH_SHAPE::SwapData( SCH_ITEM* aItem )
{
    SCH_ITEM::SwapFlags( aItem );

    SCH_SHAPE* shape = static_cast<SCH_SHAPE*>( aItem );

    EDA_SHAPE::SwapShape( shape );
    std::swap( m_layer, shape->m_layer );
}


void SCH_SHAPE::SetStroke( const STROKE_PARAMS& aStroke )
{
    m_stroke = aStroke;
}


void SCH_SHAPE::SetFilled( bool aFilled )
{
    if( !aFilled )
        m_fill = FILL_T::NO_FILL;
    else if( GetParentSymbol() )
        m_fill = FILL_T::FILLED_SHAPE;
    else
        m_fill = FILL_T::FILLED_WITH_COLOR;
}


void SCH_SHAPE::Move( const VECTOR2I& aOffset )
{
    move( aOffset );
}


void SCH_SHAPE::Normalize()
{
    if( GetShape() == SHAPE_T::RECTANGLE )
    {
        VECTOR2I size = GetEnd() - GetPosition();

        if( size.y < 0 )
        {
            SetStartY( GetStartY() + size.y );
            SetEndY( GetStartY() - size.y );
        }

        if( size.x < 0 )
        {
            SetStartX( GetStartX() + size.x );
            SetEndX( GetStartX() - size.x );
        }
    }
}


void SCH_SHAPE::MirrorHorizontally( int aCenter )
{
    flip( VECTOR2I( aCenter, 0 ), FLIP_DIRECTION::LEFT_RIGHT );
}


void SCH_SHAPE::MirrorVertically( int aCenter )
{
    flip( VECTOR2I( 0, aCenter ), FLIP_DIRECTION::TOP_BOTTOM );
}


void SCH_SHAPE::Rotate( const VECTOR2I& aCenter, bool aRotateCCW )
{
    rotate( aCenter, aRotateCCW ? ANGLE_90 : ANGLE_270 );
}


bool SCH_SHAPE::HitTest( const VECTOR2I& aPosition, int aAccuracy ) const
{
    return hitTest( aPosition, aAccuracy );
}


bool SCH_SHAPE::HitTest( const BOX2I& aRect, bool aContained, int aAccuracy ) const
{
    if( m_flags & (STRUCT_DELETED | SKIP_STRUCT ) )
        return false;

    return hitTest( aRect, aContained, aAccuracy );
}


bool SCH_SHAPE::IsEndPoint( const VECTOR2I& aPt ) const
{
    SHAPE_T shape = GetShape();

    if( ( shape == SHAPE_T::ARC ) || ( shape == SHAPE_T::BEZIER ) )
        return ( aPt == GetStart() ) || ( aPt == GetEnd() );

    return false;
}


void SCH_SHAPE::Plot( PLOTTER* aPlotter, bool aBackground, const SCH_PLOT_OPTS& aPlotOpts,
                      int aUnit, int aBodyStyle, const VECTOR2I& aOffset, bool aDimmed )
{
    if( IsPrivate() )
        return;

    // note: if aBodyStyle == -1 the outline shape is not plotted. Only the filled area
    // is plotted (used to plot cells for SCH_TABLE items

    SCH_RENDER_SETTINGS* renderSettings = getRenderSettings( aPlotter );
    int                  pen_size = GetEffectivePenWidth( renderSettings );

    static std::vector<VECTOR2I> ptList;

    if( GetShape() == SHAPE_T::POLY )
    {
        ptList.clear();

        for( const VECTOR2I& pt : m_poly.Outline( 0 ).CPoints() )
            ptList.push_back( renderSettings->TransformCoordinate( pt ) + aOffset );
    }
    else if( GetShape() == SHAPE_T::BEZIER )
    {
        ptList.clear();

        for( const VECTOR2I& pt : m_bezierPoints )
            ptList.push_back( renderSettings->TransformCoordinate( pt ) + aOffset );
    }

    COLOR4D    color = GetStroke().GetColor();
    COLOR4D    bg = renderSettings->GetBackgroundColor();
    LINE_STYLE lineStyle = GetStroke().GetLineStyle();
    FILL_T     fill = m_fill;

    if( aBackground )
    {
        if( !aPlotter->GetColorMode() )
            return;

        switch( m_fill )
        {
        case FILL_T::FILLED_SHAPE:
            return;

        case FILL_T::FILLED_WITH_COLOR:
            color = GetFillColor();
            break;

        case FILL_T::FILLED_WITH_BG_BODYCOLOR:
            color = renderSettings->GetLayerColor( LAYER_DEVICE_BACKGROUND );
            break;

        default:
            return;
        }

        pen_size = 0;
        lineStyle = LINE_STYLE::SOLID;
    }
    else /* if( aForeground ) */
    {
        if( !aPlotter->GetColorMode() || color == COLOR4D::UNSPECIFIED )
            color = renderSettings->GetLayerColor( m_layer );

        if( lineStyle == LINE_STYLE::DEFAULT )
            lineStyle = LINE_STYLE::SOLID;

        if( m_fill == FILL_T::FILLED_SHAPE )
            fill = m_fill;
        else
            fill = FILL_T::NO_FILL;

        pen_size = aBodyStyle == -1 ? 0 : GetEffectivePenWidth( renderSettings );
    }

    if( bg == COLOR4D::UNSPECIFIED || !aPlotter->GetColorMode() )
        bg = COLOR4D::WHITE;

    if( aDimmed )
    {
        color.Desaturate( );
        color = color.Mix( bg, 0.5f );
    }

    aPlotter->SetColor( color );
    aPlotter->SetCurrentLineWidth( pen_size );
    aPlotter->SetDash( pen_size, lineStyle );

    VECTOR2I start = renderSettings->TransformCoordinate( m_start ) + aOffset;
    VECTOR2I end = renderSettings->TransformCoordinate( m_end ) + aOffset;
    VECTOR2I mid, center;

    switch( GetShape() )
    {
    case SHAPE_T::ARC:
        mid = renderSettings->TransformCoordinate( GetArcMid() ) + aOffset;
        aPlotter->Arc( start, mid, end, fill, pen_size );
        break;

    case SHAPE_T::CIRCLE:
        center = renderSettings->TransformCoordinate( getCenter() ) + aOffset;
        aPlotter->Circle( center, GetRadius() * 2, fill, pen_size );
        break;

    case SHAPE_T::RECTANGLE:
        aPlotter->Rect( start, end, fill, pen_size );
        break;

    case SHAPE_T::POLY:
    case SHAPE_T::BEZIER:
        aPlotter->PlotPoly( ptList, fill, pen_size );
        break;

    default:
        UNIMPLEMENTED_FOR( SHAPE_T_asString() );
    }

    aPlotter->SetDash( pen_size, LINE_STYLE::SOLID );
}


int SCH_SHAPE::GetEffectiveWidth() const
{
    if( GetPenWidth() > 0 )
        return GetPenWidth();

    // Historically 0 meant "default width" and negative numbers meant "don't stroke".
    if( GetPenWidth() < 0 )
        return 0;

    SCHEMATIC* schematic = Schematic();

    if( schematic )
        return schematic->Settings().m_DefaultLineWidth;

    return schIUScale.MilsToIU( DEFAULT_LINE_WIDTH_MILS );
}


const BOX2I SCH_SHAPE::GetBoundingBox() const
{
    return getBoundingBox();
}


void SCH_SHAPE::PrintBackground( const SCH_RENDER_SETTINGS* aSettings, int aUnit, int aBodyStyle,
                                 const VECTOR2I& aOffset, bool aDimmed )
{
    if( IsPrivate() )
        return;

    QPainter* painter = aSettings->GetPrintPainter();
    COLOR4D  color;

    static std::vector<VECTOR2I> ptList;

    if( GetShape() == SHAPE_T::POLY )
    {
        ptList.clear();

        for( const VECTOR2I& pt : m_poly.Outline( 0 ).CPoints() )
            ptList.push_back( aSettings->TransformCoordinate( pt ) + aOffset );
    }
    else if( GetShape() == SHAPE_T::BEZIER )
    {
        ptList.clear();

        for( const VECTOR2I& pt : m_bezierPoints )
            ptList.push_back( aSettings->TransformCoordinate( pt ) + aOffset );
    }

    if( GetFillMode() == FILL_T::FILLED_WITH_COLOR )
    {
        if( GetFillColor() == COLOR4D::UNSPECIFIED )
            color = aSettings->GetLayerColor( LAYER_NOTES );
        else
            color = GetFillColor();

        switch( GetShape() )
        {
        case SHAPE_T::ARC:
            GRFilledArc( painter, GetEnd(), GetStart(), getCenter(), 0, color, color );
            break;

        case SHAPE_T::CIRCLE:
            GRFilledCircle( painter, GetStart(), GetRadius(), 0, color, color );
            break;

        case SHAPE_T::RECTANGLE:
            GRFilledRect( painter, GetStart(), GetEnd(), 0, color, color );
            break;

        case SHAPE_T::POLY:
            GRPoly( painter, (int) ptList.size(), ptList.data(), true, 0, color, color );
            break;

        case SHAPE_T::BEZIER:
            GRPoly( painter, (int) ptList.size(), ptList.data(), true, 0, color, color );
            break;

        default:
            UNIMPLEMENTED_FOR( SHAPE_T_asString() );
        }
    }
}


void SCH_SHAPE::Print( const SCH_RENDER_SETTINGS* aSettings, int aUnit, int aBodyStyle,
                       const VECTOR2I& aOffset, bool aForceNoFill, bool aDimmed )
{
    if( IsPrivate() )
        return;

    int      penWidth = GetEffectivePenWidth( aSettings );
    QPainter* painter = aSettings->GetPrintPainter();
    COLOR4D  color = GetStroke().GetColor();
    COLOR4D  bg = aSettings->GetBackgroundColor();

    if( color == COLOR4D::UNSPECIFIED )
        color = aSettings->GetLayerColor( LAYER_NOTES );

    if( bg == COLOR4D::UNSPECIFIED || GetGRForceBlackPenState() )
        bg = COLOR4D::WHITE;

    if( aDimmed )
    {
        color.Desaturate( );
        color = color.Mix( bg, 0.5f );
    }

    static std::vector<VECTOR2I> ptList;

    if( GetShape() == SHAPE_T::POLY )
    {
        ptList.clear();

        for( const VECTOR2I& pt : m_poly.Outline( 0 ).CPoints() )
            ptList.push_back( aSettings->TransformCoordinate( pt ) + aOffset );
    }
    else if( GetShape() == SHAPE_T::BEZIER )
    {
        ptList.clear();

        for( const VECTOR2I& pt : m_bezierPoints )
            ptList.push_back( aSettings->TransformCoordinate( pt ) + aOffset );
    }

    VECTOR2I start = GetStart();
    VECTOR2I end = GetEnd();
    VECTOR2I center = ( GetShape() == SHAPE_T::ARC ) ? getCenter() : VECTOR2I( 0, 0 );

    if( aSettings->m_Transform != TRANSFORM() || aOffset != VECTOR2I() )
    {
        start = aSettings->TransformCoordinate( start ) + aOffset;
        end = aSettings->TransformCoordinate( end ) + aOffset;

        if( GetShape() == SHAPE_T::ARC )
        {
            center = aSettings->TransformCoordinate( center ) + aOffset;

            EDA_ANGLE t1, t2;

            CalcArcAngles( t1, t2 );

            // N.B. The order of evaluation is critical here as MapAngles will modify t1, t2
            // and the Normalize routine depends on these modifications for the correct output
            bool transformed = aSettings->m_Transform.MapAngles( &t1, &t2 );
            EDA_ANGLE arc_angle =  ( t1 - t2 ).Normalize180();
            bool transformed2 = ( arc_angle > ANGLE_0 ) && ( arc_angle < ANGLE_180 );

            if( transformed  != transformed2 )
                std::swap( start, end );
        }
    }

    COLOR4D fillColor = COLOR4D::UNSPECIFIED;

    if( GetFillMode() == FILL_T::FILLED_SHAPE )
        fillColor = color;
    else if( GetFillMode() == FILL_T::FILLED_WITH_COLOR )
        fillColor = GetFillColor();
    else if( GetFillMode() == FILL_T::FILLED_WITH_BG_BODYCOLOR )
        fillColor = aSettings->GetLayerColor( LAYER_DEVICE_BACKGROUND );

    if( fillColor != COLOR4D::UNSPECIFIED && !aForceNoFill )
    {
        if( aDimmed )
        {
            fillColor.Desaturate( );
            fillColor = fillColor.Mix( bg, 0.5f );
        }

        switch( GetShape() )
        {
        case SHAPE_T::ARC:
            GRFilledArc( painter, end, start, center, 0, fillColor, fillColor );
            break;

        case SHAPE_T::CIRCLE:
            GRFilledCircle( painter, start, GetRadius(), 0, fillColor, fillColor );
            break;

        case SHAPE_T::RECTANGLE:
            GRFilledRect( painter, start, end, 0, fillColor, fillColor );
            break;

        case SHAPE_T::POLY:
            GRPoly( painter, (int) ptList.size(), ptList.data(), true, 0, fillColor, fillColor );
            break;

        case SHAPE_T::BEZIER:
            GRPoly( painter, (int) ptList.size(), ptList.data(), true, 0, fillColor, fillColor );
            break;

        default:
            UNIMPLEMENTED_FOR( SHAPE_T_asString() );
        }
    }

    penWidth = std::max( penWidth, aSettings->GetMinPenWidth() );

    if( penWidth > 0 )
    {
        if( GetEffectiveLineStyle() == LINE_STYLE::SOLID )
        {
            switch( GetShape() )
            {
            case SHAPE_T::ARC:
                GRArc( painter, end, start, center, penWidth, color );
                break;

            case SHAPE_T::CIRCLE:
                GRCircle( painter, start, GetRadius(), penWidth, color );
                break;

            case SHAPE_T::RECTANGLE:
                GRRect( painter, start, end, penWidth, color );
                break;

            case SHAPE_T::POLY:
                GRPoly( painter, (int) ptList.size(), ptList.data(), false, penWidth, color, color );
                break;

            case SHAPE_T::BEZIER:
                GRPoly( painter, (int) ptList.size(), ptList.data(), false, penWidth, color, color );
                break;

            default:
                UNIMPLEMENTED_FOR( SHAPE_T_asString() );
            }
        }
        else
        {
            std::vector<SHAPE*> shapes = MakeEffectiveShapes( true );

            for( SHAPE* shape : shapes )
            {
                STROKE_PARAMS::Stroke( shape, GetEffectiveLineStyle(), penWidth, aSettings,
                        [&]( const VECTOR2I& a, const VECTOR2I& b )
                        {
                            VECTOR2I ptA = aSettings->TransformCoordinate( a ) + aOffset;
                            VECTOR2I ptB = aSettings->TransformCoordinate( b ) + aOffset;
                            GRLine( painter, ptA.x, ptA.y, ptB.x, ptB.y, penWidth, color );
                        } );
            }

            for( SHAPE* shape : shapes )
                delete shape;
        }
    }
}


void SCH_SHAPE::GetMsgPanelInfo( EDA_DRAW_FRAME* aFrame, std::vector<MSG_PANEL_ITEM>& aList )
{
    if( m_layer == LAYER_DEVICE )
        getSymbolEditorMsgPanelInfo( aFrame, aList );
    else
        SCH_ITEM::GetMsgPanelInfo( aFrame, aList );

    ShapeGetMsgPanelInfo( aFrame, aList );
}


QString SCH_SHAPE::GetItemDescription( UNITS_PROVIDER* aUnitsProvider, bool aFull ) const
{
    switch( GetShape() )
    {
    case SHAPE_T::ARC:
        return QString::asprintf( "Arc, radius %s",
                                 aUnitsProvider->MessageTextFromValue( GetRadius() ).toStdString().c_str() );

    case SHAPE_T::CIRCLE:
        return QString::asprintf( "Circle, radius %s",
                                 aUnitsProvider->MessageTextFromValue( GetRadius() ).toStdString().c_str() );

    case SHAPE_T::RECTANGLE:
        return QString::asprintf( "Rectangle, width %s height %s",
                                 aUnitsProvider->MessageTextFromValue( std::abs( m_start.x - m_end.x ) ).toStdString().c_str(),
                                 aUnitsProvider->MessageTextFromValue( std::abs( m_start.y - m_end.y ) ).toStdString().c_str() );

    case SHAPE_T::POLY:
        return QString::asprintf( "Polyline, %d points",
                                 int( m_poly.Outline( 0 ).GetPointCount() ) );

    case SHAPE_T::BEZIER:
        return QString::asprintf( "Bezier Curve, %d points",
                                 int( m_bezierPoints.size() ) );

    default:
        UNIMPLEMENTED_FOR( SHAPE_T_asString() );
        return QString();
    }
}


BITMAPS SCH_SHAPE::GetMenuImage() const
{
    switch( GetShape() )
    {
    case SHAPE_T::SEGMENT:   return BITMAPS::add_line;
    case SHAPE_T::ARC:       return BITMAPS::add_arc;
    case SHAPE_T::CIRCLE:    return BITMAPS::add_circle;
    case SHAPE_T::RECTANGLE: return BITMAPS::add_rectangle;
    case SHAPE_T::POLY:      return BITMAPS::add_graphical_segments;
    case SHAPE_T::BEZIER:    return BITMAPS::add_bezier;

    default:
        UNIMPLEMENTED_FOR( SHAPE_T_asString() );
        return BITMAPS::question_mark;
    }
}


std::vector<int> SCH_SHAPE::ViewGetLayers() const
{
    std::vector<int> layers( 3 );

    layers[0] = IsPrivate() ? LAYER_PRIVATE_NOTES : m_layer;

    if( m_layer == LAYER_DEVICE )
    {
        if( m_fill == FILL_T::FILLED_WITH_BG_BODYCOLOR )
            layers[1] = LAYER_DEVICE_BACKGROUND;
        else
            layers[1] = LAYER_SHAPES_BACKGROUND;
    }
    else
    {
        layers[1] = LAYER_SHAPES_BACKGROUND;
    }

    layers[2] = LAYER_SELECTION_SHADOWS;

    return layers;
}


void SCH_SHAPE::AddPoint( const VECTOR2I& aPosition )
{
    if( GetShape() == SHAPE_T::POLY )
    {
        if( m_poly.IsEmpty() )
        {
            m_poly.NewOutline();
            m_poly.Outline( 0 ).SetClosed( false );
        }

        m_poly.Outline( 0 ).Append( aPosition, true );
    }
    else
    {
        UNIMPLEMENTED_FOR( SHAPE_T_asString() );
    }
}


bool SCH_SHAPE::operator==( const SCH_ITEM& aOther ) const
{
    if( aOther.Type() != Type() )
        return false;

    const SCH_SHAPE& other = static_cast<const SCH_SHAPE&>( aOther );

    return SCH_ITEM::operator==( aOther ) && EDA_SHAPE::operator==( other );
}


double SCH_SHAPE::Similarity( const SCH_ITEM& aOther ) const
{
    if( m_Uuid == aOther.m_Uuid )
        return 1.0;

    if( aOther.Type() != Type() )
        return 0.0;

    const SCH_SHAPE& other = static_cast<const SCH_SHAPE&>( aOther );

    double similarity = SimilarityBase( other );

    similarity *= EDA_SHAPE::Similarity( other );

    return similarity;
}


int SCH_SHAPE::compare( const SCH_ITEM& aOther, int aCompareFlags ) const
{
    int cmpFlags = aCompareFlags;

    // The object UUIDs must be compared after the shape coordinates because shapes do not
    // have immutable UUIDs.
    if( !( cmpFlags & ( SCH_ITEM::COMPARE_FLAGS::EQUALITY | SCH_ITEM::COMPARE_FLAGS::ERC ) ) )
        cmpFlags |= SCH_ITEM::COMPARE_FLAGS::EQUALITY;

    int retv = SCH_ITEM::compare( aOther, cmpFlags );

    if( retv )
        return retv;

    retv = EDA_SHAPE::Compare( &static_cast<const SCH_SHAPE&>( aOther ) );

    if( retv )
        return retv;

    if( ( aCompareFlags & SCH_ITEM::COMPARE_FLAGS::EQUALITY )
        || ( aCompareFlags & SCH_ITEM::COMPARE_FLAGS::ERC ) )
    {
        return 0;
    }

    if( m_Uuid < aOther.m_Uuid )
        return -1;

    if( m_Uuid > aOther.m_Uuid )
        return 1;

    return 0;
}


static struct SCH_SHAPE_DESC
{
    SCH_SHAPE_DESC()
    {
        ENUM_MAP<FILL_T>& fillEnum = ENUM_MAP<FILL_T>::Instance();

        if( fillEnum.Choices().count() == 0 )
        {
            fillEnum.Map( FILL_T::NO_FILL, "None" )
                    .Map( FILL_T::FILLED_SHAPE, "Body outline color" )
                    .Map( FILL_T::FILLED_WITH_BG_BODYCOLOR, "Body background color" )
                    .Map( FILL_T::FILLED_WITH_COLOR, "Fill color" );
        }

        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        REGISTER_TYPE( SCH_SHAPE );
        propMgr.AddTypeCast( new TYPE_CAST<SCH_SHAPE, SCH_ITEM> );
        propMgr.AddTypeCast( new TYPE_CAST<SCH_SHAPE, EDA_SHAPE> );
        propMgr.InheritsAfter( TYPE_HASH( SCH_SHAPE ), TYPE_HASH( SCH_ITEM ) );
        propMgr.InheritsAfter( TYPE_HASH( SCH_SHAPE ), TYPE_HASH( EDA_SHAPE ) );

        // Only polygons have meaningful Position properties.
        // On other shapes, these are duplicates of the Start properties.
        auto isPolygon =
                []( INSPECTABLE* aItem ) -> bool
                {
                    if( SCH_SHAPE* shape = dynamic_cast<SCH_SHAPE*>( aItem ) )
                        return shape->GetShape() == SHAPE_T::POLY;

                    return false;
                };

        auto isSymbolItem =
                []( INSPECTABLE* aItem ) -> bool
                {
                    if( SCH_SHAPE* shape = dynamic_cast<SCH_SHAPE*>( aItem ) )
                        return shape->GetLayer() == LAYER_DEVICE;

                    return false;
                };

        auto isSchematicItem =
                []( INSPECTABLE* aItem ) -> bool
                {
                    if( SCH_SHAPE* shape = dynamic_cast<SCH_SHAPE*>( aItem ) )
                        return shape->GetLayer() != LAYER_DEVICE;

                    return false;
                };

        auto isFillColorEditable =
                []( INSPECTABLE* aItem ) -> bool
                {
                    if( SCH_SHAPE* shape = dynamic_cast<SCH_SHAPE*>( aItem ) )
                    {
                        if( shape->GetParentSymbol() )
                            return shape->GetFillMode() == FILL_T::FILLED_WITH_COLOR;
                        else
                            return shape->IsFilled();
                    }

                    return true;
                };

        propMgr.OverrideAvailability( TYPE_HASH( SCH_SHAPE ), TYPE_HASH( SCH_ITEM ),
                                      "Position X", isPolygon );
        propMgr.OverrideAvailability( TYPE_HASH( SCH_SHAPE ), TYPE_HASH( SCH_ITEM ),
                                      "Position Y", isPolygon );

        propMgr.OverrideAvailability( TYPE_HASH( SCH_SHAPE ), TYPE_HASH( EDA_SHAPE ),
                                      "Filled", isSchematicItem );

        propMgr.OverrideWriteability( TYPE_HASH( SCH_SHAPE ), TYPE_HASH( EDA_SHAPE ),
                                      "Fill Color", isFillColorEditable );

        void ( SCH_SHAPE::*fillModeSetter )( FILL_T ) = &SCH_SHAPE::SetFillMode;
        FILL_T ( SCH_SHAPE::*fillModeGetter )() const = &SCH_SHAPE::GetFillMode;

        propMgr.AddProperty( new PROPERTY_ENUM<SCH_SHAPE, FILL_T>( "Fill",
                        fillModeSetter, fillModeGetter ),
                        "Shape Properties" )
                .SetAvailableFunc( isSymbolItem );
    }
} _SCH_SHAPE_DESC;

// ENUM_TO_WXANY( FILL_T ); // wxWidgets-specific macro, not needed in Qt version
