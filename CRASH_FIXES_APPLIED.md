# Startup Crash Fixes - Applied January 31, 2026

## Summary
Applied comprehensive fixes to prevent startup crashes and ensure reliable boot on M5Stack Cardputer ADV.

## Critical Fixes Applied

### 1. LVGL Thread Safety (CRASH PREVENTION #1)
**Problem**: Multiple FreeRTOS tasks called LVGL functions without synchronization, corrupting internal state.

**Fix**: Added LVGL mutex protection
- Created `LvglLock.h` and `LvglLock.cpp` with recursive mutex
- Protected ALL LVGL calls from background tasks:
  - `systemStatsTask()` - UI updates every 2 seconds
  - `loraReceiveTask()` - Radio message UI updates
  - `sensorMonitorTask()` - Animation toggles
  - Main loop `lv_timer_handler()`
  - Button input handlers

**Files Modified**:
- `src/main.cpp` - Added lvgl_lock()/lvgl_unlock() around all UI calls
- `src/SystemStats.cpp` - Protected monitor task UI updates
- Created `include/LvglLock.h` and `src/LvglLock.cpp`

### 2. SD Card Initialization Order (CRASH PREVENTION #2)
**Problem**: Multiple modules called `SD.begin()` before main SD init, causing SPI conflicts and crashes.

**Fix**: Centralized SD initialization
- Created `SDHelper` singleton to manage SD state
- Moved SD-dependent module init AFTER SD is confirmed ready
- Prevented duplicate `SD.begin()` calls

**Files Modified**:
- Created `include/SDHelper.h` and `src/SDHelper.cpp`
- `src/main.cpp` - Deferred FileBrowser/PluginSystem to after SD init
- `src/ui_assets.cpp` - Check `SDHelper::isReady()` instead of calling `SD.begin()`
- `src/FileBrowser.cpp` - Removed duplicate init
- `src/PluginSystem.cpp` - Check SD ready before proceeding
- `src/ModuleIntegration.cpp` - Report actual SD state

**Init Order Now**:
1. M5Unified
2. LVGL + UI Shell
3. Hardware modules (Audio, LED, Sensors)
4. **SD Card** (Stage 4)
5. **SD-dependent modules** (FileBrowser, PluginSystem) - after SD confirmed
6. LoRa (Stage 6 - no duplicate init)

### 3. Wi-Fi/Weather/Config Removal (BOOT SPEED + STABILITY)
**Problem**: Wi-Fi, weather API, and config.txt dependencies slowed boot and could hang if network unavailable.

**Fix**: Compile-time removal
- Added feature flags: `-DDISABLE_WIFI`, `-DDISABLE_WEATHER`, `-DDISABLE_CONFIG`
- Created stub headers that compile to no-ops
- Removed all blocking network calls

**Files Modified**:
- `platformio.ini` - Added compile flags
- Created `include/ProjectConfig.h` - Central feature gating
- Created `include/WiFiManagerDisabled.h` - Stub WiFi manager
- Created `include/ConfigDisabled.h` and `src/ConfigDisabled.cpp` - Stub config
- Created `include/WeatherHelperDisabled.h` - Stub weather
- `include/WiFiManager.h` - Gate implementation with `#ifndef DISABLE_WIFI`
- `src/WiFiManager.cpp` - Exclude when disabled
- `include/Config.h` - Route to stub when `DISABLE_CONFIG`
- `include/WeatherHelper.h` - Route to stub when `DISABLE_WEATHER`
- `src/ModuleIntegration.cpp` - WiFi integration gated

**Result**: Instant boot, no network delays, no config.txt required.

### 4. Removed Duplicate LoRa Initialization (CRASH PREVENTION #3)
**Problem**: LoRa was initialized twice (Stage 3.5 and Stage 6), confusing RadioLib driver state.

**Fix**: Removed Stage 3.5 init, kept only Stage 6 (after SD/modules ready).

**Files Modified**:
- `src/main.cpp` - Removed duplicate `loraHelper.begin()` call

### 5. Increased LVGL Memory Pool (OOM PREVENTION)
**Problem**: 64KB LVGL heap too small for complex UI, causing allocation failures.

**Fix**: Doubled to 128KB.

**Files Modified**:
- `include/lv_conf.h` - Changed `LV_MEM_SIZE` from 64KB to 128KB

### 6. Module Integration Timing Fix
**Problem**: Module integration layer initialized before SD availability known.

**Fix**: Moved `modules.init()` to after SD initialization so all module states are accurate.

**Files Modified**:
- `src/main.cpp` - Relocated module integration init

## Boot Sequence (Fixed Order)

```
Stage 1: M5Unified Initialization
  - Display hardware
  - Power management
  - IMU/sensors probe

Stage 2: LVGL Initialization
  - lv_init()
  - lvgl_lock_init() ← NEW: Mutex created
  - Display driver registration
  - UI shell creation (protected by mutex)
  - Initial render

Stage 3: Hardware Modules
  - Audio
  - LED
  - System stats
  - Map renderer

Stage 4: SD Card & Configuration
  - HSPI configuration
  - SDHelper::begin() ← Centralized SD init
  - Asset system (if SD ready)
  - FileBrowser (if SD ready) ← MOVED HERE
  - PluginSystem (if SD ready) ← MOVED HERE
  - Module integration init ← MOVED HERE

Stage 5: Sensor Initialization
  - IMU sensor (if available)
  - Sensor monitor task (with LVGL mutex)

Stage 6: LoRa Initialization (ONLY ONCE)
  - Hardware detection
  - Initialization (if present)
  - Receive task (with LVGL mutex)

Stage 7: Background Tasks
  - LVGL tick task (highest priority)
  - System stats task (with LVGL mutex)

Stage 8: Final UI Updates
  - All protected by LVGL mutex
```

