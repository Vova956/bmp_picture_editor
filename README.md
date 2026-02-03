Image Utility
==========================

Description
-----------
This project implements a BMP image manipulation utility in C.  
Features include:
- Loading and saving BMP images
- Fill, rotate, scale, resize, and crop
- Message embedding and extraction (steganography)

The program is console-based and can be run interactively with commands.

Build Instructions
------------------
Two Makefiles are provided: one for Linux (GNU make) and one for Windows (MSVC nmake).

Linux (tested on Ubuntu 22.04 LTS, GCC 11.4):
  $ make
  $ bin/linux/imagetool

Windows (tested on Windows 11, MSVC 19.41 via Developer Command Prompt):
  > nmake /f Makefile.win
  > bin\windows\imagetool.exe

Cleaning build files:
  Linux:   $ make clean
  Windows: > nmake /f Makefile.win clean

Test Programs
-------------
Both Makefiles include a "test" target to build and run sample test code.
Linux:   $ make test
Windows: > nmake /f Makefile.win test

Development Environment
-----------------------
Linux build tested in:
  - Ubuntu 22.04.4 LTS (64-bit)
  - GCC 11.4.0
  - Running natively on host

Windows build tested in:
  - Windows 11 Pro (64-bit)
  - Microsoft Visual Studio 2022 Build Tools
  - Developer Command Prompt (x64 Native Tools)

No virtual machines or WSL were used during development.

Notes
-----
- Only 24-bit and 32-bit uncompressed BMP images are supported.
- Ensure the "bin" directory exists before running executables.
- For interactive usage, type "help" in the program to see available commands.
