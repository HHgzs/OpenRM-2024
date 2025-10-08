#!/bin/bash

# AzureBot Demo Build Script
# Works on both macOS and Linux

echo "========================================"
echo "  AzureBot Demo Build Script"
echo "========================================"

# Check for conda environment conflicts
if [[ -n "$CONDA_PREFIX" ]]; then
    echo "⚠️  WARNING: Conda environment detected!"
    echo "   This may cause library conflicts (duplicate gflags)."
    echo ""
    echo "   Recommended: Run 'conda deactivate' first, then rebuild."
    echo ""
    read -p "   Continue anyway? (y/N): " -n 1 -r
    echo ""
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Build cancelled. Please deactivate conda and try again:"
        echo "   conda deactivate"
        echo "   ./build.sh"
        exit 1
    fi
fi

# Detect OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macOS"
    CPU_CORES=$(sysctl -n hw.ncpu)
else
    OS="Linux"
    CPU_CORES=$(nproc)
fi

echo "Operating System: $OS"
echo "CPU Cores: $CPU_CORES"
echo ""

# Clean build if requested
if [[ "$1" == "clean" ]] || [[ "$1" == "-c" ]]; then
    echo "Performing clean build..."
    rm -rf build
fi

# Create and enter build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Run CMake with explicit library preferences
echo ""
echo "Running CMake..."
if [[ "$OS" == "macOS" ]]; then
    # On macOS, prioritize Homebrew libraries
    cmake -DCMAKE_PREFIX_PATH="/opt/homebrew;/usr/local" \
          -DCMAKE_IGNORE_PATH="/opt/anaconda3;$CONDA_PREFIX" \
          ..
else
    # On Linux, use system libraries
    cmake -DCMAKE_IGNORE_PATH="$HOME/anaconda3;$HOME/miniconda3;/opt/conda;$CONDA_PREFIX" \
          ..
fi

# Check if cmake succeeded
if [ $? -ne 0 ]; then
    echo ""
    echo "❌ CMake configuration failed!"
    echo ""
    echo "Common fixes:"
    echo "  1. Make sure OpenRM is installed:"
    echo "     cd /path/to/OpenRM-2024 && sudo ./run.sh"
    echo ""
    echo "  2. Install missing dependencies:"
    echo "     macOS:  brew install opencv ceres-solver eigen"
    echo "     Linux:  sudo apt-get install libopencv-dev libceres-dev libeigen3-dev"
    echo ""
    exit 1
fi

# Build
echo ""
echo "Building demos (using $CPU_CORES cores)..."
make -j"$CPU_CORES" || {
    echo ""
    echo "❌ Build failed!"
    exit 1
}

echo ""
echo "========================================"
echo "  ✅ Build Successful!"
echo "========================================"
echo ""
echo "Run demos:"
echo "  cd build"
echo "  ./camera_test             # Test camera"
echo "  ./armor_detector_demo     # Full detection"
echo ""
if [[ -n "$CONDA_PREFIX" ]]; then
    echo "⚠️  NOTE: Conda is still active. If you get runtime errors,"
    echo "   run 'conda deactivate' before running the demos."
    echo ""
fi
echo "See README.md for more usage examples."
echo ""

