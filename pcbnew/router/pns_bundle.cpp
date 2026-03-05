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

#include "pns_bundle.h"
#include "pns_utils.h"

namespace PNS {

// Miter limit ratio. If the miter extension exceeds this factor times the
// offset distance, we switch to a bevel join to avoid spikes.
static constexpr double MITER_LIMIT = 3.0;


// ---- BUNDLE_GATEWAY ----

VECTOR2I BUNDLE_GATEWAY::Centroid() const
{
    if( m_anchors.empty() )
        return VECTOR2I( 0, 0 );

    VECTOR2I sum( 0, 0 );

    for( const VECTOR2I& a : m_anchors )
        sum += a;

    return VECTOR2I( sum.x / (int) m_anchors.size(), sum.y / (int) m_anchors.size() );
}


// ---- BUNDLE_PRIMITIVE_GROUP ----

BUNDLE_PRIMITIVE_GROUP::BUNDLE_PRIMITIVE_GROUP( const BUNDLE_PRIMITIVE_GROUP& aOther )
{
    m_prims = aOther.m_prims;
    m_anchors = aOther.m_anchors;
}


BUNDLE_PRIMITIVE_GROUP::BUNDLE_PRIMITIVE_GROUP( const std::vector<ITEM*>& aPrims )
{
    m_prims = aPrims;
    m_anchors.resize( aPrims.size() );

    for( size_t i = 0; i < aPrims.size(); i++ )
    {
        if( aPrims[i] )
            m_anchors[i] = aPrims[i]->Anchor( 0 );
    }
}


BUNDLE_PRIMITIVE_GROUP::BUNDLE_PRIMITIVE_GROUP( const std::vector<VECTOR2I>& aAnchors )
{
    m_anchors = aAnchors;
    m_prims.resize( aAnchors.size(), nullptr );
}


BUNDLE_PRIMITIVE_GROUP::~BUNDLE_PRIMITIVE_GROUP()
{
}


BUNDLE_PRIMITIVE_GROUP& BUNDLE_PRIMITIVE_GROUP::operator=( const BUNDLE_PRIMITIVE_GROUP& aOther )
{
    m_prims = aOther.m_prims;
    m_anchors = aOther.m_anchors;
    return *this;
}


void BUNDLE_PRIMITIVE_GROUP::SortByPosition( const VECTOR2I& aRefDir )
{
    if( m_anchors.size() <= 1 )
        return;

    // Compute perpendicular direction for sorting
    VECTOR2I perp( -aRefDir.y, aRefDir.x );

    // Build index array and sort by projection onto perpendicular
    std::vector<int> indices( m_anchors.size() );

    for( size_t i = 0; i < indices.size(); i++ )
        indices[i] = static_cast<int>( i );

    std::sort( indices.begin(), indices.end(), [&]( int a, int b )
    {
        int64_t projA = (int64_t) m_anchors[a].x * perp.x + (int64_t) m_anchors[a].y * perp.y;
        int64_t projB = (int64_t) m_anchors[b].x * perp.x + (int64_t) m_anchors[b].y * perp.y;
        return projA < projB;
    } );

    // Reorder both anchors and prims
    std::vector<VECTOR2I> sortedAnchors( m_anchors.size() );
    std::vector<ITEM*>    sortedPrims( m_prims.size() );

    for( size_t i = 0; i < indices.size(); i++ )
    {
        sortedAnchors[i] = m_anchors[indices[i]];

        if( indices[i] < (int) m_prims.size() )
            sortedPrims[i] = m_prims[indices[i]];
        else
            sortedPrims[i] = nullptr;
    }

    m_anchors = sortedAnchors;
    m_prims = sortedPrims;
}


VECTOR2I BUNDLE_PRIMITIVE_GROUP::Centroid() const
{
    if( m_anchors.empty() )
        return VECTOR2I( 0, 0 );

    VECTOR2I sum( 0, 0 );

    for( const VECTOR2I& a : m_anchors )
        sum += a;

    return VECTOR2I( sum.x / (int) m_anchors.size(), sum.y / (int) m_anchors.size() );
}


DIRECTION_45 BUNDLE_PRIMITIVE_GROUP::GroupDirection() const
{
    if( m_anchors.size() < 2 )
        return DIRECTION_45();

    // Direction from first to last anchor
    VECTOR2I dir = m_anchors.back() - m_anchors.front();
    return DIRECTION_45( dir );
}


// ---- BUNDLE_GATEWAYS ----

void BUNDLE_GATEWAYS::BuildForCursor( const VECTOR2I& aCursorPos )
{
    m_gateways.clear();

    if( m_lineCount <= 0 || m_pitch <= 0 )
        return;

    int totalSpan = ( m_lineCount - 1 ) * m_pitch;

    // Build gateways for horizontal and vertical orientations
    for( int dir = 0; dir < 4; dir++ )
    {
        VECTOR2I perpDir;
        bool isDiag = false;

        switch( dir )
        {
        case 0: perpDir = VECTOR2I( 0, 1 ); break;   // horizontal routing, vertical spread
        case 1: perpDir = VECTOR2I( 1, 0 ); break;   // vertical routing, horizontal spread
        case 2: perpDir = VECTOR2I( 1, 1 ); isDiag = true; break;  // diagonal
        case 3: perpDir = VECTOR2I( 1, -1 ); isDiag = true; break; // anti-diagonal
        }

        std::vector<VECTOR2I> anchors( m_lineCount );

        for( int i = 0; i < m_lineCount; i++ )
        {
            int offset = -totalSpan / 2 + i * m_pitch;
            anchors[i] = aCursorPos + VECTOR2I( perpDir.x * offset, perpDir.y * offset );
        }

        m_gateways.emplace_back( anchors, isDiag );
    }
}


void BUNDLE_GATEWAYS::BuildFromPrimitiveGroup( const BUNDLE_PRIMITIVE_GROUP& aGroup,
                                                bool aPreferDiagonal )
{
    m_gateways.clear();

    if( aGroup.Count() <= 0 )
        return;

    std::vector<VECTOR2I> anchors = aGroup.Anchors();

    bool isDiag = false;

    if( anchors.size() >= 2 )
    {
        VECTOR2I dir = anchors.back() - anchors.front();
        isDiag = ( dir.x != 0 && dir.y != 0 );
    }

    m_gateways.emplace_back( anchors, isDiag );
}


// ---- PITCH_PROFILE ----

int PITCH_PROFILE::GetOffset( int aLaneIndex, int aArcLength ) const
{
    if( aLaneIndex < 0 || aLaneIndex >= (int) targetOffsets.size() )
        return 0;

    if( !HasTransition() || aArcLength >= escapeLength + transitionLength )
    {
        // Steady state: use target pitch
        return targetOffsets[aLaneIndex];
    }

    if( aLaneIndex >= (int) startOffsets.size() )
        return targetOffsets[aLaneIndex];

    if( aArcLength < escapeLength )
    {
        // Pure escape zone: use original pad-relative offsets
        return startOffsets[aLaneIndex];
    }

    // Transition zone: smoothstep interpolation
    double t = (double) ( aArcLength - escapeLength ) / (double) transitionLength;
    // Smoothstep: 3t^2 - 2t^3
    double w = t * t * ( 3.0 - 2.0 * t );

    int startOff = startOffsets[aLaneIndex];
    int targetOff = targetOffsets[aLaneIndex];

    return startOff + (int) ( w * ( targetOff - startOff ) );
}


// ---- BUNDLE ----

BUNDLE& BUNDLE::operator=( const BUNDLE& aOther )
{
    m_lineCount = aOther.m_lineCount;
    m_width = aOther.m_width;
    m_gap = aOther.m_gap;
    m_pitch = aOther.m_pitch;
    m_spine = aOther.m_spine;
    m_lanes = aOther.m_lanes;
    m_lines = aOther.m_lines;
    m_nets = aOther.m_nets;
    return *this;
}


void BUNDLE::SetWidth( int aWidth )
{
    m_width = aWidth;
    m_pitch = m_width + m_gap;

    for( auto& line : m_lines )
        line.SetWidth( aWidth );
}


void BUNDLE::SetGap( int aGap )
{
    m_gap = aGap;
    m_pitch = m_width + m_gap;
}


void BUNDLE::SetLineCount( int aCount )
{
    m_lineCount = aCount;
    m_lanes.resize( aCount );
    m_lines.resize( aCount );
    m_nets.resize( aCount, nullptr );
}


void BUNDLE::SetLayer( int aLayer )
{
    LINK_HOLDER::SetLayer( aLayer );

    for( auto& line : m_lines )
        line.SetLayer( aLayer );
}


void BUNDLE::SetShape( const SHAPE_LINE_CHAIN& aSpine )
{
    m_spine = aSpine;
    generateOffsetLanes();
}


void BUNDLE::SetShape( const SHAPE_LINE_CHAIN& aSpine, const PITCH_PROFILE& aProfile )
{
    m_spine = aSpine;

    if( aProfile.HasTransition() )
        generateOffsetLanesWithProfile( aProfile );
    else
        generateOffsetLanes();
}


void BUNDLE::SetLaneShapes( const std::vector<SHAPE_LINE_CHAIN>& aLanes )
{
    for( int i = 0; i < m_lineCount && i < (int) aLanes.size(); i++ )
        m_lanes[i] = aLanes[i];
}


LINE& BUNDLE::Line( int aIndex )
{
    if( aIndex < 0 || aIndex >= m_lineCount )
    {
        static LINE dummy;
        return dummy;
    }

    if( !m_lines[aIndex].IsLinked() )
        updateLine( m_lines[aIndex], m_lanes[aIndex], m_nets[aIndex] );

    return m_lines[aIndex];
}


void BUNDLE::updateLine( LINE& aLine, const SHAPE_LINE_CHAIN& aShape, NET_HANDLE aNet )
{
    aLine.SetShape( aShape );
    aLine.SetWidth( m_width );
    aLine.SetNet( aNet );
    aLine.SetLayer( Layers().Start() );
    aLine.SetParent( m_parent );
    aLine.SetSourceItem( m_sourceItem );
}


/**
 * Helper: compute the left-hand perpendicular direction for a segment.
 * Returns a vector perpendicular to (B - A), pointing left, with the
 * given magnitude. Uses integer math to preserve precision.
 */
static VECTOR2I leftNormal( const SEG& aSeg )
{
    VECTOR2I dir = aSeg.B - aSeg.A;

    // Left perpendicular: rotate 90 degrees counter-clockwise
    // (x, y) -> (-y, x)
    return VECTOR2I( -dir.y, dir.x );
}


/**
 * Compute the miter point at the junction between two adjacent offset segments.
 * For 45-degree routing, the angle between segments is always a multiple of 45 degrees.
 */
VECTOR2I BUNDLE::computeMiterPoint( const VECTOR2I& aCorner, const SEG& aPrev,
                                    const SEG& aNext, int aOffset )
{
    VECTOR2I n_prev = leftNormal( aPrev );
    VECTOR2I n_next = leftNormal( aNext );

    double len_prev = n_prev.EuclideanNorm();
    double len_next = n_next.EuclideanNorm();

    if( len_prev < 1.0 || len_next < 1.0 )
        return aCorner;

    // Normalized perpendicular offsets (as floating point for the miter calculation)
    double nx_prev = (double) n_prev.x / len_prev;
    double ny_prev = (double) n_prev.y / len_prev;
    double nx_next = (double) n_next.x / len_next;
    double ny_next = (double) n_next.y / len_next;

    // Offset the corner point along each perpendicular
    double offPrev_x = aCorner.x + nx_prev * aOffset;
    double offPrev_y = aCorner.y + ny_prev * aOffset;
    double offNext_x = aCorner.x + nx_next * aOffset;
    double offNext_y = aCorner.y + ny_next * aOffset;

    // Direction vectors of the two segments
    VECTOR2I dirPrev = aPrev.B - aPrev.A;
    VECTOR2I dirNext = aNext.B - aNext.A;

    // Compute intersection of the two offset lines:
    //   P1 + t * dirPrev = P2 + s * dirNext
    // where P1 = offset along prev normal, P2 = offset along next normal
    double denom = (double) dirPrev.x * dirNext.y - (double) dirPrev.y * dirNext.x;

    if( std::abs( denom ) < 1.0 )
    {
        // Segments are nearly parallel, just use the offset corner
        return VECTOR2I( KiROUND( offPrev_x ), KiROUND( offPrev_y ) );
    }

    double dx = offNext_x - offPrev_x;
    double dy = offNext_y - offPrev_y;

    double t = ( dx * dirNext.y - dy * dirNext.x ) / denom;

    double miterX = offPrev_x + t * dirPrev.x;
    double miterY = offPrev_y + t * dirPrev.y;

    return VECTOR2I( KiROUND( miterX ), KiROUND( miterY ) );
}


SHAPE_LINE_CHAIN BUNDLE::OffsetPolyline45( const SHAPE_LINE_CHAIN& aChain, int aOffset )
{
    SHAPE_LINE_CHAIN result;

    int ptCount = aChain.PointCount();

    if( ptCount < 2 )
        return result;

    int segCount = aChain.SegmentCount();

    for( int j = 0; j < ptCount; j++ )
    {
        if( j == 0 )
        {
            // First point: offset along perpendicular of first segment
            SEG seg = aChain.CSegment( 0 );
            VECTOR2I n = leftNormal( seg );
            double len = n.EuclideanNorm();

            if( len < 1.0 )
            {
                result.Append( aChain.CPoint( 0 ) );
                continue;
            }

            double nx = (double) n.x / len;
            double ny = (double) n.y / len;

            result.Append( VECTOR2I( KiROUND( aChain.CPoint( 0 ).x + nx * aOffset ),
                                     KiROUND( aChain.CPoint( 0 ).y + ny * aOffset ) ) );
        }
        else if( j == ptCount - 1 )
        {
            // Last point: offset along perpendicular of last segment
            SEG seg = aChain.CSegment( segCount - 1 );
            VECTOR2I n = leftNormal( seg );
            double len = n.EuclideanNorm();

            if( len < 1.0 )
            {
                result.Append( aChain.CPoint( j ) );
                continue;
            }

            double nx = (double) n.x / len;
            double ny = (double) n.y / len;

            result.Append( VECTOR2I( KiROUND( aChain.CPoint( j ).x + nx * aOffset ),
                                     KiROUND( aChain.CPoint( j ).y + ny * aOffset ) ) );
        }
        else
        {
            // Interior point: miter join between adjacent segments
            SEG segPrev = aChain.CSegment( j - 1 );
            SEG segNext = aChain.CSegment( j );

            VECTOR2I n_prev = leftNormal( segPrev );
            VECTOR2I n_next = leftNormal( segNext );
            double len_prev = n_prev.EuclideanNorm();
            double len_next = n_next.EuclideanNorm();

            if( len_prev < 1.0 || len_next < 1.0 )
            {
                result.Append( aChain.CPoint( j ) );
                continue;
            }

            // Check miter ratio for limit
            double nx_p = (double) n_prev.x / len_prev;
            double ny_p = (double) n_prev.y / len_prev;
            double nx_n = (double) n_next.x / len_next;
            double ny_n = (double) n_next.y / len_next;

            // Bisector direction
            double bx = nx_p + nx_n;
            double by = ny_p + ny_n;
            double bLen = std::sqrt( bx * bx + by * by );

            if( bLen < 0.001 )
            {
                // 180 degree turn - normals cancel out, use bevel
                double d_prev = (double) aOffset;
                VECTOR2I bevel1( KiROUND( aChain.CPoint( j ).x + nx_p * d_prev ),
                                 KiROUND( aChain.CPoint( j ).y + ny_p * d_prev ) );
                VECTOR2I bevel2( KiROUND( aChain.CPoint( j ).x + nx_n * d_prev ),
                                 KiROUND( aChain.CPoint( j ).y + ny_n * d_prev ) );
                result.Append( bevel1 );
                result.Append( bevel2 );
                continue;
            }

            // cos(half angle) between the two normals
            double cosHalf = ( nx_p * bx + ny_p * by ) / bLen;

            if( std::abs( cosHalf ) < 0.001 )
                cosHalf = 0.001;

            double miterRatio = 1.0 / std::abs( cosHalf );

            if( miterRatio > MITER_LIMIT )
            {
                // Bevel join: insert two points
                VECTOR2I bevel1( KiROUND( aChain.CPoint( j ).x + nx_p * aOffset ),
                                 KiROUND( aChain.CPoint( j ).y + ny_p * aOffset ) );
                VECTOR2I bevel2( KiROUND( aChain.CPoint( j ).x + nx_n * aOffset ),
                                 KiROUND( aChain.CPoint( j ).y + ny_n * aOffset ) );
                result.Append( bevel1 );
                result.Append( bevel2 );
            }
            else
            {
                // Single miter point
                VECTOR2I miter = computeMiterPoint( aChain.CPoint( j ), segPrev, segNext,
                                                    aOffset );
                result.Append( miter );
            }
        }
    }

    // Check for self-intersection and clean up if needed
    if( result.PointCount() > 2 && result.SelfIntersecting() )
    {
        result.Simplify();
    }

    return result;
}


void BUNDLE::generateOffsetLanes()
{
    if( m_lineCount <= 0 || m_spine.PointCount() < 2 )
        return;

    for( int i = 0; i < m_lineCount; i++ )
    {
        // Signed offset distance: lane 0 is leftmost, lane N-1 is rightmost
        double d = ( (double) i - ( (double) m_lineCount - 1.0 ) / 2.0 ) * (double) m_pitch;
        int offset = KiROUND( d );

        m_lanes[i] = OffsetPolyline45( m_spine, offset );
    }
}


void BUNDLE::generateOffsetLanesWithProfile( const PITCH_PROFILE& aProfile )
{
    if( m_lineCount <= 0 || m_spine.PointCount() < 2 )
        return;

    if( !aProfile.HasTransition() )
    {
        generateOffsetLanes();
        return;
    }

    // For each lane, generate a chain by computing per-vertex offsets based on
    // arc-length position along the spine.
    for( int i = 0; i < m_lineCount; i++ )
    {
        SHAPE_LINE_CHAIN lane;
        int arcLength = 0;

        for( int j = 0; j < m_spine.PointCount(); j++ )
        {
            if( j > 0 )
            {
                SEG seg( m_spine.CPoint( j - 1 ), m_spine.CPoint( j ) );
                arcLength += seg.Length();
            }

            int offset = aProfile.GetOffset( i, arcLength );

            // Compute local perpendicular direction at this vertex
            VECTOR2I normal( 0, 0 );

            if( j == 0 && m_spine.SegmentCount() > 0 )
            {
                VECTOR2I n = leftNormal( m_spine.CSegment( 0 ) );
                double len = n.EuclideanNorm();

                if( len > 0 )
                    normal = VECTOR2I( KiROUND( (double) n.x / len * offset ),
                                       KiROUND( (double) n.y / len * offset ) );
            }
            else if( j == m_spine.PointCount() - 1 && m_spine.SegmentCount() > 0 )
            {
                VECTOR2I n = leftNormal( m_spine.CSegment( m_spine.SegmentCount() - 1 ) );
                double len = n.EuclideanNorm();

                if( len > 0 )
                    normal = VECTOR2I( KiROUND( (double) n.x / len * offset ),
                                       KiROUND( (double) n.y / len * offset ) );
            }
            else if( j > 0 && j < m_spine.SegmentCount() )
            {
                // Average the normals of adjacent segments for smooth corners
                VECTOR2I n1 = leftNormal( m_spine.CSegment( j - 1 ) );
                VECTOR2I n2 = leftNormal( m_spine.CSegment( j ) );
                double len1 = n1.EuclideanNorm();
                double len2 = n2.EuclideanNorm();

                if( len1 > 0 && len2 > 0 )
                {
                    double nx = ( (double) n1.x / len1 + (double) n2.x / len2 ) / 2.0;
                    double ny = ( (double) n1.y / len1 + (double) n2.y / len2 ) / 2.0;
                    double nLen = std::sqrt( nx * nx + ny * ny );

                    if( nLen > 0 )
                        normal = VECTOR2I( KiROUND( nx / nLen * offset ),
                                           KiROUND( ny / nLen * offset ) );
                }
            }

            lane.Append( m_spine.CPoint( j ) + normal );
        }

        m_lanes[i] = lane;
    }
}

} // namespace PNS
