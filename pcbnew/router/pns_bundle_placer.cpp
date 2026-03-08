/*
 * KiRouter - a push-and-(sometimes-)shove PCB router
 *
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <algorithm>
#include <cmath>
#include <limits>

#include "pns_walkaround.h"
#include "pns_shove.h"
#include "pns_router.h"
#include "pns_bundle_placer.h"
#include "pns_solid.h"
#include "pns_segment.h"
#include "pns_arc.h"
#include "pns_topology.h"
#include "pns_debug_decorator.h"
#include "pns_utils.h"

#include <geometry/direction45.h>

namespace PNS {

namespace
{
struct LANE_LAYOUT
{
    VECTOR2I         direction;
    VECTOR2I         perpendicular;
    VECTOR2I         centroid;
    double           directionLength = 0.0;
    int              targetPitch = 0;
    int              baseEscape = 0;
    int              mergeAlongDistance = 0;
    std::vector<int> startOffsets;
    std::vector<int> alongOffsets;
    std::vector<int> targetOffsets;
};


static bool sameLineChainGeometry( const SHAPE_LINE_CHAIN& aA, const SHAPE_LINE_CHAIN& aB )
{
    if( aA.PointCount() != aB.PointCount() )
        return false;

    for( int i = 0; i < aA.PointCount(); ++i )
    {
        if( aA.CPoint( i ) != aB.CPoint( i ) )
            return false;
    }

    return true;
}


static bool lineHasBackwardSegment( const SHAPE_LINE_CHAIN& aLane, const VECTOR2I& aRoutingDir )
{
    if( aRoutingDir == VECTOR2I( 0, 0 ) )
        return false;

    for( int i = 0; i < aLane.SegmentCount(); ++i )
    {
        VECTOR2I delta = aLane.CSegment( i ).B - aLane.CSegment( i ).A;
        int64_t  dot = (int64_t) delta.x * aRoutingDir.x + (int64_t) delta.y * aRoutingDir.y;

        if( dot < 0 )
            return true;
    }

    return false;
}


static bool lanesCross( const SHAPE_LINE_CHAIN& aA, const SHAPE_LINE_CHAIN& aB )
{
    SHAPE_LINE_CHAIN::INTERSECTIONS intersections;
    return aA.Intersect( aB, intersections, true ) > 0;
}


static bool areParallel( const VECTOR2I& aA, const VECTOR2I& aB )
{
    if( aA == VECTOR2I( 0, 0 ) || aB == VECTOR2I( 0, 0 ) )
        return false;

    return (int64_t) aA.x * aB.y == (int64_t) aA.y * aB.x;
}


static int signum( int64_t aValue )
{
    return ( aValue > 0 ) - ( aValue < 0 );
}


static double signedAxisDeparture( const VECTOR2I& aPoint, const VECTOR2I& aCentroid,
                                   const VECTOR2I& aAxis )
{
    if( aAxis == VECTOR2I( 0, 0 ) )
        return 0.0;

    VECTOR2I rel = aPoint - aCentroid;
    int64_t  cross = (int64_t) aAxis.x * rel.y - (int64_t) aAxis.y * rel.x;
    double   axisLength = aAxis.EuclideanNorm();

    return axisLength > 0.0 ? (double) cross / axisLength : 0.0;
}


static void evaluateTurnProgress( const std::vector<SHAPE_LINE_CHAIN>& aEntryLanes,
                                  const VECTOR2I& aCentroid,
                                  const VECTOR2I& aMajorAxis,
                                  const VECTOR2I& aRoutingDir,
                                  int aThreshold,
                                  bool& aLacksTurnProgress,
                                  bool& aWrongSideTurn )
{
    aLacksTurnProgress = false;
    aWrongSideTurn = false;

    if( aEntryLanes.empty() || aMajorAxis == VECTOR2I( 0, 0 ) || aRoutingDir == VECTOR2I( 0, 0 ) )
        return;

    if( areParallel( aRoutingDir, aMajorAxis ) )
        return;

    int desiredSide = signum( (int64_t) aMajorAxis.x * aRoutingDir.y
                            - (int64_t) aMajorAxis.y * aRoutingDir.x );

    if( desiredSide == 0 )
        return;

    double threshold = (double) std::max( 1, aThreshold );
    bool   reachedPositiveSide = false;
    bool   reachedNegativeSide = false;

    for( const SHAPE_LINE_CHAIN& lane : aEntryLanes )
    {
        for( int i = 0; i < lane.PointCount(); ++i )
        {
            double departure = signedAxisDeparture( lane.CPoint( i ), aCentroid, aMajorAxis );

            if( departure >= threshold )
                reachedPositiveSide = true;

            if( departure <= -threshold )
                reachedNegativeSide = true;
        }
    }

    bool reachedDesiredSide = desiredSide > 0 ? reachedPositiveSide : reachedNegativeSide;
    bool reachedOppositeSide = desiredSide > 0 ? reachedNegativeSide : reachedPositiveSide;

    if( !reachedPositiveSide && !reachedNegativeSide )
        aLacksTurnProgress = true;
    else if( !reachedDesiredSide && reachedOppositeSide )
        aWrongSideTurn = true;
}


static VECTOR2I bundleMajorAxis( const std::vector<VECTOR2I>& aAnchors )
{
    if( aAnchors.size() < 2 )
        return VECTOR2I( 0, 0 );

    int minX = aAnchors.front().x;
    int maxX = aAnchors.front().x;
    int minY = aAnchors.front().y;
    int maxY = aAnchors.front().y;

    for( const VECTOR2I& anchor : aAnchors )
    {
        minX = std::min( minX, anchor.x );
        maxX = std::max( maxX, anchor.x );
        minY = std::min( minY, anchor.y );
        maxY = std::max( maxY, anchor.y );
    }

    int spanX = maxX - minX;
    int spanY = maxY - minY;

    if( spanX == 0 && spanY == 0 )
        return VECTOR2I( 0, 0 );

    if( spanX > spanY )
        return VECTOR2I( 1, 0 );

    if( spanY > spanX )
        return VECTOR2I( 0, 1 );

    std::vector<VECTOR2I> sorted( aAnchors );

    std::sort( sorted.begin(), sorted.end(), []( const VECTOR2I& aA, const VECTOR2I& aB )
    {
        if( aA.x != aB.x )
            return aA.x < aB.x;

        return aA.y < aB.y;
    } );

    VECTOR2I delta = sorted.back() - sorted.front();

    if( delta == VECTOR2I( 0, 0 ) )
        return VECTOR2I( 0, 0 );

    DIRECTION_45 dir( delta );
    return dir.ToVector();
}


static bool buildLaneLayout( const std::vector<VECTOR2I>& aAnchors,
                             const VECTOR2I& aCentroid,
                             const SHAPE_LINE_CHAIN& aSpine,
                             int aTargetPitch,
                             int aBaseEscape,
                             LANE_LAYOUT& aLayout )
{
    if( aAnchors.empty() || aSpine.SegmentCount() < 1 || aTargetPitch <= 0 )
        return false;

    SEG      firstSeg = aSpine.CSegment( 0 );
    VECTOR2I direction = firstSeg.B - firstSeg.A;
    double   directionLength = direction.EuclideanNorm();

    if( directionLength < 1.0 )
        return false;

    aLayout.direction = direction;
    aLayout.perpendicular = VECTOR2I( -direction.y, direction.x );
    aLayout.centroid = aCentroid;
    aLayout.directionLength = directionLength;
    aLayout.targetPitch = aTargetPitch;
    aLayout.baseEscape = aBaseEscape;
    aLayout.mergeAlongDistance = 0;
    aLayout.startOffsets.assign( aAnchors.size(), 0 );
    aLayout.alongOffsets.assign( aAnchors.size(), 0 );
    aLayout.targetOffsets.assign( aAnchors.size(), 0 );

    std::vector<int> ordering( aAnchors.size() );

    for( size_t i = 0; i < aAnchors.size(); ++i )
    {
        VECTOR2I rel = aAnchors[i] - aCentroid;
        double   perpProj =
                ( (double) rel.x * aLayout.perpendicular.x
                + (double) rel.y * aLayout.perpendicular.y ) / directionLength;
        double   alongProj =
                ( (double) rel.x * direction.x
                + (double) rel.y * direction.y ) / directionLength;

        aLayout.startOffsets[i] = KiROUND( perpProj );
        aLayout.alongOffsets[i] = KiROUND( alongProj );
        ordering[i] = static_cast<int>( i );
    }

    std::sort( ordering.begin(), ordering.end(), [&]( int aA, int aB )
    {
        if( aLayout.startOffsets[aA] != aLayout.startOffsets[aB] )
            return aLayout.startOffsets[aA] < aLayout.startOffsets[aB];

        if( aLayout.alongOffsets[aA] != aLayout.alongOffsets[aB] )
            return aLayout.alongOffsets[aA] < aLayout.alongOffsets[aB];

        return aA < aB;
    } );

    std::vector<int> escapeRanks( aAnchors.size(), 0 );
    std::vector<int> tierValues;

    tierValues.reserve( aAnchors.size() );

    for( size_t k = 0; k < ordering.size(); ++k )
    {
        int idx = ordering[k];
        aLayout.targetOffsets[idx] = KiROUND(
                ( (double) k - ( (double) ordering.size() - 1.0 ) / 2.0 ) * (double) aTargetPitch );
        tierValues.push_back( std::abs( aLayout.targetOffsets[idx] ) );
    }

    std::sort( tierValues.begin(), tierValues.end(), std::greater<int>() );
    tierValues.erase( std::unique( tierValues.begin(), tierValues.end() ), tierValues.end() );

    for( size_t i = 0; i < aAnchors.size(); ++i )
    {
        int absOffset = std::abs( aLayout.targetOffsets[i] );
        auto it = std::find( tierValues.begin(), tierValues.end(), absOffset );

        if( it != tierValues.end() )
            escapeRanks[i] = std::distance( tierValues.begin(), it );

        int shift = std::abs( aLayout.targetOffsets[i] - aLayout.startOffsets[i] );
        int mergeNeed = aLayout.alongOffsets[i] + aBaseEscape + escapeRanks[i] * aTargetPitch + shift;
        aLayout.mergeAlongDistance = std::max( aLayout.mergeAlongDistance, mergeNeed );
    }

    return true;
}


static std::vector<int> orderPointsForLayout( const std::vector<VECTOR2I>& aPoints,
                                              const LANE_LAYOUT& aLayout )
{
    std::vector<int> ordering( aPoints.size() );

    for( size_t i = 0; i < aPoints.size(); ++i )
        ordering[i] = static_cast<int>( i );

    std::sort( ordering.begin(), ordering.end(), [&]( int aA, int aB )
    {
        VECTOR2I relA = aPoints[aA] - aLayout.centroid;
        VECTOR2I relB = aPoints[aB] - aLayout.centroid;
        double perpA = ( (double) relA.x * aLayout.perpendicular.x
                       + (double) relA.y * aLayout.perpendicular.y ) / aLayout.directionLength;
        double perpB = ( (double) relB.x * aLayout.perpendicular.x
                       + (double) relB.y * aLayout.perpendicular.y ) / aLayout.directionLength;

        if( KiROUND( perpA ) != KiROUND( perpB ) )
            return KiROUND( perpA ) < KiROUND( perpB );

        double alongA = ( (double) relA.x * aLayout.direction.x
                        + (double) relA.y * aLayout.direction.y ) / aLayout.directionLength;
        double alongB = ( (double) relB.x * aLayout.direction.x
                        + (double) relB.y * aLayout.direction.y ) / aLayout.directionLength;

        if( KiROUND( alongA ) != KiROUND( alongB ) )
            return KiROUND( alongA ) < KiROUND( alongB );

        return aA < aB;
    } );

    return ordering;
}


static int countPreMergeCrossings( const std::vector<SHAPE_LINE_CHAIN>& aEntryLanes )
{
    int crossingCount = 0;

    for( size_t i = 0; i < aEntryLanes.size(); ++i )
    {
        for( size_t j = i + 1; j < aEntryLanes.size(); ++j )
        {
            SHAPE_LINE_CHAIN::INTERSECTIONS intersections;

            if( aEntryLanes[i].Intersect( aEntryLanes[j], intersections, true ) > 0 )
                ++crossingCount;
        }
    }

    return crossingCount;
}


static int computeBaseEscapeLength( const BUNDLE_PRIMITIVE_GROUP& aStart,
                                    bool aChainedPlacement,
                                    int aCurrentLayer,
                                    const SIZES_SETTINGS& aSizes,
                                    const VECTOR2I& aDirection,
                                    double aDirectionLength )
{
    int targetPitch = aSizes.BundleGap() + aSizes.TrackWidth();

    if( targetPitch <= 0 )
        return 0;

    if( aDirectionLength < 1.0 )
        return targetPitch;

    if( aChainedPlacement )
        return targetPitch;

    int maxHalfExtent = 0;

    for( ITEM* item : aStart.Primitives() )
    {
        if( !item )
            continue;

        int halfExtent = 0;

        switch( item->Kind() )
        {
        case ITEM::SOLID_T:
        {
            const SHAPE* shape = item->Shape( -1 );

            if( shape )
            {
                BOX2I bbox = shape->BBox();
                int   halfW = bbox.GetWidth() / 2;
                int   halfH = bbox.GetHeight() / 2;

                halfExtent = KiROUND( halfW * std::abs( aDirection.x / aDirectionLength )
                                    + halfH * std::abs( aDirection.y / aDirectionLength ) );
            }

            break;
        }
        case ITEM::VIA_T:
            halfExtent = static_cast<const VIA*>( item )->Diameter( aCurrentLayer ) / 2;
            break;

        case ITEM::SEGMENT_T:
            halfExtent = static_cast<const SEGMENT*>( item )->Width() / 2;
            break;

        case ITEM::ARC_T:
            halfExtent = static_cast<const ARC*>( item )->Width() / 2;
            break;

        default:
            break;
        }

        maxHalfExtent = std::max( maxHalfExtent, halfExtent );
    }

    return maxHalfExtent > 0
         ? maxHalfExtent + aSizes.BundleGap() + aSizes.TrackWidth() / 2
         : targetPitch;
}
} // namespace


BUNDLE_PLACER::BUNDLE_PLACER( ROUTER* aRouter ) :
    PLACEMENT_ALGO( aRouter )
{
    m_state = RT_START;
    m_chainedPlacement = false;
    m_startDiagonal = false;
    m_fitOk = false;
    m_orthoMode = false;
    m_idle = true;
    m_hasFixedAnything = false;

    m_lineCount = 0;

    m_world = nullptr;
    m_currentNode = nullptr;
    m_lastNode = nullptr;
    m_lastFixNode = nullptr;

    m_placingVia = false;
    m_currentWidth = 0;
    m_currentLayer = 0;
    m_currentEndItem = nullptr;
    m_currentTraceOk = false;
    m_currentPreviewFullyValid = false;
    m_currentPreviewFrozen = false;
    m_hasLastValidPreview = false;

    m_escapeLength = 0;
    m_transitionLength = 0;
}


BUNDLE_PLACER::~BUNDLE_PLACER()
{
}


void BUNDLE_PLACER::setWorld( NODE* aWorld )
{
    m_world = aWorld;
}


void BUNDLE_PLACER::clearPreviewCache()
{
    m_currentPreviewFullyValid = false;
    m_currentPreviewFrozen = false;
    m_hasLastValidPreview = false;
    m_lastValidPreviewSpine.Clear();
    m_lastValidPreviewLanes.clear();
}


void BUNDLE_PLACER::cacheLastValidPreview( const SHAPE_LINE_CHAIN& aSpine,
                                           const std::vector<SHAPE_LINE_CHAIN>& aLanes )
{
    m_hasLastValidPreview = true;
    m_lastValidPreviewSpine = aSpine;
    m_lastValidPreviewLanes = aLanes;
}


bool BUNDLE_PLACER::restoreLastValidPreview()
{
    if( !m_hasLastValidPreview || m_lastValidPreviewLanes.size() != (size_t) m_lineCount )
        return false;

    m_currentTrace.SetShape( m_lastValidPreviewSpine );
    m_currentTrace.SetLaneShapes( m_lastValidPreviewLanes );
    m_currentPreviewFullyValid = false;
    m_currentPreviewFrozen = true;
    return true;
}


std::vector<VECTOR2I> BUNDLE_PLACER::resolveStartAnchors( const VECTOR2I& aStartPoint,
                                                          ITEM* aStartItem ) const
{
    std::vector<VECTOR2I> anchors;
    const std::vector<ITEM*>& prims = m_start.Primitives();

    anchors.reserve( prims.size() );

    for( ITEM* prim : prims )
        anchors.push_back( resolvePrimitiveStartAnchor( prim, aStartPoint, aStartItem ) );

    return anchors;
}


VECTOR2I BUNDLE_PLACER::resolvePrimitiveStartAnchor( ITEM* aPrim, const VECTOR2I& aStartPoint,
                                                     ITEM* /*aStartItem*/ ) const
{
    if( !aPrim )
        return VECTOR2I( 0, 0 );

    if( aPrim->OfKind( ITEM::SEGMENT_T | ITEM::ARC_T ) )
    {
        if( LINKED_ITEM* linked = dynamic_cast<LINKED_ITEM*>( aPrim ) )
            return resolveTrackLikeStartAnchor( linked, aStartPoint );
    }

    return aPrim->AnchorCount() > 0 ? aPrim->Anchor( 0 ) : VECTOR2I( 0, 0 );
}


