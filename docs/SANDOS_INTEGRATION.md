# SandOS / Veyra integration (M1)

## Scope

This milestone adds a portable Citadel-side compatibility boundary for the
published SAND Veyra ABI descriptor and status conventions. It does not change
the SandOS repository, kernel, syscall dispatcher, SAPP format, or SDK.

Target OS repository: https://github.com/jadhavdurvesh/sand-os

## Verified SandOS contract

The integration was checked against these SandOS files:

- `include/sand/veyra/abi.h`
- `include/sand/veyra/interface.h`
- `include/sand/veyra/status.h`
- `docs/VEYRA_ABI.md`
- `docs/API_CATALOG.md`

Veyra's descriptor prefix is 16 bytes: magic, major/minor ABI version,
structure size, and flags. The current magic is `0x56594153`, and the current
major version is 1. The shared status values are 0 and -1 through -10.

## What this adapter does

- Mirrors only the public, fixed-width Veyra descriptor prefix.
- Validates magic, major version, minimum descriptor size, and available bytes.
- Accepts additive minor versions and ignores unknown flag bits.
- Maps the currently documented Veyra status domain into Citadel statuses.
- Uses `memcpy` to read the descriptor prefix without assuming alignment.

The descriptor pointer must already refer to readable memory. This routine is
not a user-pointer validator and must never be used to dereference an untrusted
kernel/user address directly. A future SandOS boundary must validate and copy
user data into kernel-owned memory before invoking equivalent validation.

## What this adapter deliberately does not do

The current SandOS Veyra documents explicitly say that general syscalls,
service IPC, capability enforcement, and stable subsystem function tables are
not implemented. The kernel's current `int 0x80` dispatcher is a test gate,
not a usable general syscall ABI. Therefore this milestone does not attempt to
launch SAPP applications, read files, enumerate processes, modify settings, or
invoke kernel services.

No kernel pointers, private process structures, or syscall numbers are copied
into Citadel's public API. No SandOS kernel code is linked into the portable
Citadel core.

## SAPP compatibility

Citadel's portable core remains a normal C library at this milestone. It is
not yet packaged as SAPP because a working application-facing syscall and
service API is not available. The existing SAPP v1 container and SDK are left
unchanged. A later SAPP front-end can be added once SandOS exposes the minimum
console/input/window and service APIs needed by an interactive assistant.

## Next required SandOS milestones

1. Define safe user-memory copy and a versioned syscall dispatch contract.
2. Add kernel object/handle ownership and capability checks.
3. Add bounded IPC or a service-call mechanism with process isolation.
4. Publish a stable Veyra service interface and compatibility tests.
5. Implement a small native userland host for Citadel using only those published
   contracts, then package it using the unchanged SAPP format.

## Resource policy

The adapter is stateless, allocates no memory, starts no threads, performs no
I/O, and has no network or model dependency. It is only a validation and status
translation layer; actual runtime cost should be measured in the target build.
