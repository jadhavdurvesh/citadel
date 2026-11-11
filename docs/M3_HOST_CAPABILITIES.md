# M3 — host capabilities and interactive console

This milestone extends the portable C11 core with host-only, read-only
capabilities and a small interactive console. The same command/runtime core
is intended to be reusable by a future SandOS adapter.

## Commands
- `help`, `status`
- `system status`: physical memory totals/availability and logical CPU count
- `what time is it`, `what is the date`: host local clock
- `capabilities list`: disclose which capabilities are currently enabled
- `history`: show the last ten commands in this process (volatile, not persisted)

## Host APIs
`citadel_host_system_snapshot` exposes a read-only system snapshot.
`citadel_host_search_files` provides bounded host-side filename search:
maximum depth 8 and 16 matches. It is not yet exposed as a console command:
the command/UI must first provide a clear user-selected root and safe path
handling. Search does not read file contents or execute files.

## Explicitly deferred
Application launching and settings modification remain disabled. The demo
"open settings" intent is deliberately denied by the default host policy.
No AI model, internet, network service, GPU, background worker or third-party
runtime is required.

## CI
GitHub Actions builds and runs tests on Ubuntu and Windows. Local Windows
instructions are in `WINDOWS_TESTING.md`.
