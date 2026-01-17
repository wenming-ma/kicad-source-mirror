
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#ifndef __PNS_UTILS_H
#define __PNS_UTILS_H

#include <QString>
#include <math/vector2d.h>
#include <math/box2.h>
#include <geometry/shape_line_chain.h>
#include <geometry/shape_segment.h>
#include <geometry/shape_rect.h>
#include <geometry/shape_simple.h>

namespace PNS {

constexpr int HULL_MARGIN = 10;

class ITEM;
class LINE;
class DEBUG_DECORATOR;
class NODE;
/** Various utility functions */

const SHAPE_LINE_CHAIN ArcHull( const SHAPE_ARC& aSeg, int aClearance, int aWalkaroundThickness );

const SHAPE_LINE_CHAIN OctagonalHull( const VECTOR2I& aP0, const VECTOR2I& aSize,
                                      int aClearance, int aChamfer );

const SHAPE_LINE_CHAIN SegmentHull( const SHAPE_SEGMENT& aSeg, int aClearance,
                                    int aWalkaroundThickness );

/**
 * Function ConvexHull()
 *
 * Creates an octagonal hull around a convex polygon.
 * @param aConvex The convex polygon.
 * @param aClearance The minimum distance between polygon and hull.
 * @return A closed line chain describing the octagon.
 */
const SHAPE_LINE_CHAIN ConvexHull( const SHAPE_SIMPLE& aConvex, int aClearance );

SHAPE_RECT ApproximateSegmentAsRect( const SHAPE_SEGMENT& aSeg );

OPT_BOX2I ChangedArea( const ITEM* aItemA, const ITEM* aItemB );
OPT_BOX2I ChangedArea( const LINE& aLineA, const LINE& aLineB );

void HullIntersection( const SHAPE_LINE_CHAIN& hull, const SHAPE_LINE_CHAIN& line,
                       SHAPE_LINE_CHAIN::INTERSECTIONS& ips );

const SHAPE_LINE_CHAIN BuildHullForPrimitiveShape( const SHAPE* aShape, int aClearance,
                                                          int aWalkaroundThickness );

void NodeStats( DEBUG_DECORATOR* aDbg, QString aLabel, NODE *aNode );

}


#endif    // __PNS_UTILS_H
