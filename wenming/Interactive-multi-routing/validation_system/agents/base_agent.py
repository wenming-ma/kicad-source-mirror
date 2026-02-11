"""Base agent class using claude-agent-sdk."""

import asyncio
import json
import time
from typing import Dict, Any, List

from claude_agent_sdk import (
    query, ClaudeAgentOptions, ClaudeSDKError,
    AssistantMessage, UserMessage, SystemMessage, ResultMessage,
    TextBlock, ThinkingBlock, ToolUseBlock, ToolResultBlock,
)
from config import AGENT_CONFIG

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
        # Store full instructions to prepend to user messages.
        # Passing long system prompts via --system-prompt CLI arg hits
        # Windows command line length limits (~8191 chars).
        self.instructions = system_prompt
        self.model = AGENT_CONFIG.get(name, {}).get("model")
        self.conversation_history: List[Dict[str, str]] = []
        self.last_result: dict | None = None

    def _log(self, msg: str):
        """Print a prefixed log line for this agent."""
        print(f"  [{self.name}] {msg}")

    async def _run_query(self, prompt: str) -> str:
        """Execute a single query call, logging all intermediate events."""
        response_text = ""
        tool_start: float | None = None
        pending_tool: str | None = None
        async for msg in query(
            prompt=prompt,
            options=ClaudeAgentOptions(
                model=self.model,
                tools={"type": "preset", "preset": "claude_code"},
                permission_mode="bypassPermissions",
                setting_sources=["user", "project", "local"],
            ),
        ):
            if isinstance(msg, AssistantMessage):
                for block in msg.content:
                    if isinstance(block, TextBlock):
                        response_text += block.text
                    elif isinstance(block, ThinkingBlock):
                        preview = block.thinking[:120].replace("\n", " ")
                        self._log(f"Thinking: {preview}...")
                    elif isinstance(block, ToolUseBlock):
                        args = json.dumps(block.input, ensure_ascii=False)
                        if len(args) > 80:
                            args = args[:80] + "..."
                        self._log(f"Tool: {block.name} {args}")
                        tool_start = time.time()
                        pending_tool = block.name
            elif isinstance(msg, UserMessage):
                elapsed = ""
                if tool_start is not None:
                    elapsed = f" {time.time() - tool_start:.1f}s"
                    tool_start = None
                # tool_use_result dict (SDK-level result)
                if msg.tool_use_result is not None:
                    r = msg.tool_use_result
                    is_err = r.get("is_error", False)
                    status = "ERROR" if is_err else "OK"
                    content = str(r.get("content", ""))
                    self._log(
                        f"Tool result ({pending_tool}): "
                        f"{status} ({len(content)} chars){elapsed}"
                    )
                # content blocks (may also carry ToolResultBlock)
                elif isinstance(msg.content, list):
                    for block in msg.content:
                        if isinstance(block, ToolResultBlock):
                            status = "ERROR" if block.is_error else "OK"
                            content = str(block.content) if block.content else ""
                            self._log(
                                f"Tool result ({pending_tool}): "
                                f"{status} ({len(content)} chars){elapsed}"
                            )
                            break
                pending_tool = None
            elif isinstance(msg, SystemMessage):
                self._log(f"System: {msg.subtype}")
            elif isinstance(msg, ResultMessage):
                duration = getattr(msg, "duration_ms", 0) / 1000
                turns = getattr(msg, "num_turns", "?")
                cost = getattr(msg, "total_cost_usd", 0)
                self._log(f"Done: {duration:.1f}s | {turns} turns | ${cost:.2f}")
                self.last_result = {
                    "duration_ms": getattr(msg, "duration_ms", None),
                    "num_turns": getattr(msg, "num_turns", None),
                    "total_cost_usd": getattr(msg, "total_cost_usd", None),
                }
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
        """Build prompt string from instructions, history, and new message."""
        parts = []

        # Prepend agent instructions (sent via stdin, no length limit)
        parts.append(f"[INSTRUCTIONS]:\n{self.instructions}")

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
