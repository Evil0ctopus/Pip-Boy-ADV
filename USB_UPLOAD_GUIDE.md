# USB Upload & Monitor Setup Guide

## Overview

The Pip-Boy ADV firmware now supports automated **Build + Upload + Monitor** workflow via USB. This guide explains how to configure and use USB deployment.

---

## Prerequisites

### 1. Hardware Setup
- **M5Stack Cardputer ADV** connected via USB-C cable
- Ensure cable supports data transfer (not charging-only)
- Device should be powered on

### 2. Software Requirements
- **PlatformIO** installed (already configured)
- **CP2102 USB drivers** (auto-installed on Windows 10/11)
- **Visual Studio Code** with PlatformIO extension

---

## Configuration

### platformio.ini Settings

```ini
; Serial Configuration
monitor_speed = 115200
monitor_filters = 
  default
  time

; Upload Configuration (auto-detect if not specified)
upload_speed = 1500000
upload_port =  ; Auto-detect (leave empty)
```

**Auto-Detection**: Leave `upload_port` empty to automatically detect the device.

**Manual Port Configuration** (if auto-detect fails):
1. Open Device Manager (Win + X → Device Manager)
2. Expand "Ports (COM & LPT)"
3. Find "USB Serial Device (COMx)" or "CP2102 USB to UART Bridge (COMx)"
4. Note the COM port number (e.g., COM4)
5. In platformio.ini, set: `upload_port = COM4`

---

## VS Code Tasks

### Available Tasks

#### 1. **Build + Upload + Monitor (USB)** [DEFAULT]
- **Shortcut**: `Ctrl+Shift+B` (default build task)
- **Command**: `pio run --target upload --target monitor`
- **Description**: 
  - Compiles firmware
  - Uploads via USB
  - Opens serial monitor automatically
  - Shows live debug output

#### 2. **Build Only**
- **Description**: Compile firmware without uploading
- **Use Case**: Verify code compiles before connecting device

#### 3. **Monitor Only**
- **Description**: Open serial monitor without building/uploading
- **Use Case**: View debug output from already-running firmware
- **Baud Rate**: 115200

#### 4. **Build & Package OTA Firmware**
- **Description**: Build and package for M5 Launcher installation
- **Output**: `D:\launcher\downloads\PipBoyADV\`
- **Use Case**: SD card deployment

---

## Usage Instructions

### Method 1: Keyboard Shortcut (Fastest)
1. Connect Cardputer via USB
2. Press **`Ctrl+Shift+B`** in VS Code
3. Wait for build → upload → monitor
4. Serial output appears automatically

### Method 2: VS Code Terminal Menu
1. Press **`Ctrl+Shift+P`**
2. Type: **"Tasks: Run Task"**
3. Select: **"Build + Upload + Monitor (USB)"**

### Method 3: Command Line
```powershell
pio run --target upload --target monitor
```

---

## Expected Serial Output

### Boot Sequence (First 10 Seconds)
```
╔════════════════════════════════════════╗
║     Pip-Boy ADV Firmware v1.0.17       ║
╚════════════════════════════════════════╝

=== Stage 1: System Initialization ===
✓ M5Unified initialized
✓ M5.Display hardware initialized
✓ Display rotation set to 1 (landscape)
✓ Brightness set to 128

=== Stage 2: LVGL & UI Initialization ===
[TICK] LVGL tick task started
✓ LVGL core initialized (v8.4.0)
✓ Display buffers allocated (2x 4800 bytes)
✓ Display driver configured (240x135)
✓ Display driver registered
✓ Test pattern displayed
✓ UI shell initialized
✓ Screen refreshed

=== Stage 3: Hardware Modules ===
✓ Audio system initialized
✓ LED controller initialized

=== Stage 4: SD Card & Assets ===
⚠ SD Card not available - assets will be unavailable

=== Stage 5: Sensor Initialization ===
✓ Sensors initialized

=== Stage 6: LoRa Initialization ===
⚠ LoRa initialization failed - check hat connection

=== Stage 7: Bluetooth (Optional) ===
⊘ Bluetooth disabled (save resources)

=== Stage 8: Starting Background Tasks ===
✓ Background tasks started

=== Stage 9: Final UI Setup ===
⊘ Animation disabled (prevents crash without SD assets)
⊘ CRT vignette disabled (prevents display interference)

