# DMJ In-house Technology Integration Map

This document maps Citadel to the actual DMJ-owned technology repositories. It is based on their current repository READMEs and documented feature boundaries. This is an integration plan, not a claim that every project is already linked into Citadel.

## Design rule

**Our stack first, minimum always-on footprint.** Citadel's initial command path stays deterministic, offline, and lightweight. Reuse a DMJ technology only where its actual interface and feature set fit. Do not start an entire service just to use a small library primitive.

## Repository map

| Technology | Repository / language | Verified project responsibility | Citadel integration approach |
|---|---|---|---|
| CONTRACT | [contract](https://github.com/jadhavdurvesh/contract) / Rust | Transport-independent API application core, bounded HTTP/1.1 listener, CALLGRAPH RPC client | Candidate service/API gateway when Citadel needs a process boundary. Its HTTP listener has documented limits and should not be exposed directly to the internet. |
| OCTET | [octet](https://github.com/jadhavdurvesh/octet) / Zig | Deterministic, versioned binary representation and bounded serialization codec | Serialize IPC/provider messages. Keep schema/version handling separate from transport. Format is early research and not frozen. |
| DATAGRAM | [datagram](https://github.com/jadhavdurvesh/datagram) / Rust | UDP endpoint wrapper, DGM1 envelope, replay window, ACK1 codec, AES-GCM payload protection, congestion/MTU primitives | Optional connected transport. Its README documents missing reliability adapter/production hardening; never use for safety-critical control. |
| CALLGRAPH | [callgraph](https://github.com/jadhavdurvesh/callgraph) / Go | Typed remote-operation runtime over OCTET, FRG1 and DATAGRAM; authorization hooks, deadlines, replay protection, bounded fragmentation/retransmission, optional AES-GCM | Optional remote capability execution. Expose only allow-listed typed operations; never map it to unrestricted shell access. |
| ARGUS | [ARGUS](https://github.com/jadhavdurvesh/ARGUS) / OCaml | Explicit-state CTL model checker for finite transition systems | CI/development verification of permission state machines, confirmation flows, provider lifecycle and fail-safe invariants. Not a runtime security enforcement substitute. |
| ORBIT | [ORBIT](https://github.com/jadhavdurvesh/ORBIT) / Go | Single-controller job orchestration, worker registry, leases, retries, recovery and file-backed job snapshots | Optional scheduling/orchestration for multi-service jobs. Current README documents single-controller and non-transactional limitations. |
| PHANTOM | [PHANTOM](https://github.com/jadhavdurvesh/PHANTOM) / Erlang/OTP | Actor-based topic broker, monitored consumers, acknowledgements, bounded queues, requeue and local journal | Optional asynchronous event bus when independent services need it. At-least-once delivery means consumers must be idempotent. |
| FROSTBYTE | [FROSTBYTE](https://github.com/jadhavdurvesh/FROSTBYTE) / Zig | Versioned 16-byte binary frame header, zero-copy decoder, fixed-capacity ring buffer, incremental TCP framing | Local stream IPC framing where TCP/byte streams are appropriate. Distinct from OCTET's structured serialization and DATAGRAM's UDP envelope. |
| AETHER | [AETHER](https://github.com/jadhavdurvesh/AETHER) / NASM | Linux x86-64 syscall-only ELF64 inspection, header/table-bound validation and executable metadata reporting | Build/package-time inspection of supported ELF binaries. Inspection does not prove a binary is safe. |
| LEXORA | [LEXORA](https://github.com/jadhavdurvesh/LEXORA) / Haskell | Typed functional language lab: lexer/parser, type inference, evaluator, stack IR, IR validation and initial fuel-bounded VM | Potential future language for declarative assistant workflows. Current VM limitations mean it is not yet a complete automation runtime or drop-in intent compiler. |

## Existing transport layering

The repositories document this remote-operation path:

```text
CALLGRAPH request/response frame (CGR1)
                 |
          OCTET serialization (OCT1)
                 |
        CALLGRAPH fragmentation (FRG1)
                 |
       DATAGRAM envelope (DGM1)
                 |
                UDP/IP
```

CONTRACT includes a Rust CALLGRAPH RPC client and a bounded HTTP listener. FROSTBYTE provides a separate TCP stream framing mechanism; it is not a replacement for OCTET's field serialization or DATAGRAM's UDP transport.

## Citadel profiles

### 1. Minimal offline profile — default
Citadel C core, deterministic intents, and in-process local capability adapters. No background broker, network, remote execution, model runtime, or orchestration service.

### 2. SandOS native profile
Use a narrow local service boundary for OS operations. CONTRACT may host an API boundary where appropriate. OCTET can encode local IPC messages; FROSTBYTE can frame byte streams if the chosen IPC transport is stream-based. The final choice depends on SandOS's actual IPC APIs and measured cost.

### 3. Extended local profile
PHANTOM can route asynchronous events; ORBIT can coordinate longer-running jobs and workers. Start these only if multiple services need them. Do not make Citadel startup depend on Erlang/Go daemons for simple commands.

### 4. Connected profile
CALLGRAPH can expose typed remote operations over the existing OCTET/DATAGRAM stack. This is opt-in and subject to authentication, authorization, quotas, auditing, key management and explicit user consent. Network access stays disabled by default.

### 5. Development and verification profile
ARGUS checks finite-state policy models. AETHER inspects ELF artifacts on its supported target. LEXORA can be explored for declarative workflow syntax after its VM and language limitations are addressed. None of these tools need to ship as always-running assistant processes.

## AI connectors

- **DMJ LLM:** future local-model provider behind Citadel's versioned provider ABI.
- **RELAY:** future optional routing bridge to external/local AI providers.
- **DMJScript:** future workflow adapter, sandboxed and restricted to explicitly granted capabilities.
- **GONUL and other model experiments:** possible future provider implementations after API, memory, latency and licensing review.

AI output is always untrusted input. It can propose a capability request but cannot grant itself permission or directly execute arbitrary commands.

## Security and engineering gates

- Verify each project's license and third-party dependency notices before embedding or redistributing it. Citadel's proprietary license does not override the individual projects' licenses.
- Pin exact dependency versions/commits and maintain a compatibility matrix.
- Keep transport, serialization, RPC semantics and application authorization as separate layers.
- Bound message sizes, queues, retries, concurrent jobs, memory and execution time.
- Require authenticated peers and explicit capability authorization for remote calls.
- Treat UDP delivery as lossy; do not use these networking components for emergency-stop or hard real-time actuation.
- Keep local OS authorization authoritative. A broker, scheduler, model or remote caller cannot bypass it.
- Measure resident memory, idle CPU, wakeups, binary size and power on the actual low-end target before enabling optional services.

## Current integration status

The current Citadel M1 executable does **not** link these ten projects. It contains its own small C11 core and a dormant provider ABI. The repositories above have now been identified and their actual roles documented. Integration should proceed one adapter at a time, beginning with local service contracts and message encoding—not by starting all ten services together.
