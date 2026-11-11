# M3 — Read-only host capabilities

This milestone expands the Windows/Linux/macOS test host without coupling the
portable command core to an operating system.

Implemented:
- A bounded in-memory command history (last 10 commands; session-only).
- A live capability inventory command.
- System memory and logical CPU reporting.
- Local date/time.
- Bounded file-search API for trusted callers/tests (depth 8, 16 results).
- Tests that verify the application-launch placeholder remains denied under
  the normal read-system-only policy.

Deliberately not implemented:
- Launching applications or opening settings.
- Changing system settings.
- Unrestricted recursive file-search commands.
- Background services, network, models, or AI providers.

The capability inventory is generated from the registered runtime registry,
not a hard-coded marketing list. It omits the intentionally disabled launcher
and capabilities requiring permissions outside the host's read-only grant.

History is volatile and is never written to disk. No user file contents are
read by the assistant in this milestone.
