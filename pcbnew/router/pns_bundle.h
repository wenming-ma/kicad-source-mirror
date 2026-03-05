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

#ifndef __PNS_BUNDLE_H
#define __PNS_BUNDLE_H

#include <vector>

#include "pns_line.h"
#include "pns_via.h"
#include "pns_link_holder.h"

#include "ranged_num.h"

namespace PNS {

class BUNDLE;

/**
 * Define a "gateway" for routing a bundle - a set of N anchor points with
 * certain orientation, spacing and (optionally) predefined entry paths.
 * The routing algorithm connects such gateways with parallel lines, thus
 * creating a multi-track bundle.
 */
class BUNDLE_GATEWAY
{
public:
    BUNDLE_GATEWAY( const std::vector<VECTOR2I>& aAnchors, bool aIsDiagonal,
                    int aAllowedEntryAngles = DIRECTION_45::ANG_OBTUSE, int aPriority = 0 ) :
            m_anchors( aAnchors ),
            m_isDiagonal( aIsDiagonal ),
            m_allowedEntryAngles( aAllowedEntryAngles ),
            m_priority( aPriority ),
            m_hasEntryLines( false )
    {
    }

    ~BUNDLE_GATEWAY() {}

    bool IsDiagonal() const { return m_isDiagonal; }

    const std::vector<VECTOR2I>& Anchors() const { return m_anchors; }
    const VECTOR2I& Anchor( int aIndex ) const { return m_anchors[aIndex]; }
    int AnchorCount() const { return static_cast<int>( m_anchors.size() ); }

    int AllowedAngles() const { return m_allowedEntryAngles; }

    int Priority() const { return m_priority; }
    void SetPriority( int aPriority ) { m_priority = aPriority; }

    void SetEntryLines( const std::vector<SHAPE_LINE_CHAIN>& aEntries )
    {
        m_entries = aEntries;
        m_hasEntryLines = true;
    }

    const std::vector<SHAPE_LINE_CHAIN>& Entries() const { return m_entries; }

    bool HasEntryLines() const { return m_hasEntryLines; }

    VECTOR2I Centroid() const;

private:
    std::vector<VECTOR2I>          m_anchors;
    bool                           m_isDiagonal;
    int                            m_allowedEntryAngles;
    int                            m_priority;
    bool                           m_hasEntryLines;
    std::vector<SHAPE_LINE_CHAIN>  m_entries;
};


/**
 * Store starting/ending primitives (pads, vias or segments) for a bundle.
 */
class BUNDLE_PRIMITIVE_GROUP
{
public:
    BUNDLE_PRIMITIVE_GROUP() {}
    BUNDLE_PRIMITIVE_GROUP( const BUNDLE_PRIMITIVE_GROUP& aOther );
    BUNDLE_PRIMITIVE_GROUP( const std::vector<ITEM*>& aPrims );
    BUNDLE_PRIMITIVE_GROUP( const std::vector<VECTOR2I>& aAnchors );

    ~BUNDLE_PRIMITIVE_GROUP();

    BUNDLE_PRIMITIVE_GROUP& operator=( const BUNDLE_PRIMITIVE_GROUP& aOther );

    void SetAnchors( const std::vector<VECTOR2I>& aAnchors ) { m_anchors = aAnchors; }
    const std::vector<VECTOR2I>& Anchors() const { return m_anchors; }
    const VECTOR2I& Anchor( int aIndex ) const { return m_anchors[aIndex]; }

    void SetPrimitives( const std::vector<ITEM*>& aPrims ) { m_prims = aPrims; }
    const std::vector<ITEM*>& Primitives() const { return m_prims; }
    ITEM* Prim( int aIndex ) const { return m_prims[aIndex]; }

    int Count() const { return static_cast<int>( m_anchors.size() ); }

    /**
     * Sort anchors and primitives by their projected position perpendicular to a
     * reference direction, establishing lane order.
     */
    void SortByPosition( const VECTOR2I& aRefDir );

