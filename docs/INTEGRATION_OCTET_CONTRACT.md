# M1: OCTET + CONTRACT local message boundary

## What is implemented

Citadel now has a C11 implementation of the **OCTET frame v1 wire envelope**, based on the current OCTET draft:

- magic `OCT1`;
- version byte 1;
- flags byte;
- 32-bit big-endian payload length;
- payload bytes;
- exact total-length validation;
- bounded payload (4 KiB in this Citadel profile).

The encoder/decoder is in `include/citadel/octet.h` and `src/connectors/octet.c`. Tagged fields follow the current OCTET Zig implementation's 7-byte field header: 16-bit field ID, 8-bit wire type, 32-bit big-endian value length, then value bytes. Integer helpers use big-endian encoding.

The C implementation is a small Citadel-side interoperability adapter. It does **not** compile or link the Zig OCTET repository into the executable. This avoids imposing a Zig runtime/build toolchain on SandOS while preserving the documented wire format. The upstream OCTET format is still experimental/draft 0.2 and may change; the adapter is version-pinned to frame v1 and should be updated alongside upstream conformance tests.

## CONTRACT-shaped local API

The new `citadel/contract.h` boundary defines bounded, transport-independent request and response records inspired by the actual CONTRACT `Request`/`Response` and application dispatch concepts. They serialize as OCTET tagged fields.

Request fields:
1. request ID (u64)
2. operation ID (u16)
3. required permission bits (u32)
4. bounded opaque body (bytes)

Response fields:
1. request ID (u64)
2. status (i32 represented as 32-bit two's-complement bits)
3. bounded opaque body (bytes)

This is **not** the Rust CONTRACT crate embedded in Citadel. The Rust project is an API application framework with HTTP and a CALLGRAPH RPC client; the native local contract deliberately uses only the relevant transport-independent request/response boundary. The HTTP server and UDP RPC client are not started or linked into Citadel M1.

## Security boundary

- OCTET is serialization, not authentication, encryption, replay protection, or authorization.
- The operation ID is resolved against a local allow-list.
- The required permission bits are checked by Citadel's policy layer; serialized permission claims are descriptive and never grant authority.
- Body sizes are bounded before copying.
- Unknown message types, field order, field IDs, wire types, and trailing bytes are rejected by the current strict M1 profile.
- This local message format is not yet exposed to untrusted processes. A future IPC adapter must add peer identity and OS-enforced authorization.

## Build and tests

The C11 tests cover frame encode/decode, malformed magic, and request/response round trips. Upstream Zig OCTET tests remain authoritative for the Zig implementation. Cross-language golden vectors should be added before declaring full interoperability.
