# Solution Synthesis

## Battle Iteration 3

---

## Solutions

### IMP-001-R1: N-Line Initialization Architecture — Bridging Single-Point Start() to N-Pad Multi-Line

- **Addresses:** CRIT-001
- **Status:** APPROVED (unchanged from iteration 2)

- **Original Approach:** The design document proposes `MULTI_LINE_PLACER` inheriting from `PLACEMENT_ALGO`, but `PLACEMENT_ALGO::Start()` accepts only a single `VECTOR2I` point and a single `ITEM*`. For N-line routing, we need N start points and N start items.

- **Problem:** The `Start(const VECTOR2I& aP, ITEM* aStartItem)` signature is a pure virtual interface. Changing it would break all existing placers.

#### Recommended: Pre-Configuration Pattern

- **Based on Research:**
  - KiCad `DIFF_PAIR_PLACER::Start()` calls `FindDpPrimitivePair()` to discover companion pad from single start item
  - pcb-rnd `bus_t` struct pre-configures `width` and `pitch` before routing begins (`bus.c:30-37`)

- **Implementation Sketch:**

```cpp
// ============================================================
// Step 1: Data structure for multi-line configuration
// ============================================================

struct MULTI_LINE_CONFIG
{
    struct PAD_ENTRY
    {
        VECTOR2I     position;    // Pad center position
        ITEM*        startItem;   // PNS ITEM (pad/via) — resolved via FindItemByParent()
        NET_HANDLE   net;         // Net handle (void*), NOT int
    };

    std::vector<PAD_ENTRY>  pads;
    int                     leaderIndex;
    int                     uniformSpacing;
    DIRECTION_45::CORNER_MODE cornerMode;
};

class MULTI_LINE_PLACER : public PLACEMENT_ALGO
{
public:
    MULTI_LINE_PLACER( ROUTER* aRouter ) : PLACEMENT_ALGO( aRouter ) {}

    void SetMultiLineConfig( const MULTI_LINE_CONFIG& aConfig )
    {
        m_config = aConfig;
        m_traceCount = aConfig.pads.size();
    }

    // Standard PLACEMENT_ALGO interface — unchanged signature
    bool Start( const VECTOR2I& aP, ITEM* aStartItem ) override;
    bool Move( const VECTOR2I& aP, ITEM* aEndItem ) override;
    bool FixRoute( const VECTOR2I& aP, ITEM* aEndItem, bool aForceFinish ) override;
    const ITEM_SET Traces() override;
    const VECTOR2I& CurrentEnd() const override;
    const std::vector<NET_HANDLE> CurrentNets() const override;
    int CurrentLayer() const override;
    NODE* CurrentNode( bool aLoopsRemoved = false ) const override;

    void AdjustSpacing( int aDelta );

private:
    MULTI_LINE_CONFIG       m_config;
    int                     m_traceCount;
    std::vector<LINE>       m_heads;
    std::vector<LINE>       m_tails;
    std::vector<NET_HANDLE> m_nets;
    NODE*                   m_lastNode = nullptr;
};

// ============================================================
// Step 2: Start() — uses pre-configured pads
// ============================================================

bool MULTI_LINE_PLACER::Start( const VECTOR2I& aP, ITEM* aStartItem )
{
    if( m_config.pads.empty() )
        return false;

    m_currentStart = aP;
    m_currentEnd = aP;

    m_heads.resize( m_traceCount );
    m_tails.resize( m_traceCount );
    m_nets.resize( m_traceCount );

    for( int i = 0; i < m_traceCount; i++ )
    {
        m_nets[i] = m_config.pads[i].net;
        m_heads[i].SetNet( m_nets[i] );
        m_heads[i].SetWidth( Router()->Sizes().TraceWidth() );
        m_tails[i].SetNet( m_nets[i] );
        m_tails[i].SetWidth( Router()->Sizes().TraceWidth() );
    }

    m_currentNode = Router()->GetWorld()->Branch();
    return true;
}

// ============================================================
// Step 3: UI layer PAD-to-PNS::ITEM conversion
// Uses NODE::FindItemByParent() (pns_node.h:485)
// ============================================================

// In router_tool.cpp, when building MULTI_LINE_CONFIG:
MULTI_LINE_CONFIG config;
NODE* world = m_router->GetWorld();

for( PAD* pad : selectedPads )
{
    MULTI_LINE_CONFIG::PAD_ENTRY entry;
    entry.position = pad->GetPosition();
    entry.startItem = world->FindItemByParent( pad );
    if( entry.startItem )
        entry.net = entry.startItem->Net();

    config.pads.push_back( entry );
}

// ============================================================
// Step 4: ROUTER integration — minimal changes
// ============================================================

// Inside ROUTER::StartRouting():
case PNS_MODE_ROUTE_MULTI_LINE:
{
    auto placer = std::make_unique<MULTI_LINE_PLACER>( this );
    placer->SetMultiLineConfig( m_multiLineConfig );
    m_placer = std::move( placer );
    break;
}
```

- **Complexity:** O(N) initialization.
- **Pros:** Zero changes to PLACEMENT_ALGO interface; follows DIFF_PAIR_PLACER pattern; clean separation of concerns.
- **Cons:** Requires ROUTER to know about MULTI_LINE_PLACER-specific configuration.

#### Rationale

Pre-configuration is the natural pattern: DIFF_PAIR_PLACER already resolves additional items beyond what Start() receives. pcb-rnd's bus_t confirms pre-configuration for multi-line. The key iteration-2 fix is using `FindItemByParent()` for PAD-to-PNS::ITEM conversion and `NET_HANDLE` (void*) instead of int for net identifiers.

---

### IMP-002-R2: Router Mode Dispatch and UI Integration

- **Addresses:** CRIT-002, CRIT-014
- **Revision Notes (Iteration 3):**
  1. **FIXED (CRIT-014): PNS_MODE enum order preserved.** Was `RM_MarkObstacles=0, RM_Walkaround=1, RM_Shove=2`. Actual KiCad order is `RM_MarkObstacles=0, RM_Shove=1, RM_Walkaround=2`. Now correctly preserves existing order and appends `RM_HighlightOnly=3`.
  2. All other aspects unchanged from R1.

- **Problem:** Four integration points: (1) new enum value, (2) new case in StartRouting(), (3) UI entry point, (4) obstacle mode cycling.

#### Recommended: New ROUTER_MODE Enum Value

- **Based on Research:**
  - KiCad `ROUTER_MODE` enum (`pns_router.h:62-68`) currently has 5 values
  - `ROUTER::StartRouting()` (`pns_router.cpp:438-462`) switch has `default: return false;` — safe to add
  - Obstacle mode cycling is independent of ROUTER_MODE (controlled by `ROUTING_SETTINGS::Mode()`)

- **Implementation Sketch:**

