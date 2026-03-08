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

#ifndef __PNS_BUNDLE_PLACER_H
#define __PNS_BUNDLE_PLACER_H

#include <math/vector2d.h>

#include "pns_sizes_settings.h"
#include "pns_node.h"
#include "pns_via.h"
#include "pns_line.h"
#include "pns_algo_base.h"
#include "pns_bundle.h"

#include "pns_placement_algo.h"

namespace PNS {

class ROUTER;
class SHOVE;
class OPTIMIZER;
class VIA;
class SIZES_SETTINGS;


/**
 * Multi-track bundle placement algorithm.
 *
 * Interactively routes N parallel tracks (a bundle) and applies walkaround
 * and shove algorithms when needed. Analogous to DIFF_PAIR_PLACER but
 * generalized to N tracks with fixed pitch.
 */
class BUNDLE_PLACER : public PLACEMENT_ALGO
{
public:
    BUNDLE_PLACER( ROUTER* aRouter );
    ~BUNDLE_PLACER();

    /**
     * Start routing a bundle at point aP, taking item aStartItem as anchor.
     */
    bool Start( const VECTOR2I& aP, ITEM* aStartItem ) override;

    /**
     * Move the end of the currently routed bundle to the point aP.
     */
    bool Move( const VECTOR2I& aP, ITEM* aEndItem ) override;

    /**
     * Commit the currently routed bundle to the parent node.
     */
    bool FixRoute( const VECTOR2I& aP, ITEM* aEndItem, bool aForceFinish ) override;

    bool CommitPlacement() override;
    bool AbortPlacement() override;
    bool HasPlacedAnything() const override;

    bool ToggleVia( bool aEnabled ) override;
    bool SetLayer( int aLayer ) override;

    const ITEM_SET Traces() override;

    const VECTOR2I& CurrentStart() const override { return m_currentStart; }
    const VECTOR2I& CurrentEnd() const override { return m_currentEnd; }
    const std::vector<NET_HANDLE> CurrentNets() const override;
    int CurrentLayer() const override { return m_currentLayer; }

    NODE* CurrentNode( bool aLoopsRemoved = false ) const override;

    void FlipPosture() override;
    void UpdateSizes( const SIZES_SETTINGS& aSizes ) override;
    bool IsPlacingVia() const override { return m_placingVia; }
    void SetOrthoMode( bool aOrthoMode ) override;

    void GetModifiedNets( std::vector<NET_HANDLE>& aNets ) const override;

    /**
     * Configure the bundle placer with pre-selected start pads.
     * Called before Start() to provide the N pads that form the bundle.
     */
    void SetStartPads( const std::vector<ITEM*>& aPads );
    void SetStartPads( const std::vector<ITEM*>& aPads,
                       const std::vector<NET_HANDLE>& aNets );

private:
    /**
     * Route the bundle head using the configured routing mode.
     */
    bool route( const VECTOR2I& aP );

    /**
     * Generate the bundle spine and offset lanes towards the target point.
     */
    bool routeHead( const VECTOR2I& aP );

    ///< Route step, mark obstacles mode
    bool rhMarkObstacles( const VECTOR2I& aP );

    ///< Route step, walkaround mode
    bool rhWalkOnly( const VECTOR2I& aP );

    ///< Route step, push and shove mode
    bool rhShoveOnly( const VECTOR2I& aP );

    /**
     * Set the board to route.
     */
    void setWorld( NODE* aWorld );

    /**
     * Initialize placement state.
     */
    void initPlacement();

    /**
     * Build the pitch profile for escape/transition zone.
     */
    PITCH_PROFILE buildPitchProfile( const SHAPE_LINE_CHAIN& aSpine ) const;

    /**
     * Build structured fanout lanes from pad positions to uniform pitch.
     * Each lane gets a parallel escape, 45-degree turn, and straight run
     * before merging into uniform-offset routing from the remaining spine.
     * Returns false if the spine is too short or fanout is not applicable.
     */
    bool buildFanoutLanes( const SHAPE_LINE_CHAIN& aSpine,
                           std::vector<SHAPE_LINE_CHAIN>& aLanes ) const;

    /**
     * Generate the spine line from current start to target point.
     */
    bool buildSpine( const VECTOR2I& aP, SHAPE_LINE_CHAIN& aSpine );

    /**
     * Apply a spine to the current trace while preserving the current start anchors.
     */
    bool applySpinePreservingAnchors( const SHAPE_LINE_CHAIN& aSpine,
                                      bool aAllowProfileFallback = true );

    enum State {
        RT_START = 0,
        RT_ROUTE = 1,
        RT_FINISH = 2
    };

    State m_state;

    bool m_chainedPlacement;
    bool m_startDiagonal;
    bool m_fitOk;
    bool m_orthoMode;
    bool m_idle;
    bool m_hasFixedAnything;

    int m_lineCount;
    std::vector<NET_HANDLE> m_nets;

    BUNDLE_PRIMITIVE_GROUP m_start;
    std::vector<VECTOR2I>  m_startAnchors;

    ///< pointer to world to search colliding items
    NODE* m_world;

    ///< current routing start point (end of tail, beginning of head)
    VECTOR2I m_p_start;
    VECTOR2I m_currentStart;
    VECTOR2I m_currentEnd;

    ///< The shove engine
    std::unique_ptr<SHOVE> m_shove;

    ///< Current world state
    NODE* m_currentNode;

    ///< Postprocessed world state
    NODE* m_lastNode;
    NODE* m_lastFixNode;

    SIZES_SETTINGS m_sizes;

    ///< Are we placing a via?
    bool m_placingVia;

    ///< current track width
    int m_currentWidth;

    int m_currentLayer;

    ///< The current bundle trace
    BUNDLE m_currentTrace;
    bool   m_currentTraceOk;

    ITEM* m_currentEndItem;

    ///< Pitch transition parameters
    int m_escapeLength;
    int m_transitionLength;
};

}

#endif // __PNS_BUNDLE_PLACER_H
