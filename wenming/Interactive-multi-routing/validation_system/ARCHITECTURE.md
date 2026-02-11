# System Architecture Diagram

## System Overview

```mermaid
graph TB
    subgraph "KiCad Multi-Line Routing Validation Battle System"
        Entry[main.py<br/>Entry Point]
        Orchestrator[ValidationBattle<br/>orchestrator.py]

        Entry --> Orchestrator

        Orchestrator --> Round1[Round 1<br/>Challenges]
        Orchestrator --> Round2[Round 2<br/>Verification]
        Orchestrator --> Round3[Round 3<br/>Solutions]
        Orchestrator --> Round4[Round 4<br/>Review + Repair Loop]
        Orchestrator --> Round5[Round 5<br/>Consensus]

        Round5 --> Report[Final Report<br/>validation_report.md]
    end
```

## Agent Architecture

```mermaid
classDiagram
    class BaseAgent {
        +string name
        +string role
        +string system_prompt
        +list conversation_history
        +process(message) dict
        +parse_response(text) dict
        +reset_conversation()
    }

    class ResearchAgent {
        +search_github(query)
        +analyze_repository(url)
    }

    class ArchitectureCriticAgent {
        +challenge_architecture()
    }

    class AlgorithmCriticAgent {
        +challenge_algorithms()
    }

    class ImplementationCriticAgent {
        +challenge_implementation()
    }

    class CodeVerificationAgent {
        +string kicad_repo_path
        +read_file(filepath)
        +verify_challenge()
    }

    class SolutionSynthesizerAgent {
        +synthesize_solutions()
        +generate_alternatives()
        +revise_solution(feedback)
    }

    class CoordinatorAgent {
        +dict issues
        +int current_round
        +track_issue(issue)
        +advance_round()
        +generate_report()
    }

    BaseAgent <|-- ResearchAgent
    BaseAgent <|-- ArchitectureCriticAgent
    BaseAgent <|-- AlgorithmCriticAgent
    BaseAgent <|-- ImplementationCriticAgent
    BaseAgent <|-- CodeVerificationAgent
    BaseAgent <|-- SolutionSynthesizerAgent
    BaseAgent <|-- CoordinatorAgent
```

## Communication Flow

### Round 1: Challenges

```mermaid
sequenceDiagram
    participant Doc as Design Document
    participant Research as ResearchAgent
    participant ArchCritic as ArchitectureCritic
    participant AlgoCritic as AlgorithmCritic
    participant ImplCritic as ImplementationCritic
    participant Coord as Coordinator

    Doc->>Research: Analyze design
    Research-->>Coord: Findings

    Doc->>ArchCritic: Review architecture
    ArchCritic-->>Coord: ARCH-001, ARCH-002, ...

    Doc->>AlgoCritic: Review algorithms
    AlgoCritic-->>Coord: ALGO-001, ALGO-002, ...

    Doc->>ImplCritic: Review implementation
    ImplCritic-->>Coord: IMPL-001, IMPL-002, ...

    Coord->>Coord: Track all issues
```

### Round 2: Verification

```mermaid
sequenceDiagram
    participant Coord as Coordinator
    participant Verifier as CodeVerifier
    participant KiCad as KiCad Source Code

    Coord->>Verifier: Challenge list

    loop For each challenge
        Verifier->>KiCad: Read source files
        KiCad-->>Verifier: Code content
        Verifier->>Verifier: Analyze evidence
        Verifier-->>Coord: Valid/Invalid/Partial + Evidence
    end

    Coord->>Coord: Update issue status
```

### Round 3: Solutions

```mermaid
sequenceDiagram
    participant Coord as Coordinator
    participant Synth as SolutionSynthesizer

    Coord->>Synth: Valid issues

    loop For each valid issue
        Synth->>Synth: Generate alternatives
        Synth->>Synth: Analyze trade-offs
        Synth-->>Coord: IMP-001: Alt1, Alt2, Alt3
    end

    Coord->>Coord: Track solutions
```

### Round 4: Review + Repair Loop

```mermaid
sequenceDiagram
    participant Coord as Coordinator
    participant ArchCritic as ArchitectureCritic
    participant AlgoCritic as AlgorithmCritic
    participant ImplCritic as ImplementationCritic
    participant Synth as SolutionSynthesizer

    Note over Coord: Initial review
    Coord->>ArchCritic: Solutions
    ArchCritic-->>Coord: verdict + review
    Coord->>AlgoCritic: Solutions
    AlgoCritic-->>Coord: verdict + review
    Coord->>ImplCritic: Solutions
    ImplCritic-->>Coord: verdict + review

    Coord->>Coord: Check verdicts (approve/reject/revise)

    loop Repair Loop (max 2 iterations)
        Note over Coord: Only rejected/revised solutions
        Coord->>Synth: revise_solution + feedback
        Synth-->>Coord: Revised solution

        Coord->>ArchCritic: Re-review revised
        ArchCritic-->>Coord: verdict + review
        Coord->>AlgoCritic: Re-review revised
        AlgoCritic-->>Coord: verdict + review
        Coord->>ImplCritic: Re-review revised
        ImplCritic-->>Coord: verdict + review

        Coord->>Coord: Merge approved + new reviews
    end

    Note over Coord: Tag exhausted solutions
```

