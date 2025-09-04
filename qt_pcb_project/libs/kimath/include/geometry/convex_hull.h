
#ifndef __CONVEX_HULL_H
#define __CONVEX_HULL_H


#include <QVector>
#include <math/vector2d.h>
#include <geometry/eda_angle.h>

class SHAPE_POLY_SET;

void BuildConvexHull( QVector<VECTOR2I>& aResult, const QVector<VECTOR2I>& aPoly );

void BuildConvexHull( QVector<VECTOR2I>& aResult, const SHAPE_POLY_SET& aPolygons );

void BuildConvexHull( QVector<VECTOR2I>& aResult, const SHAPE_POLY_SET& aPolygons,
                      const VECTOR2I& aPosition, const EDA_ANGLE& aRotation );

#endif // __CONVEX_HULL_H
