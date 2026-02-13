# Validation Consensus

## Executive Summary

This consensus report synthesizes findings from the validation battle for KiCad's Interactive Multi-Line Routing feature. The validation covered **first-phase scope**: 45-degree diagonal corners (MITERED_45), miter/chamfer, rounded/fillet corners, all 4 obstacle avoidance modes (STRICT, WALKAROUND, PUSH_SHOVE, HIGHLIGHT_ONLY), uniform user-set spacing (Plan B), incremental computation, R-tree spatial indexing, single layer, N traces with 45/90 degree angle constraints.

**Overall Assessment: FEASIBLE with identified risks.**

The design document's Leader-Follower architecture is validated by real-world implementations (pcb-rnd bus routing, KiCad differential pair). KiCad's PNS router architecture is extensible enough to support MULTI_LINE_PLACER without architectural changes. However, several critical challenges require resolution before implementation.

**Validation Coverage Note:** The algo_critic, impl_critic, code_verifier, and solution_synth agents did not produce output files. Only the research_agent (5 rounds, comprehensive) and arch_critic (1 challenge identified, partial) contributed findings. This consensus is therefore primarily research-driven, with limited adversarial validation. A follow-up validation round is recommended to stress-test the proposed solutions.

---

## Research Summary

### 5 Rounds of Research Completed

**Round 1 - Broad Discovery:**
- Analyzed 6 repositories: pcb-rnd (HIGH), KiCad DP (VERY HIGH), Horizon-EDA (MEDIUM), Clipper2 (HIGH), FreeRouting (LOW), LibrePCB (LOW)
- Critical insight: True N-line routing is extremely rare in PCB EDA. Only pcb-rnd implements it via Bus Extended Object (~300 lines)
- Two implementation approaches found: perpendicular offset with angle bisector (pcb-rnd) and gateway-based with obstacle avoidance (KiCad DP)

**Round 2 - Leader-Follower Pattern Deep Dive:**
- CGAL offset algorithms analyzed: Minkowski sum convolution and straight skeleton methods
- KiCad single-line routing architecture (LINE_PLACER) deep-dived: Start() -> Move() -> FixRoute() lifecycle
- Head/tail pattern identified as ideal for incremental Leader-Follower routing
- DIRECTION_45::BuildInitialTrace() can be called N times with different anchor points
- Gateway-based approach from differential pair naturally extends to N traces

**Round 3 - Corner Spacing Maintenance:**
- Three corner strategies identified and mathematically analyzed:
  - Angle bisector (pcb-rnd): `tan(angle/2)` compensation - simple, elegant, proven
  - Miter join with limit (Clipper2): `delta / (cos_a + 1)` - production-quality, with spike prevention
  - Concentric arcs (KiCad ROUNDED modes): `r_i = r_base + i * spacing`
- Key finding: Angle bisector and miter join are mathematically equivalent
- Miter limit is critical: without it, acute angles create excessive extensions
- Clipper2's default limit of 2.0 corresponds to ~60 degree minimum angle

**Round 4 - Obstacle Avoidance for Grouped Traces:**
- Walkaround algorithm analyzed: octagonal hull approximation, CW/CCW path generation
- Two approaches for N-trace walkaround:
  - Fat trace approach: route combined-width trace, split into N traces (recommended for WALKAROUND)
  - Round-robin approach: alternate between traces (current DP method)
- Push-and-shove: ShoveMultiLines() already supports N traces but lacks inter-trace collision checking
- R-tree spatial indexing: separate R-tree per layer, O(sqrt(N)) query performance

**Round 5 - Performance and KiCad PNS Integration:**
- PLACEMENT_ALGO interface fully analyzed: clear lifecycle, extensible dispatch
- Head/tail pattern enables <16ms Move() calls (incremental computation)
- NODE branching: O(1) copy-on-write for fast rollback
- Two-level clearance caching (permanent + temporary)
- RULE_RESOLVER supports per-net-pair queries with caching
- Extension path: add PNS_MODE_ROUTE_MULTI_LINE to router dispatch, no architectural changes needed

### Key Research References