VECTOR2I BUNDLE_PLACER::resolveTrackLikeStartAnchor( LINKED_ITEM* aPrim,
                                                     const VECTOR2I& aStartPoint ) const
{
    if( !aPrim )
        return VECTOR2I( 0, 0 );

    if( !m_world )
        return aPrim->AnchorCount() > 0 ? aPrim->Anchor( 0 ) : VECTOR2I( 0, 0 );

    LINE line = m_world->AssembleLine( aPrim );

    if( line.PointCount() < 2 )
        return aPrim->AnchorCount() > 0 ? aPrim->Anchor( 0 ) : VECTOR2I( 0, 0 );

    const VECTOR2I& endpoint0 = line.CPoint( 0 );
    const VECTOR2I& endpoint1 = line.CLastPoint();
    const JOINT* joint0 = m_world->FindJoint( endpoint0, &line );
    const JOINT* joint1 = m_world->FindJoint( endpoint1, &line );
    int score0 = joint0 ? joint0->LinkCount() : 0;
    int score1 = joint1 ? joint1->LinkCount() : 0;
    bool dangling0 = score0 <= 1;
    bool dangling1 = score1 <= 1;

    if( dangling0 != dangling1 )
        return dangling0 ? endpoint0 : endpoint1;

    if( score0 != score1 )
        return score0 < score1 ? endpoint0 : endpoint1;

    int64_t dist0 = ( endpoint0 - aStartPoint ).SquaredEuclideanNorm();
    int64_t dist1 = ( endpoint1 - aStartPoint ).SquaredEuclideanNorm();

    if( dist0 != dist1 )
        return dist0 < dist1 ? endpoint0 : endpoint1;

    return endpoint0;
}