```cpp
// ============================================================
// Step 1: Enum extension — pns_router.h
// ============================================================

enum ROUTER_MODE {
    PNS_MODE_ROUTE_SINGLE = 1,
    PNS_MODE_ROUTE_DIFF_PAIR,
    PNS_MODE_TUNE_SINGLE,
    PNS_MODE_TUNE_DIFF_PAIR,
    PNS_MODE_TUNE_DIFF_PAIR_SKEW,
    PNS_MODE_ROUTE_MULTI_LINE          // NEW
};

// ============================================================
// Step 2: Obstacle avoidance mode extension — pns_routing_settings.h
// FIXED (CRIT-014): Preserve existing order!
// Actual KiCad: RM_MarkObstacles=0, RM_Shove=1, RM_Walkaround=2
// ============================================================

enum PNS_MODE {
    RM_MarkObstacles = 0,
    RM_Shove,            // value 1 — MUST stay at 1 (existing users)
    RM_Walkaround,       // value 2 — MUST stay at 2 (existing users)
    RM_HighlightOnly     // value 3 — NEW, appended at end
};

// Mode cycling in router_tool.cpp adds one more case:
// RM_MarkObstacles -> RM_Shove -> RM_Walkaround -> RM_HighlightOnly -> RM_MarkObstacles
```

- **Complexity:** O(1) for enum/dispatch. O(N) for pad sorting.

#### Rationale

Every routing variant in KiCad has its own ROUTER_MODE value. The critical CRIT-014 fix preserves the existing `PNS_MODE` enum order (`RM_Shove=1, RM_Walkaround=2`) to avoid silently swapping Shove/Walkaround behavior for existing users. `RM_HighlightOnly` is appended at value 3.

---

### IMP-003-R2: Leader-Follower Offset Geometry — Core Move() Algorithm

- **Addresses:** CRIT-001 (indirectly), CRIT-009 (NODE lifecycle), CRIT-010 (arc segment handling), CRIT-013 (ArcIndex misuse), CRIT-017 (OffsetArc sign convention), CRIT-018 (buildInitialLine private), CRIT-019 (posture tracking)
- **Revision Notes (Iteration 3):**
  1. **FIXED (CRIT-013): `ArcIndex(idx)` called with point index.** `ArcIndex()` takes a SEGMENT index (`shape_line_chain.h:860`). Now uses `IsPtOnArc(idx)` for point-based detection (correct API at `shape_line_chain.h:880`), then `ArcIndex(idx-1)` on the preceding segment to get the arc index.
  2. **FIXED (CRIT-017): `OffsetArc()` sign convention implemented.** Was a placeholder comment. Now uses cross product of (center-to-start) x (offset direction) to determine whether offset is inside or outside the curve. Inner offsets subtract from radius; outer offsets add.
  3. **FIXED (CRIT-018): Replaced `buildInitialLine()` with direct `DIRECTION_45::BuildInitialTrace()`.** `buildInitialLine()` is private to `LINE_PLACER` (`pns_line_placer.h:372`). Following the diff pair pattern (`pns_diff_pair.cpp:211-213`), `MULTI_LINE_PLACER` now calls `DIRECTION_45::BuildInitialTrace()` directly with its own posture state.
  4. **FIXED (CRIT-019): Added posture tracking.** New members: `MOUSE_TRAIL_TRACER m_mouseTrailTracer`, `DIRECTION_45 m_direction`, `VECTOR2I m_p_start`. `AddTrailPoint()` called in `Move()` (same pattern as `LINE_PLACER::Move()` at line 1541).

- **Problem:** The `Move()` method must generate N parallel traces in real-time (<16ms). Follower traces are computed via geometric offset from the leader's path.

#### Recommended: Per-Vertex Perpendicular Offset with Arc-Aware Iteration

- **Based on Research:**
  - pcb-rnd `bus.c:88-97`: perpendicular normal computation
  - pcb-rnd `bus.c:178-194`: corner compensation using `tan(angle/2)`
  - Clipper2 `clipper.offset.cpp:258-271`: miter join using `q = delta / (cos_a + 1)`
  - KiCad `SHAPE_LINE_CHAIN::IsPtOnArc()` for point-based arc detection
  - KiCad `DIRECTION_45::BuildInitialTrace()` for trace geometry construction
  - KiCad `MOUSE_TRAIL_TRACER` for posture inference from mouse movement

- **Implementation Sketch:**

