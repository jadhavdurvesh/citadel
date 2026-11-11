# Windows testing
Build using Visual Studio 2022 Developer PowerShell:
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
.\build\Release\citadel.exe
```
Or MinGW-w64:
```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
.\build\citadel.exe
```
Commands: `help`, `status`, `system status`, `capabilities list`,
`what time is it`, `what is the date`, and `history`.
The history is an in-memory ring of the last 10 commands and is discarded
when Citadel exits. The file-search API remains available to host adapters
but is not exposed as a command until an explicit user-selected root UI exists.
Opening applications and modifying settings remain disabled.
