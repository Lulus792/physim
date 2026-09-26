@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >NUL
if errorlevel 1 exit /b 1
cmake -S . -B build-asan-memory -G "Visual Studio 17 2022" -A x64 -DPHYSIM_BUILD_APP=OFF -DCMAKE_C_FLAGS=/fsanitize=address -DCMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO="/debug /INCREMENTAL:NO"
if errorlevel 1 exit /b 1
cmake --build build-asan-memory --config RelWithDebInfo --target physim-memory-tests physim-memory_owners-tests --parallel 8
if errorlevel 1 exit /b 1
ctest --test-dir build-asan-memory -C RelWithDebInfo --output-on-failure -R "^memory"
exit /b %errorlevel%
