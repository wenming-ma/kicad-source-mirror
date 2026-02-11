"""Base agent class using claude-agent-sdk."""

import asyncio
import json
from typing import Dict, Any, List

from claude_agent_sdk import query, ClaudeAgentOptions, ClaudeSDKError

MAX_RETRIES = 3
RETRY_BASE_DELAY = 2  # seconds
AGENT_TIMEOUT = 600  # 10 minutes per agent call
MAX_HISTORY_ENTRIES = 20  # 10 exchanges (user + assistant each)


class BaseAgent:
    """Base class for all validation agents.

    Uses claude-agent-sdk's query() function with full Claude Code capabilities.
    Each agent has a system prompt that defines its behavior and can use all
    built-in tools (Bash, Read, Write, Edit, Glob, Grep, WebSearch, etc.).
    No turn limit — agents run until the task is complete.
    """

    def __init__(self, name: str, role: str, system_prompt: str):
        self.name = name
        self.role = role
        self.system_prompt = system_prompt
        self.conversation_history: List[Dict[str, str]] = []

    async def _run_query(self, prompt: str) -> str:
        """Execute a single query call, collecting all text from the stream."""
        response_text = ""
        async for msg in query(
            prompt=prompt,
            options=ClaudeAgentOptions(
                system_prompt=self.system_prompt,
                tools={"type": "preset", "preset": "claude_code"},
                permission_mode="bypassPermissions",
                setting_sources=["user", "project", "local"],
            ),
        ):
            if hasattr(msg, "content"):
                for block in msg.content:
                    if hasattr(block, "text"):
                        response_text += block.text
        return response_text

    async def process(self, message: Dict[str, Any]) -> Dict[str, Any]:
        """Process incoming message with retry and timeout.

        Never raises. Returns a structured error dict on total failure.
        """
        prompt = self._build_prompt(message)
        last_error = None

        for attempt in range(1, MAX_RETRIES + 1):
            try:
                response_text = await asyncio.wait_for(
                    self._run_query(prompt), timeout=AGENT_TIMEOUT
                )

                # Track conversation
                self.conversation_history.append({
                    "role": "user",
                    "content": json.dumps(message, indent=2)
                })
                self.conversation_history.append({
                    "role": "assistant",
                    "content": response_text
                })

                # Cap history to prevent unbounded growth
                if len(self.conversation_history) > MAX_HISTORY_ENTRIES:
                    self.conversation_history = (
                        self.conversation_history[-MAX_HISTORY_ENTRIES:]
                    )

                return self.parse_response(response_text)

            except asyncio.TimeoutError:
                last_error = TimeoutError(
                    f"Agent {self.name} timed out after {AGENT_TIMEOUT}s"
                )
                if attempt < MAX_RETRIES:
                    delay = RETRY_BASE_DELAY ** attempt
                    print(f"  [{self.name}] Attempt {attempt} timed out")
                    print(f"  [{self.name}] Retrying in {delay}s...")
                    await asyncio.sleep(delay)
                else:
                    print(f"  [{self.name}] All {MAX_RETRIES} attempts failed")

            except (ClaudeSDKError, Exception) as e:
                last_error = e
                if attempt < MAX_RETRIES:
                    delay = RETRY_BASE_DELAY ** attempt
                    print(f"  [{self.name}] Attempt {attempt} failed: {e}")
                    print(f"  [{self.name}] Retrying in {delay}s...")
                    await asyncio.sleep(delay)
                else:
                    print(f"  [{self.name}] All {MAX_RETRIES} attempts failed")

        # All retries exhausted -- return structured error instead of crashing
        return {
            "agent": self.name,
            "error": True,
            "error_type": type(last_error).__name__,
            "error_message": str(last_error),
            "response": None
        }

    def _build_prompt(self, message: Dict[str, Any]) -> str:
        """Build prompt string from conversation history and new message."""
        parts = []

        # Include conversation history for context
        for entry in self.conversation_history:
            role = entry["role"].upper()
            parts.append(f"[{role}]: {entry['content']}")

        # Add new message
        parts.append(f"[NEW MESSAGE]: {json.dumps(message, indent=2)}")

        return "\n\n".join(parts)

    def parse_response(self, text: str) -> Dict[str, Any]:
        """Parse agent response into structured format."""
        try:
            return json.loads(text)
        except json.JSONDecodeError:
            return self._extract_json(text)

    def _extract_json(self, text: str) -> Dict[str, Any]:
        """Try to extract JSON from text that may contain markdown fences."""
        for marker in ("```json", "```"):
            if marker in text:
                start = text.find(marker) + len(marker)
                end = text.find("```", start)
                if end != -1:
                    try:
                        return json.loads(text[start:end].strip())
                    except json.JSONDecodeError:
                        continue

        return {
            "agent": self.name,
            "role": self.role,
            "response": text
        }

    def reset_conversation(self):
        """Clear conversation history."""
        self.conversation_history = []