### Round 5: Consensus

```mermaid
sequenceDiagram
    participant Coord as Coordinator
    participant AllData as All Round Data
    participant Report as Final Report

    AllData->>Coord: Reviews + Issues + Solutions
    Coord->>Coord: Synthesize findings
    Coord->>Coord: Build consensus
    Coord->>Coord: Document rationale
    Coord->>Report: Generate validation_report.md
```

## Data Flow

```mermaid
flowchart LR
    Input[design_document.md]

    Input --> VB[ValidationBattle]

    VB --> R1[round1_challenges.json]
    VB --> R2[round2_verifications.json]
    VB --> R3[round3_solutions.json]
    VB --> R4[round4_reviews.json]
    VB --> R5[round5_consensus.json]

    R1 --> Report[validation_report.md]
    R2 --> Report
    R3 --> Report
    R4 --> Report
    R5 --> Report

    style Input fill:#e1f5ff
    style Report fill:#d4edda
```

## Technology Stack

```mermaid
graph TB
    subgraph "Technology Stack"
        Python[Python 3.8+]

        Python --> UV[uv Package Manager]
        Python --> Anthropic[anthropic SDK]
        Python --> Requests[requests]
        Python --> DotEnv[python-dotenv]
        Python --> GitPython[gitpython]

        Anthropic --> Opus[Claude Opus 4.6<br/>Critical Analysis]
        Anthropic --> Sonnet[Claude Sonnet 4.5<br/>Coordination]
    end

    style Python fill:#3776ab,color:#fff
    style UV fill:#f59e0b,color:#fff
    style Anthropic fill:#d97706,color:#fff
    style Opus fill:#dc2626,color:#fff
    style Sonnet fill:#2563eb,color:#fff
```

## File Organization

```mermaid
graph TB
    Root[wenming/validation_system/]

    Root --> Agents[agents/]
    Root --> Docs[Documentation]
    Root --> Config[Configuration]
    Root --> Main[Main Files]

    Agents --> BaseAgent[base_agent.py]
    Agents --> Research[research_agent.py]
    Agents --> Critics[*_critic.py x3]
    Agents --> Verifier[code_verifier.py]
    Agents --> Synth[solution_synthesizer.py]
    Agents --> Coord[coordinator.py]

    Docs --> README[README.md]
    Docs --> QuickStart[QUICKSTART.md]
    Docs --> Arch[ARCHITECTURE.md]
    Docs --> Summary[IMPLEMENTATION_SUMMARY.md]
    Docs --> Checklist[CHECKLIST.md]

    Config --> Requirements[requirements.txt]
    Config --> EnvExample[.env.example]
    Config --> ConfigPy[config.py]

    Main --> MainPy[main.py]
    Main --> Orchestrator[orchestrator.py]
    Main --> Test[test_system.py]

    style Root fill:#f59e0b
    style Agents fill:#3b82f6
    style Docs fill:#10b981
    style Config fill:#8b5cf6
    style Main fill:#ef4444
```

## Execution Timeline

```mermaid
gantt
    title Validation Execution Timeline
    dateFormat mm:ss
    axisFormat %M:%S

    section Initialization
    Start validation           :00:00, 1m

    section Round 1
    Research implementations   :01:00, 4m
    Generate challenges        :05:00, 5m

    section Round 2
    Verify challenges          :10:00, 10m

    section Round 3
    Generate solutions         :20:00, 10m

    section Round 4
    Initial review             :30:00, 10m
    Repair loop (if needed)    :40:00, 10m

    section Round 5
    Build consensus            :40:00, 5m
    Generate report            :45:00, 2m
```

## Agent Interaction Matrix

```mermaid
graph LR
    subgraph "Agent Interactions"
        R[Research] -->|Findings| C[Coordinator]
        AC[ArchCritic] -->|Challenges| C
        AlC[AlgoCritic] -->|Challenges| C
        IC[ImplCritic] -->|Challenges| C

        C -->|Issues| V[CodeVerifier]
        V -->|Evidence| C

        C -->|Valid Issues| S[Synthesizer]
        S -->|Solutions| C

        C -->|Solutions| AC
        C -->|Solutions| AlC
        C -->|Solutions| IC

        AC -->|Reviews + Verdict| C
        AlC -->|Reviews + Verdict| C
        IC -->|Reviews + Verdict| C

        C -->|Feedback from critics| S
        S -->|Revised solutions| C

        C -->|Final Data| C
    end

    style R fill:#10b981
    style AC fill:#ef4444
    style AlC fill:#f59e0b
    style IC fill:#8b5cf6
    style V fill:#3b82f6
    style S fill:#ec4899
    style C fill:#6366f1
```

