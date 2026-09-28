@echo off
setlocal
python "%~dp0build.py" --sanitizers --no-app --test --test-filter "memory*"
exit /b %errorlevel%
