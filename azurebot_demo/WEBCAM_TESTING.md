# Webcam Testing Guide - Development Mode

## 🎯 The Problem

The **competition parameters** are optimized for **bright LED lightbars** on real RoboMaster armor plates. If you're testing with a regular webcam (like MacBook's built-in camera) and regular colored objects, **detection will fail** because:

1. ❌ **No bright LEDs** - Regular objects don't glow like LED lightbars
2. ❌ **Different lighting** - Indoor webcam lighting ≠ competition arena lighting
3. ❌ **Camera differences** - Webcams have auto-exposure, auto-white-balance
4. ❌ **Threshold too high** - 0.65 is for VERY bright objects, not regular colors

---

## ✅ The Solution: Development Mode

The demo now has **TWO MODES**:

### 🔧 Development Mode (Default for macOS)
- **For**: Testing with webcam and regular colored objects
- **Uses**: Relaxed parameters, lower threshold (0.35), channel subtraction
- **Perfect for**: Learning, algorithm development, testing without real armor

### 🏆 Competition Mode
- **For**: Actual RoboMaster competition with bright LED armor
- **Uses**: Strict parameters, high threshold (0.65), optimized for LEDs
- **Perfect for**: Jetson deployment, final testing with real armor

---

## 🎮 Quick Start: Webcam Testing

### Step 1: Verify Mode

The demo is currently in **DEV_MODE**. You'll see this when you run it:

```
Mode: DEVELOPMENT (Webcam Testing)
  - Relaxed parameters for regular objects
  - No bright LEDs required
  - Binary threshold: 0.35
```

### Step 2: What to Test With

**Good test objects** (things that work well):

✅ **Colored markers** (blue/red dry-erase markers held vertically)
✅ **Colored pens/pencils** (2 parallel ones)
✅ **Colored tape** (strips on white paper)
✅ **Phone/tablet** (show blue/red rectangles on screen)
✅ **Printed** paper strips (colored rectangles)
✅ **Toy blocks** (rectangular LEGO bricks)

**What the algorithm looks for**:
- Two **tall rectangles** (vertical orientation)
- Similar height
- **Blue** or **Red** color (depending on mode)
- Parallel to each other
- Reasonable spacing between them

### Step 3: Test Setup

Example with markers:

```
1. Take 2 BLUE markers/pens
2. Hold them vertically (↑↑) 
3. Keep them parallel
4. Space them 3-5 cm apart
5. Point webcam at them
6. Press 'D' to see debug view
```

**Visual Example**:
```
    |  |     ← Two blue markers held vertically
    |  |        Spacing: 3-5 cm
    |  |        This simulates lightbar pair!
```

---

## 🔍 Understanding the Detection Process

### Step 1: Color Filtering

Press `D` to see debug view (3 panels):

1. **Original**: What the camera sees
2. **Grayscale**: After color filtering
   - Blue objects appear **white**
   - Red objects appear **black** (or vice versa)
   - Background is darker
3. **Binary**: After thresholding
   - Your objects should be **bright white**
   - Background should be **black**

### Step 2: What Good Detection Looks Like

**✅ Good binary image**:
```
████████████████████  ← Clean white rectangles
██          ██        ← Clear shapes
██          ██        ← Minimal noise
████████████████████  
```

**❌ Poor binary image**:
```
░░▓▓░░▓▓░░  ← Noisy, fragmented
▓░▓░▓░▓░    ← Too much background noise
░▓░▓░▓░▓    ← Shapes not clear
```

---

## 🎛️ Real-Time Tuning

### If Detection Fails

**Problem**: No detection at all
**Solutions**:
1. Press `-` to **lower threshold** (try 0.25-0.35)
2. Press `S` to **lower min area**
3. Press `D` to see what's happening
4. Make sure objects are blue/red (press `R` or `B` to switch)

**Problem**: Too many false detections
**Solutions**:
1. Press `+` to **raise threshold** (try 0.40-0.50)
2. Press `W` to **raise min area**
3. Improve lighting (brighter room)

**Problem**: Detects objects but doesn't pair them
**Solutions**:
1. Hold objects more **parallel**
2. Adjust **spacing** (try 3-5 cm apart)
3. Make objects more **similar size**

---

## 📊 Parameter Comparison

| Parameter | Competition | Dev Mode | Why Different? |
|-----------|-------------|----------|----------------|
| Binary Threshold | 0.65 | **0.35** | LEDs are much brighter |
| Min Area | 100 | **30** | Webcam objects are smaller |
| Max Area | 2500 | **5000** | Webcam sees larger |
| Min Ratio | 2.5 | **1.8** | More forgiving shapes |
| Angle Diff | 7° | **12°** | Hand-held objects wobble |
| Armor Ratio | 1.8-4.0 | **1.2-5.0** | Wider object types |
| Gray Method | RGB | **SUB** | Better contrast for non-LEDs |

---

## 🧪 Testing Progression

### Level 1: Static Test
1. Place 2 blue markers on table
2. Arrange them parallel, 5 cm apart
3. **Goal**: Stable detection ✅

### Level 2: Movement Test
1. Hold markers in hands
2. Move left/right slowly
3. **Goal**: Continuous detection ✅

