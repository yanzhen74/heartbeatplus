@echo off
setlocal enabledelayedexpansion

echo Building HeartbeatPlus with vcpkg installed nanoMSG...

REM REM Check Visual Studio
REM if not exist "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\Tools\MSVC" (
    REM echo Error: Visual Studio 2013 not found
    REM pause
    REM exit /b 1
REM )

REM Activate Visual Studio development environment
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\amd64\vcvars64.bat"

if not exist build mkdir build

echo Compiling client library...
cl /c src/client.cpp /Iinclude /I"vcpkg/installed/x64-windows/include" /Fo:build/client.obj /W3 /EHsc /DNN_STATIC_LIB /D_CRT_SECURE_NO_WARNINGS
if !errorlevel! neq 0 (
    echo Client library compilation failed
    pause
    exit /b 1
)

echo Compiling main program...
cl /c src/heartbeat.cpp /Iinclude /I"vcpkg/installed/x64-windows/include" /Fo:build/heartbeat.obj /W3 /EHsc /DNN_STATIC_LIB /D_CRT_SECURE_NO_WARNINGS
if !errorlevel! neq 0 (
    echo Main program compilation failed
    pause
    exit /b 1
)

echo Linking executable...
link build/heartbeat.obj build/client.obj Ws2_32.lib Mswsock.lib /LIBPATH:"vcpkg/installed/x64-windows/lib" nanomsg.lib /OUT:build/heartbeat.exe /SUBSYSTEM:CONSOLE
if !errorlevel! neq 0 (
    echo Linking failed
    pause
    exit /b 1
)

echo.
echo Build successful!
echo Executable located at build\heartbeat.exe
echo.
echo Usage:
echo   As server: build\heartbeat.exe server tcp://*:6969
echo   As client: build\heartbeat.exe client tcp://localhost:6969 clientname
echo.
pause