## System State Machine

```mermaid
stateDiagram-v2
    [*] --> Initialized
    Initialized --> Round1: Start validation

    Round1 --> Round2: Challenges collected
    Round2 --> Round3: Challenges verified
    Round3 --> Round4: Solutions generated

    state Round4 {
        [*] --> InitialReview
        InitialReview --> CheckVerdicts
        CheckVerdicts --> RepairLoop: Rejected solutions exist
        CheckVerdicts --> Done: All approved
        RepairLoop --> ReReview: Synthesizer revises
        ReReview --> CheckVerdicts: Re-check verdicts
        RepairLoop --> Done: Max iterations reached
        Done --> [*]
    }

    Round4 --> Round5: Solutions reviewed
    Round5 --> Complete: Consensus reached

    Complete --> [*]: Report generated

    Round1: Round 1 - Challenges
    Round2: Round 2 - Verification
    Round3: Round 3 - Solutions
    Round5: Round 5 - Consensus
```

## Deployment Architecture

```mermaid
graph TB
    subgraph "User Environment"
        User[User]
        Terminal[Terminal/CLI]
    end

    subgraph "Validation System"
        Main[main.py]
        Battle[ValidationBattle]
        Agents[7 Agents]
    end

    subgraph "External Services"
        Anthropic[Anthropic API<br/>Claude Opus/Sonnet]
    end

    subgraph "Data Sources"
        KiCad[KiCad Source Code]
        Design[Design Document]
    end

    subgraph "Outputs"
        JSON[JSON Files]
        Report[Markdown Report]
    end

    User --> Terminal
    Terminal --> Main
    Main --> Battle
    Battle --> Agents
    Agents --> Anthropic
    Agents --> KiCad
    Design --> Battle
    Battle --> JSON
    Battle --> Report

    style User fill:#3b82f6,color:#fff
    style Anthropic fill:#d97706,color:#fff
    style KiCad fill:#10b981,color:#fff
    style Report fill:#d4edda
```

## Cost and Performance Metrics

```mermaid
pie title API Call Distribution
    "Research (Sonnet)" : 1
    "Critics Round 1 (Opus)" : 3
    "Verification (Opus)" : 15
    "Solutions (Opus)" : 10
    "Critics Round 4 Initial (Opus)" : 30
    "Repair: Revise (Opus)" : 10
    "Repair: Re-review (Opus)" : 30
    "Consensus (Sonnet)" : 1
```

## Error Handling Flow

```mermaid
flowchart TD
    Start[Start Validation]

    Start --> CheckEnv{Environment OK?}
    CheckEnv -->|No| EnvError[Error: Missing API Key]
    CheckEnv -->|Yes| CheckDoc{Design Doc Exists?}

    CheckDoc -->|No| DocError[Error: File Not Found]
    CheckDoc -->|Yes| RunRound1[Run Round 1]

    RunRound1 --> APICall1{API Success?}
    APICall1 -->|No| APIError[Error: API Failure]
    APICall1 -->|Yes| RunRound2[Run Round 2]

    RunRound2 --> Continue[Continue Rounds 3-5]
    Continue --> Success[Generate Report]

    EnvError --> Exit[Exit with Error]
    DocError --> Exit
    APIError --> Exit
    Success --> End[Complete]

    style EnvError fill:#ef4444,color:#fff
    style DocError fill:#ef4444,color:#fff
    style APIError fill:#ef4444,color:#fff
    style Success fill:#10b981,color:#fff
```

## Package Management with uv

```mermaid
flowchart LR
    subgraph "Package Management"
        UV[uv Package Manager]

        UV -->|Install| Deps[Dependencies]
        UV -->|Run| Python[Python Scripts]
        UV -->|Manage| Env[Virtual Environments]

        Deps --> Anthropic[anthropic]
        Deps --> Requests[requests]
        Deps --> DotEnv[python-dotenv]
        Deps --> Git[gitpython]
    end

    style UV fill:#f59e0b,color:#fff
    style Deps fill:#3b82f6,color:#fff
```

---

## Legend

- **Blue**: Core system components
- **Green**: Data sources and successful outputs
- **Red**: Critical analysis agents
- **Orange**: Research and coordination
- **Purple**: Implementation components
- **Pink**: Solution generation

## Notes

- All diagrams use Mermaid syntax for easy rendering in Markdown viewers
- The system is designed for modularity and extensibility
- Each agent operates independently but coordinates through the orchestrator
- The 5-round workflow ensures thorough validation through multiple perspectives
- Round 4 includes an iterative repair loop (max 2 iterations) where rejected solutions are revised by the synthesizer and re-reviewed by critics, minimizing LLM calls by only re-reviewing revised solutions
- Critics use a structured verdict format (approve/reject/revise) in Round 4 reviews; approval requires >= 2 approves and zero rejects
- Package management is handled by uv for fast and reliable dependency installation