void BUNDLE_PLACER::SetOrthoMode( bool aOrthoMode )
{
    m_orthoMode = aOrthoMode;

    if( !m_idle )
        Move( m_currentEnd, nullptr );
}


bool BUNDLE_PLACER::ToggleVia( bool aEnabled )
{
    m_placingVia = aEnabled;

    if( !m_idle )
        Move( m_currentEnd, nullptr );

    return true;
}


void BUNDLE_PLACER::SetStartPads( const std::vector<ITEM*>& aPads )
{
    m_start = BUNDLE_PRIMITIVE_GROUP();
    m_start.SetPrimitives( aPads );
    m_start.SetAnchors( std::vector<VECTOR2I>( aPads.size() ) );
    m_lineCount = static_cast<int>( aPads.size() );

    // Extract nets from pads
    m_nets.clear();

    for( ITEM* pad : aPads )
    {
        m_nets.push_back( pad ? pad->Net() : nullptr );
    }
}


void BUNDLE_PLACER::SetStartPads( const std::vector<ITEM*>& aPads,
                                   const std::vector<NET_HANDLE>& aNets )
{
    m_start = BUNDLE_PRIMITIVE_GROUP();
    m_start.SetPrimitives( aPads );
    m_start.SetAnchors( std::vector<VECTOR2I>( aPads.size() ) );
    m_lineCount = static_cast<int>( aPads.size() );
    m_nets = aNets;
}


