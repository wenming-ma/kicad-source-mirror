"""Base agent class using claude-agent-sdk."""

import asyncio
import json
from typing import Dict, Any, List

from claude_agent_sdk import (
    ClaudeSDKClient, ClaudeAgentOptions, ClaudeSDKError,
    AssistantMessage, SystemMessage, ResultMessage,
    TextBlock, ThinkingBlock, ToolUseBlock,
)
from claude_agent_sdk.types import HookMatcher
from config import AGENT_CONFIG

MAX_RETRIES = 3
RETRY_BASE_DELAY = 2  # seconds
MAX_HISTORY_ENTRIES = 20  # 10 exchanges (user + assistant each)
RESPONSE_TIMEOUT = 600  # seconds (10 min); interrupt if no new message arrives within this window
DISCONNECT_TIMEOUT = 30  # seconds; max wait for subprocess cleanup
DEFAULT_MAX_TURNS = 50  # safety cap on agent turns per invocation
EMPTY_TOOL_THRESHOLD = 3       # consecutive empty tool calls before interrupt
MAX_RECOVERY_ATTEMPTS = 2      # max interrupt+re-prompt cycles per process() call
THINKING_BUDGET_TOKENS = 10000  # extended thinking budget
DRAIN_TIMEOUT = 30             # seconds to drain messages after interrupt
DEGRADED_TIMEOUT = 60          # seconds; shorter timeout after empty Write/Edit detected


