@echo off
REM Compile all C files in the folder
gcc *.c -IC:/msys64/mingw64/include -LC:/msys64/mingw64/lib -lraylib -lopengl32 -lgdi32 -lwinmm -luser32 -lkernel32 -o main.exe

REM Run the program after compilation
if %errorlevel%==0 main.exe
pause