bool BUNDLE_PLACER::Start( const VECTOR2I& aP, ITEM* aStartItem )
{
    VECTOR2I p( aP );

    setWorld( Router()->GetWorld() );
    m_currentNode = m_world;

    if( m_lineCount <= 0 )
    {
        Router()->SetFailureReason( _( "No pads selected for bundle routing." ) );
        return false;
    }

    m_startAnchors = resolveStartAnchors( aP, aStartItem );
    m_start.SetAnchors( m_startAnchors );

    VECTOR2I centroid = m_start.Centroid();

    // Compute escape/transition geometry without reordering lane identity.
    int targetPitch = m_sizes.BundleGap() + m_sizes.TrackWidth();
    int maxPadSpacing = 0;

    for( size_t i = 0; i < m_startAnchors.size(); ++i )
    {
        int nearestSpacing = std::numeric_limits<int>::max();

        for( size_t j = 0; j < m_startAnchors.size(); ++j )
        {
            if( i == j )
                continue;

            nearestSpacing = std::min( nearestSpacing,
                                       ( m_startAnchors[i] - m_startAnchors[j] ).EuclideanNorm() );
        }

        if( nearestSpacing != std::numeric_limits<int>::max() )
            maxPadSpacing = std::max( maxPadSpacing, nearestSpacing );
    }

    m_escapeLength = std::max( 3 * targetPitch, 3 * maxPadSpacing );
    m_transitionLength = 8 * targetPitch;

    // Set up the bundle
    m_currentWidth = m_sizes.TrackWidth();

    m_currentTrace = BUNDLE( m_lineCount, m_currentWidth, m_sizes.BundleGap() );
    m_currentTrace.SetNets( m_nets );

    m_p_start = centroid;
    m_currentStart = p;
    m_currentEnd = p;
    m_placingVia = false;
    m_chainedPlacement = false;
    m_currentTraceOk = false;
    m_lastFixNode = nullptr;
    clearPreviewCache();

    initPlacement();

    return true;
}


void BUNDLE_PLACER::initPlacement()
{
    m_idle = false;
    m_orthoMode = false;
    m_currentEndItem = nullptr;

    NODE* world = Router()->GetWorld();

    world->KillChildren();
    NODE* rootNode = world->Branch();

    setWorld( rootNode );

    m_lastNode = nullptr;
    m_currentNode = rootNode;

    m_shove = std::make_unique<SHOVE>( m_currentNode, Router() );
}


PITCH_PROFILE BUNDLE_PLACER::buildPitchProfile( const SHAPE_LINE_CHAIN& aSpine ) const
{
    PITCH_PROFILE profile;
    int targetPitch = m_sizes.BundleGap() + m_sizes.TrackWidth();

    if( m_startAnchors.empty() || m_lineCount <= 1 || aSpine.SegmentCount() < 1 || targetPitch <= 0 )
        return profile;

    SEG firstSeg = aSpine.CSegment( 0 );
    VECTOR2I  spineDir = firstSeg.B - firstSeg.A;
    double    dirLen = spineDir.EuclideanNorm();
    int       baseEscape =
            computeBaseEscapeLength( m_start, m_chainedPlacement, m_currentLayer,
                                     m_sizes, spineDir, dirLen );
    LANE_LAYOUT layout;

    if( !buildLaneLayout( m_startAnchors, m_start.Centroid(), aSpine, targetPitch, baseEscape, layout ) )
        return profile;

    profile.startOffsets = layout.startOffsets;
    profile.targetOffsets = layout.targetOffsets;

    bool needsTransition = false;

    for( int i = 0; i < m_lineCount; i++ )
    {
        if( std::abs( profile.startOffsets[i] - profile.targetOffsets[i] ) > targetPitch / 4 )
            needsTransition = true;
    }

    if( needsTransition )
    {
        profile.escapeLength = std::max( m_escapeLength, layout.baseEscape );
        profile.transitionLength = m_transitionLength;
    }

    return profile;
}


bool BUNDLE_PLACER::buildFanoutLanes( const SHAPE_LINE_CHAIN& aSpine,
                                      bool aStartDiagonal,
                                      std::vector<SHAPE_LINE_CHAIN>& aLanes,
                                      std::vector<SHAPE_LINE_CHAIN>* aEntryLanes ) const
{
    if( m_lineCount <= 1 || (int) m_startAnchors.size() != m_lineCount )
        return false;

    if( aSpine.SegmentCount() < 1 )
        return false;

    int targetPitch = m_sizes.BundleGap() + m_sizes.TrackWidth();

    if( targetPitch <= 0 )
        return false;

    SEG firstSeg = aSpine.CSegment( 0 );
    VECTOR2I spineDir = firstSeg.B - firstSeg.A;
    double   dirLen = spineDir.EuclideanNorm();
    int      baseEscape =
            computeBaseEscapeLength( m_start, m_chainedPlacement, m_currentLayer,
                                     m_sizes, spineDir, dirLen );
    LANE_LAYOUT layout;

    if( !buildLaneLayout( m_startAnchors, m_start.Centroid(), aSpine, targetPitch, baseEscape, layout ) )
        return false;

    int spineLen = aSpine.Length();
    int effectiveMergeAlong = std::min( layout.mergeAlongDistance, spineLen );

    if( effectiveMergeAlong <= 0 )
        return false;

    bool             hasFullMerge = effectiveMergeAlong == layout.mergeAlongDistance;
    SHAPE_LINE_CHAIN clippedSpine( aSpine );
    SHAPE_LINE_CHAIN postSpine;

    if( effectiveMergeAlong < spineLen )
    {
        VECTOR2I splitPt = aSpine.PointAlong( effectiveMergeAlong );
        SHAPE_LINE_CHAIN spineCopy( aSpine );
        int splitIdx = spineCopy.Split( splitPt );

        if( splitIdx < 0 )
            return false;

        clippedSpine = spineCopy.Slice( 0, splitIdx );

        if( hasFullMerge && splitIdx < spineCopy.PointCount() - 1 )
            postSpine = spineCopy.Slice( splitIdx, -1 );
    }

    aLanes.resize( m_lineCount );

    if( aEntryLanes )
        aEntryLanes->resize( m_lineCount );

    for( int i = 0; i < m_lineCount; i++ )
    {
        SHAPE_LINE_CHAIN mergeLane = BUNDLE::OffsetPolyline45( clippedSpine, layout.targetOffsets[i] );

        if( mergeLane.PointCount() < 1 )
            return false;

        VECTOR2I mergeAnchor = mergeLane.CLastPoint();
        SHAPE_LINE_CHAIN lane;

        if( mergeAnchor == m_startAnchors[i] )
        {
            lane.Append( mergeAnchor );
        }
        else
        {
            lane = DIRECTION_45().BuildInitialTrace( m_startAnchors[i], mergeAnchor, aStartDiagonal );
        }

        if( lane.PointCount() == 0 )
            return false;

        if( aEntryLanes )
            ( *aEntryLanes )[i] = lane;

        if( hasFullMerge && postSpine.PointCount() >= 2 )
        {
            SHAPE_LINE_CHAIN uniformLane = BUNDLE::OffsetPolyline45( postSpine, layout.targetOffsets[i] );
            int startIdx = uniformLane.PointCount() > 0 && uniformLane.CPoint( 0 ) == lane.CLastPoint()
                         ? 1
                         : 0;

            for( int j = startIdx; j < uniformLane.PointCount(); ++j )
                lane.Append( uniformLane.CPoint( j ) );
        }

        if( lane.PointCount() > 2 )
            lane.Simplify();

        aLanes[i] = lane;
    }

    return true;
}