    /**
     * Return the centroid (geometric center) of all anchor positions.
     */
    VECTOR2I Centroid() const;

    /**
     * Return the dominant direction of the primitive group (from first to last anchor).
     */
    DIRECTION_45 GroupDirection() const;

private:
    std::vector<ITEM*>    m_prims;
    std::vector<VECTOR2I> m_anchors;
};


/**
 * A set of gateways calculated for the cursor or starting/ending primitive group.
 */
class BUNDLE_GATEWAYS
{
public:
    BUNDLE_GATEWAYS( int aGap, int aLineCount ) :
        m_gap( aGap ),
        m_lineCount( aLineCount )
    {
        m_pitch = 0;
    }

    void Clear() { m_gateways.clear(); }

    void SetPitch( int aPitch ) { m_pitch = aPitch; }
    int Pitch() const { return m_pitch; }

    void BuildForCursor( const VECTOR2I& aCursorPos );
    void BuildFromPrimitiveGroup( const BUNDLE_PRIMITIVE_GROUP& aGroup, bool aPreferDiagonal );

    std::vector<BUNDLE_GATEWAY>& Gateways() { return m_gateways; }
    const std::vector<BUNDLE_GATEWAY>& CGateways() const { return m_gateways; }

private:
    int m_gap;
    int m_pitch;
    int m_lineCount;
    std::vector<BUNDLE_GATEWAY> m_gateways;
};


/**
 * Pitch profile used to control per-lane offsets along the spine arc length.
 * Enables smooth transition from irregular pad spacing to uniform bundle pitch.
 */
struct PITCH_PROFILE
{
    int escapeLength;                    ///< Arc length before transition starts
    int transitionLength;                ///< Arc length over which transition occurs
    std::vector<int> startOffsets;       ///< Per-lane initial offsets (from pad positions)
    std::vector<int> targetOffsets;      ///< Per-lane target offsets (uniform pitch)

    PITCH_PROFILE() :
        escapeLength( 0 ),
        transitionLength( 0 )
    {}

    /**
     * Return the signed offset for the given lane at the given arc-length position.
     * Uses smoothstep interpolation in the transition zone.
     */
    int GetOffset( int aLaneIndex, int aArcLength ) const;

    bool HasTransition() const { return transitionLength > 0; }
};


/**
 * A multi-track bundle consisting of N parallel lines routed along a common spine.
 * Analogous to DIFF_PAIR but generalized to N tracks.
 */
class BUNDLE : public LINK_HOLDER
{
public:
    BUNDLE() :
        LINK_HOLDER( ITEM::BUNDLE_T ),
        m_lineCount( 0 ),
        m_width( 0 ),
        m_gap( 0 ),
        m_pitch( 0 )
    {
    }

    BUNDLE( int aLineCount, int aWidth, int aGap ) :
        LINK_HOLDER( ITEM::BUNDLE_T ),
        m_lineCount( aLineCount ),
        m_width( aWidth ),
        m_gap( aGap ),
        m_pitch( aWidth + aGap )
    {
        m_lanes.resize( aLineCount );
        m_lines.resize( aLineCount );
        m_nets.resize( aLineCount, nullptr );
    }

    BUNDLE( const BUNDLE& aOther ) :
        LINK_HOLDER( ITEM::BUNDLE_T )
    {
        *this = aOther;
    }

    static inline bool ClassOf( const ITEM* aItem )
    {
        return aItem && ITEM::BUNDLE_T == aItem->Kind();
    }

    BUNDLE* Clone() const override
    {
        assert( false );
        return nullptr;
    }

    BUNDLE& operator=( const BUNDLE& aOther );

    virtual void ClearLinks() override
    {
        m_links.clear();

        for( auto& line : m_lines )
            line.ClearLinks();
    }