╔════════════════════════════════════════╗
║      Boot Complete - System Ready     ║
╚════════════════════════════════════════╝
```

### Runtime Loop (Continuous)
```
[LOOP] LVGL running - 200 ticks/sec
[LOOP] LVGL running - 198 ticks/sec
[LOOP] LVGL running - 202 ticks/sec
[LOOP] LVGL running - 199 ticks/sec
...
```

**✅ SUCCESS CRITERIA**: 
- Continuous `[LOOP]` messages every second
- Tick count between 180-220 ticks/sec
- No gaps, pauses, or crashes

---

## Troubleshooting

### Problem: "Please specify upload_port"

**Cause**: PlatformIO cannot detect the device

**Solutions**:
1. **Check USB Connection**
   - Try different USB cable
   - Try different USB port on computer
   - Ensure device is powered on

2. **Install Drivers**
   - Windows 10/11: Drivers auto-install
   - Older Windows: Download CP2102 drivers from Silicon Labs

3. **Manual Port Configuration**
   ```ini
   upload_port = COM4  ; Replace with your actual port
   ```

4. **Check Device Manager**
   - Open Device Manager (Win + X)
   - Look for "Ports (COM & LPT)"
   - Device should appear as "USB Serial Device (COMx)"
   - If showing "Unknown Device", driver installation failed

---

### Problem: Upload Starts But Fails Mid-Transfer

**Cause**: Device not in bootloader mode or driver issue

**Solutions**:
1. **Reset During Upload**
   - Click Upload in VS Code
   - When "Connecting..." appears, press RESET button on device
   - Release after "Writing at 0x..."" appears

2. **Check Upload Speed**
   - Some cables can't handle 1.5Mbps
   - Reduce speed in platformio.ini:
     ```ini
     upload_speed = 921600  ; Half speed
     ```

3. **Disable Antivirus**
   - Some antivirus blocks esptool.py
   - Add PlatformIO folder to exceptions

---

### Problem: Monitor Opens But Shows No Output

**Cause**: Wrong baud rate or device not transmitting

**Solutions**:
1. **Verify Baud Rate**
   - Should be 115200 (already configured)
   - Check platformio.ini: `monitor_speed = 115200`

2. **Reset Device**
   - Press RESET button after monitor opens
   - Boot messages should appear

3. **Check Code**
   - Ensure `Serial.begin(115200)` in setup()
   - Ensure Serial.println() calls exist in loop()

---

### Problem: Black Screen After Upload

**Cause**: Runtime crash (animation system without SD assets)

**Status**: ✅ **FIXED in v1.0.16+**

**Verification**:
- Serial shows continuous `[LOOP] LVGL running`
- If loop stops after 1-2 messages, animation crashed LVGL

**Solutions** (already applied):
- Boot animation disabled
- CRT vignette disabled
- Animation system has NULL checks
- Button A (animation toggle) disabled

---

### Problem: UI Flickers or Glitches

**Cause**: LVGL timing or display driver conflict

**Solutions**:
1. **Check Tick Rate**
   - Should show ~200 ticks/sec
   - If <100, loop is blocked somewhere

2. **Disable Modules One by One**
   - Comment out in main.cpp:
     ```cpp
     // loraHelper.begin();  // Test without LoRa
     // sensors.begin();     // Test without sensors
     ```

3. **Check for Delays**
   - No delay() calls > 10ms in loop()
   - No blocking while() loops
   - No synchronous file I/O

---

## Performance Monitoring

### Key Metrics

**From Serial Output:**
```
[LOOP] LVGL running - 200 ticks/sec
```

- **180-220 ticks/sec**: ✅ Excellent (5ms loop delay)
- **150-179 ticks/sec**: ⚠️ Warning (loop taking >6ms)
- **<150 ticks/sec**: ❌ Critical (blocking operations in loop)
- **Only 1-2 ticks total**: ❌ Crash (loop stopped)

### Debug Commands

**Enable Verbose Logging** (in setup()):
```cpp
Serial.setDebugOutput(true);
```

**Check Free Memory**:
```cpp
Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
```

**Check LVGL Memory**:
```cpp
lv_mem_monitor_t mon;
lv_mem_monitor(&mon);
Serial.printf("LVGL: %d/%d bytes used\n", mon.total_size - mon.free_size, mon.total_size);
```

---

## Runtime Stability Tests

### 5-Minute Burn-In Test
1. Upload firmware via USB
2. Open serial monitor
3. Let run for 5 minutes
4. Verify:
   - ✅ Continuous LVGL ticks
   - ✅ No reboots or resets
   - ✅ UI remains visible
   - ✅ Tick rate stays consistent

### Button Input Test
1. Press **Button B** (tab switch)
   - Should show: `Button B: Switched to tab X`
   - UI should update to new tab
   
2. Press **Button C** (brightness)
   - Should show: `Button C: Brightness = XXX`
   - Screen should brighten/dim

3. Press **Button A** (animation - disabled)
   - Should show: `Button A: Animation disabled`
   - No crash or reboot

### Module Stress Test
1. Switch to **STATS** tab
   - CPU, Memory, PSRAM should update every second

2. Switch to **RADIO** tab
   - If LoRa hat connected, frequency should display
   - If not connected, error message shown

3. Switch to **SYSTEM** tab
   - System info should display correctly

---

## Next Steps After Successful USB Upload

### 1. Add SD Card Assets (Optional)
- Copy animation frames to SD card:
  - `/assets/Images/walking_0.png` through `walking_15.png`
  - `/assets/Images/thumbsup_0.png` through `thumbsup_7.png`
- Uncomment animation code in main.cpp (line 631)
- Rebuild and re-upload

### 2. Enable CRT Effects (Optional)
- Uncomment vignette overlay in ui_theme.cpp (line 245)
- Rebuild and test for flickering

### 3. Deploy to M5 Launcher
- Run task: **"Build & Package OTA Firmware"**
- Insert SD card with M5 Launcher
- Install from: M5 Launcher → Install Firmware → PipBoyADV

---

## Summary

### Quick Reference

| Task | Shortcut | Purpose |
|------|----------|---------|
| Build + Upload + Monitor | `Ctrl+Shift+B` | Full USB deployment |
| Build Only | - | Test compilation |
| Monitor Only | - | View debug output |
| Package OTA | - | Create M5 Launcher package |

### Port Configuration

**Auto-Detect** (recommended):
```ini
upload_port =  ; Leave empty
```

**Manual** (if auto-detect fails):
```ini
upload_port = COM4  ; Your actual port
```

### Expected Results

✅ **Build**: ~10-15 seconds  
✅ **Upload**: ~30-45 seconds  
✅ **Boot**: ~5 seconds to UI display  
✅ **Runtime**: Continuous LVGL ticks at ~200/sec  
✅ **Stability**: No crashes for 5+ minutes  

---

**STATUS**: ✅ USB upload automation configured and ready

**Test Command**: `Ctrl+Shift+B` (when device connected)

**Expected Output**: Firmware uploads and serial monitor shows live debug output