bool BUNDLE_PLACER::buildPreviewCandidate( const SHAPE_LINE_CHAIN& aSpine,
                                           bool aStartDiagonal,
                                           const VECTOR2I& aRoutingDir,
                                           std::vector<SHAPE_LINE_CHAIN>& aLanes,
                                           PREVIEW_METRICS& aMetrics ) const
{
    aMetrics = PREVIEW_METRICS();
    std::vector<SHAPE_LINE_CHAIN> entryLanes;

    if( !buildFanoutLanes( aSpine, aStartDiagonal, aLanes, &entryLanes ) )
        return false;

    aMetrics.anchoredGeometryBuilt = true;

    int targetPitch = m_sizes.BundleGap() + m_sizes.TrackWidth();
    SEG firstSeg = aSpine.CSegment( 0 );
    VECTOR2I spineDir = firstSeg.B - firstSeg.A;
    double dirLen = spineDir.EuclideanNorm();
    int baseEscape =
            computeBaseEscapeLength( m_start, m_chainedPlacement, m_currentLayer,
                                     m_sizes, spineDir, dirLen );
    LANE_LAYOUT layout;

    if( !buildLaneLayout( m_startAnchors, m_start.Centroid(), aSpine, targetPitch, baseEscape, layout ) )
        return false;

    VECTOR2I majorAxis = bundleMajorAxis( m_startAnchors );
    int      turnThreshold = targetPitch / 2;

    evaluateTurnProgress( entryLanes, m_start.Centroid(), majorAxis, aRoutingDir,
                          turnThreshold, aMetrics.lacksTurnProgress,
                          aMetrics.wrongSideTurn );

    std::vector<VECTOR2I> entryStarts;
    std::vector<VECTOR2I> entryEnds;

    entryStarts.reserve( entryLanes.size() );
    entryEnds.reserve( entryLanes.size() );

    for( int i = 0; i < m_lineCount; ++i )
    {
        const SHAPE_LINE_CHAIN& lane = aLanes[i];
        const SHAPE_LINE_CHAIN& entryLane = entryLanes[i];

        if( lane.PointCount() == 0 || lane.CPoint( 0 ) != m_startAnchors[i] )
            return false;

        if( lane.SelfIntersecting() || entryLane.SelfIntersecting() )
            aMetrics.hasSelfIntersections = true;

        if( lineHasBackwardSegment( lane, aRoutingDir ) )
            aMetrics.hasBackwardSegments = true;

        aMetrics.totalLength += lane.Length();
        aMetrics.maxLength = std::max( aMetrics.maxLength, lane.Length() );
        entryStarts.push_back( entryLane.CPoint( 0 ) );
        entryEnds.push_back( entryLane.CLastPoint() );
    }

    std::vector<int> startOrder = orderPointsForLayout( entryStarts, layout );
    std::vector<int> endOrder = orderPointsForLayout( entryEnds, layout );

    if( startOrder != endOrder && countPreMergeCrossings( entryLanes ) > 0 )
        aMetrics.hasCrossings = true;

    aMetrics.topologyViolations = ( aMetrics.hasBackwardSegments ? 1 : 0 )
                                + ( aMetrics.hasSelfIntersections ? 1 : 0 )
                                + ( aMetrics.hasCrossings ? 1 : 0 );

    return true;
}


bool BUNDLE_PLACER::choosePreviewForSpine( const SHAPE_LINE_CHAIN& aSpine,
                                           const VECTOR2I& aRoutingDir,
                                           PREVIEW_SELECTION& aSelection ) const
{
    auto prefersSelection =
            [&]( const PREVIEW_SELECTION& aA, const PREVIEW_SELECTION& aB ) -> bool
            {
                if( aA.fullyValid != aB.fullyValid )
                    return aA.fullyValid;

                if( aA.metrics.topologyViolations != aB.metrics.topologyViolations )
                    return aA.metrics.topologyViolations < aB.metrics.topologyViolations;

                if( aA.metrics.lacksTurnProgress != aB.metrics.lacksTurnProgress )
                    return !aA.metrics.lacksTurnProgress;

                if( aA.metrics.wrongSideTurn != aB.metrics.wrongSideTurn )
                    return !aA.metrics.wrongSideTurn;

                if( aA.metrics.totalLength != aB.metrics.totalLength )
                    return aA.metrics.totalLength < aB.metrics.totalLength;

                if( aA.metrics.maxLength != aB.metrics.maxLength )
                    return aA.metrics.maxLength < aB.metrics.maxLength;

                bool preferredA = aA.startDiagonal == m_startDiagonal;
                bool preferredB = aB.startDiagonal == m_startDiagonal;

                if( preferredA != preferredB )
                    return preferredA;

                return aA.order < aB.order;
            };

    bool haveSelection = false;

    for( int order = 0; order < 2; ++order )
    {
        PREVIEW_SELECTION candidate;
        candidate.spine = aSpine;
        candidate.startDiagonal = order != 0;
        candidate.order = order;
        candidate.anchoredGeometryBuilt = buildPreviewCandidate( aSpine, candidate.startDiagonal,
                                                                 aRoutingDir, candidate.lanes,
                                                                 candidate.metrics );

        if( !candidate.anchoredGeometryBuilt )
            continue;

        candidate.fullyValid = !candidate.metrics.hasBackwardSegments
                            && !candidate.metrics.hasSelfIntersections
                            && !candidate.metrics.hasCrossings
                            && !candidate.metrics.lacksTurnProgress
                            && !candidate.metrics.wrongSideTurn;

        if( !haveSelection || prefersSelection( candidate, aSelection ) )
        {
            aSelection = std::move( candidate );
            haveSelection = true;
        }
    }

    return haveSelection;
}


