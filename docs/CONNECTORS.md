# Connector contracts

## Current state

M1 defines a versioned C provider structure in `include/citadel/provider.h`. It is dormant: the host does not load a shared library, call initialize, invoke generation, or connect to a network.

## ABI rules

- ABI version must match exactly; descriptor structure size is checked.
- Providers declare stable identifiers, kind, capabilities, memory budget metadata, and whether they require network access.
- Requests and responses use caller-provided buffers.
- Model output is untrusted data, never authority. Proposed tool actions must be re-authorized through Citadel capabilities.
- Provider activation must be explicit and policy-controlled.

## Planned adapters

**DMJ LLM:** a future local-model provider translating its inference API into the Citadel contract. No Python, PyTorch, tokenizer, or model artifacts are part of the core.

**RELAY:** a separate optional bridge mapping Citadel requests to RELAY and translating normalized responses. Avoid linking Python into the C core; use a separate process/module boundary.

**Network:** a separately enabled service. Before implementation it must define TLS verification, endpoint allow-lists, timeouts, cancellation, privacy disclosure, and credential storage. The core must never open sockets.

**DMJScript:** an optional sandboxed automation extension. It must not inherit unrestricted core/OS privileges; expose narrow capability handles only.
