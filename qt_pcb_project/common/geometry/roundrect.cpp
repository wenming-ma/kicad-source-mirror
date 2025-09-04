#include "geometry/roundrect.h"

#include <ki_exception.h>
#include <geometry/shape_poly_set.h>
#include <geometry/shape_utils.h>
#include <QString>
#include <QCoreApplication>
#include <QtGlobal>


namespace
{

SHAPE_ARC MakeCornerArcCw90( const SHAPE_RECT& aRect, int aRadius, DIRECTION_45::Directions aDir )
{
    const VECTOR2I center = KIGEOM::GetPoint( aRect, aDir );
    return KIGEOM::MakeArcCw90( center, aRadius, aDir );
}


SHAPE_ARC MakeSideArcCw180( const SHAPE_RECT& aRect, int aRadius, DIRECTION_45::Directions aDir )
{
    const VECTOR2I center = KIGEOM::GetPoint( aRect, aDir );
    return KIGEOM::MakeArcCw180( center, aRadius, aDir );
}

} // namespace


ROUNDRECT::ROUNDRECT( SHAPE_RECT aRect, int aRadius ) :
        m_rect( std::move( aRect ) ),
        m_radius( aRadius )
{
    if( m_radius > m_rect.MajorDimension() )
    {
        throw KI_PARAM_ERROR(
                QCoreApplication::translate("ROUNDRECT", "Roundrect radius is larger than the rectangle's major dimension") );
    }

    if( m_radius < 0 )
    {
        throw KI_PARAM_ERROR( QCoreApplication::translate("ROUNDRECT", "Roundrect radius must be non-negative") );
    }
}


ROUNDRECT ROUNDRECT::OutsetFrom( const SHAPE_RECT& aRect, int aOutset )
{
    return ROUNDRECT( aRect.GetInflated( aOutset ), aOutset );
}


ROUNDRECT ROUNDRECT::GetInflated( int aOutset ) const
{
    return ROUNDRECT( m_rect.GetInflated( aOutset ), m_radius + aOutset );
}


void ROUNDRECT::TransformToPolygon( SHAPE_POLY_SET& aBuffer ) const
{
    const int         idx = aBuffer.NewOutline();
    SHAPE_LINE_CHAIN& outline = aBuffer.Outline( idx );

    const int w = m_rect.GetWidth();
    const int h = m_rect.GetHeight();
    const int x_edge = m_rect.GetWidth() - 2 * m_radius;
    const int y_edge = m_rect.GetHeight() - 2 * m_radius;

    // This is a class invariant
    Q_ASSERT( x_edge >= 0 );
    Q_ASSERT( y_edge >= 0 );
    Q_ASSERT( m_radius >= 0 );

    const VECTOR2I& m_p0 = m_rect.GetPosition();

    if( m_radius == 0 )
    {
        // It's just a rectangle
        outline.Append( m_p0 );
        outline.Append( m_p0 + VECTOR2I( w, 0 ) );
        outline.Append( m_p0 + VECTOR2I( w, h ) );
        outline.Append( m_p0 + VECTOR2I( 0, h ) );
    }
    else if( x_edge == 0 && y_edge == 0 )
    {
        // It's a circle
        outline.Append( SHAPE_ARC( m_p0 + VECTOR2I( m_radius, m_radius ),
                                   m_p0 + VECTOR2I( -m_radius, 0 ), ANGLE_360 ) );
    }
    else
    {
        const SHAPE_RECT inner_rect{ m_p0 + VECTOR2I( m_radius, m_radius ), x_edge, y_edge };

        if( x_edge > 0 )
        {
            // Either a normal roundrect or an oval with x_edge > 0

            // Start to the right of the top left radius
            outline.Append( m_p0 + VECTOR2I( m_radius, 0 ) );

            // Top side
            outline.Append( m_p0 + VECTOR2I( m_radius + x_edge, 0 ) );

            if( y_edge > 0 )
            {
                outline.Append( MakeCornerArcCw90( inner_rect, m_radius, DIRECTION_45::NE ) );
                outline.Append( m_p0 + VECTOR2I( w, m_radius + y_edge ) );
                outline.Append( MakeCornerArcCw90( inner_rect, m_radius, DIRECTION_45::SE ) );
            }
            else
            {
                outline.Append( MakeSideArcCw180( inner_rect, m_radius, DIRECTION_45::E ) );
            }

            // Bottom side
            outline.Append( m_p0 + VECTOR2I( m_radius, h ) );

            if( y_edge > 0 )
            {
                outline.Append( MakeCornerArcCw90( inner_rect, m_radius, DIRECTION_45::SW ) );
                outline.Append( m_p0 + VECTOR2I( 0, m_radius ) );
                outline.Append( MakeCornerArcCw90( inner_rect, m_radius, DIRECTION_45::NW ) );
            }
            else
            {
                outline.Append( MakeSideArcCw180( inner_rect, m_radius, DIRECTION_45::W ) );
            }
        }
        else
        {
            // x_edge is 0 but y_edge is not, so it's an oval the other way up
            outline.Append( m_p0 + VECTOR2I( 0, m_radius ) );
            outline.Append( MakeSideArcCw180( inner_rect, m_radius, DIRECTION_45::N ) );
            outline.Append( m_p0 + VECTOR2I( w, m_radius + y_edge ) );
            outline.Append( MakeSideArcCw180( inner_rect, m_radius, DIRECTION_45::S ) );
        }
    }

    outline.SetClosed( true );
}