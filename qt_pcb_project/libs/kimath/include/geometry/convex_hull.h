
#ifndef __CONVEX_HULL_H
#define __CONVEX_HULL_H


#include <vector>
#include <math/vector2d.h>
#include <geometry/eda_angle.h>

class SHAPE_POLY_SET;

void BuildConvexHull( std::vector<VECTOR2I>& aResult, const std::vector<VECTOR2I>& aPoly );

void BuildConvexHull( std::vector<VECTOR2I>& aResult, const SHAPE_POLY_SET& aPolygons );

void BuildConvexHull( std::vector<VECTOR2I>& aResult, const SHAPE_POLY_SET& aPolygons,
                      const VECTOR2I& aPosition, const EDA_ANGLE& aRotation );

#endif // __CONVEX_HULL_H
