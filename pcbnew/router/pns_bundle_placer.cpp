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

#include <cmath>
#include <algorithm>

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
    m_start = BUNDLE_PRIMITIVE_GROUP( aPads );
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
    m_start = BUNDLE_PRIMITIVE_GROUP( aPads );
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

    // Sort the start anchors perpendicular to routing direction
    VECTOR2I routingDir;

    // Use cursor position relative to centroid to determine routing direction
    VECTOR2I centroid = m_start.Centroid();
    VECTOR2I toCursor = aP - centroid;

    if( std::abs( toCursor.x ) > std::abs( toCursor.y ) )
        routingDir = VECTOR2I( ( toCursor.x > 0 ) ? 1 : -1, 0 );
    else
        routingDir = VECTOR2I( 0, ( toCursor.y > 0 ) ? 1 : -1 );

    m_start.SortByPosition( routingDir );
    m_startAnchors = m_start.Anchors();

    // Compute escape/transition geometry
    int targetPitch = m_sizes.BundleGap() + m_sizes.TrackWidth();
    int maxPadSpacing = 0;

    for( size_t i = 1; i < m_startAnchors.size(); i++ )
    {
        int dist = ( m_startAnchors[i] - m_startAnchors[i - 1] ).EuclideanNorm();
        maxPadSpacing = std::max( maxPadSpacing, dist );
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

    if( m_startAnchors.empty() || m_lineCount <= 1 || aSpine.SegmentCount() < 1 )
        return profile;

    VECTOR2I centroid = m_start.Centroid();

    profile.startOffsets.resize( m_lineCount );
    profile.targetOffsets.resize( m_lineCount );

    // Compute the perpendicular (left normal) of the spine's first segment
    SEG firstSeg = aSpine.CSegment( 0 );
    VECTOR2I spineDir = firstSeg.B - firstSeg.A;
    VECTOR2I perp( -spineDir.y, spineDir.x );  // left normal
    double perpLen = perp.EuclideanNorm();

    if( perpLen < 1.0 )
        return profile;

    // For each pad, project (padPos - centroid) onto the perpendicular to get real start offset
    for( int i = 0; i < m_lineCount; i++ )
    {
        VECTOR2I rel = m_startAnchors[i] - centroid;
        double proj = ( (double) rel.x * perp.x + (double) rel.y * perp.y ) / perpLen;
        profile.startOffsets[i] = KiROUND( proj );
    }

    // Sort pad indices by projection onto current spine perpendicular
    // to prevent lane crossing when spine direction differs from initial sort direction
    std::vector<int> perpOrder( m_lineCount );

    for( int i = 0; i < m_lineCount; i++ )
        perpOrder[i] = i;

    std::sort( perpOrder.begin(), perpOrder.end(), [&]( int a, int b )
    {
        return profile.startOffsets[a] < profile.startOffsets[b];
    } );

    // Assign target offsets in perpendicular order (not by original index)
    for( int k = 0; k < m_lineCount; k++ )
    {
        int i = perpOrder[k];
        profile.targetOffsets[i] = KiROUND(
            ( (double) k - ( (double) m_lineCount - 1.0 ) / 2.0 ) * (double) targetPitch );
    }

    bool needsTransition = false;

    for( int i = 0; i < m_lineCount; i++ )
    {
        if( std::abs( profile.startOffsets[i] - profile.targetOffsets[i] ) > targetPitch / 4 )
            needsTransition = true;
    }

    if( needsTransition )
    {
        profile.escapeLength = m_escapeLength;
        profile.transitionLength = m_transitionLength;
    }

    return profile;
}


