@echo off
REM SimpleRDBMS Build Script for Windows
REM This script configures and builds the SimpleRDBMS project

setlocal enabledelayedexpansion

echo.
echo ========================================
echo SimpleRDBMS Build Script
echo ========================================
echo.

REM Check if build directory exists
if not exist "build" (
    echo Creating build directory...
    mkdir build
)

cd build

REM Try to find Qt6 installation
echo Configuring CMake...
echo.

REM Users may need to set Qt6_DIR explicitly
REM Example: cmake .. -DQt6_DIR="C:\Qt\6.6.0\msvc2019_64\lib\cmake\Qt6"
REM For now, let CMake try to find it automatically

cmake .. -G "MinGW Makefiles"

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] CMake configuration failed!
    echo.
    echo Please ensure Qt6 is installed and set CMAKE_PREFIX_PATH:
    echo   cmake .. -DCMAKE_PREFIX_PATH="C:\path\to\Qt6"
    echo.
    echo Common Qt6 paths on Windows:
    echo   C:\Qt\6.x.x\msvc2019_64
    echo   C:\Qt\6.x.x\msvc2022_64
    echo.
    pause
    exit /b 1
)

echo.
echo CMake configuration successful!
echo.
echo Building project...
echo.

cmake --build . --target SimpleRDBMS_new --config Release

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

echo.
echo Deploying Qt dependencies...
echo.

C:\Qt\6.10.1\mingw_64\bin\windeployqt.exe --dir .\bin .\bin\SimpleRDBMS_new.exe

if %errorlevel% neq 0 (
    echo.
    echo [WARNING] Deployment might have issues. Please check if Qt DLLs are present in bin/
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Executable location: .\bin\SimpleRDBMS_new.exe
echo.
echo To run the application:
echo   .\bin\SimpleRDBMS_new.exe
echo.
pause
