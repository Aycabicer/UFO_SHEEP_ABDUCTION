@echo off
setlocal

set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"

if not exist "C:\msys64\usr\bin\bash.exe" (
    echo ERROR: C:\msys64\usr\bin\bash.exe bulunamadi.
    exit /b 1
)

subst X: "%ROOT%" >nul 2>&1

C:\msys64\usr\bin\bash.exe -lc "export PATH='/ucrt64/bin:/usr/bin:/bin'; cd '/x'; ./app"
set "CODE=%ERRORLEVEL%"

subst X: /d >nul 2>&1
exit /b %CODE%
