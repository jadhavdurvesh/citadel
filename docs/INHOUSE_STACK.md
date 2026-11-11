# In-house Technology Integration Map

Citadel prioritizes DMJ in-house technologies where they provide a useful, stable capability. This is an integration plan, not a claim that every named project is already linked into the runtime. The actual repositories and versioned interfaces must be inspected before importing code.

## Design rule

**Own stack first; minimal always-on set.** Citadel's low-level command path must stay small, deterministic, offline, and independent of optional services. A technology is included only where its job is clear and its runtime cost is justified.

## Technology mapping

| Technology | Language | Citadel responsibility | Runtime policy |
|---|---|---|---|
| CONTRACT | Rust | Typed service/API contracts between Citadel and platform adapters | Use at the service boundary; do not duplicate core parsing |
| OCTET | Zig | Compact, versioned binary serialization of IPC messages and connector metadata | Used when IPC is introduced; no JSON dependency in core |
| FROSTBYTE | Zig | Binary frame boundaries, length validation, stream framing for local IPC and later network transports | Small library/service boundary; not always-on by itself |
| PHANTOM | Erlang/OTP | Optional asynchronous message broker for independent services and multi-agent/provider events | Disabled for single-process/offline profile |
| ORBIT | Go | Optional lifecycle orchestration for several separately running services/providers | Start only when multi-service mode is enabled |
| DATAGRAM | Rust | Optional UDP transport/discovery for network-connected mode | Network disabled by default; no socket code in core |
| CALLGRAPH | Go | Isolated remote task execution using explicit capability grants | Disabled by default; never a general unrestricted shell |
| ARGUS | OCaml | Formal checks for command policies, permission transitions, and provider state machines | Development/CI verification, not a runtime dependency |
| AETHER | NASM | ELF/executable inspection for architecture, class, ABI, and segment sanity checks | Build/install-time inspection; never executes inspected files |
| LEXORA | Haskell | Authoring and compiling declarative intent/alias/rule definitions into compact tables or a static manifest | Build-time compiler; generated output requires no Haskell runtime |

## Relationship to existing DMJ projects

| Project | Integration |
|---|---|
| DMJScript | Optional workflow/action language adapter. Scripts receive narrow capabilities and run outside the privileged core. |
| RELAY | Optional bridge for provider selection/failover. Keep its Python runtime outside Citadel's native core. |
| DMJ LLM | Future local-model provider behind Citadel's versioned provider ABI. |
| GONUL / other model research | Future alternative provider, only after inference API and resource requirements are verified. |
| SandOS / SAPP | Native platform adapter and public SAPP-facing assistant API, after the actual OS ABI is inspected. |

## Proposed IPC envelope

OCTET should serialize a versioned message envelope containing:

- magic and protocol version;
- message type and flags;
- request/correlation identifier;
- capability identifier;
- payload length;
- bounded payload;
- optional integrity field defined by OCTET's own format.

FROSTBYTE should frame the serialized message on stream transports. The receiver must reject unsupported versions, over-limit lengths, malformed frames, and unknown message types before decoding or dispatching. Authentication and authorization remain separate from serialization and framing.

## Security boundaries

- A provider's response is untrusted data, not an instruction with authority.
- All requested actions resolve to registered Citadel capabilities and are checked again by the host/OS.
- CALLGRAPH tasks must be signed or authenticated, scoped, audited, time-limited, and non-shell by default.
- DATAGRAM must not be enabled without endpoint policy, authentication, rate limits, and explicit user/system configuration.
- PHANTOM messages must carry identity, origin, request ID, and capability scope; the broker cannot grant privilege.
- ORBIT may start/stop services but must not bypass the capability and permission system.
- AETHER inspects executable metadata only; successful inspection does not establish that a binary is safe.
- ARGUS verifies policy invariants but is not a substitute for runtime enforcement.
- LEXORA-generated rules must be bounded and declarative; do not compile arbitrary user text into privileged native code.

## Profiles

### Minimal / offline profile (default)
Citadel core, deterministic intents, local capability adapters. No broker, Go services, remote execution, network, model, or background orchestration.

### Native system profile
Adds CONTRACT-based service boundary and OCTET/FROSTBYTE local IPC as needed to communicate with SandOS services.

### Extended local profile
May start PHANTOM and ORBIT only when multiple local services need coordination. Optional DMJ LLM provider remains independently enabled.

### Connected profile
May enable DATAGRAM and a separately reviewed network connector. Remote AI through RELAY is opt-in and must be visible to the user.

### Development / verification profile
Runs LEXORA to compile rules and ARGUS/AETHER checks in CI or packaging. These tools are not required on end-user machines.

## Implementation gate

Before linking a technology, verify its repository, license, build instructions, public API, dependency tree, target architecture, and resource cost. Until that review is complete, Citadel should depend only on its small native core and its documented local interfaces.
