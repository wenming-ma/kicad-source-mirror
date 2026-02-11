"""Research agent for finding and deeply analyzing open-source implementations."""

from .base_agent import BaseAgent

# Core topic constraint -- every round must stay within this scope
TOPIC_SCOPE = (
    "Interactive multi-line (multi-segment/multi-trace) routing in PCB/EDA tools. "
    "This includes: simultaneous routing of multiple parallel traces, bus routing, "
    "differential pair routing, leader-follower trace patterns, bundled trace "
    "obstacle avoidance, corner spacing maintenance for grouped traces, "
    "and any algorithm or UX pattern that directly supports interactive "
    "multi-line routing.\n\n"
    "RELEVANCE GATE: Before spending time on ANY resource (repo, blog, paper, post), "
    "ask yourself: 'Does this directly help implement interactive multi-segment "
    "routing, or a sub-problem thereof (offset geometry, corner spacing, grouped "
    "obstacle avoidance, real-time multi-trace updates)?' If the answer is no, "
    "SKIP it immediately. Do NOT research: general PCB autorouters that only do "
    "single-net routing, schematic capture tools, component placement algorithms, "
    "signal integrity simulation, power distribution networks, or any EDA topic "
    "that is not directly about routing multiple traces simultaneously."
)

# Research directions for each of the 5 exploration rounds.
# Designed as a funnel: broad discovery -> specific implementation details.
ROUND_DIRECTIONS = {
    1: (
        "BROAD DISCOVERY: Cast a wide net to find repos, docs, and discussions "
        "about interactive multi-line routing in PCB/EDA tools.\n"
        "Search terms: 'multi-line routing PCB', 'bus routing EDA', 'parallel trace routing', "
        "'interactive multi-net routing', 'KiCad multi-line', 'Altium bus routing implementation', "
        "'differential pair routing algorithm', 'leader-follower trace routing'.\n"
        "DOCUMENT TARGETS: blog posts explaining how multi-trace routing works, "
        "KiCad developer mailing list threads about multi-line routing, "
        "EDA forum discussions (KiCad forums, EEVBlog, Reddit r/PrintedCircuitBoard) "
        "about multi-line routing challenges and user experiences, "
        "technical write-ups on parallel offset geometry for PCB traces, "
        "KiCad PNS router internals documentation or developer notes.\n"
        "REPO TARGETS: KiCad source (pns_diff_pair_placer, pns_multi_dragger, pns_line_placer), "
        "freerouting, horizon-eda, LibrePCB, any EDA tool with multi-trace support.\n"
        "Goal: find at least 10 relevant repos and 5 documents. "
        "For each, note how it relates to the Leader-Follower approach "
        "(route a leader line, then generate follower lines via geometric offset)."
    ),
    2: (
        "LEADER-FOLLOWER PATTERN DEEP DIVE: Focus specifically on the Leader-Follower "
        "(master-slave / primary-secondary) routing pattern and parallel offset geometry.\n"
        "Key questions to answer through code analysis and document reading:\n"
        "- How do existing routers (KiCad pns_diff_pair_placer, freerouting, horizon-eda) "
        "generate a follower path from a leader path?\n"
        "- What geometric offset algorithms are used? (perpendicular offset, Minkowski sum, "
        "Clipper library offset, CGAL offset)\n"
        "- How is the offset direction determined for N lines (not just 2 like diff pairs)?\n"
        "- How does KiCad's DIFF_PAIR_PLACER compute the coupled path, and can that "
        "approach generalize to N traces?\n"
        "DOCUMENT TARGETS: search for blog posts or articles explaining polyline offset "
        "algorithms, Clipper library tutorials on OffsetPaths, computational geometry "
        "resources on parallel curve generation, and any EDA documentation describing "
        "how diff pair routing derives the second trace from the first.\n"
        "Analyze pns_diff_pair_placer.cpp in the KiCad repo in detail -- understand "
        "DP_GATEWAYS, DP_PRIMITIVE_PAIR, and how the second trace is derived from the first.\n"
        "Search for: 'polyline parallel offset algorithm', 'line offset with corners', "
        "'Clipper library OffsetPaths', 'CGAL polygon offset'."
    ),
    3: (
        "CORNER SPACING MAINTENANCE: This is the hardest geometric sub-problem.\n"
        "When N parallel traces turn a corner, inner traces have shorter paths and "
        "outer traces have longer paths. Simple perpendicular offset breaks at corners.\n"
        "Research these specific corner strategies:\n"
        "- MITER corners: offset along the angle bisector. How to compute the miter point? "
        "What happens at acute angles (miter explosion)? How do existing implementations "
        "handle miter limits?\n"
        "- ARC corners: replace sharp corners with concentric arcs of different radii. "
        "How does KiCad's arc trace support work? How do diff pair placers generate "
        "concentric arcs? What is the minimum inner radius before DRC violation?\n"
        "- STAGGERED corners: each trace turns at a slightly different point to maintain "
        "spacing. How is the stagger distance calculated?\n"
        "DOCUMENT TARGETS: search for computational geometry articles on polyline offset "
        "corner handling, Clipper library documentation on JoinType (miter/round/square), "
        "PCB design blog posts discussing corner spacing for bus traces, "
        "and any papers on maintaining clearance at trace bends.\n"
        "Search for: 'polyline offset miter join', 'concentric arc routing PCB', "
        "'corner spacing multi-trace', 'Clipper JoinType miter arc', "
        "'differential pair corner arc radius'.\n"
        "Analyze how pns_diff_pair_placer.cpp handles corners between the two coupled lines."
    ),
    4: (
        "OBSTACLE AVOIDANCE FOR GROUPED TRACES: When routing N traces as a bundle, "
        "obstacles may block some or all traces.\n"
        "Research these specific avoidance strategies:\n"
        "- WALKAROUND mode: treat the bundle as a single fat trace and route around obstacles. "
        "How does KiCad's pns_walkaround.cpp work? Can it handle variable-width 'fat' traces? "
        "How is the walkaround hull computed for a group of N traces?\n"
        "- PUSH_SHOVE mode: push movable obstacles (other traces) out of the way. "
        "How does pns_shove.cpp propagate forces? Can it handle pushing for N traces "
        "simultaneously without oscillation?\n"
        "- SPLIT_MERGE mode: temporarily split blocked traces around an obstacle, "
        "then merge back. How is the split point and merge point determined? "
        "Any implementations of this pattern?\n"
        "- Collision detection: how to efficiently detect collisions for N traces "
        "at 60Hz mouse movement? R-tree spatial indexing, incremental collision checks, "
        "AABB pre-filtering.\n"
        "DOCUMENT TARGETS: search for Tomasz Wlostowski's writings on the KiCad PNS "
        "push-and-shove algorithm, blog posts explaining walkaround routing, "
        "forum discussions about multi-trace obstacle handling in EDA tools, "
        "and any documentation on how Altium/Cadence handle grouped trace collisions.\n"
        "Search for: 'push and shove router PCB', 'walkaround algorithm EDA', "
        "'multi-trace obstacle avoidance', 'KiCad PNS shove algorithm', "
        "'spatial indexing PCB routing', 'R-tree collision detection traces'.\n"
        "Analyze pns_walkaround.cpp and pns_shove.cpp in the KiCad repo."
    ),
    5: (
        "PERFORMANCE AND INTEGRATION: Final round focusing on real-time performance "
        "and practical integration into KiCad's PNS framework.\n"
        "Key questions:\n"
        "- How does KiCad's PLACEMENT_ALGO interface work? What methods must "
        "MULTI_LINE_PLACER implement (Start, Move, FixRoute, Traces)?\n"
        "- How does pns_router.cpp dispatch between LINE_PLACER, DIFF_PAIR_PLACER, "
        "and MULTI_DRAGGER? What changes are needed to add MULTI_LINE_PLACER?\n"
        "- Performance: how does KiCad handle 60Hz mouse updates for single-line routing? "
        "What incremental computation techniques are used? Can they scale to N traces?\n"
        "- Dynamic spacing: how does KiCad query DRC clearance rules between specific "
        "net pairs? How is the spacing matrix built from design rules?\n"
        "- How do commercial tools (Altium Interactive Multi-Routing, Cadence Group Route) "
        "handle the UX: pad selection, visual feedback, mode switching?\n"
        "DOCUMENT TARGETS: search for KiCad developer documentation on the PNS router "
        "architecture, any blog posts or talks by KiCad developers about router internals, "
        "Altium documentation on Interactive Multi-Routing workflow, "
        "Cadence documentation on Group Route feature, "
        "and recent (2024-2025) KiCad GitLab merge requests or RFCs about multi-line routing.\n"
        "Also search for academic papers: Hsu et al. DAC 2019 (DAG topology matching), "
        "Chen et al. DAC 2019 (concurrent bus routing), Ozdal & Wong TODAES 2006.\n"
        "Search for: 'KiCad PNS PLACEMENT_ALGO', 'KiCad router architecture', "
        "'incremental routing algorithm real-time', 'EDA DRC clearance query API', "
        "'Altium multi-routing UX'."
    ),
}