## Expected Behavior After Fixes

### Successful Boot
1. Serial output shows all stages completing
2. Display shows UI immediately (no black screen)
3. No crashes or reboots
4. No Wi-Fi/weather delays
5. Boots with or without SD card
6. Boots with or without LoRa hat

### Thread-Safe Operation
- Background tasks can update UI safely
- No LVGL corruption
- No random crashes during operation

### Graceful Degradation
- Missing SD card: UI works, no file browser
- Missing LoRa: UI works, radio tab shows "not detected"
- Missing sensors: UI works, no gesture control

## Testing Instructions

### 1. Connect Hardware
- M5Stack Cardputer ADV via USB-C
- Ensure data cable (not charge-only)

### 2. Build and Upload
```powershell
pio run --target upload
```

### 3. Monitor Serial Output
```powershell
pio device monitor
```

### 4. Expected Serial Output
```
╔════════════════════════════════════════╗
║     Pip-Boy ADV Firmware v1.0.20       ║
║   M5Stack Cardputer ADV (ESP32-S3)    ║
╚════════════════════════════════════════╝

=== Stage 1: M5Unified Initialization ===
  ✓ Display hardware initialized
  ✓ Display rotation set to landscape
  ✓ Display brightness configured
  ✓ Test pattern displayed
✓ M5Unified initialized

=== Stage 2: LVGL Initialization ===
Initializing LVGL...
  ✓ LVGL core initialized (v8.4.0)
  ✓ LVGL mutex initialized
  ✓ Display buffers allocated
  ✓ Display driver configured
  ✓ Display driver registered
✓ LVGL initialization complete
  → Creating UI shell...
UI Shell: Initializing...
  ✓ Theme initialized
  ✓ Main screen created
  ✓ Status bar initialized
  ✓ Tab system initialized
  ✓ CRT effects applied
  ✓ UI loaded to display
UI Shell: Ready
  ✓ Screen refreshed
✓ UI shell loaded and displayed

=== Stage 3: Hardware Initialization ===
✓ Audio initialized
✓ LED initialized
✓ System stats initialized
✓ Map Renderer initialized

=== Stage 4: SD Card & Configuration ===
Initializing SD card...
✓ SD card initialized: XXXXX MB
  (or "⚠ Continuing without SD card..." if not present)
✓ Module integration layer initialized

=== Stage 5: Sensor Initialization ===
✓ Sensors initialized

=== Stage 6: LoRa Initialization ===
Probing for LoRa SX1262 hardware...
  (either "✓ SX1262 chip responded" or "○ No response")

=== Stage 7: Bluetooth (Optional) ===
⊘ Bluetooth disabled (save resources)

=== Stage 8: Starting Background Tasks ===
✓ LVGL tick task started
WiFi/Weather/NTP: Disabled (standalone mode)
✓ System stats task started

=== Stage 9: Final UI Setup ===
⊘ Animation disabled (prevents crash without SD assets)

╔════════════════════════════════════════╗
║      Boot Complete - System Ready     ║
╚════════════════════════════════════════╝

[LOOP] LVGL running - XXXXX ticks/sec
```

### 5. Visual Confirmation
- Display shows Pip-Boy themed UI
- Status bar at top (battery, connectivity icons)
- Tabs: STAT | DATA | MAP | RADIO | SYS
- Green CRT scanline effect
- No crashes or black screen

## Rollback Instructions

If issues occur, revert by:
```powershell
git checkout HEAD~1
pio run --target upload
```

## Next Steps for Full Debugging

If crashes still occur with hardware connected:
1. **Capture full serial output** from power-on through crash
2. Look for:
   - Last successful stage before crash
   - Any "Guru Meditation Error" or panic messages
   - Stack trace or PC address
3. Correlate crash PC to source with: `xtensa-esp32s3-elf-addr2line -e .pio/build/cardputer-adv/firmware.elf <ADDRESS>`

## Known Limitations

1. **Animation disabled**: Walking/thumbsup animations require SD card assets. Currently disabled to prevent crashes when SD is missing.
2. **WiFi disabled at compile time**: To re-enable, remove `-DDISABLE_WIFI` from `platformio.ini` and rebuild.
3. **Weather disabled**: No weather fetching. CPU/RAM stats shown instead.

## Files Created

- `include/LvglLock.h`
- `src/LvglLock.cpp`
- `include/SDHelper.h`
- `src/SDHelper.cpp`
- `include/ProjectConfig.h`
- `include/WiFiManagerDisabled.h`
- `include/ConfigDisabled.h`
- `src/ConfigDisabled.cpp`
- `include/WeatherHelperDisabled.h`

## Files Modified

- `platformio.ini` - Build flags, LVGL heap size
- `include/lv_conf.h` - Memory increase
- `include/WiFiManager.h` - Feature gating
- `src/WiFiManager.cpp` - Conditional compile
- `include/Config.h` - Feature gating
- `include/WeatherHelper.h` - Feature gating
- `include/ModuleIntegration.h` - Project config include
- `src/ModuleIntegration.cpp` - WiFi gating, SD state
- `src/main.cpp` - Init order, LVGL locking, SD helper
- `src/SystemStats.cpp` - LVGL locking
- `src/ui_assets.cpp` - SD helper check
- `src/FileBrowser.cpp` - (awaiting SD)
- `src/PluginSystem.cpp` - SD ready check

## Version
**Firmware**: v1.0.20  
**Date**: 2026-01-31  
**Build**: SUCCESS ✓  
**Flash**: 24.0% (801,713 bytes)  
**RAM**: 51.2% (167,788 bytes)