```cpp
// ============================================================
// NEW (CRIT-018, CRIT-019): Posture tracking and leader routing
// MULTI_LINE_PLACER builds its own leader trace using
// DIRECTION_45::BuildInitialTrace() — same as diff pair pattern.
// ============================================================

class MULTI_LINE_PLACER : public PLACEMENT_ALGO
{
    // ... (members from IMP-001-R1) ...

    // NEW (CRIT-019): Posture tracking state
    MOUSE_TRAIL_TRACER  m_mouseTrailTracer;  // Infers routing direction from mouse movement
    DIRECTION_45        m_direction;          // Current routing direction (updated on tail extend)
    VECTOR2I            m_p_start;            // Leader's current start point (end of tail)

    // Arc tracking
    std::set<ssize_t>   m_processedArcs;     // Arcs already offset in current Move()
};

// ============================================================
// NEW (CRIT-018): routeLeaderHead() — replaces buildInitialLine()
// Pattern: DIRECTION_45::BuildInitialTrace() directly
// Reference: pns_diff_pair.cpp:211 uses DIRECTION_45().BuildInitialTrace()
// Reference: pns_line_placer.cpp:2057-2060 for posture logic
// ============================================================

bool MULTI_LINE_PLACER::routeLeaderHead( const VECTOR2I& aP, LINE& aHead )
{
    SHAPE_LINE_CHAIN l;
    DIRECTION_45::CORNER_MODE cornerMode = Settings().GetCornerMode();

    if( m_p_start == aP )
    {
        l.Clear();
    }
    else
    {
        // Use posture tracker to determine initial direction
        // Same logic as pns_line_placer.cpp:2057-2060
        if( !m_tails[m_config.leaderIndex].PointCount() )
        {
            // No tail yet — use mouse trail posture
            DIRECTION_45 guessedDir = m_mouseTrailTracer.GetPosture( aP );
            l = guessedDir.BuildInitialTrace( m_p_start, aP, false, cornerMode );
        }
        else
        {
            // Tail exists — use established direction
            l = m_direction.BuildInitialTrace( m_p_start, aP, false, cornerMode );
        }
    }

    aHead.SetLayer( m_currentLayer );
    aHead.SetShape( l );
    aHead.SetWidth( Router()->Sizes().TraceWidth() );
    aHead.SetNet( m_nets[m_config.leaderIndex] );

    return true;
}

// ============================================================
// Core offset algorithm — arc-aware version
// FIXED (CRIT-013): Uses IsPtOnArc() for point-based detection
// ============================================================

SHAPE_LINE_CHAIN MULTI_LINE_PLACER::OffsetPolyline(
    const SHAPE_LINE_CHAIN& aPath, int aOffset ) const
{
    SHAPE_LINE_CHAIN result;
    const int pointCount = aPath.PointCount();

    if( pointCount < 2 )
        return result;

    for( int idx = 0; idx < pointCount; idx++ )
    {
        // --- Arc detection (CRIT-013 FIX) ---
        // IsPtOnArc() takes a POINT index (shape_line_chain.h:880) — CORRECT
        // ArcIndex() takes a SEGMENT index (shape_line_chain.h:860) — use on adjacent segment
        if( idx > 0 && idx < pointCount - 1 && aPath.IsPtOnArc( idx ) )
        {
            // Get arc index from the PRECEDING segment (idx-1 is a valid segment index)
            ssize_t arcIdx = aPath.ArcIndex( idx - 1 );
            if( arcIdx < 0 )
            {
                // Try the following segment
                arcIdx = aPath.ArcIndex( idx );
            }

            if( arcIdx >= 0 && !m_processedArcs.count( arcIdx ) )
            {
                m_processedArcs.insert( arcIdx );
                OffsetArc( result, aPath, arcIdx, aOffset );
                // Skip remaining points of this arc
                while( idx + 1 < pointCount
                       && aPath.IsPtOnArc( idx + 1 )
                       && aPath.ArcIndex( idx ) == arcIdx )
                {
                    idx++;
                }
                continue;
            }
        }

        // --- Linear segment offset ---
        VECTOR2I offsetPoint;

        if( idx == 0 )
        {
            VECTOR2I dir = aPath.CPoint( 1 ) - aPath.CPoint( 0 );
            VECTOR2D normal = GetPerpendicularNormalD( dir );
            offsetPoint = aPath.CPoint( 0 ) + VECTOR2I(
                KiROUND( normal.x * aOffset ),
                KiROUND( normal.y * aOffset ) );
        }
        else if( idx == pointCount - 1 )
        {
            VECTOR2I dir = aPath.CPoint( idx ) - aPath.CPoint( idx - 1 );
            VECTOR2D normal = GetPerpendicularNormalD( dir );
            offsetPoint = aPath.CPoint( idx ) + VECTOR2I(
                KiROUND( normal.x * aOffset ),
                KiROUND( normal.y * aOffset ) );
        }
        else
        {
            // Interior point: angle bisector compensation
            // Delegates to IMP-004-R2's OffsetCorner() for style dispatch
            OffsetCorner( result, aPath, idx, aOffset,
                          m_config.cornerMode, m_config.miterRatio );
            continue;  // OffsetCorner appends directly to result
        }

        result.Append( offsetPoint );
    }

    return result;
}

// ============================================================
// Arc offset: generate concentric arc
// FIXED (CRIT-017): Sign convention fully implemented
// ============================================================

void MULTI_LINE_PLACER::OffsetArc(
    SHAPE_LINE_CHAIN& aResult,
    const SHAPE_LINE_CHAIN& aPath,
    ssize_t aArcIdx,
    int aOffset ) const
{
    const SHAPE_ARC& leaderArc = aPath.Arc( aArcIdx );
    VECTOR2I center = leaderArc.GetCenter();
    double leaderRadius = leaderArc.GetRadius();

    // FIXED (CRIT-017): Determine if offset is inside or outside the curve
    // using cross product of (center-to-start) and (travel direction at start).
    //
    // For a CCW arc: offset to the LEFT of travel = toward center = inner
    // For a CW arc:  offset to the LEFT of travel = away from center = outer
    //
    // Cross product of (start - center) x (travel direction at start):
    //   positive => CCW arc, negative => CW arc
    // Our offset convention: positive aOffset = left of travel direction
    //
    // If arc is CCW and offset is positive (left) => offset is toward center => INNER
    // If arc is CCW and offset is negative (right) => offset is away from center => OUTER
    // If arc is CW and offset is positive (left) => offset is away from center => OUTER
    // If arc is CW and offset is negative (right) => offset is toward center => INNER

    VECTOR2D centerToStart = VECTOR2D( leaderArc.GetP0() - center );
    // Travel direction at start: perpendicular to radius, in arc direction
    // For CCW: travel = (-centerToStart.y, centerToStart.x)
    // Cross product of centerToStart x travelDir determines arc orientation
    // But simpler: use IsClockwise() which KiCad already provides
    bool isCW = leaderArc.IsClockwise();

    // For CW arc: left of travel = outside => positive offset increases radius
    // For CCW arc: left of travel = inside => positive offset decreases radius
    double signedOffset;
    if( isCW )
        signedOffset = aOffset;   // CW: left = outer, positive offset = larger radius
    else
        signedOffset = -aOffset;  // CCW: left = inner, positive offset = smaller radius

    double followerRadius = leaderRadius + signedOffset;

    if( followerRadius <= 0 )
    {
        // Inner radius went negative — degenerate to a point
        aResult.Append( center );
        return;
    }

    // Construct concentric arc: same center, same angles, different radius
    // Compute follower start/end as radial projections from shared center
    VECTOR2D startDir = VECTOR2D( leaderArc.GetP0() - center );
    double startLen = startDir.EuclideanNorm();
    VECTOR2D endDir = VECTOR2D( leaderArc.GetP1() - center );
    double endLen = endDir.EuclideanNorm();

    VECTOR2I arcStart, arcEnd;
    if( startLen > 1.0 )
    {
        arcStart = center + VECTOR2I(
            KiROUND( startDir.x / startLen * followerRadius ),
            KiROUND( startDir.y / startLen * followerRadius ) );
    }
    else
    {
        arcStart = center;
    }

    if( endLen > 1.0 )
    {
        arcEnd = center + VECTOR2I(
            KiROUND( endDir.x / endLen * followerRadius ),
            KiROUND( endDir.y / endLen * followerRadius ) );
    }
    else
    {
        arcEnd = center;
    }

    SHAPE_ARC followerArc;
    followerArc.ConstructFromStartEndCenter( arcStart, arcEnd, center, isCW );
    aResult.Append( followerArc );
}

// Zero-length guard
static VECTOR2D GetPerpendicularNormalD( const VECTOR2I& aDir )
{
    double len = std::sqrt( (double)aDir.x * aDir.x + (double)aDir.y * aDir.y );
    if( len < 1.0 )
        return VECTOR2D( 0, 1 );  // Default: up, not (0,0) which causes NaN
    return VECTOR2D( -(double)aDir.y / len, (double)aDir.x / len );
}

// ============================================================
// Move() with NODE lifecycle and posture tracking
// FIXED (CRIT-019): AddTrailPoint() called for posture inference
// ============================================================

bool MULTI_LINE_PLACER::Move( const VECTOR2I& aP, ITEM* aEndItem )
{
    // NODE lifecycle (CRIT-009)
    if( m_lastNode )
    {
        delete m_lastNode;
        m_lastNode = nullptr;
    }

    // Step 1: Route leader trace using own posture tracking (CRIT-018)
    LINE leaderHead;
    bool ok = routeLeaderHead( aP, leaderHead );
    if( !ok )
        return false;

    // Step 2: Generate follower traces (arc-aware)
    m_processedArcs.clear();
    SHAPE_LINE_CHAIN leaderPath = m_tails[m_config.leaderIndex].CLine();
    leaderPath.Append( leaderHead.CLine() );

    std::vector<SHAPE_LINE_CHAIN> followerPaths =
        GenerateFollowerTraces( leaderPath );

    // Step 3: Update heads
    for( int i = 0; i < m_traceCount; i++ )
        m_heads[i].SetShape( ExtractHead( followerPaths[i], m_tails[i] ) );

    // Step 4: Branch and check collisions
    m_lastNode = m_currentNode->Branch();

    for( int i = 0; i < m_traceCount; i++ )
    {
        if( m_lastNode->CheckColliding( &m_heads[i] ) )
        {
            if( Settings().Mode() == RM_MarkObstacles )
            {
                m_fitOk = false;
                break;
            }
        }
    }

    // Step 5: Update posture tracker (CRIT-019)
    // Same pattern as pns_line_placer.cpp:1541
    m_mouseTrailTracer.AddTrailPoint( aP );

    return true;
}
```

