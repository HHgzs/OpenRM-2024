# Quick Start: MacBook Webcam Testing

## 🚨 Why Detection Failed Before

Your MacBook camera sees **regular objects**, not **bright LED lightbars**:

| Competition Setup | Your Webcam |
|-------------------|-------------|
| ✅ Bright LEDs (very bright!) | ❌ Regular lighting |
| ✅ Industrial camera | ❌ Webcam with auto-exposure |
| ✅ Threshold 0.65 works | ❌ Threshold 0.65 TOO HIGH |

**Result**: Nothing detected because threshold was optimized for glowing LEDs! 💡

---

## ✅ Solution: Development Mode

The demo NOW uses **relaxed parameters** perfect for webcam testing!

When you run it, you'll see:
```
Mode: DEVELOPMENT (Webcam Testing)
  - Relaxed parameters for regular objects
  - No bright LEDs required
  - Binary threshold: 0.35
```

---

## 🎯 How to Test RIGHT NOW

### Option 1: Two Markers (Easiest!)

1. **Get 2 blue markers/pens** (or red)
2. **Hold them vertically** like this:
   ```
       |    |
       |    |     ← About 5 cm apart
       |    |     ← Keep parallel!
   ```
3. **Run the demo**:
   ```bash
   cd /Users/zhonghaoliu/Desktop/RoboMaster/OpenRM-2024/azurebot_demo/build
   ./armor_detector_demo
   ```
4. **Press `D`** to see debug view
5. **Adjust if needed**: Press `-` to lower threshold

### Option 2: Phone Screen

1. Open drawing app on phone
2. Draw 2 blue vertical rectangles
3. Point webcam at phone
4. Should detect immediately!

### Option 3: Paper Cutouts

1. Cut 2 rectangles from blue paper (2cm × 8cm each)
2. Tape them to white background, 5 cm apart
3. Hold in front of camera
4. Detected! ✅

---

## 🎛️ Key Controls

| Key | Action | When to Use |
|-----|--------|-------------|
| `D` | Toggle debug view | See what camera processes |
| `-` | Lower threshold | If nothing detected |
| `+` | Raise threshold | If too many false detections |
| `B` | Blue detection | Switch to blue objects |
| `R` | Red detection | Switch to red objects |
| `S` | Lower min area | If objects too small |
| `W` | Raise min area | If too much noise |

---

## 🐛 Still Not Working?

### Try This:

```bash
# Run the demo
./armor_detector_demo 0 blue

# Immediately press:
# 1. 'D' to see debug view
# 2. '-' five times (lower threshold to ~0.25)
# 3. Hold 2 blue markers vertically in front of camera
```

### What You Should See:

**Debug View (3 panels)**:
1. **Original**: Your camera feed
2. **Grayscale**: Blue objects appear bright
3. **Binary**: Your objects should be WHITE blobs

If objects are NOT white in binary view:
- Press `-` more times (lower threshold)
- Improve lighting (turn on room lights)
- Use brighter colored objects

---

## 📈 Success Checklist

- [ ] Can see my face in camera feed
- [ ] Pressed `D` to see debug view
- [ ] Have 2 blue/red objects ready
- [ ] Objects are vertical (taller than wide)
- [ ] Objects are 3-7 cm apart
- [ ] Pressed `-` to lower threshold if needed
- [ ] Can see white blobs in binary view
- [ ] **GREEN BOXES appear around objects!** ✅
- [ ] **RED BOX connects them as armor!** ✅✅

---

## 🎓 What's Different Now?

### Before (Competition Parameters):
```cpp
binary_threshold = 0.65    // For bright LEDs
min_area = 100             // Strict filtering
angle_diff = 7°            // Very strict
method = RGB               // For LEDs
```
**Result**: ❌ Nothing detected on webcam

### After (Development Parameters):
```cpp
binary_threshold = 0.35    // For regular objects
min_area = 30              // More relaxed
angle_diff = 12°           // More forgiving
method = SUB               // Better contrast
```
**Result**: ✅ Works with markers/paper!

---

## 🏆 When to Switch to Competition Mode

Once you have **real armor with bright LEDs**:

1. Edit `armor_detector_demo.cpp` line 62:
   ```cpp
   #define DEV_MODE false   // Switch to competition mode
   ```
2. Rebuild: `./build.sh`
3. Deploy to Jetson!

---

## 📚 Learn More

- **[WEBCAM_TESTING.md](WEBCAM_TESTING.md)** - Complete webcam guide
- **[PARAMETER_TUNING.md](PARAMETER_TUNING.md)** - Parameter theory
- **[CROSS_PLATFORM_GUIDE.md](CROSS_PLATFORM_GUIDE.md)** - Setup guide

---

## 💡 TL;DR

```bash
# The fix:
# 1. Demo now uses DEV_MODE (threshold 0.35 instead of 0.65)
# 2. Get 2 blue markers
# 3. Hold vertically, parallel, 5 cm apart
# 4. Run demo
# 5. Press 'D' to debug
# 6. Press '-' if needed
# 7. Should work! ✅
```

**That's it! Now go test it!** 🚀

---

**Pro Tip**: The BEST test object is 2 blue dry-erase markers held in your hands. Instant armor simulator! 🖊️🖊️