| Source | Relevance | Key Contribution |
|--------|-----------|-----------------|
| pcb-rnd bus.c | VERY HIGH | N-line perpendicular offset + angle bisector corner compensation |
| KiCad pns_diff_pair_placer | VERY HIGH | PLACEMENT_ALGO template, gateway-based routing, 3 obstacle modes |
| Clipper2 clipper.offset.cpp | HIGH | Production-quality miter join with limit, multiple join types |
| CGAL Minkowski_sum_2 | HIGH | Robust offset geometry, winding number self-intersection handling |
| KiCad pns_line_placer | VERY HIGH | Head/tail incremental computation, routeStep() architecture |
| KiCad pns_walkaround | VERY HIGH | Hull-based CW/CCW path generation, graph-based walkaround |
| KiCad pns_shove | HIGH | ShoveMultiLines(), rank-based priority system |

---

## Key Findings

### Finding 1: [VALIDATED] Leader-Follower Architecture is Sound
The design document's Leader-Follower pattern is validated by pcb-rnd's Bus Extended Object implementation. Route a leader trace using existing single-line engine, generate N-1 followers via perpendicular offset with corner compensation. The perpendicular offset formula is universal across all analyzed implementations:
```
offset_point = original_point + rotate_90(direction) * offset_distance
```

### Finding 2: [CRITICAL CHALLENGE] ARCH-001 - Start() Interface Mismatch
**Source:** Architecture Critic

The PLACEMENT_ALGO::Start() interface accepts only a single `VECTOR2I aP` and `ITEM* aStartItem`. MULTI_LINE_PLACER needs N start items across N different nets. DIFF_PAIR_PLACER works around this via `FindDpPrimitivePair()` internal discovery, but no such mechanism exists for N arbitrary nets.

**Proposed Resolution:** Pre-selection phase before Start(). The UI layer (PCB_EDIT_FRAME) collects N pad selections and passes them to MULTI_LINE_PLACER via a dedicated initialization method (e.g., `SetStartItems(std::vector<ITEM*>)`) called before Start(). Start() then uses the first item as the leader anchor and computes follower anchors from the pre-set items. This follows the pattern used by MULTI_DRAGGER which also receives multiple items before its main loop.

### Finding 3: [VALIDATED] Corner Handling - Three Viable Strategies
All three first-phase corner styles have proven implementations:
- **MITERED_45**: pcb-rnd's `tan(angle/2)` formula (simplest, ~1 line of code per vertex)
- **Miter/Chamfer**: Clipper2's miter join with limit (prevents spikes at acute angles)
- **ROUNDED_45/ROUNDED_90**: KiCad's existing arc support with concentric radii `r_i = r_base + i * spacing`

**Risk:** Miter limit is essential. Without it, near-180-degree turns cause `tan(90) -> infinity`. Recommend Clipper2's approach: fall back to square join when `cos_a <= (2 - miter_limit^2) / miter_limit^2`.

### Finding 4: [VALIDATED] All 4 Obstacle Avoidance Modes are Implementable
- **STRICT**: Trivial - check all N traces for collision, stop if any blocked
- **WALKAROUND**: Fat trace approach (route combined-width trace, split into N) is recommended over round-robin for N > 2
- **PUSH_SHOVE**: ShoveMultiLines() already exists in KiCad (confirmed in Horizon-EDA fork). Needs inter-head collision checking extension
- **HIGHLIGHT_ONLY**: Extend existing DRC violation highlighting to N traces

**Risk:** WALKAROUND fat-trace-to-individual-trace splitting is non-trivial. The split must maintain corner compensation and spacing constraints. Round-robin fallback should be available.

### Finding 5: [VALIDATED] Performance Budget is Adequate
- Head/tail pattern: only 2-5 segments recalculated per Move()
- NODE branching: O(1) copy-on-write
- R-tree: O(log N) per layer with batch query optimization
- Clearance caching: two-level cache avoids repeated DRC evaluation
- Timeout protection: existing 100ms timeout prevents frame drops
- Target: <16.67ms per Move() for 60Hz. Current single-line achieves 8-21ms. With N=4-8 traces and batch optimization, budget is achievable.

### Finding 6: [VALIDATED] Uniform Spacing (Plan B) is Sufficient for First Phase
Simple offset computation: `offset_i = (i - centerIndex) * uniformSpacing`. Adjustable via hotkey/scroll wheel. Per-net-pair spacing matrix (Plan A) deferred but infrastructure exists via RULE_RESOLVER.

