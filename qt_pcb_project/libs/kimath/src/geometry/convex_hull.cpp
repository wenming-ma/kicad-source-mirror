
#include <geometry/convex_hull.h>
#include <geometry/shape_line_chain.h>
#include <geometry/shape_poly_set.h>
#include <math/vector2d.h>
#include <trigo.h>
#include <vector>
#include <algorithm>


typedef long long coord2_t;
static bool compare_point( const VECTOR2I& ref, const VECTOR2I& p )
{
    return ref.x < p.x || (ref.x == p.x && ref.y < p.y);
}

static coord2_t cross_product( const VECTOR2I& O, const VECTOR2I& A, const VECTOR2I& B )
{
    return (coord2_t) (A.x - O.x) * (coord2_t) (B.y - O.y)
           - (coord2_t) (A.y - O.y) * (coord2_t) (B.x - O.x);
}


void BuildConvexHull( std::vector<VECTOR2I>& aResult, const std::vector<VECTOR2I>& aPoly )
{
    std::vector<VECTOR2I> poly = aPoly;
    int point_count = poly.size();

    if( point_count < 2 )
        return;
    std::sort( poly.begin(), poly.end(), compare_point );

    int k = 0;
    aResult.resize( 2 * point_count );
    for( int ii = 0; ii < point_count; ++ii )
    {
        while( k >= 2 && cross_product( aResult[k - 2], aResult[k - 1], poly[ii] ) <= 0 )
            k--;

        aResult[k++] = poly[ii];
    }
    for( int ii = point_count - 2, t = k + 1; ii >= 0; ii-- )
    {
        while( k >= t && cross_product( aResult[k - 2], aResult[k - 1], poly[ii] ) <= 0 )
            k--;

        aResult[k++] = poly[ii];
    }

    if( k > 1 && aResult[0] == aResult[k - 1] )
        k -= 1;

    aResult.resize( k );
}


void BuildConvexHull( std::vector<VECTOR2I>& aResult, const SHAPE_POLY_SET& aPolygons )
{
    BuildConvexHull( aResult, aPolygons, VECTOR2I( 0, 0 ), ANGLE_0 );
}


void BuildConvexHull( std::vector<VECTOR2I>& aResult, const SHAPE_POLY_SET& aPolygons,
                      const VECTOR2I& aPosition, const EDA_ANGLE& aRotation )
{
    std::vector<VECTOR2I> buf;

    for( int cnt = 0; cnt < aPolygons.OutlineCount(); cnt++ )
    {
        const SHAPE_LINE_CHAIN& poly = aPolygons.COutline( cnt );

        for( int ii = 0; ii < poly.PointCount(); ++ii )
            buf.emplace_back( poly.CPoint( ii ).x, poly.CPoint( ii ).y );
    }

    BuildConvexHull( aResult, buf );

    for( unsigned ii = 0; ii < aResult.size(); ii++ )
    {
        RotatePoint( aResult[ii], aRotation );
        aResult[ii] += aPosition;
    }
}