bool BUNDLE_PLACER::buildSpineCandidates( const VECTOR2I& aP,
                                          std::vector<SPINE_CANDIDATE>& aCandidates ) const
{
    aCandidates.clear();

    if( aP == m_p_start )
        return false;

    for( bool startDiagonal : { false, true } )
    {
        SHAPE_LINE_CHAIN spine = DIRECTION_45().BuildInitialTrace( m_p_start, aP, startDiagonal );

        if( spine.PointCount() < 2 )
            continue;

        bool duplicate = false;

        for( const SPINE_CANDIDATE& existing : aCandidates )
        {
            if( sameLineChainGeometry( existing.spine, spine ) )
            {
                duplicate = true;
                break;
            }
        }

        if( !duplicate )
            aCandidates.push_back( { spine, startDiagonal } );
    }

    return !aCandidates.empty();
}


bool BUNDLE_PLACER::applySpinePreservingAnchors( const SHAPE_LINE_CHAIN& aSpine,
                                                 bool aAllowProfileFallback )
{
    if( aSpine.PointCount() < 2 )
        return false;

    m_currentTrace.SetShape( aSpine );

    if( m_lineCount <= 1 || (int) m_startAnchors.size() != m_lineCount )
    {
        m_currentPreviewFullyValid = true;
        m_currentPreviewFrozen = false;
        return true;
    }

    VECTOR2I routingDir = aSpine.CLastPoint() - m_p_start;
    PREVIEW_SELECTION selection;

    if( choosePreviewForSpine( aSpine, routingDir, selection ) )
    {
        if( selection.fullyValid )
        {
            m_currentTrace.SetShape( selection.spine );
            m_currentTrace.SetLaneShapes( selection.lanes );
            cacheLastValidPreview( selection.spine, selection.lanes );
            m_currentPreviewFullyValid = true;
            m_currentPreviewFrozen = false;
            return true;
        }

        if( restoreLastValidPreview() )
            return true;

        m_currentTrace.SetShape( selection.spine );
        m_currentTrace.SetLaneShapes( selection.lanes );
        m_currentPreviewFullyValid = false;
        m_currentPreviewFrozen = false;
        return true;
    }

    if( restoreLastValidPreview() )
        return true;

    if( !aAllowProfileFallback )
        return false;

    return false;
}


bool BUNDLE_PLACER::routeHead( const VECTOR2I& aP )
{
    m_fitOk = false;
    std::vector<SPINE_CANDIDATE> spineCandidates;

    if( !buildSpineCandidates( aP, spineCandidates ) )
        return false;

    m_currentTrace.SetGap( m_sizes.BundleGap() );
    m_currentTrace.SetWidth( m_currentWidth );
    m_currentTrace.SetLayer( m_currentLayer );
    m_currentPreviewFullyValid = false;
    m_currentPreviewFrozen = false;

    if( m_lineCount <= 1 || (int) m_startAnchors.size() != m_lineCount )
    {
        std::stable_sort( spineCandidates.begin(), spineCandidates.end(),
                          [&]( const SPINE_CANDIDATE& aA, const SPINE_CANDIDATE& aB )
        {
            bool preferredA = aA.startDiagonal == m_startDiagonal;
            bool preferredB = aB.startDiagonal == m_startDiagonal;

            if( preferredA != preferredB )
                return preferredA;

            return aA.startDiagonal < aB.startDiagonal;
        } );

        m_currentTrace.SetShape( spineCandidates.front().spine );
        m_currentPreviewFullyValid = true;
        m_currentPreviewFrozen = false;
        m_currentTrace.SetNets( m_nets );
        m_currentTraceOk = true;
        return true;
    }

    VECTOR2I                     routingDir = aP - m_p_start;
    PREVIEW_SELECTION            bestSelection;
    bool                         haveBestSelection = false;

    auto prefersSelection =
            [&]( const PREVIEW_SELECTION& aA, const PREVIEW_SELECTION& aB ) -> bool
            {
                if( aA.fullyValid != aB.fullyValid )
                    return aA.fullyValid;

                if( aA.metrics.topologyViolations != aB.metrics.topologyViolations )
                    return aA.metrics.topologyViolations < aB.metrics.topologyViolations;

                if( aA.metrics.lacksTurnProgress != aB.metrics.lacksTurnProgress )
                    return !aA.metrics.lacksTurnProgress;

                if( aA.metrics.wrongSideTurn != aB.metrics.wrongSideTurn )
                    return !aA.metrics.wrongSideTurn;

                if( aA.metrics.totalLength != aB.metrics.totalLength )
                    return aA.metrics.totalLength < aB.metrics.totalLength;

                if( aA.metrics.maxLength != aB.metrics.maxLength )
                    return aA.metrics.maxLength < aB.metrics.maxLength;

                bool preferredA = aA.startDiagonal == m_startDiagonal;
                bool preferredB = aB.startDiagonal == m_startDiagonal;

                if( preferredA != preferredB )
                    return preferredA;

                return aA.order < aB.order;
            };

    for( size_t i = 0; i < spineCandidates.size(); ++i )
    {
        PREVIEW_SELECTION selection;

        if( !choosePreviewForSpine( spineCandidates[i].spine, routingDir, selection ) )
            continue;

        selection.order = static_cast<int>( i );

        if( !haveBestSelection || prefersSelection( selection, bestSelection ) )
        {
            bestSelection = std::move( selection );
            haveBestSelection = true;
        }
    }

    if( haveBestSelection )
    {
        if( bestSelection.fullyValid )
        {
            m_currentTrace.SetShape( bestSelection.spine );
            m_currentTrace.SetLaneShapes( bestSelection.lanes );
            cacheLastValidPreview( bestSelection.spine, bestSelection.lanes );
            m_currentPreviewFullyValid = true;
            m_currentPreviewFrozen = false;
        }
        else if( restoreLastValidPreview() )
        {
            // frozen preview restored
        }
        else
        {
            m_currentTrace.SetShape( bestSelection.spine );
            m_currentTrace.SetLaneShapes( bestSelection.lanes );
            m_currentPreviewFullyValid = false;
            m_currentPreviewFrozen = false;
        }
    }
    else if( !restoreLastValidPreview() )
    {
        return false;
    }

    m_currentTrace.SetNets( m_nets );
    m_currentTraceOk = true;

    return true;
}


