# Cross-Platform Development Guide
## Building OpenRM Demos on macOS and Linux

This guide ensures the demos work reliably for all students, regardless of their development environment.

---

## 🚨 Common Issue: Conda/Anaconda Conflicts

### The Problem

If you have **Anaconda** or **Miniconda** installed and get this error:

```
ERROR: flag 'flagfile' was defined more than once
```

**Cause**: Both conda and your system package manager (Homebrew on macOS, apt on Linux) provide the `gflags` library. When both are active, they create duplicate symbol conflicts.

### The Solution

**Always deactivate conda before building/running OpenRM projects:**

```bash
# Deactivate conda
conda deactivate

# Verify it's deactivated
echo $CONDA_PREFIX    # Should be empty

# Clean rebuild
cd azurebot_demo
./build.sh clean

# Run demos
cd build
./armor_detector_demo
```

---

## 📋 Platform-Specific Setup

### macOS (Development Environment)

**1. Install Homebrew dependencies:**
```bash
brew install opencv ceres-solver eigen
```

**2. Build OpenRM:**
```bash
cd OpenRM-2024
./run.sh
```

**3. Build demos:**
```bash
# Make sure conda is deactivated!
conda deactivate

cd azurebot_demo
./build.sh
```

**4. Run:**
```bash
cd build
./camera_test           # Test webcam
./armor_detector_demo   # Full detection
```

---

### Linux (Jetson/Production Environment)

**1. Install system dependencies:**
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libopencv-dev \
    libceres-dev \
    libeigen3-dev \
    git
```

**2. Build OpenRM:**
```bash
cd OpenRM-2024
./run.sh
```

**3. Build demos:**
```bash
# Make sure conda is deactivated!
conda deactivate  # If using conda

cd azurebot_demo
./build.sh
```

**4. Run:**
```bash
cd build
./camera_test 0         # Test camera ID 0
./armor_detector_demo 0 blue  # Detect blue armor
```

---

## ⚠️ Build Script Features

The updated `build.sh` now includes:

### 1. **Conda Detection**
- Warns if conda is active
- Prevents accidental builds with wrong libraries

### 2. **Clean Build Option**
```bash
./build.sh clean   # Removes old build and starts fresh
```

### 3. **Cross-Platform CPU Detection**
- Automatically uses all available CPU cores
- macOS: Uses `sysctl -n hw.ncpu`
- Linux: Uses `nproc`

### 4. **Library Path Prioritization**
- macOS: Prioritizes Homebrew (`/opt/homebrew`)
- Linux: Prioritizes system libraries (`/usr/lib`, `/usr/local`)
- Both: Explicitly ignores conda paths

---

## 🔍 Troubleshooting

### Issue 1: "Cannot find OpenRM"

**Symptom:**
```
CMake Error: Could not find a package configuration file provided by "OpenRM"
```

**Solution:**
```bash
# Install OpenRM first
cd /path/to/OpenRM-2024
sudo ./run.sh

# Verify installation
ls /usr/local/lib/libopenrm_*
ls /usr/local/include/openrm/
```

---

### Issue 2: gflags Conflict

**Symptom:**
```
ERROR: flag 'flagfile' was defined more than once
```

**Solution:**
```bash
conda deactivate
cd azurebot_demo
rm -rf build
./build.sh
```

---

### Issue 3: Camera Not Found (macOS)

**Symptom:**
```
ERROR: Cannot open camera 0
```

**Solutions:**
1. **Grant camera permissions:**
   - System Settings → Privacy & Security → Camera
   - Enable Terminal/iTerm

2. **Try different camera IDs:**
   ```bash
   ./camera_test 0
   ./camera_test 1
   ```

3. **Check if camera is in use:**
   - Close Photo Booth, Zoom, or other camera apps

---

### Issue 4: Low FPS

**Solutions:**
- Disable debug view (press `D` in the demo)
- Lower camera resolution in code
- Close other applications

---

## 📝 Best Practices for Students

### DO ✅
- ✅ Always deactivate conda before building
- ✅ Use system package managers (Homebrew/apt) for C++ libraries
- ✅ Test on macOS first, then deploy to Linux
- ✅ Use `./build.sh clean` if you change dependencies

### DON'T ❌
- ❌ Build with conda active (causes library conflicts)
- ❌ Mix conda and system libraries
- ❌ Use `conda install opencv` for C++ projects
- ❌ Ignore build warnings

---

## 🎓 Development Workflow

### Phase 1: Algorithm Development (macOS)
```bash
# On your MacBook
cd OpenRM-2024/azurebot_demo
./build.sh clean
cd build
./armor_detector_demo   # Use built-in webcam

# Iterate on algorithm parameters
# Edit armor_detector_demo.cpp
# Rebuild: cd .. && ./build.sh
```

### Phase 2: Testing (macOS with USB Camera)
```bash
# Connect USB camera
./camera_test 1         # Find camera ID
./armor_detector_demo 1 red  # Test with real setup
```

### Phase 3: Deployment (Jetson)
```bash
# On Jetson Nano/Xavier
git pull  # Update code
cd OpenRM-2024
./run.sh  # Rebuild library

cd azurebot_demo
./build.sh clean  # Clean build
cd build
./armor_detector_demo 0 blue  # Run with GigE/USB3 camera
```

---

## 🔧 Environment Check Script

Save this as `check_env.sh`:

```bash
#!/bin/bash

echo "=== OpenRM Environment Check ==="
echo ""

# Check conda
if [[ -n "$CONDA_PREFIX" ]]; then
    echo "❌ Conda is ACTIVE: $CONDA_PREFIX"
    echo "   Run: conda deactivate"
else
    echo "✅ Conda is deactivated"
fi
echo ""

# Check OpenRM installation
if [ -f /usr/local/lib/libopenrm_attack.dylib ] || [ -f /usr/local/lib/libopenrm_attack.so ]; then
    echo "✅ OpenRM is installed"
else
    echo "❌ OpenRM not found"
    echo "   Run: cd OpenRM-2024 && sudo ./run.sh"
fi
echo ""

# Check OpenCV
if pkg-config --exists opencv4; then
    echo "✅ OpenCV $(pkg-config --modversion opencv4)"
else
    echo "❌ OpenCV not found"
fi
echo ""

# Check OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Platform: macOS"
    echo "CPU Cores: $(sysctl -n hw.ncpu)"
else
    echo "Platform: Linux"
    echo "CPU Cores: $(nproc)"
fi
echo ""

echo "=== Ready to build! ==="
```

Make it executable:
```bash
chmod +x check_env.sh
./check_env.sh
```

---

## 📞 Getting Help

**Before asking for help, try:**
1. Run `./check_env.sh` (see above)
2. Deactivate conda and rebuild clean
3. Check the error message against this guide

**When reporting issues, include:**
- Operating system (macOS/Linux)
- Output of `./check_env.sh`
- Full error message
- Output of `cmake --version`

---

## 🎯 Summary

| Issue | Solution |
|-------|----------|
| gflags conflict | `conda deactivate` before building |
| Can't find OpenRM | `cd OpenRM-2024 && sudo ./run.sh` |
| Camera not working | Check permissions, try different IDs |
| Build warnings | Use `./build.sh clean` |
| Cross-platform | Use system packages (Homebrew/apt) |

**Golden Rule**: Keep conda for Python projects, use system packages for C++ projects! 🎉

---

*Last updated: October 2025*

