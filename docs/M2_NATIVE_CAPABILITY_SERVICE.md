# M2 — Native capability service

## Purpose

M2 connects the OCTET/CONTRACT message boundary to Citadel's local capability allow-list. This is an in-process service dispatcher, not yet a SandOS IPC daemon. It can be embedded by a native SandOS adapter once the OS's real process and IPC contracts are verified.

## Request lifecycle

1. Decode a bounded CONTRACT request from an OCTET frame.
2. Resolve its operation ID against the locally registered operation table.
3. Check the operation's required permissions against the caller policy supplied by the trusted host.
4. Require an explicit confirmation bit for operations marked confirmation-sensitive.
5. Invoke the registered handler.
6. Encode a CONTRACT response with the same request ID and a status/body.

The `required_permissions` value in the incoming request is never trusted to grant authority. It is metadata only. The local operation descriptor is authoritative.

## Limits and safety

- Maximum 32 registered operations.
- Request and response bodies are bounded by CONTRACT's 1 KiB body limit.
- No dynamic allocation in the dispatch path.
- Unknown operations return `CITADEL_ERR_NOT_FOUND`.
- Missing permissions or confirmation return `CITADEL_ERR_DENIED` without calling the handler.
- Duplicate operation IDs and invalid registration are rejected.
- No network, background service, process launch, filesystem mutation, or OS-specific syscall is enabled by this layer.

## SandOS integration boundary

This M2 service deliberately does not invent a SandOS IPC ABI. The current public `codename-Sand` repository contents inspected for this task are a Python desktop prototype and do not expose a verified native kernel/service/SAPP interface. Therefore no SandOS files are changed and no SAPP ABI is guessed. A dedicated SandOS native adapter remains blocked on locating the actual OS source/ABI or its current SDK.

## Build

The new module is part of `citadel_core` and is covered by the normal CMake/CTest suite.
