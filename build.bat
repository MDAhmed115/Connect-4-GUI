@echo off
setlocal

set CXX=C:\msys64\mingw64\bin\g++.exe
set INC=C:\msys64\mingw64\include
set LIB=C:\msys64\mingw64\lib
set OUTPUT=main.exe

"%CXX%" *.cpp -std=c++17 -g -O2 ^
 -I"%INC%" ^
 -L"%LIB%" ^
 -lraylib ^
 -lopengl32 -lgdi32 -lwinmm -lpthread -lm ^
 -o %OUTPUT%

if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

echo Build successful.
exit /b 0
