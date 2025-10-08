# Armor Detection Parameter Tuning Guide

This guide explains the optimized parameters used in the armor detector, based on [TJURM competition experience](https://github.com/HHgzs/TJURM-2024/wiki/TJURM%E8%87%AA%E7%9E%84%E7%AE%97%E6%B3%95Wiki).

---

## 📊 Parameter Changes Summary

### Binary Threshold
```cpp
binary_threshold = 0.65  // Was: 0.5
```
**Why**: RoboMaster armor LEDs are very bright. Higher threshold (0.6-0.7) filters background noise while keeping bright lightbars.

**Tuning**:
- Too low (< 0.5): False positives from reflections
- Optimal (0.6-0.7): Clean lightbar detection
- Too high (> 0.8): Miss dimmer lightbars

---

## 🔦 Lightbar Detection Parameters

### Area Constraints

```cpp
min_lightbar_area = 100.0   // Was: 50.0
max_lightbar_area = 2500.0  // Was: 3000.0
```

**Why**: 
- Real armor lightbars have consistent sizes
- Smaller min filters noise from specular highlights
- Tighter max prevents detecting entire armor plates as lightbars

**Typical lightbar areas at different distances**:
- 1m: 800-1500 pixels²
- 3m: 200-600 pixels²
- 5m: 100-300 pixels²

### Aspect Ratio

```cpp
min_lightbar_ratio = 2.5   // Was: 2.0
max_lightbar_ratio = 12.0  // Was: 10.0
```

**Why**: 
- Armor lightbars are tall rectangles (height/width)
- Standard ratio: 3-8
- Angled lightbars can appear taller (up to 12)

**Common ratios**:
- Frontal view: 3.0-5.0
- 30° angle: 4.0-7.0
- 45° angle: 5.0-10.0

### Angle Constraint

```cpp
max_lightbar_angle = 35.0   // Was: 45.0
```

**Why**: 
- Stricter angle reduces false positives
- Lightbars at > 35° are usually not valid targets
- Competition data shows most valid detections < 30°

---

## 🎯 Armor Matching Parameters

### Angle Difference

```cpp
max_angle_diff = 7.0   // Was: 8.0
```

**Why**: 
- Lightbar pairs must be nearly parallel
- Competition standard: < 10°
- Stricter (7°) improves precision

**Effects**:
- < 5°: May miss slight rotations
- 5-10°: Optimal range
- \> 10°: Too many false matches

### Length Ratio

```cpp
max_length_ratio = 1.8   // Was: 1.5
```

**Why**: 
- Paired lightbars should be similar length
- Allows for perspective distortion
- Ratio = longer / shorter

**Interpretation**:
- 1.0: Perfect match
- 1.0-1.5: Very good match
- 1.5-2.0: Acceptable with perspective
- \> 2.0: Likely false pair

### Armor Width/Height Ratio

```cpp
min_armor_ratio = 1.8   // Was: 1.5
max_armor_ratio = 4.0   // Was: 4.5
```

**Why**: Different armor types have distinct aspect ratios

**RoboMaster Armor Types**:

| Type | Actual Size (mm) | Typical Ratio |
|------|------------------|---------------|
| Small Infantry | 125 × 55 | 2.3 |
| Large Infantry | 230 × 55 | 4.2 |
| Balance | 230 × 60 | 3.8 |
| Hero | 230 × 127 | 1.8 |
| Outpost | 230 × 127 | 1.8 |

**Recommended settings**:
- **Infantry robot**: 1.8-3.5 (catches small armor)
- **Hero/Sentry**: 1.5-4.0 (includes large armor)
- **General purpose**: 1.8-4.0 (current setting)

### Center Alignment

```cpp
max_center_offset = 0.4   // Was: 0.5
```

**Why**: 
- Lightbar centers should align horizontally
- Normalized by average lightbar length
- Stricter = fewer false positives

**Formula**: `offset = vertical_distance / avg_length`

**Typical values**:
- Perfect alignment: 0.0-0.1
- Good: 0.1-0.3
- Acceptable: 0.3-0.5
- Suspicious: > 0.5

### Distance Ratio

```cpp
distance_ratio: 2.0-4.5   // Was: 1.5-5.0
```

**Why**: 
- Distance between lightbars relative to their length
- Too close: overlapping or noise
- Too far: unlikely to be a pair

**Formula**: `distance_ratio = center_distance / avg_length`

**Interpretation**:
- < 2.0: Too close (noise/artifacts)
- 2.0-3.0: Small armor
- 3.0-4.5: Standard/Large armor
- \> 4.5: Too far apart

---

## 🎨 Initial Contour Filtering

Applied in `getLightbarsFromContours()`:

```cpp
min_rect_side = 8.0    // Was: 5.0
max_rect_side = 800.0  // Was: 5000.0
min_value_area = 50.0  // Was: 10.0
min_ratio_area = 0.6   // Was: 0.5
max_angle = 60.0       // Was: 90.0
```

### Why These Changes?

1. **min_rect_side** (8.0): Filters tiny noise pixels
2. **max_rect_side** (800.0): Prevents detecting huge blobs
3. **min_value_area** (50.0): Minimum contour area (stricter)
4. **min_ratio_area** (0.6): Contour must fill ≥60% of bounding rect
5. **max_angle** (60.0): Initial coarse filter

---

## 🔧 Runtime Tuning

### Keyboard Controls

While running the demo, you can adjust parameters in real-time:

| Key | Action | When to Use |
|-----|--------|-------------|
| `+` | Increase threshold | Too many false positives |
| `-` | Decrease threshold | Missing lightbars |
| `W` | Increase min area | Too much noise |
| `S` | Decrease min area | Missing small lightbars |
| `D` | Toggle debug view | See preprocessing steps |

### Debug View Interpretation

Press `D` to show three views side-by-side:
1. **Original**: Raw camera feed
2. **Grayscale**: After color filtering
3. **Binary**: After thresholding

**What to look for**:
- ✅ **Good**: Lightbars are bright white, background is black
- ⚠️ **Too sensitive**: Lots of white noise in background
- ⚠️ **Too strict**: Lightbars are fragmented or missing

---

## 🎯 Environment-Specific Tuning

### Bright Environment (Outdoor/High Light)
```cpp
binary_threshold = 0.75  // Higher to filter bright background
min_lightbar_area = 150.0  // Larger threshold
```

### Dim Environment (Indoor/Low Light)
```cpp
binary_threshold = 0.55  // Lower to catch dimmer LEDs
min_lightbar_area = 80.0   // Lower to catch smaller areas
```

### Camera Exposure Settings

Optimal camera settings for RoboMaster:
```cpp
cap.set(CAP_PROP_EXPOSURE, -7);      // Manual exposure, adjust -7 to -9
cap.set(CAP_PROP_GAIN, 0);           // Minimize gain
cap.set(CAP_PROP_AUTO_EXPOSURE, 0.25); // Manual exposure mode
```

**Note**: These may not work on all cameras, especially webcams.

---

## 📈 Performance vs. Accuracy Trade-offs

### More False Positives (Looser Parameters)
**Use when**: Need to catch all possible armors, have downstream filtering
```cpp
binary_threshold = 0.55
max_angle_diff = 10.0
max_armor_ratio = 5.0
```

### Fewer False Positives (Stricter Parameters) ⭐ **Current**
**Use when**: Need high precision, competition settings
```cpp
binary_threshold = 0.65
max_angle_diff = 7.0
max_armor_ratio = 4.0
```

### Balanced (Recommended for Learning)
**Use when**: Testing and learning the system
```cpp
binary_threshold = 0.60
max_angle_diff = 8.0
max_armor_ratio = 4.5
```

---

## 🧪 Testing Methodology

### Step 1: Test with Static Armor
1. Place armor plate at 1-5 meters
2. Verify detection at all distances
3. Tune `min_lightbar_area` if needed

### Step 2: Test with Movement
1. Move armor left/right, up/down
2. Check detection stability
3. Tune `max_angle_diff` if losing tracking

### Step 3: Test with Rotation
1. Rotate armor ±30°
2. Verify detection at angles
3. Adjust `max_lightbar_angle` if needed

### Step 4: Test with Multiple Armors
1. Place multiple armors in view
2. Verify correct pairing
3. Tune `max_center_offset` if mis-pairing

---

## 📚 Further Reading

- **TJURM Wiki**: https://github.com/HHgzs/TJURM-2024/wiki
- **OpenRM Source**: Examine `src/pointer/` for implementation details
- **Competition Videos**: https://www.bilibili.com/video/BV1jApUe1EPT/

---

## 💡 Pro Tips

1. **Start conservative**: Use stricter parameters, then loosen if needed
2. **Use debug view**: Press `D` frequently while tuning
3. **Test at competition lighting**: RoboMaster arenas are bright!
4. **Log your parameters**: Document what works for your setup
5. **Test all armor types**: Don't just optimize for infantry armor

---

## ✅ Validation Checklist

Before deploying to Jetson:

- [ ] Detects armor at 1-7 meters
- [ ] < 5% false positive rate
- [ ] Works with ±30° armor rotation
- [ ] Stable detection during movement
- [ ] Distinguishes between armor types
- [ ] Runs at > 30 FPS
- [ ] Works in competition lighting

---

## 🔄 Common Issues and Fixes

### Issue: Too Many False Positives
**Solutions**:
- Increase `binary_threshold` (0.65 → 0.70)
- Increase `min_lightbar_area` (100 → 150)
- Decrease `max_angle_diff` (7.0 → 5.0)

### Issue: Missing Some Armors
**Solutions**:
- Decrease `binary_threshold` (0.65 → 0.60)
- Decrease `min_lightbar_area` (100 → 80)
- Increase `max_armor_ratio` (4.0 → 4.5)

### Issue: Mis-pairing Lightbars
**Solutions**:
- Decrease `max_center_offset` (0.4 → 0.3)
- Decrease `max_angle_diff` (7.0 → 5.0)
- Tighten `distance_ratio` range (2.0-4.0)

### Issue: Detection Unstable
**Solutions**:
- Add temporal filtering (Kalman filter)
- Loosen `max_angle_diff` slightly
- Check camera exposure settings

---

**Last Updated**: October 2025  
**Based on**: TJURM 2024 Competition Experience