    /**
     * Set the spine (center path) and recompute all offset lanes.
     */
    void SetShape( const SHAPE_LINE_CHAIN& aSpine );

    /**
     * Set the spine with a pitch profile for smooth transition.
     */
    void SetShape( const SHAPE_LINE_CHAIN& aSpine, const PITCH_PROFILE& aProfile );

    /**
     * Set pre-computed lane shapes directly.
     */
    void SetLaneShapes( const std::vector<SHAPE_LINE_CHAIN>& aLanes );

    void SetNets( const std::vector<NET_HANDLE>& aNets ) { m_nets = aNets; }
    const std::vector<NET_HANDLE>& Nets() const { return m_nets; }
    NET_HANDLE Net( int aIndex ) const { return m_nets[aIndex]; }

    void SetWidth( int aWidth );
    int Width() const { return m_width; }

    void SetGap( int aGap );
    int Gap() const { return m_gap; }

    int Pitch() const { return m_pitch; }

    int LineCount() const { return m_lineCount; }
    void SetLineCount( int aCount );

    /**
     * Return the LINE object for a specific lane. Reconstructs it from
     * lane shape, net, width, and layer if not already up-to-date.
     */
    LINE& Line( int aIndex );

    const SHAPE_LINE_CHAIN& CLane( int aIndex ) const { return m_lanes[aIndex]; }
    const SHAPE_LINE_CHAIN& Spine() const { return m_spine; }

    /**
     * Return the total width of the bundle from outer edge to outer edge.
     * Equal to (N-1)*pitch + width.
     */
    int TotalWidth() const
    {
        if( m_lineCount <= 0 )
            return 0;

        return ( m_lineCount - 1 ) * m_pitch + m_width;
    }

    /**
     * Return half the total bundle width, useful for C-space obstacle expansion.
     */
    int HalfBundleWidth() const { return TotalWidth() / 2; }

    void Clear()
    {
        m_spine.Clear();

        for( auto& lane : m_lanes )
            lane.Clear();
    }

    void Append( const BUNDLE& aOther )
    {
        m_spine.Append( aOther.m_spine );

        for( int i = 0; i < m_lineCount && i < aOther.m_lineCount; i++ )
            m_lanes[i].Append( aOther.m_lanes[i] );
    }

    bool Empty() const
    {
        if( m_lineCount == 0 )
            return true;

        for( int i = 0; i < m_lineCount; i++ )
        {
            if( m_lanes[i].SegmentCount() == 0 )
                return true;
        }

        return false;
    }

    void SetLayer( int aLayer );

public:
    /**
     * Offset a polyline by a signed distance. Positive = left, negative = right.
     * Specialized for 45-degree routing with miter joins.
     */
    static SHAPE_LINE_CHAIN OffsetPolyline45( const SHAPE_LINE_CHAIN& aChain, int aOffset );

private:
    /**
     * Core offset algorithm: generate N offset lanes from the spine.
     * For 45-degree routing, uses miter joins at corners.
     */
    void generateOffsetLanes();

    /**
     * Generate offset lanes using a pitch profile for transition zones.
     */
    void generateOffsetLanesWithProfile( const PITCH_PROFILE& aProfile );

    /**
     * Compute miter point at the junction of two offset segments.
     */
    static VECTOR2I computeMiterPoint( const VECTOR2I& aCorner, const SEG& aPrev,
                                       const SEG& aNext, int aOffset );

    void updateLine( LINE& aLine, const SHAPE_LINE_CHAIN& aShape, NET_HANDLE aNet );

    int                            m_lineCount;
    int                            m_width;
    int                            m_gap;
    int                            m_pitch;

    SHAPE_LINE_CHAIN               m_spine;
    std::vector<SHAPE_LINE_CHAIN>  m_lanes;
    std::vector<LINE>              m_lines;
    std::vector<NET_HANDLE>        m_nets;
};

}

#endif // __PNS_BUNDLE_H
