# Citadel

**Citadel** is a lightweight, native-first assistant runtime being developed for SandOS. Its M1 core is deterministic and offline: bounded command parsing, intent matching, policy-gated capability dispatch, and a dormant, versioned AI-provider connector contract.

Citadel does not require an LLM, model files, network access, GPU, or third-party runtime service to perform its initial assistant tasks.

**Status:** M1 foundation / early development. Integrations listed below are planned or selectively reusable components; they are not all linked into the current executable.

## Build

Requirements: CMake 3.16+ and a C11 compiler.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## DMJ in-house technologies

Citadel is designed around the DMJ technology ecosystem. We prefer our own implementations over conventional external infrastructure when the existing project provides the required capability and passes compatibility, security, and resource checks.

| In-house technology | Language | What it contributes to Citadel |
|---|---|---|
| [CONTRACT](https://github.com/jadhavdurvesh/contract) | Rust | HTTP/API application core and CALLGRAPH RPC client. Candidate service/API boundary for external native services. |
| [OCTET](https://github.com/jadhavdurvesh/octet) | Zig | Deterministic, versioned binary serialization for messages between independent components. |
| [DATAGRAM](https://github.com/jadhavdurvesh/datagram) | Rust | Bounded UDP endpoint and datagram primitives for a future explicitly enabled connected mode. |
| [CALLGRAPH](https://github.com/jadhavdurvesh/callgraph) | Go | Typed remote-operation runtime, request correlation, authorization hooks, bounded transport and optional encrypted RPC. |
| [ARGUS](https://github.com/jadhavdurvesh/ARGUS) | OCaml | CTL model checker for checking finite-state policy and lifecycle models during development and CI. |
| [ORBIT](https://github.com/jadhavdurvesh/ORBIT) | Go | Optional job orchestration and worker lifecycle for multi-service workflows. |
| [PHANTOM](https://github.com/jadhavdurvesh/PHANTOM) | Erlang/OTP | Optional actor-based broker for asynchronous events, topic routing, acknowledgements and bounded queues. |
| [FROSTBYTE](https://github.com/jadhavdurvesh/FROSTBYTE) | Zig | Bounded binary framing, zero-copy frame decoding and stream framing for local IPC where needed. |
| [AETHER](https://github.com/jadhavdurvesh/AETHER) | NASM | ELF64 executable inspection for build/package validation on supported Linux x86-64 targets. |
| [LEXORA](https://github.com/jadhavdurvesh/LEXORA) | Haskell | Typed language/compiler technology that may support a future declarative assistant workflow language; its current IR/VM limitations must be respected. |

### How these fit together

The existing transport stack has a defined layering:

```text
Citadel capability / service request
                 |
              CONTRACT
                 |
             CALLGRAPH
                 |
              OCTET
                 |
      DATAGRAM (UDP profile)
                 |
               UDP/IP
```

FROSTBYTE is a separate stream-framing technology and should be used for stream IPC where its framing model fits; it is not interchangeable with OCTET serialization or DATAGRAM's UDP envelope.

ARGUS and AETHER are verification/inspection tools, not always-running assistant services. LEXORA is a language/compiler project, not yet a drop-in intent compiler. ORBIT and PHANTOM are optional infrastructure for multi-service deployments, not dependencies of the minimal assistant.

## Other DMJ projects

- [DMJScript](https://github.com/jadhavdurvesh/dmjscript): possible opt-in workflow/action adapter, with narrow capability grants and isolation.
- [RELAY](https://github.com/jadhavdurvesh/Relay): optional future AI-provider routing bridge.
- [DMJ LLM](https://github.com/jadhavdurvesh/DMJ-LLM): future local-model provider behind the Citadel provider ABI.
- [SandOS](https://github.com/jadhavdurvesh/codename-Sand): future native platform adapter and SAPP-facing API, after the actual OS interfaces are verified.

## Integration status and resource policy

**Current M1 implementation:** the Citadel C11 core, deterministic intent registry, permission-gated dispatcher, CLI demonstration host, tests, and dormant provider ABI.

**Not yet integrated into the M1 executable:** CONTRACT, OCTET, DATAGRAM, CALLGRAPH, ARGUS, ORBIT, PHANTOM, FROSTBYTE, AETHER, LEXORA, DMJScript, RELAY, and DMJ LLM. They are documented as concrete integration targets, not falsely represented as linked dependencies.

**Default profile:** offline and deterministic. No broker, network stack, remote execution, model, or background orchestration is started. Optional components are enabled only when a real use case needs them and their resource/security costs have been measured.

See [docs/INHOUSE_STACK.md](docs/INHOUSE_STACK.md), [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md), [docs/CONNECTORS.md](docs/CONNECTORS.md), and [docs/SECURITY.md](docs/SECURITY.md).

## License

Citadel is proprietary software, not open source. Public repository visibility grants only the limited personal-evaluation rights expressly set out in [LICENSE](LICENSE). All other rights are reserved.

Copyright (c) 2026 Durvesh Jadhav / DMJ.