### Finding 7: [GAP] Missing Adversarial Validation
The algo_critic, impl_critic, code_verifier, and solution_synth agents did not produce outputs. This means:
- No algorithmic edge-case stress testing was performed
- No implementation-level code review of proposed solutions
- No formal verification of correctness claims
- No synthesized solution document with cross-referenced evidence

---

## Recommendations

### R1: Resolve Start() Interface Before Implementation
Implement pre-selection initialization pattern. Add `SetStartItems(std::vector<ITEM*>)` to MULTI_LINE_PLACER. The UI layer collects pad selections, sorts by spatial position, and passes to placer before calling Start(). This is the cleanest approach that doesn't require modifying the PLACEMENT_ALGO base interface.

### R2: Implement Corner Handling in Priority Order
1. **First**: MITERED_45 with angle bisector (`tan(angle/2)`) - simplest, proven by pcb-rnd
2. **Second**: Add miter limit (Clipper2's formula) to prevent spikes at acute angles
3. **Third**: ROUNDED_45/ROUNDED_90 with concentric arcs - more complex but better signal integrity

### R3: Implement Obstacle Avoidance Incrementally
1. **First**: STRICT mode (trivial, good for testing)
2. **Second**: HIGHLIGHT_ONLY mode (trivial, useful for layout planning)
3. **Third**: WALKAROUND mode using fat trace approach with fallback to round-robin
4. **Fourth**: PUSH_SHOVE mode extending ShoveMultiLines() with inter-head collision checking

### R4: Performance - Batch R-Tree Queries
For N traces, compute union bounding box and query R-tree once, then filter candidates per trace. This provides ~N x speedup over individual queries. Implement early in development to establish performance baseline.

### R5: Add Miter Limit from Day One
Do not ship MITERED_45 without a miter limit. Use Clipper2's formula:
```cpp
double temp_lim = (miter_limit <= 1) ? 2.0 : 2.0 / (miter_limit * miter_limit);
if (cos_a > temp_lim - 1) DoMiter(); else DoSquare();
```
Default miter_limit = 2.0 (prevents spikes for angles > 120 degrees).

### R6: Run Missing Validation Agents
Schedule a follow-up validation round with algo_critic, impl_critic, code_verifier, and solution_synth to stress-test the proposed solutions before implementation begins.

---

## Implementation Plan

### Phase 1: Skeleton + STRICT Mode (Week 1-2)

**Goal:** Basic multi-line routing with STRICT obstacle avoidance

1. **Add PNS_MODE_ROUTE_MULTI_LINE** to `ROUTER_MODE` enum in `pns_router.h`
2. **Add dispatch case** in `ROUTER::StartRouting()` switch statement
3. **Create MULTI_LINE_PLACER class** inheriting PLACEMENT_ALGO:
   - `SetStartItems(std::vector<ITEM*>)` - pre-selection initialization
   - `Start()` - initialize leader trace + N-1 follower offsets
   - `Move()` - route leader via existing LINE_PLACER logic, generate followers via perpendicular offset
   - `FixRoute()` - commit all N traces to NODE
   - `Traces()` - return ITEM_SET with all N traces
4. **Implement perpendicular offset** for straight segments only (no corner compensation yet)
5. **Implement STRICT mode** - check all N traces for collision, stop if any blocked
6. **Implement uniform spacing** with hotkey adjustment

**Key files to create/modify:**
- NEW: `pcbnew/router/pns_multi_line_placer.cpp/h`
- MODIFY: `pcbnew/router/pns_router.h` (add enum value)
- MODIFY: `pcbnew/router/pns_router.cpp` (add dispatch case)
- MODIFY: `pcbnew/router/pns_sizes_settings.h` (add multi-line spacing settings)

### Phase 2: Corner Handling (Week 3-4)

**Goal:** Correct spacing at all corner types

1. **Implement MITERED_45 corner compensation** using `tan(angle/2)` formula from pcb-rnd
2. **Add miter limit** using Clipper2's formula to prevent spikes
3. **Implement ROUNDED_45/ROUNDED_90** with concentric arcs (`r_i = r_base + i * spacing`)
4. **Validate minimum arc radius** >= 3x trace width

**Algorithm reference:**
```
// For each vertex at angle theta between adjacent segments:
compensation = offset_distance * tan(theta / 2.0)
// Apply along segment direction at each vertex
```

### Phase 3: WALKAROUND + HIGHLIGHT_ONLY (Week 5-6)

**Goal:** Non-trivial obstacle avoidance

1. **Implement HIGHLIGHT_ONLY** - route all traces ignoring obstacles, mark collisions via DRC highlighting
2. **Implement WALKAROUND** using fat trace approach:
   - Compute bundle width = sum(trace_widths) + (N-1) * spacing
   - Route center trace with bundle width through walkaround engine
   - Split result into N individual traces with corner compensation
   - Fallback to round-robin if fat trace splitting fails
3. **Implement batch R-tree queries** for performance

### Phase 4: PUSH_SHOVE + Polish (Week 7-8)

**Goal:** Complete obstacle avoidance + production quality

1. **Extend ShoveMultiLines()** with inter-head collision checking
2. **Add group constraint validation** during shove
3. **Implement oscillation prevention** for grouped traces
4. **Performance profiling** and optimization for N=8 traces at 60Hz
5. **UI integration**: menu items, toolbar buttons, status bar feedback
6. **Undo/Redo support** via FixedTail stages

---

## Risks

### Risk 1: [HIGH] WALKAROUND Fat Trace Splitting Complexity
Splitting a fat walkaround trace back into N individual traces while maintaining corner compensation and spacing is non-trivial. The split must handle cases where the walkaround path has different geometry than the original straight path.

**Mitigation:** Implement round-robin walkaround as fallback. Start with fat trace for simple cases, fall back to round-robin when splitting fails.

### Risk 2: [MEDIUM] Performance Degradation at N > 8
While the performance budget analysis suggests feasibility for N=4-8, real-world scenarios with complex obstacle layouts may exceed the 16.67ms budget. Push-and-shove with N traces creates O(N^2) inter-trace collision checks.

**Mitigation:** Implement timeout protection (already exists in KiCad at 100ms). Gracefully degrade to STRICT mode if timeout exceeded. Batch R-tree queries provide ~N x speedup.

### Risk 3: [MEDIUM] Corner Self-Intersection at Large N
With many traces (N > 8) and tight corners, outer traces may self-intersect due to excessive miter extension. The miter limit prevents spikes but may create visible discontinuities.

**Mitigation:** Miter limit with square join fallback. For ROUNDED modes, enforce minimum radius >= 3x trace width. Warn user when spacing * N exceeds safe corner radius.

### Risk 4: [LOW] Start() Interface Compatibility
Adding SetStartItems() before Start() creates an implicit ordering dependency. If Start() is called without SetStartItems(), the placer must handle this gracefully.

**Mitigation:** Assert in Start() that start items have been set. Return false with error message if not. Document the required call sequence.

### Risk 5: [HIGH] Incomplete Validation
Missing algo_critic, impl_critic, code_verifier, and solution_synth outputs mean the proposed solutions have not been adversarially tested. Edge cases in corner compensation, obstacle avoidance, and performance may be undiscovered.

**Mitigation:** Run follow-up validation round before implementation. Add comprehensive unit tests for offset geometry, corner compensation, and collision detection.

---

## Deferred Features (NOT in First Phase)

The following are explicitly out of scope for first-phase implementation:
- Corner styles: 90-degree (MITERED_90), any-angle
- Spacing: per-net-pair DRC spacing matrix (Plan A)
- Performance: deferred precise computation, parallel computation (N > 8)
- Layer switching, length matching, signal integrity, differential pair coupling

---

## Next Steps

1. **Immediate:** Run missing validation agents (algo_critic, impl_critic, code_verifier, solution_synth) to stress-test proposed solutions
2. **Week 1:** Begin Phase 1 implementation - skeleton MULTI_LINE_PLACER with STRICT mode
3. **Week 2:** Validate perpendicular offset and uniform spacing with manual testing
4. **Week 3-4:** Implement corner handling with unit tests for angle bisector and miter limit
5. **Week 5-8:** Progressive obstacle avoidance implementation (HIGHLIGHT_ONLY -> WALKAROUND -> PUSH_SHOVE)
6. **Ongoing:** Performance profiling at each phase to ensure 60Hz target is met
