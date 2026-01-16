# Building SimpleRDBMS from Source

This guide provides detailed instructions for building SimpleRDBMS on different platforms.

## Table of Contents
- [Requirements](#requirements)
- [Windows](#windows)
- [Linux](#linux)
- [macOS](#macos)
- [Troubleshooting](#troubleshooting)

---

## Requirements

### Core Requirements
- **C++ Compiler:** C++17 compatible
  - Windows: MSVC 2019+ or Clang
  - Linux: GCC 7+ or Clang 5+
  - macOS: Clang (via Xcode)
- **CMake:** Version 3.20 or later
- **Qt6:** Version 6.0 or later
  - Core, Gui, Widgets, Sql modules
- **Disk Space:** ~500 MB for build

### Optional
- **Git:** For version control
- **Ninja:** Faster build system (optional, uses default on each platform)

---

## Windows

### Step 1: Install Prerequisites

#### Option A: Using Qt Official Installer (Recommended)

1. Download Qt from [qt.io](https://www.qt.io/download)
2. Run the installer
3. Select **Qt 6.x.x** (latest stable)
4. Choose **MSVC 2022 64-bit** (or your compiler version)
5. Select **Developer and Designer Tools** → **CMake**
6. Install to default location (typically `C:\Qt`)

#### Option B: Using vcpkg (Alternative)

```powershell
# Install vcpkg if not already installed
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\vcpkg integrate install

# Install Qt6
.\vcpkg install qt6:x64-windows
```

#### Option C: Using Chocolatey

```powershell
choco install cmake qt6-base
```

### Step 2: Verify CMake Installation

```powershell
cmake --version
```

Should output version 3.20 or higher.

### Step 3: Configure and Build

#### Using Qt Official Installer:

Open **Command Prompt** or **PowerShell** in the project directory:

```powershell
# Navigate to project
cd c:\path\to\simplerdbms

# Option 1: Use the build script (recommended)
.\build.bat

# Option 2: Manual configuration
mkdir build
cd build
cmake .. -DQt6_DIR="C:\Qt\6.x.x\msvc2022_64\lib\cmake\Qt6" -G "Visual Studio 17 2022"
cmake --build . --config Release
```

#### Using vcpkg:

```powershell
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:\vcpkg\scripts\buildsystems\vcpkg.cmake"
cmake --build . --config Release
```

### Step 4: Run the Application

```powershell
.\bin\SimpleRDBMS.exe
```

---

## Linux

### Step 1: Install Prerequisites

#### Ubuntu/Debian:

```bash
# Update package list
sudo apt update

# Install build tools
sudo apt install -y build-essential cmake

# Install Qt6
sudo apt install -y qt6-base-dev qt6-core qt6-gui qt6-widgets qt6-sql

# Verify installations
cmake --version
qmake --version
```

#### Fedora/RHEL:

```bash
# Install build tools
sudo dnf install -y gcc-c++ cmake

# Install Qt6
sudo dnf install -y qt6-qtbase qt6-qtbase-devel

# Verify
cmake --version
```

#### Arch Linux:

```bash
# Install from AUR
sudo pacman -S base-devel cmake qt6-base

# Verify
cmake --version
```

### Step 2: Configure and Build

```bash
# Navigate to project
cd /path/to/simplerdbms

# Option 1: Use build script (recommended)
chmod +x build.sh
./build.sh

# Option 2: Manual configuration
mkdir -p build
cd build
cmake .. -G "Unix Makefiles"
cmake --build . --config Release

# Install (optional)
sudo cmake --install . --prefix /usr/local
```

### Step 3: Run the Application

```bash
# From build directory
./bin/SimpleRDBMS

# Or if installed
/usr/local/bin/SimpleRDBMS
```

---

## macOS

### Step 1: Install Prerequisites

#### Using Homebrew (Recommended):

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake qt6

# Verify
cmake --version
```

#### Using Qt Official Installer:

1. Download Qt from [qt.io](https://www.qt.io/download)
2. Run the installer for macOS
3. Select Qt 6.x.x and native macOS
4. Install to default location

### Step 2: Configure and Build

#### Using Homebrew Qt:

```bash
# Navigate to project
cd /path/to/simplerdbms

# Option 1: Use build script
chmod +x build.sh
./build.sh

# Option 2: Manual configuration
mkdir -p build
cd build
export CMAKE_PREFIX_PATH=/usr/local/opt/qt6:$CMAKE_PREFIX_PATH
cmake .. -G "Unix Makefiles"
cmake --build . --config Release
```

#### Using Qt Official Installer:

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_PREFIX_PATH="/path/to/Qt/6.x.x/macos/lib/cmake" -G "Unix Makefiles"
cmake --build . --config Release
```

### Step 3: Run the Application

```bash
# From build directory
./bin/SimpleRDBMS

# Or as macOS app bundle (optional)
./SimpleRDBMS.app/Contents/MacOS/SimpleRDBMS
```

---

## Troubleshooting

### Qt6 Not Found

**Problem:** `Could not find a package configuration file provided by "Qt6"`

**Solution:**
1. Verify Qt6 is installed: `qmake --version`
2. Set CMAKE_PREFIX_PATH:
   ```bash
   export CMAKE_PREFIX_PATH=/path/to/Qt6/lib/cmake:$CMAKE_PREFIX_PATH
   cmake ..
   ```
3. Or use -DQt6_DIR:
   ```bash
   cmake .. -DQt6_DIR="/path/to/Qt6/lib/cmake/Qt6"
   ```

### C++17 Not Supported

**Problem:** Compiler error about C++17 features

**Solution:**
- **Windows:** Update MSVC (use Visual Studio Installer)
- **Linux:** Install newer GCC: `sudo apt install g++-11`
- **macOS:** Update Xcode: `xcode-select --install`

### CMake Version Too Old

**Problem:** `CMake 3.20 or higher is required`

**Solution:**
- **Windows:** Download from [cmake.org](https://cmake.org/download/)
- **Linux:** `sudo apt install cmake` (may need backports)
- **macOS:** `brew upgrade cmake`

### Build Fails with Linking Errors

**Problem:** Linker errors related to Qt

**Solution:**
1. Clean and rebuild:
   ```bash
   rm -rf build
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```
2. Ensure Qt modules are complete:
   - Windows: Qt Maintenance Tool → Modify Installation
   - Linux: Reinstall: `sudo apt install qt6-base-dev`
   - macOS: `brew upgrade qt6`

### Application Won't Run

**Problem:** "libQt6Core.so.6 not found" (Linux) or library not found (macOS)

**Solution:**
- Add Qt library path to environment:
  ```bash
  export LD_LIBRARY_PATH=/path/to/Qt6/lib:$LD_LIBRARY_PATH  # Linux
  export DYLD_LIBRARY_PATH=/path/to/Qt6/lib:$DYLD_LIBRARY_PATH  # macOS
  ```
- Or use `windeployqt` (Windows) / `macdeployqt` (macOS) to bundle libraries

### Out of Memory During Build

**Problem:** Build fails with memory error

**Solution:**
- Limit build parallelism:
  ```bash
  cmake --build . -j 2  # Use 2 cores instead of all
  ```
- Close other applications to free memory

### Permissions Denied

**Problem:** Permission error on build script

**Solution:**
```bash
chmod +x build.sh
./build.sh
```

---

## Development Build vs Release Build

### Debug Build (Development)
```bash
# Includes debug symbols, slower
cmake --build . --config Debug
# or
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

### Release Build (Production)
```bash
# Optimized, no debug symbols, smaller
cmake --build . --config Release
# or
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

---

## IDE Integration

### Visual Studio Code

1. Install extensions:
   - C/C++ by Microsoft
   - CMake Tools by Microsoft
   - Qt Tools by Qt Company

2. Create `.vscode/settings.json`:
```json
{
    "cmake.cmakePath": "/usr/bin/cmake",
    "cmake.configureSettings": {
        "CMAKE_PREFIX_PATH": "/path/to/Qt6/lib/cmake"
    }
}
```

### Visual Studio 2022

1. Open as folder
2. CMake will auto-detect and configure
3. Select proper Qt path if needed

### Qt Creator

1. Open `CMakeLists.txt` as project
2. Select Qt 6.x kit
3. Configure and build

---

## Clean Build

To perform a clean build:

```bash
# Remove build directory
rm -rf build           # Linux/macOS
rmdir /s build         # Windows

# Create fresh build
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

---

## Next Steps

After successful build:
1. Run the application: `./bin/SimpleRDBMS`
2. Check the [README.md](README.md) for usage
3. Read [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md) for development
4. See [Contributing Guide](CONTRIBUTING.md) for development

---

## Support

If you encounter build issues:
1. Check your `CMakeError.log` in the build directory
2. Review [Troubleshooting](#troubleshooting) section
3. Verify all prerequisites are installed
4. Try a clean build

**Common Help Resources:**
- [Qt Documentation](https://doc.qt.io)
- [CMake Documentation](https://cmake.org/documentation)
- [GitHub Issues](https://github.com/simplerdbms/simplerdbms/issues)

---

**Last Updated:** January 15, 2026
