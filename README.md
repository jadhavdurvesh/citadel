# Citadel

Lightweight, native-first assistant runtime for SandOS. M1 is deterministic and offline: bounded command parsing, intent matching, policy-gated capability dispatch, and a dormant versioned AI provider ABI. No model, network, GPU, or third-party runtime dependency is required by the core.

**Status:** M1 foundation / early development.

## Build

Requires CMake 3.16+ and a C11 compiler.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## In-house integration roadmap

- [RELAY](https://github.com/jadhavdurvesh/Relay): optional future bridge, not linked into core.
- [DMJ LLM](https://github.com/jadhavdurvesh/DMJ-LLM): optional future local model provider.
- [DMJScript](https://github.com/jadhavdurvesh/dmjscript): optional sandboxed tool adapter.
- [SandOS](https://github.com/jadhavdurvesh/codename-Sand): future platform adapter after native APIs and SAPP ABI are verified.

The provider contract is dormant in M1; no providers are loaded or started. See docs for architecture and security boundaries.


## DMJ in-house technology stack

Citadel is designed to use our own technologies where they fit, without making all of them mandatory at runtime. The integration map covers CONTRACT, OCTET, FROSTBYTE, PHANTOM, ORBIT, DATAGRAM, CALLGRAPH, ARGUS, AETHER, and LEXORA, plus DMJScript, RELAY, DMJ LLM, GONUL, and SandOS. Some are build-time tools, some are optional services, and some are future connectors. None are claimed to be integrated until their source APIs are verified. See [docs/INHOUSE_STACK.md](docs/INHOUSE_STACK.md).

**Default profile:** offline, deterministic, no broker, network, model, or Go/Erlang service. This protects the low-resource target.

## License

Citadel is proprietary software, not open source. Public repository visibility grants only the limited personal-evaluation rights expressly set out in [LICENSE](LICENSE). All other rights are reserved.

Copyright (c) 2026 Durvesh Jadhav / DMJ.
