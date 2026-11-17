# SandOS adapter (M5)

`include/citadel/sandos.h` and `src/adapters/sandos.c` implement what Citadel does on SAND OS, without any OS calls.
The host passes a `CitadelSandPlatform` table (list/launch apps, read/write settings, wall time, system info);
on SAND OS the table is backed by SAPP syscalls (the `citadel` app in the SAND OS repository), in tests by a fake.
The adapter needs only `memcpy`/`memset`/`memcmp`/`strlen`/`strcmp`, so it links into a freestanding SAPP.

| You say | Capability | Policy |
|---|---|---|
| `help`, `what can you do` | `sand.help` | none |
| `apps`, `list apps`, `show apps` | `sand.apps` | LAUNCH_APP |
| `open <app>`, `launch <app>`, `start <app>` (aliases: terminal, settings, stopwatch, files) | `sand.open` | LAUNCH_APP |
| `time`, `what time is it`, `date`, `what is the date` | `sand.time` | READ_SYSTEM |
| `status`, `system status`, `uptime` | `sand.status` | READ_SYSTEM |
| `settings`, `show settings` | `sand.show_settings` | READ_SYSTEM |
| `set accent <name>`, `set motion on\|off`, `set clock 12\|24` | `sand.set` | MODIFY_SETTINGS **and confirmation** |
| `note <text>` (≤60 chars), `notes` / `show notes` | `sand.note`, `sand.notes` | FILE_ACCESS |
| `clear notes` | `sand.notes_clear` | FILE_ACCESS **and confirmation** |
| `remind me in <n> <seconds\|minutes\|hours> [to <text>]` (up to 24 h), `reminders`, `clear reminders` | `sand.remind`, `sand.reminders`, `sand.reminders_clear` | SCHEDULE |

Changing settings never runs on the first request: `citadel_dispatch` returns `CITADEL_ERR_DENIED` until the host
sets `confirmation_granted`, and `citadel_requires_confirmation()` lets the host ask the user first without executing anything.

## Core changes in this milestone
- Intent phrases may end in a standalone `*` token that matches one or more arguments ("open *"). `*` anywhere else, or as the whole phrase, is rejected at registration.
- `citadel_requires_confirmation(registry, command)`: read-only query used to prompt before a confirmed capability runs.

## Limits
- Matching is still exact phrase matching, not language understanding; input should be lower-cased by the host.
- The clock is the machine's real-time clock, which SAND OS does not synchronise.
- Tests use a fake platform; the real SAND OS binding is tested in the SAND OS repository under QEMU only.

## Notes and reminders

The adapter keeps no state. The host supplies `note_*` and `reminder_*` hooks in `CitadelSandPlatform`;
a NULL hook makes the matching intents answer `ERR_UNAVAILABLE`. The host stores notes and decides how
reminders fire when due (the core has no clock or timer of its own).