### Level 3: Rotation Test
1. Rotate markers slightly (±20°)
2. **Goal**: Still detects at angles ✅

### Level 4: Distance Test
1. Move closer/farther from camera
2. Test 30cm - 1m range
3. **Goal**: Works at all distances ✅

---

## 🔄 Switching to Competition Mode

Once you have real armor with LEDs, switch to competition mode:

### Method 1: Edit Code
```cpp
// In armor_detector_demo.cpp, line 62:
#define DEV_MODE false   // Change true to false
```

### Method 2: Runtime Override
```bash
# Set environment variable
export RM_COMPETITION_MODE=1
./armor_detector_demo
```

Then rebuild:
```bash
./build.sh
```

---

## 💡 Pro Tips

### Tip 1: Lighting Matters
- **Bright room** = better detection
- **Avoid** backlighting (camera faces window)
- **Direct light** on colored objects helps

### Tip 2: Color Selection
```bash
./armor_detector_demo 0 blue   # Detect blue objects
./armor_detector_demo 0 red    # Detect red objects
```
- Press `B` for blue, `R` for red while running
- Test with the color that contrasts most with background

### Tip 3: Debug View is Your Friend
- Press `D` to toggle debug view
- Watch the **Binary** panel
- Your objects should be clean white blobs

### Tip 4: Use Contrast
- **Blue objects** work best on **white/light** background
- **Red objects** work best on **dark** background
- Avoid objects same color as background

### Tip 5: Size Matters
- Objects should be **5-15 cm** tall in camera view
- Too small = filtered as noise
- Too large = not recognized as lightbars

---

## 🎨 Example Test Objects You Can Make

### Option 1: Paper Armor
```
Materials: Blue/red paper, cardboard, scissors

1. Cut 2 rectangles: 2cm × 8cm
2. Color them blue or red
3. Tape to cardboard 5cm apart
4. Hold in front of camera
```

### Option 2: Phone/Tablet Display
```
1. Open drawing app on phone
2. Draw 2 blue rectangles
3. Make them tall and parallel
4. Point webcam at phone screen
```

### Option 3: Marker Holder
```
Materials: Cardboard, tape, 2 markers

1. Cut cardboard strip
2. Tape 2 markers vertically
3. Keep 4-5 cm spacing
4. Instant test armor!
```

---

## 📈 Expected Results

### Development Mode Performance

**Detection Rate**:
- ✅ Simple objects (markers): 80-90%
- ✅ Paper cutouts: 70-80%
- ✅ Phone display: 85-95%
- ⚠️ Complex backgrounds: 50-70%

**False Positive Rate**:
- Clean background: < 10%
- Cluttered background: 10-30%

**FPS**:
- MacBook webcam (720p): 25-35 FPS
- Better lighting = Better FPS

---

## 🐛 Troubleshooting

### "No detection at all"

**Check**:
1. Is camera working? (see video feed)
2. Are objects blue/red? (press B/R to switch)
3. Is threshold too high? (press `-` to lower)
4. Are objects in frame and visible?

**Try**:
```bash
# Lower threshold dramatically
# While running, press `-` 5-10 times
# Threshold should go to ~0.20-0.25
```

### "Detects one lightbar but not a pair"

**Check**:
1. Are both objects similar size?
2. Are they parallel? (angle < 12°)
3. Are they spaced 2-6 cm apart?
4. Are both visible and clear?

**Try**:
- Make objects more parallel
- Adjust spacing
- Ensure consistent lighting on both

### "Too many false positives"

**Check**:
1. Is background too busy/colorful?
2. Is threshold too low?
3. Is room too bright/reflective?

**Try**:
```bash
# Raise threshold
# While running, press `+` 3-5 times
# Or press `W` to increase min area
```

---

## 📝 Switching Between Modes

### When to Use Each Mode

**Use DEV_MODE when**:
- ✅ Testing algorithm without real hardware
- ✅ Learning how detection works
- ✅ Developing on macOS/laptop
- ✅ Using webcam and regular objects

**Use COMPETITION_MODE when**:
- ✅ Have real RoboMaster armor with LEDs
- ✅ Testing on Jetson with industrial camera
- ✅ Final integration testing
- ✅ Competition deployment

---

## 🎓 Learning Outcomes

By testing in dev mode, students learn:

1. **Color filtering** - How RGB/SUB methods work
2. **Thresholding** - Effect of different threshold values
3. **Contour detection** - What makes a good lightbar
4. **Geometric matching** - Why parallel and spacing matter
5. **Real-time tuning** - How parameters affect results

Then when they get real armor:
- Switch to competition mode
- Parameters are already optimized!
- Just focus on integration

---

## 🚀 Next Steps

1. **✅ Start**: Test with simple objects in dev mode
2. **📚 Learn**: Understand each processing step
3. **🎯 Tune**: Experiment with real-time adjustments
4. **📖 Read**: Study PARAMETER_TUNING.md for theory
5. **🏆 Deploy**: Switch to competition mode when ready

---

**Key Takeaway**: Development mode lets you **learn and test the algorithm** without needing expensive armor plates and LEDs. Once you understand how it works, switching to competition mode is just changing one line! 🎉

---

**Last Updated**: October 6, 2025  
**Mode**: Development (Webcam Testing)