bool BUNDLE_PLACER::rhMarkObstacles( const VECTOR2I& aP )
{
    if( !routeHead( aP ) )
        return false;

    bool anyCollision = false;

    for( int i = 0; i < m_lineCount; i++ )
    {
        if( m_currentNode->CheckColliding( &m_currentTrace.Line( i ) ) )
        {
            anyCollision = true;
            break;
        }
    }

    m_fitOk = !anyCollision && m_currentPreviewFullyValid && !m_currentPreviewFrozen;
    return m_fitOk;
}


bool BUNDLE_PLACER::rhWalkOnly( const VECTOR2I& aP )
{
    if( !routeHead( aP ) )
        return false;

    // Try bundle-level spine walkaround:
    // Inflate obstacles by bundle half-width + clearance and walkaround the spine
    WALKAROUND walkaround( m_currentNode, Router() );

    walkaround.SetSolidsOnly( false );
    walkaround.SetIterationLimit( Settings().WalkaroundIterationLimit() );
    walkaround.SetAllowedPolicies( { WALKAROUND::WP_SHORTEST } );

    // Create a virtual spine LINE with inflated width for walkaround
    LINE spineLine;
    spineLine.SetShape( m_currentTrace.Spine() );
    spineLine.SetWidth( m_currentTrace.TotalWidth() + m_sizes.Clearance() * 2 );
    spineLine.SetLayer( m_currentLayer );

    if( m_nets.size() > 0 )
        spineLine.SetNet( m_nets[0] );

    auto wf = walkaround.Route( spineLine );

    if( wf.status[WALKAROUND::WP_SHORTEST] == WALKAROUND::ST_DONE )
    {
        SHAPE_LINE_CHAIN walkedSpine = wf.lines[WALKAROUND::WP_SHORTEST].CLine();

        if( !applySpinePreservingAnchors( walkedSpine ) )
        {
            m_fitOk = false;
            return false;
        }

        // Verify all lanes are collision-free
        bool allClear = true;

        for( int i = 0; i < m_lineCount; i++ )
        {
            if( m_currentNode->CheckColliding( &m_currentTrace.Line( i ) ) )
            {
                allClear = false;
                break;
            }
        }

        m_fitOk = allClear && m_currentPreviewFullyValid && !m_currentPreviewFrozen;
    }
    else
    {
        m_fitOk = false;
    }

    return m_fitOk;
}


bool BUNDLE_PLACER::rhShoveOnly( const VECTOR2I& aP )
{
    m_currentNode = m_shove->CurrentNode();

    if( !routeHead( aP ) )
        return false;

    m_fitOk = false;

    // Phase 0: Check if we already fit without shoving
    bool allClear = true;

    for( int i = 0; i < m_lineCount; i++ )
    {
        if( m_currentNode->CheckColliding( &m_currentTrace.Line( i ) ) )
        {
            allClear = false;
            break;
        }
    }

    if( allClear )
    {
        // No collision in current shove state. Still run SHOVE so that
        // reduceSpringback() can pop stale springback nodes, allowing
        // previously-shoved obstacles to spring back to their original
        // positions (mirroring LINE_PLACER behaviour).
        m_shove->ClearHeads();

        for( int i = 0; i < m_lineCount; i++ )
        {
            LINE lane( m_currentTrace.Line( i ) );
            m_shove->AddHeads( lane, SHOVE::SHP_SHOVE | SHOVE::SHP_IGNORE );
        }

        SHOVE::SHOVE_STATUS status = m_shove->Run();
        m_currentNode = m_shove->CurrentNode();

        if( status == SHOVE::SH_OK )
        {
            // Update lane geometry from shove results (springback may have
            // restored obstacles, causing Run() to re-shove and modify heads)
            std::vector<SHAPE_LINE_CHAIN> shovedLanes( m_lineCount );

            for( int i = 0; i < m_lineCount; i++ )
            {
                if( m_shove->HeadsModified( i ) )
                {
                    LINE modifiedHead = m_shove->GetModifiedHead( i );

                    if( modifiedHead.PointCount() > 0 && modifiedHead.CPoint( 0 ) == m_startAnchors[i] )
                        shovedLanes[i] = modifiedHead.CLine();
                    else
                        shovedLanes[i] = m_currentTrace.CLane( i );
                }
                else
                {
                    shovedLanes[i] = m_currentTrace.CLane( i );
                }
            }

            m_currentTrace.SetLaneShapes( shovedLanes );
        }

        m_fitOk = m_currentPreviewFullyValid && !m_currentPreviewFrozen;
        return m_fitOk;
    }

    // Phase 1: Pre-walk against solids only, then attempt shove. This keeps
    // shove mode responsive near fixed obstacles without degrading into full
    // walkaround.
    WALKAROUND walkaround( m_currentNode, Router() );
    walkaround.SetSolidsOnly( true );
    walkaround.SetIterationLimit( Settings().WalkaroundIterationLimit() );
    walkaround.SetAllowedPolicies( { WALKAROUND::WP_SHORTEST } );

    LINE spineLine;
    spineLine.SetShape( m_currentTrace.Spine() );
    spineLine.SetWidth( m_currentTrace.TotalWidth() + m_sizes.Clearance() * 2 );
    spineLine.SetLayer( m_currentLayer );

    if( m_nets.size() > 0 )
        spineLine.SetNet( m_nets[0] );

    auto wf = walkaround.Route( spineLine );

    if( wf.status[WALKAROUND::WP_SHORTEST] == WALKAROUND::ST_DONE )
    {
        SHAPE_LINE_CHAIN walkedSpine = wf.lines[WALKAROUND::WP_SHORTEST].CLine();

        if( !applySpinePreservingAnchors( walkedSpine ) )
            return false;
    }

    // Phase 2: Use SHOVE with all N lanes as heads
    m_shove->ClearHeads();

    for( int i = 0; i < m_lineCount; i++ )
    {
        LINE lane( m_currentTrace.Line( i ) );
        m_shove->AddHeads( lane, SHOVE::SHP_SHOVE | SHOVE::SHP_IGNORE );
    }

    SHOVE::SHOVE_STATUS status = m_shove->Run();

    m_currentNode = m_shove->CurrentNode();

    if( status == SHOVE::SH_OK )
    {
        // Update lane geometry from shove results
        std::vector<SHAPE_LINE_CHAIN> shovedLanes( m_lineCount );

        for( int i = 0; i < m_lineCount; i++ )
        {
            if( m_shove->HeadsModified( i ) )
            {
                LINE modifiedHead = m_shove->GetModifiedHead( i );

                if( modifiedHead.PointCount() > 0 && modifiedHead.CPoint( 0 ) == m_startAnchors[i] )
                    shovedLanes[i] = modifiedHead.CLine();
                else
                    shovedLanes[i] = m_currentTrace.CLane( i );
            }
            else
            {
                shovedLanes[i] = m_currentTrace.CLane( i );
            }
        }

        m_currentTrace.SetLaneShapes( shovedLanes );

        // Verify all lanes
        allClear = true;

        for( int i = 0; i < m_lineCount; i++ )
        {
            if( !m_currentNode->CheckColliding( &m_currentTrace.Line( i ) ) )
                continue;

            allClear = false;
            break;
        }

        m_fitOk = allClear && m_currentPreviewFullyValid && !m_currentPreviewFrozen;
    }

    // Bundle shove mode must not silently switch to full walkaround, otherwise
    // a live mode change to shove appears to apply walkaround semantics.
    return m_fitOk;
}


