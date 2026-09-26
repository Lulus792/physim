@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >NUL
if errorlevel 1 exit /b 1
cmake -S . -B build-clang-ninja -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER="C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/bin/clang-cl.exe" -DCMAKE_MAKE_PROGRAM="C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe"
if errorlevel 1 exit /b 1
cmake --build build-clang-ninja --parallel 8
if errorlevel 1 exit /b 1
ctest --test-dir build-clang-ninja --output-on-failure