- **Complexity:** O(N x V) per Move() where V = vertices in leader head (~3-5). Arc handling adds O(A) where A = arc segments (typically 0-2).

#### Rationale

The four critical fixes in this revision:
1. **CRIT-013**: `IsPtOnArc(idx)` is the correct point-based API; `ArcIndex(idx-1)` converts to segment-based arc lookup. The previous code called `ArcIndex(idx)` with a point index, which is a segment-index API.
2. **CRIT-017**: The sign convention for `OffsetArc()` is now fully implemented using `IsClockwise()`. For CW arcs, left-of-travel is outside (positive offset = larger radius). For CCW arcs, left-of-travel is inside (positive offset = smaller radius).
3. **CRIT-018**: `buildInitialLine()` is private to `LINE_PLACER`. `MULTI_LINE_PLACER` now has its own `routeLeaderHead()` that calls `DIRECTION_45::BuildInitialTrace()` directly — the same approach used by `pns_diff_pair.cpp:211`.
4. **CRIT-019**: Posture tracking via `MOUSE_TRAIL_TRACER` + `DIRECTION_45 m_direction` + `VECTOR2I m_p_start`. `AddTrailPoint()` is called at the end of each `Move()`, matching `LINE_PLACER::Move()` at line 1541.

---

### IMP-004-R2: Corner Styles for Multi-Line — MITERED_45, Miter/Chamfer, and ROUNDED

- **Addresses:** First-phase corner style requirements, CRIT-006 (negative inner radius), CRIT-010 (arc integration), CRIT-020 (arc start/end not on follower circle)
- **Revision Notes (Iteration 3):**
  1. **FIXED (CRIT-020): ROUNDED case arc start/end now computed as radial projections from `leaderCenter`.** Was computing `arcStart`/`arcEnd` as simple perpendicular offsets from the vertex. These points are NOT on a circle centered at `leaderCenter` — they're on a different circle centered at the offset vertex. Now correctly projects from `leaderCenter` along the radial direction to the leader's arc start/end points, scaled to `followerRadius`.
  2. All other aspects (miter formula, bevel fallback, clockwise determination) unchanged from R1.

- **Problem:** IMP-003-R2's `OffsetPolyline()` handles linear segments. Corner vertices need style-specific handling: miter point for MITERED_45, truncated miter for chamfer, concentric arc for ROUNDED.

#### Recommended: Unified Corner Handler with Correct Concentric Arc Geometry

- **Based on Research:**
  - Clipper2 `clipper.offset.cpp:258-271` (DoMiter): `q = delta / (cos_a + 1)`
  - Clipper2 `clipper.offset.cpp:273-310` (DoRound): arc approximation at offset distance
  - KiCad `SHAPE_ARC::ConstructFromStartEndCenter()` (`shape_arc.h:114-116`)
  - pcb-rnd `bus.c:178-194`: `tune = tan(angle/2)` for corner compensation

- **Implementation Sketch:**