bool BUNDLE_PLACER::route( const VECTOR2I& aP )
{
    switch( Settings().Mode() )
    {
    case RM_MarkObstacles:
        return rhMarkObstacles( aP );
    case RM_Walkaround:
        return rhWalkOnly( aP );
    case RM_Shove:
        return rhShoveOnly( aP );
    default:
        break;
    }

    return false;
}


const ITEM_SET BUNDLE_PLACER::Traces()
{
    ITEM_SET t;

    for( int i = 0; i < m_lineCount; i++ )
        t.Add( &m_currentTrace.Line( i ) );

    return t;
}


void BUNDLE_PLACER::FlipPosture()
{
    m_startDiagonal = !m_startDiagonal;

    if( !m_idle )
        Move( m_currentEnd, nullptr );
}


NODE* BUNDLE_PLACER::CurrentNode( bool aLoopsRemoved ) const
{
    if( m_lastNode )
        return m_lastNode;

    return m_currentNode;
}


bool BUNDLE_PLACER::SetLayer( int aLayer )
{
    if( m_idle )
    {
        m_currentLayer = aLayer;
        return true;
    }

    return false;
}


bool BUNDLE_PLACER::Move( const VECTOR2I& aP, ITEM* aEndItem )
{
    m_currentEndItem = aEndItem;
    m_fitOk = false;

    delete m_lastNode;
    m_lastNode = nullptr;

    bool retval = route( aP );

    NODE* latestNode = m_currentNode;
    m_lastNode = latestNode->Branch();

    assert( m_lastNode != nullptr );
    m_currentEnd = aP;

    return retval;
}


void BUNDLE_PLACER::UpdateSizes( const SIZES_SETTINGS& aSizes )
{
    m_sizes = aSizes;

    if( !m_idle )
    {
        m_currentWidth = m_sizes.TrackWidth();
        m_currentTrace.SetWidth( m_currentWidth );
        m_currentTrace.SetGap( m_sizes.BundleGap() );
    }
}


bool BUNDLE_PLACER::FixRoute( const VECTOR2I& aP, ITEM* aEndItem, bool aForceFinish )
{
    // Note: unlike LINE_PLACER, we intentionally skip the per-lane collision check here.
    // Bundle lanes of different nets inevitably pass near each other's starting pads,
    // causing CheckColliding to always report false positives. The routing modes
    // (MarkObstacles/Walkaround/Shove) already provide visual collision feedback
    // during Move(), and the DRC will catch real violations after commit.

    // Check that we have valid lane geometry
    for( int i = 0; i < m_lineCount; i++ )
    {
        if( m_currentTrace.CLane( i ).SegmentCount() < 1 )
            return false;
    }

    TOPOLOGY topo( m_lastNode );

    if( !aForceFinish && !Settings().GetFixAllSegments() )
    {
        // Remove last segment of each lane (the volatile head)
        std::vector<SHAPE_LINE_CHAIN> trimmedLanes( m_lineCount );

        for( int i = 0; i < m_lineCount; i++ )
        {
            trimmedLanes[i] = m_currentTrace.CLane( i );

            if( trimmedLanes[i].SegmentCount() > 1 )
                trimmedLanes[i].Remove( -1, -1 );
        }

        m_currentTrace.SetLaneShapes( trimmedLanes );
    }

    // Add all N lines to the node
    for( int i = 0; i < m_lineCount; i++ )
    {
        LINE lane( m_currentTrace.Line( i ) );
        m_lastNode->Add( lane );
        topo.SimplifyLine( &lane );
    }

    m_lastFixNode = m_lastNode;

    // Avoid use-after-free: CommitPlacement calls NODE::Commit which invalidates shove state
    if( Settings().Mode() == RM_Shove )
        m_shove = std::make_unique<SHOVE>( m_world, Router() );

    m_hasFixedAnything = true;
    CommitPlacement();
    m_placingVia = false;
    m_lastFixNode = nullptr;

    if( aForceFinish )
    {
        clearPreviewCache();
        m_idle = true;
        return true;
    }
    else
    {
        // Update start point for chained placement
        VECTOR2I newStart( 0, 0 );

        for( int i = 0; i < m_lineCount; i++ )
        {
            if( m_currentTrace.CLane( i ).PointCount() > 0 )
            {
                m_startAnchors[i] = m_currentTrace.CLane( i ).CLastPoint();
                newStart += m_startAnchors[i];
            }
        }

        if( m_lineCount > 0 )
            m_p_start = VECTOR2I( newStart.x / m_lineCount, newStart.y / m_lineCount );

        m_start.SetAnchors( m_startAnchors );

        m_chainedPlacement = true;
        clearPreviewCache();
        initPlacement();
        return false;
    }
}


bool BUNDLE_PLACER::AbortPlacement()
{
    m_world->KillChildren();
    m_lastNode = nullptr;
    clearPreviewCache();
    return true;
}


bool BUNDLE_PLACER::HasPlacedAnything() const
{
    return m_hasFixedAnything;
}


bool BUNDLE_PLACER::CommitPlacement()
{
    if( m_lastFixNode )
        Router()->CommitRouting( m_lastFixNode );

    m_lastFixNode = nullptr;
    m_lastNode = nullptr;
    m_currentNode = nullptr;
    clearPreviewCache();
    return true;
}


void BUNDLE_PLACER::GetModifiedNets( std::vector<NET_HANDLE>& aNets ) const
{
    for( const auto& net : m_nets )
        aNets.push_back( net );
}


const std::vector<NET_HANDLE> BUNDLE_PLACER::CurrentNets() const
{
    return m_nets;
}

} // namespace PNS
