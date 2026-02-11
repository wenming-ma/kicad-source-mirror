"""Quick test script to verify the validation system setup."""

import asyncio
import json
import os
import sys
import tempfile
from pathlib import Path
from unittest.mock import AsyncMock, patch, MagicMock

# Add parent directory to path
sys.path.insert(0, str(Path(__file__).parent))

def test_imports():
    """Test that all modules can be imported."""
    print("Testing imports...")
    try:
        from agents import (
            BaseAgent,
            ResearchAgent,
            ArchitectureCriticAgent,
            AlgorithmCriticAgent,
            ImplementationCriticAgent,
            CodeVerificationAgent,
            SolutionSynthesizerAgent,
            CoordinatorAgent
        )
        from orchestrator import ValidationBattle
        from config import KICAD_REPO_PATH, DESIGN_DOC_PATH
        print("[PASS] All imports successful")
        return True
    except ImportError as e:
        print(f"[FAIL] Import error: {e}")
        return False

def test_agent_initialization():
    """Test that agents can be initialized."""
    print("\nTesting agent initialization...")
    try:
        from agents import (
            ResearchAgent,
            ArchitectureCriticAgent,
            AlgorithmCriticAgent,
            ImplementationCriticAgent,
            SolutionSynthesizerAgent,
            CoordinatorAgent
        )

        # Note: Skip CodeVerificationAgent as it requires kicad_repo_path
        agents = [
            ResearchAgent(),
            ArchitectureCriticAgent(),
            AlgorithmCriticAgent(),
            ImplementationCriticAgent(),
            SolutionSynthesizerAgent(),
            CoordinatorAgent()
        ]

        for agent in agents:
            print(f"  [OK] {agent.name} initialized")

        print("[PASS] All agents initialized successfully")
        return True
    except Exception as e:
        print(f"[FAIL] Agent initialization error: {e}")
        return False

def test_configuration():
    """Test configuration."""
    print("\nTesting configuration...")
    try:
        from config import (
            ANTHROPIC_API_KEY,
            KICAD_REPO_PATH,
            DESIGN_DOC_PATH,
            OUTPUT_DIR
        )

        print(f"  KICAD_REPO_PATH: {KICAD_REPO_PATH}")
        print(f"  DESIGN_DOC_PATH: {DESIGN_DOC_PATH}")
        print(f"  OUTPUT_DIR: {OUTPUT_DIR}")

        if not ANTHROPIC_API_KEY:
            print("  [WARN] ANTHROPIC_API_KEY not set")
        else:
            print("  [OK] ANTHROPIC_API_KEY is set")

        print("[PASS] Configuration loaded")
        return True
    except Exception as e:
        print(f"[FAIL] Configuration error: {e}")
        return False