```cpp
// ============================================================
// OffsetCorner() — called from OffsetPolyline() for interior LINEAR vertices
// For arc segments, OffsetArc() in IMP-003-R2 handles them separately.
// ============================================================

void MULTI_LINE_PLACER::OffsetCorner(
    SHAPE_LINE_CHAIN& aResult,
    const SHAPE_LINE_CHAIN& aLeaderPath,
    int aVertexIdx,
    int aOffset,
    DIRECTION_45::CORNER_MODE aMode,
    double aMiterRatio ) const
{
    VECTOR2I vertex = aLeaderPath.CPoint( aVertexIdx );
    VECTOR2I dirIn  = vertex - aLeaderPath.CPoint( aVertexIdx - 1 );
    VECTOR2I dirOut = aLeaderPath.CPoint( aVertexIdx + 1 ) - vertex;

    VECTOR2D normIn  = GetPerpendicularNormalD( dirIn );
    VECTOR2D normOut = GetPerpendicularNormalD( dirOut );

    double cos_a = normIn.x * normOut.x + normIn.y * normOut.y;

    switch( aMode )
    {
    case DIRECTION_45::CORNER_MODE::MITERED_45:
    {
        const double MITER_LIMIT_THRESHOLD = -0.5;  // cos(120 deg)

        if( cos_a > MITER_LIMIT_THRESHOLD )
        {
            // Miter join: q = offset / (cos_a + 1)
            double q = (double)aOffset / ( cos_a + 1.0 );
            VECTOR2I miterPt = vertex + VECTOR2I(
                KiROUND( ( normIn.x + normOut.x ) * q ),
                KiROUND( ( normIn.y + normOut.y ) * q ) );

            // Miter ratio support (inside MITERED_45 case)
            if( aMiterRatio < 1.0 )
            {
                VECTOR2I bevelPt1 = vertex + VECTOR2I(
                    KiROUND( normIn.x * aOffset ),
                    KiROUND( normIn.y * aOffset ) );
                VECTOR2I bevelPt2 = vertex + VECTOR2I(
                    KiROUND( normOut.x * aOffset ),
                    KiROUND( normOut.y * aOffset ) );

                // Interpolate: chamfer = bevel + ratio * (miter - bevel)
                VECTOR2I chamferPt1 = bevelPt1 + VECTOR2I(
                    KiROUND( ( miterPt.x - bevelPt1.x ) * aMiterRatio ),
                    KiROUND( ( miterPt.y - bevelPt1.y ) * aMiterRatio ) );
                VECTOR2I chamferPt2 = bevelPt2 + VECTOR2I(
                    KiROUND( ( miterPt.x - bevelPt2.x ) * aMiterRatio ),
                    KiROUND( ( miterPt.y - bevelPt2.y ) * aMiterRatio ) );

                aResult.Append( chamferPt1 );
                aResult.Append( chamferPt2 );
            }
            else
            {
                aResult.Append( miterPt );
            }
        }
        else
        {
            // Acute angle: bevel fallback (two points)
            aResult.Append( vertex + VECTOR2I(
                KiROUND( normIn.x * aOffset ),
                KiROUND( normIn.y * aOffset ) ) );
            aResult.Append( vertex + VECTOR2I(
                KiROUND( normOut.x * aOffset ),
                KiROUND( normOut.y * aOffset ) ) );
        }
        break;
    }

    case DIRECTION_45::CORNER_MODE::ROUNDED_45:
    case DIRECTION_45::CORNER_MODE::ROUNDED_90:
    {
        // ROUNDED corners on LINEAR vertices (rare — typically the leader
        // trace already has SHAPE_ARC segments handled by OffsetArc).
        // This handles the scenario where BuildInitialTrace() produced
        // a rounded corner stored as linear approximation points.

        // Compute the leader's arc radius from the vertex geometry
        double diagLen = std::sqrt( (double)dirIn.x * dirIn.x +
                                    (double)dirIn.y * dirIn.y );
        double leaderRadius = diagLen / ( 2.0 * cos( M_PI * 67.5 / 180.0 ) );

        // Arc center is offset along the angle bisector, NOT at vertex
        VECTOR2D bisector( normIn.x + normOut.x, normIn.y + normOut.y );
        double bisLen = std::sqrt( bisector.x * bisector.x +
                                   bisector.y * bisector.y );
        if( bisLen > 1e-6 )
        {
            bisector.x /= bisLen;
            bisector.y /= bisLen;
        }

        double halfAngle = std::acos( std::clamp( cos_a, -1.0, 1.0 ) ) / 2.0;
        double centerDist = leaderRadius / std::cos( halfAngle );
        VECTOR2I leaderCenter = vertex + VECTOR2I(
            KiROUND( bisector.x * centerDist ),
            KiROUND( bisector.y * centerDist ) );

        // Follower radius = leaderRadius + offset
        double followerRadius = leaderRadius + aOffset;

        // Negative radius -> bevel fallback (CRIT-006)
        if( followerRadius <= 0 )
        {
            aResult.Append( vertex + VECTOR2I(
                KiROUND( normIn.x * aOffset ),
                KiROUND( normIn.y * aOffset ) ) );
            aResult.Append( vertex + VECTOR2I(
                KiROUND( normOut.x * aOffset ),
                KiROUND( normOut.y * aOffset ) ) );
            break;
        }

        // FIXED (CRIT-020): Compute follower arc start/end as RADIAL PROJECTIONS
        // from leaderCenter, NOT as perpendicular offsets from vertex.
        //
        // For concentric arcs, start/end must lie on the follower circle
        // (centered at leaderCenter, radius = followerRadius).
        //
        // Leader arc start/end are at distance leaderRadius from leaderCenter.
        // Follower arc start/end are in the SAME radial direction, at followerRadius.
        //
        // Geometric formula:
        //   followerStart = leaderCenter + normalize(leaderArcStart - leaderCenter) * followerRadius

        // Compute leader arc start/end points (tangent points on leader circle)
        // For a 45-degree routing corner, the arc tangent points are where
        // the incoming/outgoing segments meet the arc.
        // Approximate: project from leaderCenter along incoming/outgoing normals
        VECTOR2D inDir = VECTOR2D( dirIn );
        double inLen = inDir.EuclideanNorm();
        if( inLen > 1.0 ) { inDir.x /= inLen; inDir.y /= inLen; }

        VECTOR2D outDir = VECTOR2D( dirOut );
        double outLen = outDir.EuclideanNorm();
        if( outLen > 1.0 ) { outDir.x /= outLen; outDir.y /= outLen; }

        // Leader arc start: on the incoming segment side
        // = leaderCenter + normalize(tangentPointIn - leaderCenter) * leaderRadius
        // tangentPointIn is where the incoming segment meets the arc
        VECTOR2I leaderArcStart = vertex - VECTOR2I(
            KiROUND( inDir.x * leaderRadius * std::tan( halfAngle ) ),
            KiROUND( inDir.y * leaderRadius * std::tan( halfAngle ) ) );
        VECTOR2I leaderArcEnd = vertex + VECTOR2I(
            KiROUND( outDir.x * leaderRadius * std::tan( halfAngle ) ),
            KiROUND( outDir.y * leaderRadius * std::tan( halfAngle ) ) );

        // Radial projection from leaderCenter
        VECTOR2D startRadial = VECTOR2D( leaderArcStart - leaderCenter );
        double startRadLen = startRadial.EuclideanNorm();
        VECTOR2D endRadial = VECTOR2D( leaderArcEnd - leaderCenter );
        double endRadLen = endRadial.EuclideanNorm();

        VECTOR2I arcStart, arcEnd;
        if( startRadLen > 1.0 )
        {
            arcStart = leaderCenter + VECTOR2I(
                KiROUND( startRadial.x / startRadLen * followerRadius ),
                KiROUND( startRadial.y / startRadLen * followerRadius ) );
        }
        else
        {
            arcStart = leaderCenter;
        }

        if( endRadLen > 1.0 )
        {
            arcEnd = leaderCenter + VECTOR2I(
                KiROUND( endRadial.x / endRadLen * followerRadius ),
                KiROUND( endRadial.y / endRadLen * followerRadius ) );
        }
        else
        {
            arcEnd = leaderCenter;
        }

        // Determine CW/CCW from turn direction
        double cross = (double)dirIn.x * dirOut.y - (double)dirIn.y * dirOut.x;
        bool clockwise = ( aOffset > 0 ) ? ( cross < 0 ) : ( cross > 0 );

        SHAPE_ARC arc;
        arc.ConstructFromStartEndCenter( arcStart, arcEnd, leaderCenter, clockwise );
        aResult.Append( arc );
        break;
    }

    default:
        // Fallback: simple perpendicular offset
        aResult.Append( vertex + VECTOR2I(
            KiROUND( normIn.x * aOffset ),
            KiROUND( normIn.y * aOffset ) ) );
        break;
    }
}
```

- **Complexity:** O(1) per corner vertex. Arc construction is O(1) for SHAPE_ARC.

#### Rationale

The critical CRIT-020 fix: for concentric arcs, the follower's start/end points must lie on the follower circle (centered at `leaderCenter`, radius `followerRadius`). The previous code computed `arcStart`/`arcEnd` as perpendicular offsets from the vertex — these points lie on a circle centered at the offset vertex, NOT at `leaderCenter`. The fix uses radial projection: `followerStart = leaderCenter + normalize(leaderArcStart - leaderCenter) * followerRadius`. This ensures `ConstructFromStartEndCenter()` receives points that are equidistant from the center, producing a valid arc.

---

### IMP-005-R2: Obstacle Avoidance — All 4 Modes for N-Trace Bundle

