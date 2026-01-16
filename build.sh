#!/bin/bash
# SimpleRDBMS Build Script for Linux/macOS

echo ""
echo "========================================"
echo "SimpleRDBMS Build Script"
echo "========================================"
echo ""

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir -p build
fi

cd build

echo "Configuring CMake..."
echo ""

# On macOS, Qt might be installed via Homebrew or official installer
# Set CMAKE_PREFIX_PATH if CMake can't find Qt6
# Example: export CMAKE_PREFIX_PATH=/usr/local/opt/qt6:$CMAKE_PREFIX_PATH

cmake .. -G "Unix Makefiles"

if [ $? -ne 0 ]; then
    echo ""
    echo "[ERROR] CMake configuration failed!"
    echo ""
    echo "Please ensure Qt6 is installed and set CMAKE_PREFIX_PATH:"
    echo "  export CMAKE_PREFIX_PATH=/path/to/Qt6"
    echo "  cmake .."
    echo ""
    echo "Common Qt6 paths:"
    echo "  Linux: /usr/lib/cmake/Qt6 or /opt/Qt/6.x.x/gcc_64"
    echo "  macOS: /usr/local/opt/qt6 (Homebrew)"
    echo ""
    exit 1
fi

echo ""
echo "CMake configuration successful!"
echo ""
echo "Building project..."
echo ""

cmake --build . --config Release

if [ $? -ne 0 ]; then
    echo ""
    echo "[ERROR] Build failed!"
    exit 1
fi

echo ""
echo "========================================"
echo "Build completed successfully!"
echo "========================================"
echo ""
echo "Executable location: ./bin/SimpleRDBMS"
echo ""
echo "To run the application:"
echo "  ./bin/SimpleRDBMS"
echo ""
