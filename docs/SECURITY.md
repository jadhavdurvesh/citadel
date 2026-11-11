# Security model and threat boundaries

## M1 guarantees

- No shell execution, network support, or dynamic library loading in core.
- Fixed-size input/token/output structures.
- Exact registered intent-to-capability mapping.
- Explicit permission-bit checks before callback execution.
- Optional confirmation check before capability dispatch.

## Not yet guaranteed

- Demo policy is not secure against a malicious in-process caller.
- Registry mutation is not thread-safe.
- Provider ABI is not a safe dynamic plugin loader.
- No sandbox, signing, secure update, credential vault, or OS-level authorization integration exists.
- No hostile-plugin isolation claim is made.

## Future hardening

Before external providers: signed manifests, version pinning, ABI checks, process isolation, least-privilege IPC, quotas, cancellation, crash containment, audit logs, and explicit user consent.

Before SandOS integration: use native OS authorization, avoid shell interpolation, canonicalize file paths, validate app identifiers, and require confirmation for destructive or privileged operations.