bool BUNDLE_PLACER::buildFanoutLanes( const SHAPE_LINE_CHAIN& aSpine,
                                      std::vector<SHAPE_LINE_CHAIN>& aLanes ) const
{
    if( m_lineCount <= 1 || (int) m_startAnchors.size() != m_lineCount )
        return false;

    if( aSpine.SegmentCount() < 1 )
        return false;

    int targetPitch = m_sizes.BundleGap() + m_sizes.TrackWidth();

    if( targetPitch <= 0 )
        return false;

    // Get spine direction D and perpendicular P from first segment (integer vectors)
    SEG firstSeg = aSpine.CSegment( 0 );
    VECTOR2I D = firstSeg.B - firstSeg.A;
    VECTOR2I P( -D.y, D.x );  // left perpendicular
    double dLen = D.EuclideanNorm();

    if( dLen < 1.0 )
        return false;

    VECTOR2I centroid = m_start.Centroid();

    // For each anchor, compute perpendicular offset (startOff), target offset, shift, and
    // along-spine offset
    std::vector<int> startOff( m_lineCount );
    std::vector<int> targetOff( m_lineCount );
    std::vector<int> shift( m_lineCount );
    std::vector<int> alongOff( m_lineCount );

    for( int i = 0; i < m_lineCount; i++ )
    {
        VECTOR2I rel = m_startAnchors[i] - centroid;

        // Project onto perpendicular
        double perpProj = ( (double) rel.x * P.x + (double) rel.y * P.y ) / dLen;
        startOff[i] = KiROUND( perpProj );

        // Along-spine offset from centroid
        double alongProj = ( (double) rel.x * D.x + (double) rel.y * D.y ) / dLen;
        alongOff[i] = KiROUND( alongProj );
    }

    // Sort anchor indices by projection onto CURRENT spine perpendicular P
    // This prevents lane crossing when spine direction differs from initial sort direction
    std::vector<int> perpOrder( m_lineCount );

    for( int i = 0; i < m_lineCount; i++ )
        perpOrder[i] = i;

    std::sort( perpOrder.begin(), perpOrder.end(), [&]( int a, int b )
    {
        return startOff[a] < startOff[b];
    } );

    // Assign target offsets in perpendicular order (not by original index)
    for( int k = 0; k < m_lineCount; k++ )
    {
        int i = perpOrder[k];
        targetOff[i] = KiROUND(
            ( (double) k - ( (double) m_lineCount - 1.0 ) / 2.0 ) * (double) targetPitch );
    }

    for( int i = 0; i < m_lineCount; i++ )
    {
        shift[i] = targetOff[i] - startOff[i];
    }

    // Sort indices by |shift| ascending (smallest perpendicular shift turns first)
    std::vector<int> rankOrder( m_lineCount );

    for( int i = 0; i < m_lineCount; i++ )
        rankOrder[i] = i;

    std::sort( rankOrder.begin(), rankOrder.end(), [&]( int a, int b )
    {
        return std::abs( shift[a] ) < std::abs( shift[b] );
    } );

    // Compute baseEscape from the start geometry.
    // Initial placement should clear the actual start primitives before turning.
    // Chained placement starts from prior track endpoints, so a bundle pitch is
    // enough to keep the ordered transition from folding back into the corner.
    int maxHalfExtent = 0;

    if( !m_chainedPlacement )
    {
        for( ITEM* item : m_start.Primitives() )
        {
            if( !item )
                continue;

            int halfExtent = 0;

            switch( item->Kind() )
            {
            case ITEM::SOLID_T:
            {
                // Pad: project BBox half-extents onto spine direction
                const SHAPE* shape = item->Shape( -1 );

                if( shape )
                {
                    BOX2I bbox = shape->BBox();
                    int halfW = bbox.GetWidth() / 2;
                    int halfH = bbox.GetHeight() / 2;

                    halfExtent = KiROUND( halfW * std::abs( D.x / dLen )
                                        + halfH * std::abs( D.y / dLen ) );
                }
                break;
            }
            case ITEM::VIA_T:
            {
                // Via: circular obstacle, radius = Diameter/2, direction-independent
                const VIA* via = static_cast<const VIA*>( item );
                halfExtent = via->Diameter( m_currentLayer ) / 2;
                break;
            }
            case ITEM::SEGMENT_T:
            {
                // Track endpoint: circular end-cap, radius = Width/2
                const SEGMENT* seg = static_cast<const SEGMENT*>( item );
                halfExtent = seg->Width() / 2;
                break;
            }
            case ITEM::ARC_T:
            {
                // Arc endpoint: circular end-cap, radius = Width/2
                const ARC* arc = static_cast<const ARC*>( item );
                halfExtent = arc->Width() / 2;
                break;
            }
            default:
                break;
            }

            maxHalfExtent = std::max( maxHalfExtent, halfExtent );
        }
    }

    int baseEscape = maxHalfExtent > 0
                   ? maxHalfExtent + m_sizes.BundleGap() + m_sizes.TrackWidth() / 2
                   : targetPitch;
    std::vector<int> escapeLen( m_lineCount );

    escapeLen[rankOrder[0]] = baseEscape;

    for( int k = 1; k < m_lineCount; k++ )
    {
        int prev = rankOrder[k - 1];
        escapeLen[rankOrder[k]] = escapeLen[prev] + targetPitch;
    }

    // Compute along-spine distances and find merge point
    std::vector<int> turnLen( m_lineCount );
    std::vector<int> totalDist( m_lineCount );
    int mergeAlongDist = 0;

    for( int i = 0; i < m_lineCount; i++ )
    {
        turnLen[i] = std::abs( shift[i] );
        totalDist[i] = alongOff[i] + escapeLen[i] + turnLen[i];
        mergeAlongDist = std::max( mergeAlongDist, totalDist[i] );
    }

    // Guard: spine must be long enough for the fanout zone
    int spineLen = aSpine.Length();

    if( mergeAlongDist <= 0 )
        return false;

    if( mergeAlongDist >= spineLen )
    {
        // Short spine: build direct fan from each anchor to the target offset at the spine end
        VECTOR2I spineEnd = aSpine.CPoint( aSpine.PointCount() - 1 );

        aLanes.resize( m_lineCount );

        for( int i = 0; i < m_lineCount; i++ )
        {
            VECTOR2I target(
                spineEnd.x + KiROUND( (double) P.x / dLen * targetOff[i] ),
                spineEnd.y + KiROUND( (double) P.y / dLen * targetOff[i] ) );

            DIRECTION_45 dir( target - m_startAnchors[i] );
            aLanes[i] = dir.BuildInitialTrace( m_startAnchors[i], target, m_startDiagonal );
        }

        return true;
    }

    // Split spine at mergeAlongDist
    VECTOR2I splitPt = aSpine.PointAlong( mergeAlongDist );
    SHAPE_LINE_CHAIN spineCopy( aSpine );
    int splitIdx = spineCopy.Split( splitPt );

    if( splitIdx < 0 )
        return false;

    SHAPE_LINE_CHAIN postSpine = spineCopy.Slice( splitIdx, -1 );

    if( postSpine.PointCount() < 2 )
        return false;

    // Build per-lane fanout polylines
    aLanes.resize( m_lineCount );

    for( int i = 0; i < m_lineCount; i++ )
    {
        SHAPE_LINE_CHAIN fanout;

        // Start at the current lane anchor position
        fanout.Append( m_startAnchors[i] );

        // Escape end: anchorPos + D_scaled * escapeLen[i]
        VECTOR2I escapeEnd(
            m_startAnchors[i].x + KiROUND( D.x / dLen * escapeLen[i] ),
            m_startAnchors[i].y + KiROUND( D.y / dLen * escapeLen[i] ) );

        fanout.Append( escapeEnd );

        // 45° turn if shift is non-zero
        VECTOR2I turnEnd = escapeEnd;

        if( shift[i] != 0 )
        {
            int absShift = std::abs( shift[i] );
            turnEnd = VECTOR2I(
                escapeEnd.x + KiROUND( D.x / dLen * absShift )
                            + KiROUND( P.x / dLen * shift[i] ),
                escapeEnd.y + KiROUND( D.y / dLen * absShift )
                            + KiROUND( P.y / dLen * shift[i] ) );

            fanout.Append( turnEnd );
        }

        // Straight run to fill to merge distance
        int straightRun = mergeAlongDist - totalDist[i];

        if( straightRun > 0 )
        {
            VECTOR2I mergePoint(
                turnEnd.x + KiROUND( D.x / dLen * straightRun ),
                turnEnd.y + KiROUND( D.y / dLen * straightRun ) );

            fanout.Append( mergePoint );
        }

        // Generate uniform offset lane from postSpine
        int offset = targetOff[i];
        SHAPE_LINE_CHAIN uniformLane = BUNDLE::OffsetPolyline45( postSpine, offset );

        // Append uniform lane (skip its first point if it overlaps with fanout end)
        if( uniformLane.PointCount() > 0 )
        {
            for( int j = 0; j < uniformLane.PointCount(); j++ )
                fanout.Append( uniformLane.CPoint( j ) );
        }

        aLanes[i] = fanout;
    }

    return true;
}