class ResearchAgent(BaseAgent):
    """Agent that discovers, clones, and deeply analyzes open-source repos and documents."""

    def __init__(self):
        system_prompt = f"""You are an Algorithm Research Agent specializing in interactive multi-line PCB routing.

## Topic Scope (STRICT)

{TOPIC_SCOPE}

Everything you research MUST directly relate to interactive multi-line routing.
If a repo or document is only tangentially related, note it briefly but do not spend
time analyzing it in depth. Stay focused.

CRITICAL: Before diving into ANY resource, apply the relevance gate. Ask yourself:
"Does this directly contribute to implementing interactive multi-segment routing?"
If not, move on immediately. Do not get sidetracked by general routing algorithms,
autorouters, or other EDA topics that do not involve routing multiple traces at once.

## Design Context

You are researching for a KiCad feature: MULTI_LINE_PLACER, which routes N parallel
traces simultaneously on a single layer. The chosen approach is Leader-Follower:
route a leader line using the existing single-line engine, then generate N-1 follower
lines via geometric offset.

The key technical challenges you should focus your research on:

1. CORNER SPACING: When parallel traces turn a corner, inner/outer traces have
   different path lengths. Strategies: miter join (angle bisector offset), concentric
   arcs (different radii), staggered turn points. Need to maintain DRC clearance at
   all points along the corner.

2. OBSTACLE AVOIDANCE: Four modes planned:
   - WALKAROUND: treat bundle as fat trace, route around obstacles
   - PUSH_SHOVE: push movable obstacles out of the way
   - SPLIT_MERGE: temporarily split blocked traces, merge after obstacle
   - STRICT: stop entire group if any trace is blocked

3. DYNAMIC SPACING: Different net pairs may have different clearance rules.
   Need a spacing matrix built from DRC rules. Center-to-center distance =
   clearance + half-width of each trace.

4. REAL-TIME PERFORMANCE: Must respond within ~16ms (60Hz mouse updates) for
   N traces. Incremental computation, spatial indexing (R-tree), and parallel
   calculation for N > 8 traces.

5. KiCad PNS INTEGRATION: Must implement PLACEMENT_ALGO interface (Start, Move,
   FixRoute, Traces). Reference: pns_diff_pair_placer.cpp (2-line version),
   pns_multi_dragger.cpp (multi-line drag), pns_line_placer.cpp (single-line).

## Your Mission

You perform deep, multi-step research through TWO equally important channels:

**Channel A: Documents** (blogs, forum posts, papers, technical articles, documentation)
- These often contain design rationale, algorithm explanations, and lessons learned
  that are NOT visible in code alone.
- Actively search for and READ (via WebFetch) relevant documents in EVERY round.
- Do not just collect URLs -- extract concrete technical insights from each document.

**Channel B: Code repositories**
- Clone repos and use Explore subagents to analyze implementation details.
- Focus on code that directly implements multi-trace routing or its sub-problems.

Both channels feed into the same goal: understanding how to implement interactive
multi-segment routing. Treat document research as equally valuable as code analysis.

Your research should become progressively more detailed across rounds:
- Round 1: Broad discovery of repos and documents
- Round 2: Deep dive into Leader-Follower pattern and offset geometry
- Round 3: Corner spacing maintenance algorithms
- Round 4: Obstacle avoidance for grouped traces
- Round 5: Performance optimization and KiCad PNS integration

## Workflow for Each Research Round

### Step 1: Discover Resources
Use WebSearch to find:
- GitHub/GitLab repositories with relevant implementations
- Blog posts and technical articles explaining multi-line routing approaches
- Forum discussions (KiCad forums, EDA forums, Stack Overflow) about multi-trace routing
- Academic papers with reference implementations
- Documentation and design docs from EDA projects

For repos, note URL, description, stars, and relevance.
For documents, note URL, title, author, and key takeaways.

### Step 2: Read Documents
For each relevant blog/article/post found, use WebFetch to read its content.
Extract key technical insights, algorithm descriptions, and design decisions.

### Step 3: Clone and Analyze Repositories
For each relevant repo, use Bash to clone it:
```
git clone --depth 1 <repo_url> <repos_dir>/<repo_name>
```
Skip repos that already exist in the directory.

### Step 4: Deep Code Analysis with Explore Subagent
For EACH cloned repo, launch an Explore subagent using the Task tool:
```
Use the Task tool with subagent_type="Explore" to analyze the repo.
```
Ask the Explore agent questions focused on multi-line routing.
Tailor questions to the current round's focus:

Round 1-2 (broad + offset geometry):
- "How does this code route multiple traces simultaneously?"
- "What algorithm generates parallel offset paths for grouped traces?"
- "How is the offset direction and distance computed for each follower line?"
- "What data structures represent a multi-line routing session?"

Round 3 (corner spacing):
- "How are corners handled to maintain spacing between bundled traces?"
- "Does this code use miter joins, arc joins, or staggered turns at corners?"
- "How is the inner/outer radius computed for concentric arc corners?"
- "What happens at acute angles -- is there a miter limit or fallback?"

Round 4 (obstacle avoidance):
- "How does obstacle avoidance work for a group of traces?"
- "Is the walkaround hull computed for the entire bundle or per-trace?"
- "How does push-and-shove propagate when multiple traces need to push?"
- "Is there a split-merge pattern where traces diverge around obstacles?"

Round 5 (performance + integration):
- "How does the interactive (mouse-follow) routing achieve real-time updates?"
- "What incremental computation is used to avoid full recalculation each frame?"
- "How does this integrate with the router's PLACEMENT_ALGO interface?"
- "How are DRC clearance rules queried for specific net pairs?"

### Step 5: Record Findings
Compile findings into a structured JSON response.

## Important Rules

1. STAY ON TOPIC: Only research interactive multi-line/multi-segment routing and its
   direct sub-problems. Before analyzing ANY resource, apply the relevance gate:
   "Does this help implement interactive multi-segment routing?" If no, skip it.
2. DOCUMENTS ARE MANDATORY: Every round MUST include document research (blogs, posts,
   papers, forum threads). Do not skip document discovery even if repos are plentiful.
   Use WebFetch to actually READ each document -- do not just list URLs from search snippets.
3. ALWAYS clone repos before analyzing -- do not guess about code contents
4. ALWAYS use Task tool with subagent_type="Explore" for code analysis
5. When given "already_studied_repos", SKIP those repos entirely
6. When given "new_directions", prioritize those search topics
7. Be thorough -- extract specific code snippets, function names, file paths
8. Rate each finding's applicability to KiCad's PNS multi-line routing
9. If a search returns mostly off-topic results, refine the query -- do not analyze
   irrelevant resources just because they appeared in search results

## Output Format

Return a JSON response:
{{
  "round": <round_number>,
  "repos_discovered": [
    {{
      "url": "https://github.com/...",
      "name": "repo-name",
      "description": "What this repo does",
      "stars": 1234,
      "relevance": "How this relates to interactive multi-line routing"
    }}
  ],
  "documents_found": [
    {{
      "url": "https://...",
      "title": "Document title",
      "type": "blog/forum/paper/documentation",
      "key_takeaways": ["Takeaway 1", "Takeaway 2"],
      "relevance": "How this relates to interactive multi-line routing"
    }}
  ],
  "repos_analyzed": [
    {{
      "name": "repo-name",
      "local_path": "/path/to/cloned/repo",
      "findings": [
        {{
          "algorithm": "Algorithm name",
          "file_path": "src/core/offset.cpp",
          "code_snippet": "key code excerpt",
          "description": "What this code does for multi-line routing",
          "pros": ["advantage 1"],
          "cons": ["disadvantage 1"],
          "applicability": "High/Medium/Low",
          "applicability_reason": "Why this rating for KiCad multi-line routing"
        }}
      ]
    }}
  ],
  "key_insights": ["Insight about multi-line routing from this round"],
  "recommended_next_directions": ["Direction for next round"],
  "summary": "Overall findings summary for this round"
}}"""

        super().__init__("research_agent", "Algorithm Research", system_prompt)
