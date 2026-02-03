@echo off
echo Compiling and running Image Utility (interactive CLI) on Windows...

:: set project root (parent of script folder)
set "ROOT=%~dp0.."

:: ensure bin\windows exists
if not exist "%ROOT%\bin\windows" mkdir "%ROOT%\bin\windows"

:: compile into bin\windows
cl /nologo /W3 /TC /I "%ROOT%\include" "%ROOT%\src\image.c" "%ROOT%\src\main.c" /Fe"%ROOT%\bin\windows\imagetool_script.exe"

if %ERRORLEVEL% EQU 0 (
    echo Compilation successful.
    "%ROOT%\bin\windows\imagetool_script.exe"
) else (
    echo Compilation failed.
    exit /b 1
)

pause