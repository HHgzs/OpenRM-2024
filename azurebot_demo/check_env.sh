#!/bin/bash

echo "=== OpenRM Environment Check ==="
echo ""

# Check conda
if [[ -n "$CONDA_PREFIX" ]]; then
    echo "❌ Conda is ACTIVE: $CONDA_PREFIX"
    echo "   Run: conda deactivate"
    CONDA_OK=false
else
    echo "✅ Conda is deactivated"
    CONDA_OK=true
fi
echo ""

# Check OpenRM installation
if [ -f /usr/local/lib/libopenrm_attack.dylib ] || [ -f /usr/local/lib/libopenrm_attack.so ]; then
    echo "✅ OpenRM is installed"
    OPENRM_OK=true
else
    echo "❌ OpenRM not found"
    echo "   Run: cd OpenRM-2024 && sudo ./run.sh"
    OPENRM_OK=false
fi
echo ""

# Check OpenCV
if pkg-config --exists opencv4 2>/dev/null; then
    echo "✅ OpenCV $(pkg-config --modversion opencv4)"
    OPENCV_OK=true
elif brew list opencv &>/dev/null 2>&1; then
    echo "✅ OpenCV (Homebrew)"
    OPENCV_OK=true
else
    echo "❌ OpenCV not found"
    if [[ "$OSTYPE" == "darwin"* ]]; then
        echo "   Run: brew install opencv"
    else
        echo "   Run: sudo apt-get install libopencv-dev"
    fi
    OPENCV_OK=false
fi
echo ""

# Check Ceres
if pkg-config --exists ceres 2>/dev/null; then
    echo "✅ Ceres $(pkg-config --modversion ceres)"
    CERES_OK=true
elif brew list ceres-solver &>/dev/null 2>&1; then
    echo "✅ Ceres (Homebrew)"
    CERES_OK=true
else
    echo "❌ Ceres not found"
    if [[ "$OSTYPE" == "darwin"* ]]; then
        echo "   Run: brew install ceres-solver"
    else
        echo "   Run: sudo apt-get install libceres-dev"
    fi
    CERES_OK=false
fi
echo ""

# Check OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Platform: macOS"
    echo "CPU Cores: $(sysctl -n hw.ncpu)"
else
    echo "Platform: Linux"
    echo "CPU Cores: $(nproc 2>/dev/null || echo 'unknown')"
fi
echo ""

# Summary
echo "==================================="
if [[ "$CONDA_OK" == true ]] && [[ "$OPENRM_OK" == true ]] && [[ "$OPENCV_OK" == true ]] && [[ "$CERES_OK" == true ]]; then
    echo "✅ Environment is ready!"
    echo "   Run: ./build.sh"
else
    echo "❌ Fix the issues above first"
fi
echo "==================================="

