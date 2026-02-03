@echo off
echo Compiling and running Image Utility tests on Windows...

:: set project root (parent of script folder)
set "ROOT=%~dp0.."

:: ensure bin\windows exists
if not exist "%ROOT%\bin\windows" mkdir "%ROOT%\bin\windows"

:: compile into bin\windows\test_all.exe
cl /nologo /W3 /TC /I "%ROOT%\include" "%ROOT%\src\image.c" "%ROOT%\src\test.c" /Fe"%ROOT%\bin\windows\test_all.exe"

if %ERRORLEVEL% EQU 0 (
    echo Compilation successful.
    "%ROOT%\bin\windows\test_all.exe"
) else (
    echo Compilation failed.
    exit /b 1
)

pause