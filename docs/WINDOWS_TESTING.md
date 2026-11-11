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
Commands: `help`, `status`, `system status`, `what time is it`, `what is the date`.
Opening applications and changing settings are intentionally excluded. The
bounded file-search API is implemented for host use and tests, but is not
exposed as a command until the UI can require an explicit user-selected root.
Search is limited to depth 8 and 16 results; POSIX symbolic links are not followed.