def test_orchestrator():
    """Test orchestrator initialization."""
    print("\nTesting orchestrator...")
    try:
        from orchestrator import ValidationBattle
        from config import KICAD_REPO_PATH

        battle = ValidationBattle(KICAD_REPO_PATH)
        print(f"  [OK] ValidationBattle initialized with {len(battle.agents)} agents")

        status = battle.coordinator.get_status()
        print(f"  [OK] Coordinator status: Round {status['round']}, "
              f"{status['total_issues']} issues")

        print("[PASS] Orchestrator working")
        return True
    except Exception as e:
        print(f"[FAIL] Orchestrator error: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all tests."""
    print("=" * 60)
    print("Validation System Test Suite")
    print("=" * 60)

    results = []
    results.append(("Imports", test_imports()))
    results.append(("Agent Initialization", test_agent_initialization()))
    results.append(("Configuration", test_configuration()))
    results.append(("Orchestrator", test_orchestrator()))

    print("\n" + "=" * 60)
    print("Test Results")
    print("=" * 60)

    for name, result in results:
        status = "[PASS]" if result else "[FAIL]"
        print(f"{name}: {status}")

    all_passed = all(result for _, result in results)

    print("\n" + "=" * 60)
    if all_passed:
        print("All tests passed! System is ready.")
    else:
        print("Some tests failed. Please fix errors before running.")
    print("=" * 60)

    return 0 if all_passed else 1


# ---------------------------------------------------------------------------
# Pytest-based fault tolerance tests
# ---------------------------------------------------------------------------

class TestBaseAgentRetry:
    """Test retry and timeout behavior in BaseAgent.process()."""

    def test_retry_on_exception_returns_error_dict(self):
        """Agent returns structured error after all retries fail."""
        from agents.base_agent import BaseAgent
        import agents.base_agent as base_mod

        agent = BaseAgent("test_agent", "tester", "You are a test agent.")
        original_delay = base_mod.RETRY_BASE_DELAY
        base_mod.RETRY_BASE_DELAY = 0.01

        async def _failing_query(prompt):
            raise RuntimeError("simulated failure")

        agent._run_query = _failing_query

        try:
            result = asyncio.run(agent.process({"msg": "hello"}))
        finally:
            base_mod.RETRY_BASE_DELAY = original_delay

        assert result["error"] is True
        assert result["agent"] == "test_agent"
        assert result["error_type"] == "RuntimeError"
        assert "simulated failure" in result["error_message"]
        assert result["response"] is None
        print("[PASS] test_retry_on_exception_returns_error_dict")

    def test_success_on_second_attempt(self):
        """Agent succeeds after first attempt fails."""
        from agents.base_agent import BaseAgent
        import agents.base_agent as base_mod

        agent = BaseAgent("test_agent", "tester", "You are a test agent.")
        original_delay = base_mod.RETRY_BASE_DELAY
        base_mod.RETRY_BASE_DELAY = 0.01

        call_count = 0

        async def _flaky_query(prompt):
            nonlocal call_count
            call_count += 1
            if call_count == 1:
                raise RuntimeError("transient failure")
            return '{"status": "ok"}'

        agent._run_query = _flaky_query

        try:
            result = asyncio.run(agent.process({"msg": "hello"}))
        finally:
            base_mod.RETRY_BASE_DELAY = original_delay

        assert result.get("error") is not True
        assert result["status"] == "ok"
        assert call_count == 2
        print("[PASS] test_success_on_second_attempt")

    def test_timeout_returns_error_dict(self):
        """Agent returns error dict when query times out."""
        from agents.base_agent import BaseAgent
        import agents.base_agent as base_mod

        agent = BaseAgent("slow_agent", "tester", "You are a test agent.")

        async def _hanging_query(prompt):
            await asyncio.sleep(999)
            return '{"status": "ok"}'

        agent._run_query = _hanging_query

        original_timeout = base_mod.AGENT_TIMEOUT
        base_mod.AGENT_TIMEOUT = 0.1
        original_delay = base_mod.RETRY_BASE_DELAY
        base_mod.RETRY_BASE_DELAY = 0.01

        try:
            result = asyncio.run(agent.process({"msg": "hello"}))
            assert result["error"] is True
            assert "timed out" in result["error_message"]
        finally:
            base_mod.AGENT_TIMEOUT = original_timeout
            base_mod.RETRY_BASE_DELAY = original_delay

        print("[PASS] test_timeout_returns_error_dict")

    def test_conversation_history_cap(self):
        """Conversation history is capped at MAX_HISTORY_ENTRIES."""
        from agents.base_agent import BaseAgent, MAX_HISTORY_ENTRIES
        import agents.base_agent as base_mod

        agent = BaseAgent("test_agent", "tester", "You are a test agent.")

        async def _ok_query(prompt):
            return '{"status": "ok"}'

        agent._run_query = _ok_query

        for i in range(MAX_HISTORY_ENTRIES):
            asyncio.run(agent.process({"msg": f"message {i}"}))

        assert len(agent.conversation_history) <= MAX_HISTORY_ENTRIES
        print("[PASS] test_conversation_history_cap")


class TestCoordinatorErrorTracking:
    """Test error tracking in CoordinatorAgent."""

    def test_track_error(self):
        from agents.coordinator import CoordinatorAgent

        coord = CoordinatorAgent()
        err = {
            "agent": "arch_critic",
            "error": True,
            "error_type": "RuntimeError",
            "error_message": "boom",
        }
        coord.track_error(err)

        assert len(coord.errors) == 1
        assert coord.errors[0]["agent"] == "arch_critic"
        print("[PASS] test_track_error")

    def test_errors_in_report(self):
        from agents.coordinator import CoordinatorAgent

        coord = CoordinatorAgent()
        coord.track_error({
            "agent": "algo_critic",
            "error_type": "TimeoutError",
            "error_message": "timed out after 600s",
        })

        report = coord.generate_report()
        assert "Agent Errors" in report
        assert "algo_critic" in report
        assert "timed out" in report
        print("[PASS] test_errors_in_report")

    def test_no_error_section_when_clean(self):
        from agents.coordinator import CoordinatorAgent

        coord = CoordinatorAgent()
        report = coord.generate_report()
        assert "Agent Errors" not in report
        print("[PASS] test_no_error_section_when_clean")


class TestCheckpointResume:
    """Test checkpoint save/load in ValidationBattle."""

    def test_save_and_load_checkpoint(self):
        from orchestrator import ValidationBattle

        with tempfile.TemporaryDirectory() as tmpdir:
            battle = ValidationBattle(".", repos_dir=tmpdir)
            state = {"research_findings": [{"round": 1, "data": "test"}]}
            battle._save_checkpoint(tmpdir, "research", state)

            loaded = battle._load_checkpoint(tmpdir)
            assert loaded is not None
            assert loaded["completed_round"] == "research"
            assert loaded["state"]["research_findings"][0]["round"] == 1

        print("[PASS] test_save_and_load_checkpoint")

    def test_load_checkpoint_returns_none_when_missing(self):
        from orchestrator import ValidationBattle

        with tempfile.TemporaryDirectory() as tmpdir:
            battle = ValidationBattle(".", repos_dir=tmpdir)
            loaded = battle._load_checkpoint(tmpdir)
            assert loaded is None

        print("[PASS] test_load_checkpoint_returns_none_when_missing")


def run_fault_tolerance_tests():
    """Run all fault tolerance test classes."""
    print("\n" + "=" * 60)
    print("Fault Tolerance Tests")
    print("=" * 60)

    test_classes = [
        TestBaseAgentRetry(),
        TestCoordinatorErrorTracking(),
        TestCheckpointResume(),
    ]

    failures = 0
    for tc in test_classes:
        for method_name in sorted(dir(tc)):
            if not method_name.startswith("test_"):
                continue
            method = getattr(tc, method_name)
            try:
                method()
            except Exception as e:
                print(f"[FAIL] {tc.__class__.__name__}.{method_name}: {e}")
                import traceback
                traceback.print_exc()
                failures += 1

    if failures == 0:
        print("\nAll fault tolerance tests passed.")
    else:
        print(f"\n{failures} fault tolerance test(s) failed.")
    return failures


if __name__ == "__main__":
    exit_code = main()
    ft_failures = run_fault_tolerance_tests()
    sys.exit(exit_code or (1 if ft_failures else 0))
