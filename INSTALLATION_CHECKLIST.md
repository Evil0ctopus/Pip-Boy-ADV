# Pip-Boy ADV - M5 Launcher Installation Checklist

## Pre-Installation Verification

### ✅ Build Status
- [x] Firmware compiles without errors
- [x] Version: 1.0.7
- [x] Build date: 2026-01-28 21:10:50
- [x] RAM usage: 31.2% (102,204 bytes)
- [x] Flash usage: 24.0% (803,537 bytes)

### ✅ OTA Package Created
- [x] firmware.bin (784 KB)
- [x] bootloader.bin
- [x] partitions.bin
- [x] app.json (manifest)
- [x] icon.png
- [x] Location: `D:\launcher\downloads\PipBoyADV\`

### ✅ Code Fixes Applied
- [x] All multiple definition errors resolved
- [x] LoRa module integrated (`loraHelper` references)
- [x] ui_radio.cpp fixed (extern declarations added)
- [x] Module integration layer created
- [x] M5 Launcher path updated in tasks.json
- [x] Boot animation temporarily disabled

---

## Installation Steps

### Method 1: M5 Launcher (Recommended)

#### Step 1: Prepare SD Card
```
1. Insert SD card into computer
2. Verify D:\launcher\downloads\PipBoyADV\ contains:
   - firmware.bin
   - bootloader.bin
   - partitions.bin
   - app.json
   - icon.png
3. Safely eject SD card
```

#### Step 2: Install via M5 Launcher
```
1. Insert SD card into M5Stack Cardputer
2. Power on device
3. Open M5 Launcher application
4. Navigate to: Install Firmware
5. Select: PipBoyADV
6. Confirm installation
7. Wait for flash process (approx. 30-60 seconds)
8. Device will reboot automatically
```

#### Step 3: Verify Installation
```
Expected boot sequence:
1. M5Stack boot logo (2-3 seconds)
2. Pip-Boy theme initialization
3. Status bar appears (top of screen)
4. Tab system loads (STAT, INV, DATA, MAP, RADIO, SYSTEM)
5. Green CRT theme active
6. System stats populate

✅ SUCCESS: Green UI visible, tabs functional
❌ FAILURE: Black screen or crash
```

---

### Method 2: Direct USB Flashing

#### Prerequisites
```powershell
# Ensure PlatformIO is installed
pio --version

# Connect Cardputer via USB
# Check COM port in Device Manager
```

#### Flash Command
```powershell
cd "C:\Users\jlors\Desktop\Pip-Boy-ADV"
pio run --target upload
```

#### Monitor Serial Output
```powershell
pio device monitor
```

---

## Troubleshooting

### Issue: Black Screen After Installation

**Possible Causes:**
1. Boot animation blocking (already disabled)
2. LVGL buffer allocation failure
3. Display driver not initialized
4. Font loading error

**Debug Steps:**
```powershell
# Connect via USB and monitor serial output
pio device monitor

