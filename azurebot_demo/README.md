# AzureBot Demo - OpenRM-2024

Complete armor detection demonstration using the OpenRM-2024 library.

## 🎯 Features

- **Two Modes**: DEV (webcam testing) and COMPETITION (real armor LEDs)
- **Cross-Platform**: Works on both macOS (development) and Linux (Jetson deployment)
- **Real-Time Detection**: Uses OpenCV VideoCapture for camera input
- **Complete Pipeline**: Color preprocessing → Lightbar detection → Armor matching
- **Interactive Controls**: Switch colors, toggle debug view in real-time
- **Performance Monitoring**: FPS and detection time display

### 🔧 Development vs Competition Mode

| Aspect | Dev Mode 🔧 | Competition Mode 🏆 |
|--------|-------------|---------------------|
| **Use Case** | Webcam + regular objects | Real armor + bright LEDs |
| **Threshold** | 0.35 (low) | 0.65 (high) |
| **Parameters** | Relaxed | Strict (TJURM standard) |
| **Perfect For** | Learning, testing | Jetson deployment |

**Current Mode**: Development (good for MacBook webcam testing)  
**See**: [WEBCAM_TESTING.md](WEBCAM_TESTING.md) for webcam testing guide

## 📋 Prerequisites

### Quick Environment Check

```bash
cd azurebot_demo
./check_env.sh    # Checks if all dependencies are ready
```

### Full Setup

Make sure OpenRM-2024 is already built and installed:

```bash
cd /path/to/OpenRM-2024
sudo ./run.sh
```

This will install the OpenRM libraries to `/usr/local/lib/` and headers to `/usr/local/include/openrm/`.

**⚠️ Important**: If you use Anaconda/Miniconda, **deactivate it** before building:
```bash
conda deactivate
```
See [CROSS_PLATFORM_GUIDE.md](CROSS_PLATFORM_GUIDE.md) for details.

## 🔨 Building the Demo

### Quick Build (Recommended)

```bash
cd azurebot_demo
./build.sh
```

### Manual Build

```bash
cd azurebot_demo
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

## 🚀 Running the Demos

### 1. Camera Test (Simple)

First, verify your camera is working:

```bash
./build/camera_test          # Use default camera (ID 0)
./build/camera_test 1        # Use camera ID 1
```

**Expected Output**: Live video feed with FPS counter

### 2. Armor Detector (Full Pipeline)

Run the complete armor detection:

```bash
# Basic usage (default camera, blue armor)
./build/armor_detector_demo

# Specify camera and color
./build/armor_detector_demo 0 blue   # Camera 0, blue armor
./build/armor_detector_demo 1 red    # Camera 1, red armor
```

**🎯 For MacBook Webcam Testing**:
The demo is in **DEV_MODE** by default. Test with:
- 2 blue/red markers held vertically (||)
- Colored paper strips
- Blue/red rectangles on phone screen
- See [WEBCAM_TESTING.md](WEBCAM_TESTING.md) for detailed guide

**Interactive Controls**:
- `Q` or `ESC`: Quit program
- `R`: Switch to RED armor detection
- `B`: Switch to BLUE armor detection  
- `D`: Toggle debug view (shows preprocessing steps)
- `+/-`: Adjust binary threshold (critical for webcam!)
- `W/S`: Adjust minimum lightbar area

## 📊 Understanding the Output

### Main View
- **Green Boxes**: Detected lightbars (armor light strips)
- **Red Boxes**: Matched armor pairs
- **Yellow Cross**: Armor center point
- **Stats Panel**: FPS, detection time, armor count

### Debug View (Press `D`)
Shows three-channel visualization:
- **Red Channel**: Binary image (thresholded)
- **Green Channel**: Grayscale (color-filtered)
- **Blue Channel**: Original image
- Overlaid with all detection results

## 🎨 Detection Algorithm

```
Input Image
    ↓
Color Filtering (Red/Blue separation)
    ↓
Binary Thresholding
    ↓
Contour Detection
    ↓
Lightbar Filtering (size, ratio, angle)
    ↓
Lightbar Pairing (geometry matching)
    ↓
Armor Validation
    ↓
Output with Bounding Boxes
```

## 🔧 Tuning Parameters

The demo now uses **competition-optimized parameters** from [TJURM team](https://github.com/HHgzs/TJURM-2024/wiki):

```cpp
struct DetectionConfig {
    // Preprocessing
    double binary_threshold = 0.65;     // Competition standard: 0.6-0.7
    
    // Lightbar detection (competition-tested)
    double min_lightbar_area = 100.0;   // Filter noise
    double max_lightbar_area = 2500.0;  // Reasonable max
    double min_lightbar_ratio = 2.5;    // Tall rectangles
    double max_lightbar_ratio = 12.0;   // Allow angled views
    double max_lightbar_angle = 35.0;   // Stricter angle
    