- **Addresses:** CRIT-003 (ShoveMultiLines doesn't exist), CRIT-005 (fat-trace net), CRIT-007 (inter-head collision), CRIT-012 (head-to-head via blind spot), CRIT-015 (WALKAROUND type names), CRIT-016 (SH_HEAD_MODIFIED dead code), CRIT-018 (buildInitialLine private), CRIT-021 (collision check on wrong node)
- **Revision Notes (Iteration 3):**
  1. **FIXED (CRIT-015): WALKAROUND type names corrected.** `WALKAROUND::WALKAROUND_STATUS` -> `WALKAROUND::STATUS` (`pns_walkaround.h:61`). `WALKAROUND::STUCK` -> `WALKAROUND::ST_STUCK` (`pns_walkaround.h:66`).
  2. **FIXED (CRIT-016): Removed `SH_HEAD_MODIFIED` from status check.** Only check `st == SHOVE::SH_OK`. `SH_HEAD_MODIFIED` exists in the enum (`pns_shove.h:54`) but is NEVER returned by any function. The diff pair placer only checks `SH_OK` (`pns_diff_pair_placer.cpp:379`). `HeadsModified(i)` and `GetModifiedHead(i)` are still used — they check `m_headLines[i].geometryModified` which IS set during `reconstructHeads()`.
  3. **FIXED (CRIT-018): Replaced all `buildInitialLine()` calls with `routeLeaderHead()`.** All four modes now use `routeLeaderHead()` from IMP-003-R2, which calls `DIRECTION_45::BuildInitialTrace()` directly with own posture state.
  4. **FIXED (CRIT-021): All modes now branch before collision checking.** `routeHeadStrict()` and `routeHeadWalkaround()` were checking collisions on `m_currentNode` (base world). Now all modes branch first (`m_lastNode = m_currentNode->Branch()`), add heads to the branch, then check collisions on `m_lastNode`. This matches the pattern in `LINE_PLACER::Move()` (`pns_line_placer.cpp:1525-1526`).

- **Problem:** Each obstacle mode has different complexity. All four modes must use the correct KiCad APIs and branch/collision patterns.

#### Mode 1: STRICT (Maps to RM_MarkObstacles)

```cpp
bool MULTI_LINE_PLACER::routeHeadStrict( const VECTOR2I& aP )
{
    // FIXED (CRIT-018): Use routeLeaderHead() instead of buildInitialLine()
    LINE leaderHead;
    routeLeaderHead( aP, leaderHead );
    auto followers = GenerateFollowerTraces( leaderHead.CLine() );

    for( int i = 0; i < m_traceCount; i++ )
    {
        m_heads[i].SetShape( followers[i] );
        m_heads[i].SetWidth( Router()->Sizes().TraceWidth() );
        m_heads[i].SetNet( m_nets[i] );
    }

    // FIXED (CRIT-021): Branch BEFORE collision checking
    // Pattern from pns_line_placer.cpp:1525-1526
    m_lastNode = m_currentNode->Branch();

    bool anyCollision = false;
    for( int i = 0; i < m_traceCount; i++ )
    {
        if( m_lastNode->CheckColliding( &m_heads[i] ) )
        {
            anyCollision = true;
            break;
        }
    }

    m_fitOk = !anyCollision;
    return true;
}
```

#### Mode 2: WALKAROUND (Maps to RM_Walkaround) — Fat Trace with nullptr Net

```cpp
bool MULTI_LINE_PLACER::routeHeadWalkaround( const VECTOR2I& aP )
{
    int traceWidth = Router()->Sizes().TraceWidth();
    int bundleWidth = m_traceCount * traceWidth
                    + ( m_traceCount - 1 ) * m_config.uniformSpacing;

    LINE fatTrace;
    fatTrace.SetWidth( bundleWidth );

    // CRIT-005: Set net to nullptr so fat trace collides with EVERYTHING.
    fatTrace.SetNet( nullptr );

    // FIXED (CRIT-018): Build fat trace using own posture tracking
    SHAPE_LINE_CHAIN fatPath;
    DIRECTION_45::CORNER_MODE cornerMode = Settings().GetCornerMode();

    if( !m_tails[m_config.leaderIndex].PointCount() )
    {
        DIRECTION_45 guessedDir = m_mouseTrailTracer.GetPosture( aP );
        fatPath = guessedDir.BuildInitialTrace( m_p_start, aP, false, cornerMode );
    }
    else
    {
        fatPath = m_direction.BuildInitialTrace( m_p_start, aP, false, cornerMode );
    }

    fatTrace.SetLayer( m_currentLayer );
    fatTrace.SetShape( fatPath );

    WALKAROUND walkaround( m_currentNode, Router() );
    walkaround.SetSolidsOnly( false );
    walkaround.SetIterationLimit( 20 );

    LINE walkResult;
    // FIXED (CRIT-015): Correct type names
    // WALKAROUND::STATUS (not WALKAROUND_STATUS), ST_STUCK (not STUCK)
    WALKAROUND::STATUS status = walkaround.Route( fatTrace, walkResult );

    if( status == WALKAROUND::ST_STUCK )
        return false;

    // Split fat trace into N individual traces
    SHAPE_LINE_CHAIN centerPath = walkResult.CLine();
    auto followerPaths = GenerateFollowerTraces( centerPath );

    for( int i = 0; i < m_traceCount; i++ )
    {
        m_heads[i].SetShape( followerPaths[i] );
        m_heads[i].SetWidth( traceWidth );
        m_heads[i].SetNet( m_nets[i] );
    }

    // FIXED (CRIT-021): Branch before collision checking
    m_lastNode = m_currentNode->Branch();

    // Verify no individual trace collides after splitting
    for( int i = 0; i < m_traceCount; i++ )
    {
        if( m_lastNode->CheckColliding( &m_heads[i] ) )
        {
            m_fitOk = false;
            return true;
        }
    }

    m_fitOk = true;
    return true;
}
```

#### Mode 3: PUSH_SHOVE (Maps to RM_Shove) — Using Correct KiCad API

```cpp
bool MULTI_LINE_PLACER::routeHeadShove( const VECTOR2I& aP )
{
    // Step 1: Route leader and generate followers
    // FIXED (CRIT-018): Use routeLeaderHead() instead of buildInitialLine()
    LINE leaderHead;
    routeLeaderHead( aP, leaderHead );
    auto followerPaths = GenerateFollowerTraces( leaderHead.CLine() );

    for( int i = 0; i < m_traceCount; i++ )
    {
        m_heads[i].SetShape( followerPaths[i] );
        m_heads[i].SetWidth( Router()->Sizes().TraceWidth() );
        m_heads[i].SetNet( m_nets[i] );
    }

    // Step 2: Use ClearHeads/AddHeads/Run pattern
    // from pns_diff_pair_placer.cpp:371-375
    m_shove->ClearHeads();

    for( int i = 0; i < m_traceCount; i++ )
    {
        m_shove->AddHeads( m_heads[i] );
    }

    SHOVE::SHOVE_STATUS st = m_shove->Run();

    // FIXED (CRIT-016): Only check SH_OK, NOT SH_HEAD_MODIFIED
    // SH_HEAD_MODIFIED is never returned by any function (dead code in enum).
    // Diff pair placer only checks SH_OK (pns_diff_pair_placer.cpp:379).
    // HeadsModified(i) / GetModifiedHead(i) are still valid and used below.
    if( st == SHOVE::SH_OK )
    {
        m_currentNode = m_shove->CurrentNode();

        // Retrieve modified heads
        for( int i = 0; i < m_traceCount; i++ )
        {
            if( m_shove->HeadsModified( i ) )
                m_heads[i] = m_shove->GetModifiedHead( i );
        }

        // Step 3: Post-shove collision verification (CRIT-007)
        // SHOVE processes heads sequentially (pns_shove.cpp:2462-2583).
        // Head N's shove may push obstacles into Head 0's path.
        // Pattern from pns_diff_pair_placer.cpp:392-394
        bool allClear = true;
        for( int i = 0; i < m_traceCount; i++ )
        {
            if( m_currentNode->CheckColliding( &m_heads[i] ) )
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

    return true;
}
```

#### Mode 4: HIGHLIGHT_ONLY (New RM_HighlightOnly)

```cpp
bool MULTI_LINE_PLACER::routeHeadHighlightOnly( const VECTOR2I& aP )
{
    // FIXED (CRIT-018): Use routeLeaderHead() instead of buildInitialLine()
    LINE leaderHead;
    routeLeaderHead( aP, leaderHead );
    auto followerPaths = GenerateFollowerTraces( leaderHead.CLine() );

    m_violations.clear();

    // FIXED (CRIT-021): Branch before checking
    m_lastNode = m_currentNode->Branch();

    for( int i = 0; i < m_traceCount; i++ )
    {
        m_heads[i].SetShape( followerPaths[i] );
        m_heads[i].SetWidth( Router()->Sizes().TraceWidth() );
        m_heads[i].SetNet( m_nets[i] );

        NODE::OPT_OBSTACLE obs = m_lastNode->NearestObstacle( &m_heads[i] );
        if( obs )
        {
            m_violations.push_back( {
                .traceIndex = i,
                .obstacle = obs->m_item,
                .position = obs->m_ipFirst
            } );
        }
    }

    for( const auto& v : m_violations )
        m_heads[v.traceIndex].Mark( MK_VIOLATION );

    m_fitOk = true;  // Always "fit" — violations are informational
    return true;
}
```

#### Known Limitation: Head-to-Head Via Collision (CRIT-012)

`pns_item.cpp:137-138` explicitly states: "you can't route two independent tracks at once so it shouldn't come up." This assumption is violated by N-line routing. When N heads are in the node simultaneously, head-to-head via collisions will NOT be detected.

**First-phase impact:** None — first phase is single-layer, no vias. Head-to-head LINE collisions ARE detected (only via-to-via is blind).

**Future fix (deferred):** When via support is added, extend `pns_item.cpp` collision logic to handle head-via-to-head-via by checking `MK_HEAD` marks on both items.

#### Rationale

The four iteration-3 fixes:
1. **CRIT-015**: Trivial compilation fix — `WALKAROUND::STATUS` and `WALKAROUND::ST_STUCK` are the correct type/value names from `pns_walkaround.h:61-66`.
2. **CRIT-016**: `SH_HEAD_MODIFIED` is dead code — it exists in the enum but is never returned. The diff pair placer confirms: only `SH_OK` is checked (`pns_diff_pair_placer.cpp:379`). The `HeadsModified(i)` / `GetModifiedHead(i)` methods are separate and still valid.
3. **CRIT-018**: All four modes now use `routeLeaderHead()` (IMP-003-R2) which calls `DIRECTION_45::BuildInitialTrace()` directly. Zero dependency on `LINE_PLACER` private methods.
4. **CRIT-021**: All modes now branch `m_lastNode = m_currentNode->Branch()` before collision checking. This ensures collision detection sees the correct world state including any previously committed traces.

---

### IMP-006-R1: Performance — Incremental Computation (Simplified)

- **Addresses:** CRIT-004 (QueryColliding API mismatch), CRIT-008 (GetHash doesn't exist)
- **Status:** APPROVED (unchanged from iteration 2)

- **Problem:** Each Move() must route leader, generate N-1 followers, check N collisions. For N=8, must stay under 16.67ms.

#### Recommended: Two-Tier Optimization (Head/Tail + Generation Counter)

- **Based on Research:**
  - KiCad head/tail pattern: only head (2-5 segments) recalculated per Move()
  - KiCad NODE branching: O(1) with copy-on-write
  - Critic's performance budget: 2.7-5.7ms total for N=8 even without batch optimization

- **Implementation Sketch:**

```cpp
// ============================================================
// Tier 1: Head/Tail Pattern for N Traces
// ============================================================

bool MULTI_LINE_PLACER::Move( const VECTOR2I& aP, ITEM* aEndItem )
{
    // NODE lifecycle (CRIT-009)
    if( m_lastNode )
    {
        delete m_lastNode;
        m_lastNode = nullptr;
    }

    // Only the leader head changes each frame
    LINE leaderHead;
    routeLeaderHead( aP, leaderHead );  // 2-5ms (same as single-line)

    // Generate follower heads from leader head only (not full path)
    SHAPE_LINE_CHAIN leaderHeadPath = leaderHead.CLine();
    auto followerHeadPaths = GenerateFollowerTraces( leaderHeadPath );  // O(N*V), V~3

    for( int i = 0; i < m_traceCount; i++ )
        m_heads[i].SetShape( followerHeadPaths[i] );

    // Collision detection: N individual CheckColliding() calls
    // Each is O(log M) R-tree query. For N=8, M=10000: 8 * 13 = 104 node visits ~ 0.5ms
    m_lastNode = m_currentNode->Branch();
    m_fitOk = true;

    for( int i = 0; i < m_traceCount; i++ )
    {
        if( m_lastNode->CheckColliding( &m_heads[i] ) )
        {
            m_fitOk = false;
            if( Settings().Mode() == RM_MarkObstacles )
                break;  // STRICT: stop on first collision
        }
    }

    // Update posture tracker
    m_mouseTrailTracer.AddTrailPoint( aP );

    return true;
}

// ============================================================
// Tier 2: Follower Tail Caching with Generation Counter
// ============================================================

class MULTI_LINE_PLACER
{
    // Generation counter instead of GetHash() (CRIT-008)
    std::vector<SHAPE_LINE_CHAIN> m_cachedFollowerTails;
    int m_tailGeneration = 0;
    int m_cachedTailGeneration = -1;

    void updateFollowerTails()
    {
        if( m_tailGeneration != m_cachedTailGeneration )
        {
            m_cachedFollowerTails = GenerateFollowerTraces(
                m_tails[m_config.leaderIndex].CLine() );
            m_cachedTailGeneration = m_tailGeneration;
        }
    }

    void onTailModified()
    {
        m_tailGeneration++;
    }
};
```

- **Performance Budget (N=8 traces):**
  - Leader head routing: 2-5ms (unchanged from single-line)
  - Follower generation: 0.1ms (N=8, V=3, simple arithmetic)
  - N individual collision checks: 0.5ms (8 x O(log M), M~10000)
  - NODE branching: <0.1ms (copy-on-write)
  - **Total: 2.7-5.7ms** — well within 16.67ms budget

- **Complexity:** O(N x V + N x log M) per Move(). Dominated by leader routing (2-5ms).

#### Rationale

Batch R-tree optimization saves ~0.4ms for N=8 — not worth the implementation cost. N individual `CheckColliding()` calls are simple, correct, and fast enough. The generation counter for cache invalidation is O(1) and avoids the non-existent `GetHash()` API entirely.

---

### IMP-007-R1: Uniform Spacing Control with Hotkey/Scroll Wheel Adjustment

- **Addresses:** CRIT-011 (Clearance nullptr crash)
- **Status:** APPROVED (unchanged from iteration 2; CRIT-022 retracted)

- **Problem:** Spacing changes during routing must immediately update all follower traces, validate minimum spacing, and provide visual feedback.

#### Recommended: Spacing as MULTI_LINE_PLACER State with Hotkey Binding

- **Based on Research:**
  - KiCad `NODE::GetClearance(const ITEM*, const ITEM*)` at `pns_node.h:258`
  - KiCad `SIZES_SETTINGS::Clearance()` for global minimum
  - Altium Designer: keyboard shortcuts for on-the-fly parameter adjustment

- **Implementation Sketch:**

```cpp
void MULTI_LINE_PLACER::AdjustSpacing( int aDeltaNm )
{
    int newSpacing = m_config.uniformSpacing + aDeltaNm;

    // Use Router()->Sizes().Clearance() for global minimum
    // NOT GetRuleResolver()->Clearance(nullptr, nullptr) which segfaults (CRIT-011)
    int globalClearance = Router()->Sizes().Clearance();
    int minSpacing = Router()->Sizes().TraceWidth() + globalClearance;

    // For more precise minimum: use actual clearance between adjacent traces
    if( m_traceCount >= 2 && m_heads[0].SegmentCount() > 0 )
    {
        int actualClearance = m_currentNode->GetClearance(
            &m_heads[0], &m_heads[1] );  // pns_node.h:258
        minSpacing = Router()->Sizes().TraceWidth() + actualClearance;
    }

    newSpacing = std::max( newSpacing, minSpacing );

    // Enforce maximum: prevent unreasonably wide bundles
    int maxSpacing = 10 * Router()->Sizes().TraceWidth();
    newSpacing = std::min( newSpacing, maxSpacing );

    m_config.uniformSpacing = newSpacing;

    // Invalidate follower tail cache (forces recomputation)
    m_tailGeneration++;  // Uses generation counter from IMP-006-R1
}

// In router_tool.cpp — hotkey handler during routing:
void ROUTER_TOOL::onMultiLineSpacingAdjust( int aDelta )
{
    if( m_router->Mode() != PNS_MODE_ROUTE_MULTI_LINE )
        return;

    MULTI_LINE_PLACER* placer =
        static_cast<MULTI_LINE_PLACER*>( m_router->Placer() );

    // aDelta from scroll wheel: +/- 50000 nm (50 um) per tick
    // aDelta from hotkey: +/- 25400 nm (1 mil) per press
    placer->AdjustSpacing( aDelta );

    // Force redraw with new spacing
    m_router->Move( m_endSnapPoint, m_endItem );
    updateEndItem( m_endSnapPoint );
}
```

- **Complexity:** O(1) for spacing adjustment. Follower recomputation happens in next Move() call.

#### Rationale

`Router()->Sizes().Clearance()` provides a safe global minimum without dereferencing any items. For more precise spacing, `NODE::GetClearance(&m_heads[0], &m_heads[1])` uses the actual DRC rules between adjacent traces. `LINE::SegmentCount()` exists at `pns_line.h:144` (CRIT-022 retracted).

---

## Revisions

### IMP-002-R2
- **Revision Notes:** Fixed CRIT-014 — PNS_MODE enum order preserved (`RM_MarkObstacles=0, RM_Shove=1, RM_Walkaround=2, RM_HighlightOnly=3`).

### IMP-003-R2
- **Revision Notes:** Fixed CRIT-013 (IsPtOnArc for point-based detection), CRIT-017 (OffsetArc sign convention using IsClockwise), CRIT-018 (routeLeaderHead with BuildInitialTrace), CRIT-019 (MOUSE_TRAIL_TRACER posture tracking).

### IMP-004-R2
- **Revision Notes:** Fixed CRIT-020 (arc start/end as radial projections from leaderCenter, not perpendicular offsets from vertex).

### IMP-005-R2
- **Revision Notes:** Fixed CRIT-015 (WALKAROUND::STATUS/ST_STUCK), CRIT-016 (removed SH_HEAD_MODIFIED check), CRIT-018 (routeLeaderHead replaces buildInitialLine), CRIT-021 (branch before collision checking in all modes).

---

## Summary

This iteration-3 synthesis addresses all 22 critic challenges (CRIT-001 through CRIT-022) across 7 solutions:

| ID | Topic | Verdict | Key Iteration-3 Changes |
|---|---|---|---|
| IMP-001-R1 | N-line initialization | APPROVED | No changes needed. |
| IMP-002-R2 | Router mode dispatch | REVISED | CRIT-014: PNS_MODE enum order preserved. |
| IMP-003-R2 | Leader-follower offset | REVISED | CRIT-013: IsPtOnArc; CRIT-017: OffsetArc sign; CRIT-018: routeLeaderHead; CRIT-019: posture tracking. |
| IMP-004-R2 | Corner styles | REVISED | CRIT-020: Radial projection for concentric arc start/end. |
| IMP-005-R2 | Obstacle avoidance | REVISED | CRIT-015: type names; CRIT-016: SH_OK only; CRIT-018: routeLeaderHead; CRIT-021: branch first. |
| IMP-006-R1 | Performance | APPROVED | No changes needed. |
| IMP-007-R1 | Spacing control | APPROVED | No changes needed (CRIT-022 retracted). |

**All challenges resolved:**

| Challenge | Severity | Resolution |
|---|---|---|
| CRIT-001 | High | IMP-001-R1: NET_HANDLE types fixed |
| CRIT-002 | High | IMP-002-R2: RM_HighlightOnly scope clarified |
| CRIT-003 | Critical | IMP-005-R2: ClearHeads/AddHeads/Run replaces non-existent ShoveMultiLines |
| CRIT-004 | Medium | IMP-006-R1: Batch query removed; N individual CheckColliding calls |
| CRIT-005 | Medium | IMP-005-R2: Fat-trace net = nullptr |
| CRIT-006 | Low | IMP-004-R2: Bevel fallback for negative inner radius |
| CRIT-007 | High | IMP-005-R2: Post-shove collision verification for all N heads |
| CRIT-008 | Medium | IMP-006-R1: Generation counter replaces non-existent GetHash() |
| CRIT-009 | Medium | IMP-003-R2: delete m_lastNode before re-branching |
| CRIT-010 | Medium | IMP-003-R2 + IMP-004-R2: Arc-aware OffsetPolyline with OffsetArc delegation |
| CRIT-011 | Low | IMP-007-R1: Router()->Sizes().Clearance() replaces nullptr crash |
| CRIT-012 | Medium | IMP-005-R2: Documented as known limitation (no vias in first phase) |
| CRIT-013 | Medium | IMP-003-R2: IsPtOnArc(idx) for point-based detection, ArcIndex(idx-1) for segment lookup |
| CRIT-014 | Medium | IMP-002-R2: PNS_MODE enum order preserved (RM_Shove=1, RM_Walkaround=2) |
| CRIT-015 | Low | IMP-005-R2: WALKAROUND::STATUS and ST_STUCK (correct type names) |
| CRIT-016 | Low | IMP-005-R2: Only check SH_OK (SH_HEAD_MODIFIED is dead code) |
| CRIT-017 | Medium | IMP-003-R2: OffsetArc sign convention using IsClockwise() |
| CRIT-018 | High | IMP-003-R2 + IMP-005-R2: routeLeaderHead() with BuildInitialTrace() replaces private buildInitialLine() |
| CRIT-019 | Medium | IMP-003-R2: MOUSE_TRAIL_TRACER + DIRECTION_45 m_direction + VECTOR2I m_p_start |
| CRIT-020 | Medium | IMP-004-R2: Radial projection from leaderCenter for concentric arc start/end |
| CRIT-021 | Medium | IMP-005-R2: All modes branch m_lastNode before collision checking |
| CRIT-022 | Low | Self-correction retracted: LINE::SegmentCount() exists at pns_line.h:144 |

**Key architectural decisions unchanged from iteration 1:**
1. Zero changes to PLACEMENT_ALGO interface (pre-configuration pattern)
2. New ROUTER_MODE, not sub-mode
3. Leader-follower with perpendicular offset (pcb-rnd algorithm)
4. Fat-trace WALKAROUND as default obstacle mode

**Key iteration-3 improvements:**
1. All `buildInitialLine()` calls replaced with `routeLeaderHead()` using `DIRECTION_45::BuildInitialTrace()` directly (CRIT-018)
2. Posture tracking added via `MOUSE_TRAIL_TRACER` (CRIT-019)
3. Arc geometry corrected: `IsPtOnArc()` for detection (CRIT-013), `IsClockwise()` for sign convention (CRIT-017), radial projection for concentric arcs (CRIT-020)
4. All obstacle modes branch before collision checking (CRIT-021)
5. PNS_MODE enum order preserved (CRIT-014)
6. WALKAROUND/SHOVE type names corrected (CRIT-015, CRIT-016)