class BaseAgent:
    """Base class for all validation agents.

    Uses ClaudeSDKClient for stateful conversations.  The SDK manages
    its own execution (timeouts, tool calls, context).  We only:
      1. Stream messages for logging + text extraction.
      2. Catch SDK failures and retry on the same client so
         conversation context is never lost.
    """

    def __init__(self, name: str, role: str, system_prompt: str):
        self.name = name
        self.role = role
        self.instructions = system_prompt
        self.model = AGENT_CONFIG.get(name, {}).get("model")
        self.conversation_history: List[Dict[str, str]] = []
        self.last_result: dict | None = None
        self._compaction_count: int = 0
        self._consecutive_empty_tools: int = 0
        self._recovery_attempts: int = 0
        self._task_context: str = ""

    def _log(self, msg: str):
        """Print a prefixed log line for this agent."""
        print(f"  [{self.name}] {msg}")

    def _process_message(self, msg):
        """Process a single streamed message, logging and extracting text."""
        text = ""
        if isinstance(msg, AssistantMessage):
            for block in msg.content:
                if isinstance(block, TextBlock):
                    text += block.text
                elif isinstance(block, ThinkingBlock):
                    preview = block.thinking[:120].replace("\n", " ")
                    self._log(f"Thinking: {preview}...")
                elif isinstance(block, ToolUseBlock):
                    args = json.dumps(block.input, ensure_ascii=False)
                    is_empty = (
                        not block.input or block.input == {}
                        or (block.name in ("Edit", "Write", "MultiEdit")
                            and all(v in (None, "", {}, [])
                                    for v in block.input.values()))
                    )
                    if is_empty and block.name in ("Edit", "Write", "MultiEdit"):
                        self._consecutive_empty_tools += 1
                        self._log(
                            f"EMPTY-Tool: {block.name} {args} "
                            f"(#{self._consecutive_empty_tools})"
                        )
                    else:
                        self._consecutive_empty_tools = 0
                        self._log(f"Tool: {block.name} {args}")
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
        return text

    async def _drain_messages(self, client: ClaudeSDKClient):
        """Drain remaining messages after an interrupt."""
        try:
            async def _drain():
                async for m in client.receive_response():
                    self._process_message(m)
            await asyncio.wait_for(_drain(), timeout=DRAIN_TIMEOUT)
        except asyncio.TimeoutError:
            self._log("Drain timed out, proceeding anyway")

    async def _receive_until_done_or_degraded(
        self, client: ClaudeSDKClient
    ) -> bool:
        """Inner receive loop.

        Returns True if the stream completed normally, False if context
        degradation was detected (consecutive empty tool calls).

        Uses a shorter timeout after empty Write/Edit calls are detected,
        so a stuck model is caught in ~60s instead of waiting the full 600s.
        """
        response_iter = client.receive_response().__aiter__()
        while True:
            timeout = (
                DEGRADED_TIMEOUT
                if self._consecutive_empty_tools > 0
                else RESPONSE_TIMEOUT
            )
            try:
                msg = await asyncio.wait_for(
                    response_iter.__anext__(), timeout=timeout
                )
            except StopAsyncIteration:
                return True  # normal completion
            except asyncio.TimeoutError:
                self._log(
                    f"No message for {timeout}s"
                    f"{' (degraded mode)' if self._consecutive_empty_tools > 0 else ''}"
                    f", sending interrupt..."
                )
                await client.interrupt()
                await self._drain_messages(client)
                raise

            self._partial_response += self._process_message(msg)

            if (self._consecutive_empty_tools >= EMPTY_TOOL_THRESHOLD
                    and self._recovery_attempts < MAX_RECOVERY_ATTEMPTS):
                return False  # degradation detected

    async def _receive_response(self, client: ClaudeSDKClient) -> str:
        """Consume the message stream with automatic degradation recovery.

        Delegates to _receive_until_done_or_degraded for the inner loop.
        When degradation is detected (consecutive empty Edit/Write calls),
        interrupts the agent, re-prompts with recovery context, and retries.
        """
        self._partial_response = ""
        while True:
            completed = await self._receive_until_done_or_degraded(client)
            if completed:
                return self._partial_response

            if self._recovery_attempts >= MAX_RECOVERY_ATTEMPTS:
                self._log(
                    f"Max recovery attempts ({MAX_RECOVERY_ATTEMPTS}) exhausted"
                )
                return self._partial_response

            self._recovery_attempts += 1
            self._log(
                f"Recovery {self._recovery_attempts}/{MAX_RECOVERY_ATTEMPTS}: "
                f"interrupt + re-prompt"
            )
            await client.interrupt()
            await self._drain_messages(client)
            self._consecutive_empty_tools = 0

            await client.query(self._build_recovery_prompt())
            # loop back to _receive_until_done_or_degraded with fresh iter

    async def _on_pre_compact(self, input_data, tool_use_id, context):
        """PreCompact hook: log the event and inject recovery context."""
        trigger = input_data.get("trigger", "unknown")
        self._compaction_count += 1
        self._log(f"PreCompact #{self._compaction_count} (trigger={trigger})")
        return {
            "hookSpecificOutput": {
                "hookEventName": "PreCompact",
                "additionalContext": (
                    f"CRITICAL CONTEXT FOR POST-COMPACTION CONTINUITY:\n"
                    f"Agent: {self.name} ({self.role})\n"
                    f"{self._task_context}\n"
                    f"If you lose track, READ your output file to see progress, "
                    f"then continue. Do NOT issue Edit/Write with empty parameters."
                ),
            }
        }

    def _build_recovery_prompt(self) -> str:
        """Build a compact re-orientation prompt after degradation."""
        return (
            f"RECOVERY: Your context was compacted and you lost track of your "
            f"edits. You were issuing empty Edit/Write calls.\n\n"
            f"{self._task_context}\n\n"
            f"INSTRUCTIONS:\n"
            f"1. Read your output file to see what you have written so far.\n"
            f"2. Determine what remains to be done.\n"
            f"3. Continue writing to your output file.\n"
            f"4. Do NOT issue Edit or Write with empty parameters."
        )

    def _build_task_context(self, message: Dict[str, Any]) -> str:
        """Extract key task identity from the message for recovery prompts."""
        parts = [f"Agent: {self.name} ({self.role})"]
        parts.append(f"Task type: {message.get('type', 'unknown')}")
        for key in (
            "critic_md", "solution_synth_md",
            "research_agent_md", "coordinator_md",
        ):
            if key in message:
                parts.append(f"Output file ({key}): {message[key]}")
        if "battle_iteration" in message:
            parts.append(f"Battle iteration: {message['battle_iteration']}")
        return "\n".join(parts)

    async def _safe_disconnect(self, client: ClaudeSDKClient):
        """Disconnect with a timeout to prevent hanging on subprocess.wait()."""
        try:
            await asyncio.wait_for(client.disconnect(), timeout=DISCONNECT_TIMEOUT)
        except asyncio.TimeoutError:
            self._log(
                f"disconnect() timed out after {DISCONNECT_TIMEOUT}s, "
                "subprocess may be orphaned"
            )
        except Exception as e:
            self._log(f"disconnect() error (ignored): {e}")

    async def process(self, message: Dict[str, Any]) -> Dict[str, Any]:
        """Run the agent.  Retry on SDK failure, preserving context.

        Uses manual connect/disconnect instead of ``async with`` to wrap
        disconnect() in a timeout — the SDK's subprocess.wait() can hang
        indefinitely if the child process ignores SIGTERM.
        """
        prompt = self._build_prompt(message)
        last_error = None
        self._partial_response = ""

        # Reset per-invocation recovery state
        self._compaction_count = 0
        self._consecutive_empty_tools = 0
        self._recovery_attempts = 0
        self._task_context = self._build_task_context(message)

        options = ClaudeAgentOptions(
            model=self.model,
            system_prompt={"type": "preset", "preset": "claude_code"},
            tools={"type": "preset", "preset": "claude_code"},
            permission_mode="bypassPermissions",
            setting_sources=["user", "project", "local"],
            max_turns=DEFAULT_MAX_TURNS,
            thinking={
                "type": "enabled",
                "budget_tokens": THINKING_BUDGET_TOKENS,
            },
            hooks={
                "PreCompact": [
                    HookMatcher(
                        matcher=None,
                        hooks=[self._on_pre_compact],
                    ),
                ],
            },
        )

        client = ClaudeSDKClient(options=options)
        try:
            await client.connect()

            for attempt in range(1, MAX_RETRIES + 1):
                try:
                    if attempt == 1:
                        await client.query(prompt)
                    else:
                        await client.query(
                            "Your previous response was interrupted. "
                            "The conversation above contains all your "
                            "work so far. Continue from where you left "
                            "off and complete the task. You MUST ensure "
                            "your output file is written before finishing."
                        )

                    response_text = await self._receive_response(client)

                    self.conversation_history.append({
                        "role": "user",
                        "content": json.dumps(message, indent=2),
                    })
                    self.conversation_history.append({
                        "role": "assistant",
                        "content": response_text,
                    })
                    if len(self.conversation_history) > MAX_HISTORY_ENTRIES:
                        self.conversation_history = (
                            self.conversation_history[-MAX_HISTORY_ENTRIES:]
                        )
                    return self.parse_response(response_text)

                except (ClaudeSDKError, Exception) as e:
                    last_error = e
                    if attempt < MAX_RETRIES:
                        delay = RETRY_BASE_DELAY ** attempt
                        self._log(f"Attempt {attempt} failed: {e}")
                        self._log(f"Retrying in {delay}s...")
                        await asyncio.sleep(delay)
                    else:
                        self._log(
                            f"All {MAX_RETRIES} attempts failed"
                        )

        except (ClaudeSDKError, Exception) as e:
            last_error = e
            self._log(f"Client connection failed: {e}")

        finally:
            await self._safe_disconnect(client)

        # All retries exhausted -- return partial response if available
        partial = self._partial_response
        if partial:
            self._log(f"Returning partial response ({len(partial)} chars)")
            return self.parse_response(partial)

        return {
            "agent": self.name,
            "error": True,
            "error_type": type(last_error).__name__ if last_error else "Unknown",
            "error_message": str(last_error) if last_error else "All attempts failed",
            "response": None,
        }

    def _build_prompt(self, message: Dict[str, Any]) -> str:
        """Build prompt string from instructions, history, and new message."""
        parts = []

        # Prepend agent instructions (sent via stdin, no length limit)
        parts.append(f"[INSTRUCTIONS]:\n{self.instructions}")

        # AUTONOMY CONSTRAINT -- agents run unattended in a pipeline
        parts.append(
            "[AUTONOMY CONSTRAINT]\n"
            "You are running as an autonomous agent inside an automated pipeline. "
            "There is NO human operator watching your output.\n"
            "- NEVER use AskUserQuestion, TodoWrite, TaskCreate, TaskUpdate, "
            "or any interactive/UI tool.\n"
            "- NEVER pause to ask for confirmation or clarification.\n"
            "- If you are unsure about something, make your best judgment and "
            "document the assumption in your output file.\n"
            "- Your ONLY output channel is writing to the markdown file specified "
            "in the message. All findings, analysis, and conclusions MUST be "
            "written there."
        )

        # Include conversation history for context
        for entry in self.conversation_history:
            role = entry["role"].upper()
            parts.append(f"[{role}]: {entry['content']}")

        # Add new message
        parts.append(f"[NEW MESSAGE]: {json.dumps(message, indent=2)}")

        return "\n\n".join(parts)

    def parse_response(self, text: str) -> Dict[str, Any]:
        """Parse agent response into structured format.

        Tries multiple strategies because the accumulated text may contain
        intermediate output before the final JSON answer:
          1. Whole text as JSON
          2. Last markdown code fence
          3. Last bare {...} block (scan from end)
          4. Fallback wrapper
        """
        text = text.strip()
        if not text:
            return {"agent": self.name, "role": self.role, "response": ""}

        # 1. Whole text
        try:
            return json.loads(text)
        except (json.JSONDecodeError, ValueError):
            pass

        # 2. Last markdown code fence (prefer ```json, then ```)
        for marker in ("```json", "```"):
            # Find the LAST occurrence of this marker
            idx = text.rfind(marker)
            if idx == -1:
                continue
            start = idx + len(marker)
            end = text.find("```", start)
            if end != -1:
                try:
                    return json.loads(text[start:end].strip())
                except (json.JSONDecodeError, ValueError):
                    continue

        # 3. Last bare {...} block — scan from end
        depth = 0
        end_pos = -1
        for i in range(len(text) - 1, -1, -1):
            ch = text[i]
            if ch == '}':
                if depth == 0:
                    end_pos = i
                depth += 1
            elif ch == '{':
                depth -= 1
                if depth == 0:
                    try:
                        return json.loads(text[i:end_pos + 1])
                    except (json.JSONDecodeError, ValueError):
                        break  # malformed, give up

        return {"agent": self.name, "role": self.role, "response": text}

    def reset_conversation(self):
        """Clear conversation history."""
        self.conversation_history = []
