@echo off
setlocal

set CXX=C:\msys64\mingw64\bin\g++.exe
set BIN=C:\msys64\mingw64\bin
set INC=C:\msys64\mingw64\include
set LIB=C:\msys64\mingw64\lib

if exist dist rmdir /s /q dist
mkdir dist

echo Compiling...

"%CXX%" *.cpp -std=c++17 -O2 ^
 -I"%INC%" ^
 -L"%LIB%" -lraylib -lopengl32 -lgdi32 -lwinmm -lpthread -lm ^
 -o dist\connect4.exe

if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

echo Copying DLLs...

copy "%BIN%\libraylib.dll" dist\ >nul
copy "%BIN%\libgcc_s_seh-1.dll" dist\ >nul
copy "%BIN%\libstdc++-6.dll" dist\ >nul
copy "%BIN%\libwinpthread-1.dll" dist\ >nul
copy "%BIN%\zlib1.dll" dist\ >nul

if exist "%BIN%\glfw3.dll" (
    copy "%BIN%\glfw3.dll" dist\ >nul
) else if exist "%BIN%\libglfw3.dll" (
    copy "%BIN%\libglfw3.dll" dist\ >nul
) else if exist "%BIN%\libglfw-3.dll" (
    copy "%BIN%\libglfw-3.dll" dist\ >nul
)

if exist "%BIN%\libopenal.dll" (
    copy "%BIN%\libopenal.dll" dist\ >nul
) else if exist "%BIN%\OpenAL32.dll" (
    copy "%BIN%\OpenAL32.dll" dist\ >nul
)

echo Copying resources...

if not exist dist\resources mkdir dist\resources
if exist resources\fonts (
    xcopy /e /i /y resources\fonts dist\resources\fonts >nul
)

echo Done. Portable build in dist\
exit /b 0
