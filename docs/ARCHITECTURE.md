# Architecture

## Core boundary

The portable core contains no operating-system calls, file access, process launching, sockets, model runtime, GPU code, or background worker. It accepts bounded text, resolves a registered intent, checks policy, and invokes a registered capability callback.

## Memory model

Input is limited to 256 bytes; commands contain at most 16 tokens, each at most 47 bytes. Registry capacities and output size are fixed in the public header. Callers own registry, command, policy, and result structures. Core functions do not retain parsed-input pointers after the synchronous call. These bounds are not a measured resident-memory claim.

## Intent matching

M1 uses deterministic phrase matching. It is not natural-language understanding. Optional classifiers may be added behind a separate interface in later versions.

## Platform adapters

An adapter maps registered capabilities to native operations. It must validate identifiers, canonicalize paths, enforce user/session policy, and handle OS errors. The portable core never executes shell commands.

## Safety model

Capabilities declare permission bits and whether confirmation is required. The host supplies policy. The current policy structure is a minimal demonstration mechanism, not a security boundary against malicious in-process code. Production SandOS integration must use authorization issued by the OS security boundary.