# Look for error messages during boot
# Check for:
# - "LVGL init failed"
# - "Display init failed"  
# - Memory allocation errors
```

**Quick Fix:**
```cpp
// In src/ui_shell.cpp, verify line 63 is commented:
// ui_theme_create_boot_animation(ui_shell);
```

---

### Issue: UI Displays But Freezes

**Possible Causes:**
1. Module initialization blocking main loop
2. LVGL task handler not called
3. FreeRTOS task priority conflict

**Debug Steps:**
```cpp
// Check src/main.cpp loop() function
// Verify lv_timer_handler() is called regularly (line 614)
// Verify modules.update() is non-blocking (line 617)
```

---

### Issue: LoRa Module Not Working

**Verification:**
1. Navigate to RADIO tab
2. Check status label: "LoRa Initialized" or "LoRa Offline"
3. If offline, check hardware connection

**Hardware Check:**
- M5Stack LoRa868/915 Unit connected via Grove port
- Power supply sufficient (5V 2A recommended)
- SX1262 chip detected

---

### Issue: Compilation Errors

**Common Errors:**
```
undefined reference to `lora`
```
**Fix:** Replace `lora.` with `loraHelper.` and add `extern LoRaHelper loraHelper;`

```
multiple definition of `audio`
```
**Fix:** Remove duplicate global declarations, keep only in main.cpp

```
LV_FONT_MONTSERRAT_XX not enabled
```
**Fix:** Edit `include/lv_conf.h`, set font to 1

---

## Post-Installation Testing

### Basic Functionality Test
```
1. Power on device
2. UI loads with green theme ✅
3. Navigate between tabs using keyboard ✅
4. Status bar updates in real-time ✅
5. System stats display correctly ✅
```

### Module Test Checklist
- [ ] WiFi scan works (SYSTEM tab → WiFi)
- [ ] Bluetooth scan works (SYSTEM tab → Bluetooth)
- [ ] LoRa send/receive works (RADIO tab)
- [ ] GPS updates location (if GPS module connected)
- [ ] IR transmit/receive works (if IR module connected)
- [ ] LED effects work (SK6812 strip)

### Performance Test
```
Monitor for:
- Smooth UI animations
- Responsive keyboard input
- No memory leaks (check Serial output)
- Stable FreeRTOS tasks
- No watchdog resets
```

---

## Re-Enabling Boot Animation (Optional)

**Only re-enable after confirming UI works correctly!**

### Edit src/ui_shell.cpp
```cpp
// Line 63: Uncomment boot animation
ui_theme_create_boot_animation(ui_shell);  // ✅ Re-enabled after testing
```

### Add Safe Delay
```cpp
// Optional: Add delay before animation
vTaskDelay(pdMS_TO_TICKS(500));  // Wait 500ms
ui_theme_create_boot_animation(ui_shell);
```

### Rebuild and Test
```powershell
pio run
# Run OTA packaging task again
# Re-install via M5 Launcher
```

---

## Emergency Recovery

### Restore Factory Firmware
```powershell
# Flash original M5 Launcher firmware
esptool.py --chip esp32s3 --port COM3 erase_flash
```

### Reflash Bootloader
```powershell
esptool.py --chip esp32s3 --port COM3 \
  --before default_reset --after hard_reset \
  write_flash -z --flash_mode dio --flash_freq 80m \
  0x0 .pio/build/cardputer-adv/bootloader.bin \
  0x8000 .pio/build/cardputer-adv/partitions.bin \
  0x10000 .pio/build/cardputer-adv/firmware.bin
```

---

## Success Criteria

### ✅ Installation Successful If:
1. Device boots without errors
2. Green Pip-Boy UI displays
3. Tabs are functional and switchable
4. Status bar updates in real-time
5. System stats populate correctly
6. No crashes or freezes
7. Modules respond (WiFi, Bluetooth, LoRa)

### ❌ Installation Failed If:
1. Black screen persists >10 seconds
2. Device crashes during boot
3. UI elements don't load
4. Continuous reboot loop
5. Serial output shows panics/errors

---

## Support Resources

### Documentation
- `M5_LAUNCHER_FIX_SUMMARY.md` - Complete fix details
- `BUILD_GUIDE.md` - Build instructions
- `QUICK_REFERENCE_ADV.md` - Command reference
- `ARCHITECTURE.md` - System architecture

### Serial Debugging
```powershell
# Monitor real-time boot output
pio device monitor --baud 115200

# Look for initialization messages:
# ○ M5.begin() initialized
# ○ LVGL v8.4.0 initialized
# ○ Display driver registered
# ○ UI Shell initialized
# ○ Modules initialized
```

---

## Revision History

- **2026-01-28** - Initial checklist created
- **Version 1.0.7** - Post M5 Launcher compatibility fixes
- **Status:** Ready for testing

---

**Next Action:** Install firmware via M5 Launcher and verify UI displays correctly!
