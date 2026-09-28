@echo off
setlocal
python "%~dp0build.py" --compiler clang-cl --build-dir "%~dp0..\build\native\Clang" --test %*
exit /b %errorlevel%
