@echo off
echo Building POS App with Qt HTTP Server...

REM Set Qt and MinGW environment
set PATH=C:\Qt\6.9.1\mingw_64\bin;C:\Qt\Tools\mingw1310_64\bin;%PATH%

REM Clean previous build
if exist build rmdir /s /q build
mkdir build
cd build

echo Configuring with CMake...
cmake -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=C:\Qt\6.9.1\mingw_64 -DCMAKE_C_COMPILER=C:\Qt\Tools\mingw1310_64\bin\gcc.exe -DCMAKE_CXX_COMPILER=C:\Qt\Tools\mingw1310_64\bin\g++.exe ..

if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

echo Building with MinGW...
C:\Qt\Tools\mingw1310_64\bin\mingw32-make.exe

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build successful!
echo Running application with HTTP server...
./POSApp.exe

pause 