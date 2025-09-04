
#include <trigo.h>
#include <convert_basic_shapes_to_polygon.h>
#include <geometry/geometry_utils.h>

#include <callback_gal.h>

using namespace KIGFX;


void CALLBACK_GAL::DrawGlyph( const KIFONT::GLYPH& aGlyph, int aNth, int aTotal )
{
    if( aGlyph.IsStroke() )
    {
        const KIFONT::STROKE_GLYPH& glyph = static_cast<const KIFONT::STROKE_GLYPH&>( aGlyph );

        for( const QVector<VECTOR2D>& pointList : glyph )
        {
            for( size_t ii = 1; ii < pointList.size(); ii++ )
            {
                if( m_stroke )
                {
                    m_strokeCallback( pointList[ ii - 1 ], pointList[ ii ] );
                }
                else
                {
                    int            strokeWidth = GetLineWidth();
                    SHAPE_POLY_SET poly;

                    // Use ERROR_INSIDE because it avoids Clipper and is therefore much faster.
                    TransformOvalToPolygon( poly, pointList[ ii - 1 ], pointList[ ii ],
                                            strokeWidth, strokeWidth / 180, ERROR_INSIDE );

                    m_outlineCallback( poly.Outline( 0 ) );
                }
            }
        }
    }
    else if( aGlyph.IsOutline() )
    {
        if( m_triangulate )
        {
            const KIFONT::OUTLINE_GLYPH& glyph = static_cast<const KIFONT::OUTLINE_GLYPH&>( aGlyph );

            glyph.Triangulate( m_triangleCallback );
        }
        else
        {
            KIFONT::OUTLINE_GLYPH glyph = static_cast<const KIFONT::OUTLINE_GLYPH&>( aGlyph );

            if( glyph.HasHoles() )
                glyph.Fracture();

            for( int ii = 0; ii < glyph.OutlineCount(); ++ii )
                m_outlineCallback( glyph.Outline( ii ) );
        }
    }
}


