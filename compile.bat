@echo off
setlocal

set CC=C:\msys64\mingw64\bin\gcc.exe
set BIN=C:\msys64\mingw64\bin
set INC=C:\msys64\mingw64\include
set LIB=C:\msys64\mingw64\lib

if exist dist rmdir /s /q dist
mkdir dist

echo Compiling...
"%CC%" *.c -std=c99 -O2 ^
    -I"%INC%" ^
    -L"%LIB%" -lraylib -lopengl32 -lgdi32 -lwinmm -lkernel32 ^
    -o dist\connect4.exe

if errorlevel 1 (
    echo Build failed.
    pause
    exit /b
)

echo Build OK. Copying DLLs...

copy "%BIN%\libraylib.dll" dist\ >nul
copy "%BIN%\glfw3.dll" dist\ >nul
copy "%BIN%\libgcc_s_seh-1.dll" dist\ >nul
copy "%BIN%\libstdc++-6.dll" dist\ >nul
copy "%BIN%\libwinpthread-1.dll" dist\ >nul
copy "%BIN%\zlib1.dll" dist\ >nul
copy "%BIN%\libopenal.dll" dist\ >nul

echo Done. Portable build available in dist\
pause
