"""Base agent class using claude-agent-sdk."""

import asyncio
import json
from typing import Dict, Any, List

from claude_agent_sdk import (
    ClaudeSDKClient, ClaudeAgentOptions, ClaudeSDKError,
    AssistantMessage, SystemMessage, ResultMessage,
    TextBlock, ThinkingBlock, ToolUseBlock,
)
from config import AGENT_CONFIG

MAX_RETRIES = 3
RETRY_BASE_DELAY = 2  # seconds
MAX_HISTORY_ENTRIES = 20  # 10 exchanges (user + assistant each)
RESPONSE_TIMEOUT = 222  # seconds (3.7 min); interrupt if no new message arrives within this window


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

    async def _receive_response(self, client: ClaudeSDKClient) -> str:
        """Consume the message stream, log progress, return final text.

        Uses a *per-message* timeout rather than a whole-response timeout.
        As long as messages keep flowing the timer resets, so a legitimately
        long session with many tool calls will never be interrupted.  But if
        a single tool call hangs and no message arrives for RESPONSE_TIMEOUT
        seconds, we send an interrupt (programmatic ESC) and re-raise
        TimeoutError for the caller's retry logic.
        """
        self._partial_response = ""
        response_iter = client.receive_response().__aiter__()

        while True:
            try:
                msg = await asyncio.wait_for(
                    response_iter.__anext__(), timeout=RESPONSE_TIMEOUT
                )
            except StopAsyncIteration:
                break
            except asyncio.TimeoutError:
                self._log(
                    f"No message for {RESPONSE_TIMEOUT}s, sending interrupt..."
                )
                await client.interrupt()
                # Drain the ResultMessage produced by the interrupt so the
                # message stream is clean for the next query() call.
                try:
                    async def _drain():
                        async for m in client.receive_response():
                            self._process_message(m)
                    await asyncio.wait_for(_drain(), timeout=30)
                except asyncio.TimeoutError:
                    pass
                raise

            self._partial_response += self._process_message(msg)

        return self._partial_response

    async def process(self, message: Dict[str, Any]) -> Dict[str, Any]:
        """Run the agent.  Retry on SDK failure, preserving context.

        On exception the same ClaudeSDKClient is reused, so the full
        conversation history (tool calls, results, thinking) is kept.
        """
        prompt = self._build_prompt(message)
        last_error = None
        self._partial_response = ""

        options = ClaudeAgentOptions(
            model=self.model,
            tools={"type": "preset", "preset": "claude_code"},
            permission_mode="bypassPermissions",
            setting_sources=["user", "project", "local"],
        )

        try:
            async with ClaudeSDKClient(options=options) as client:
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