    // Armor matching (TJURM standard)
    double max_angle_diff = 7.0;        // Strict parallel
    double max_length_ratio = 1.8;      // Similar lengths
    double min_armor_ratio = 1.8;       // Standard infantry
    double max_armor_ratio = 4.0;       // Balance/Hero
};
```

**📖 See [PARAMETER_TUNING.md](PARAMETER_TUNING.md) for detailed explanation**

### Quick Tuning

Runtime adjustments (no rebuild needed):
- Press `+/-` to adjust binary threshold
- Press `W/S` to adjust minimum lightbar area
- Press `D` to see preprocessing steps

After editing code, rebuild:
```bash
./build.sh
```

## 🐛 Troubleshooting

### Camera Not Found
```
ERROR: Cannot open camera 0
```
**Solution**: Try different camera IDs (0, 1, 2, etc.) or check camera permissions

### Low FPS
**Possible causes**:
- High resolution (try lower resolution in code)
- Debug view enabled (press `D` to disable)
- Slow hardware

### No Armors Detected

**If using webcam (MacBook camera)**:
1. ✅ Demo is in DEV_MODE (you'll see this in startup message)
2. Press `-` several times to **lower threshold** (try 0.25-0.35)
3. Press `D` to see debug view
4. Test with **2 blue markers** held vertically (||)
5. Make sure markers are parallel and 3-5 cm apart
6. See [WEBCAM_TESTING.md](WEBCAM_TESTING.md) for complete guide

**If using real armor with LEDs**:
- Switch to COMPETITION mode (edit line 62 in armor_detector_demo.cpp)
- Press `R` or `B` to switch detection color
- Adjust lighting conditions
- Tune detection parameters (see above)

### gflags Duplicate Symbol Error

**Error message**:
```
ERROR: flag 'flagfile' was defined more than once
```

**Cause**: This happens when both conda and system libraries (Homebrew/apt) provide gflags, causing a symbol conflict.

**Solution**:
```bash
# Deactivate conda environment
conda deactivate

# Clean rebuild the demo
cd azurebot_demo
./build.sh clean

# Run the demo (still with conda deactivated)
cd build
./armor_detector_demo
```

**For students using conda**: Always build and run OpenRM projects **outside** of conda environments to avoid library conflicts. The build script will now warn you if conda is active.

### Build Errors

**If you see "Cannot find OpenRM"**:
```bash
# Make sure OpenRM is installed
cd /path/to/OpenRM-2024
sudo ./run.sh

# Verify installation
ls /usr/local/lib/libopenrm_*
ls /usr/local/include/openrm/
```

**If you see "Cannot find Ceres" or "Cannot find Eigen"**:
```bash
# macOS
brew install ceres-solver eigen

# Ubuntu/Jetson
sudo apt-get install libceres-dev libeigen3-dev
```

## 📚 Code Structure

```
azurebot_demo/
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
├── build.sh                    # Quick build script
├── camera_test.cpp             # Simple camera verification
└── armor_detector_demo.cpp     # Full detection pipeline
```

## 🎓 Learning Resources

### OpenRM Modules Used

| Module | Purpose | Headers |
|--------|---------|---------|
| **pointer** | Color processing, lightbar detection | `<pointer/pointer.h>` |
| **structure** | Data structures (enums, stamps) | `<structure/enums.hpp>` |
| **utils** | Timing, debug printing | `<utils/timer.h>` |

### Key Functions

```cpp
// Color filtering
rm::getGrayScale(src, gray, color, method);

// Binary thresholding
rm::getBinary(gray, binary, threshold, method);

// Lightbar detection
rm::getLightbarsFromContours(contours, lightbars);

// Armor matching
rm::getArmorPairsFromLightbars(lightbars, armor_pairs);
```

## 🎯 Next Steps

1. **For macOS Students**: 
   - Use this demo for algorithm development
   - Test with webcam or video files
   - Debug and tune parameters

2. **For Jetson Deployment**:
   - Transfer code to Jetson
   - Switch to industrial camera (GigE/USB3)
   - Enable CUDA acceleration (if using TensorRT models)
   - Integrate with serial communication for turret control

3. **Advanced Features to Add**:
   - PnP solver for 3D position estimation
   - Kalman filter for motion prediction
   - Multiple armor tracking
   - Target selection logic

## 📞 Support

- **Issues**: Report in the main OpenRM-2024 repository
- **Wiki**: https://github.com/HHgzs/TJURM-2024/wiki
- **Video Tutorial**: https://www.bilibili.com/video/BV1jApUe1EPT/

## 📝 License

Same as OpenRM-2024 main project.

---

**Happy Detecting! 🎉**

