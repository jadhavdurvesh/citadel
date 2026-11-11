# M4: read-only application catalogue

The portable C11 core now exposes a bounded in-memory application catalogue
(`include/citadel/catalog.h`). Integrators register up to 32 application
identifiers and display names. IDs are unique, input lengths are bounded,
and control characters are rejected. The catalogue supports exact lookup and
bounded formatting without dynamic allocation or disk access.

The host console accepts `apps` and `list apps`. The default catalogue is
empty by design: Citadel does not pretend to discover installed Windows apps
or SandOS SAPPs, and it does not launch anything.

The next OS-specific adapter can populate the catalogue from a trusted SAPP
registry after SandOS provides a stable application enumeration interface.
Windows can similarly add an explicitly authorized inventory adapter. Neither
adapter should live in the portable core.

Build and test:
```sh
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```