bool BUNDLE_PLACER::buildSpine( const VECTOR2I& aP, SHAPE_LINE_CHAIN& aSpine )
{
    // Build a 45-degree constrained polyline from m_p_start to aP
    // Use simple two-segment L-shape (horizontal/vertical then diagonal, or vice versa)

    DIRECTION_45 dir( aP - m_p_start );

    aSpine.Clear();
    aSpine.Append( m_p_start );

    VECTOR2I delta = aP - m_p_start;

    if( delta.x == 0 && delta.y == 0 )
    {
        return false;
    }

    // Build 45-degree constrained path using DIRECTION_45
    SHAPE_LINE_CHAIN line = dir.BuildInitialTrace( m_p_start, aP, m_startDiagonal );

    aSpine = line;
    return aSpine.PointCount() >= 2;
}


bool BUNDLE_PLACER::routeHead( const VECTOR2I& aP )
{
    m_fitOk = false;

    SHAPE_LINE_CHAIN spine;

    if( !buildSpine( aP, spine ) )
        return false;

    m_currentTrace.SetGap( m_sizes.BundleGap() );
    m_currentTrace.SetWidth( m_currentWidth );
    m_currentTrace.SetLayer( m_currentLayer );

    // For multi-lane placement, try a structured transition from the current
    // anchors to the uniform bundle pitch on the new spine.
    if( m_lineCount > 1 && (int) m_startAnchors.size() == m_lineCount )
    {
        std::vector<SHAPE_LINE_CHAIN> lanes;

        if( buildFanoutLanes( spine, lanes ) )
        {
            m_currentTrace.SetShape( spine );
            m_currentTrace.SetLaneShapes( lanes );
        }
        else
        {
            // Fallback: uniform offset with anchor prepend
            m_currentTrace.SetShape( spine );

            // Compute perpendicular order for current spine to prevent lane crossing
            SEG firstSeg = spine.CSegment( 0 );
            VECTOR2I spDir = firstSeg.B - firstSeg.A;
            VECTOR2I spPerp( -spDir.y, spDir.x );
            VECTOR2I cent = m_start.Centroid();

            std::vector<int> perpOrder( m_lineCount );

            for( int i = 0; i < m_lineCount; i++ )
                perpOrder[i] = i;

            std::sort( perpOrder.begin(), perpOrder.end(), [&]( int a, int b )
            {
                int64_t projA = (int64_t)( m_startAnchors[a].x - cent.x ) * spPerp.x
                              + (int64_t)( m_startAnchors[a].y - cent.y ) * spPerp.y;
                int64_t projB = (int64_t)( m_startAnchors[b].x - cent.x ) * spPerp.x
                              + (int64_t)( m_startAnchors[b].y - cent.y ) * spPerp.y;
                return projA < projB;
            } );

            // Map: perpOrder[k] = anchor index that should get lane k's geometry
            std::vector<SHAPE_LINE_CHAIN> fallbackLanes( m_lineCount );

            for( int k = 0; k < m_lineCount; k++ )
            {
                int anchorIdx = perpOrder[k];
                fallbackLanes[anchorIdx] = m_currentTrace.CLane( k );

                if( fallbackLanes[anchorIdx].PointCount() > 0
                    && fallbackLanes[anchorIdx].CPoint( 0 ) != m_startAnchors[anchorIdx] )
                {
                    SHAPE_LINE_CHAIN withAnchor;
                    withAnchor.Append( m_startAnchors[anchorIdx] );
                    withAnchor.Append( fallbackLanes[anchorIdx] );
                    fallbackLanes[anchorIdx] = withAnchor;
                }
            }

            m_currentTrace.SetLaneShapes( fallbackLanes );
        }
    }
    else
    {
        // Single lane: uniform offset only
        m_currentTrace.SetShape( spine );
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

    m_fitOk = !anyCollision;
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

        // Regenerate lanes from the walked-around spine
        PITCH_PROFILE profile = buildPitchProfile( walkedSpine );

        if( profile.HasTransition() )
            m_currentTrace.SetShape( walkedSpine, profile );
        else
            m_currentTrace.SetShape( walkedSpine );

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

        m_fitOk = allClear;
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
                    shovedLanes[i] = modifiedHead.CLine();
                }
                else
                {
                    shovedLanes[i] = m_currentTrace.CLane( i );
                }
            }

            m_currentTrace.SetLaneShapes( shovedLanes );
        }

        m_fitOk = true;
        return true;
    }

    // Phase 1: Try walkaround on solids first, then shove
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
        PITCH_PROFILE profile = buildPitchProfile( walkedSpine );

        if( profile.HasTransition() )
            m_currentTrace.SetShape( walkedSpine, profile );
        else
            m_currentTrace.SetShape( walkedSpine );
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
                shovedLanes[i] = modifiedHead.CLine();
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

        m_fitOk = allClear;
    }

    // If shove failed or post-shove verification failed, fall back to walkaround
    if( !m_fitOk )
        return rhWalkOnly( aP );

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
        initPlacement();
        return false;
    }
}


bool BUNDLE_PLACER::AbortPlacement()
{
    m_world->KillChildren();
    m_lastNode = nullptr;
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
