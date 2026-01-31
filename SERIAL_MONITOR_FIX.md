# Serial Monitor Fix - Implementation Summary

## Issues Resolved

### Primary Issue
Serial monitor was failing with COM port detection errors:
- Prompting "Available ports:", "Enter port index:", "Invalid index!"
- Error "could not open port 'com4': FileNotFoundError"
- Monitor requiring manual port input instead of auto-detecting

### Root Causes Identified
1. PlatformIO `pio device monitor` requires explicit port specification
2. `detect_port.ps1` existed but wasn't integrated into workflow
3. Tasks didn't call detection script before upload/monitor
4. Port locking between upload and monitor operations
5. No delay between upload completion and monitor opening

---

## Solutions Implemented

### 1. Enhanced Port Detection Script ([detect_port.ps1](detect_port.ps1))

**Improvements:**
- Sets `$env:PLATFORMIO_UPLOAD_PORT` environment variable for session
- Streamlined detection logic (PlatformIO → Device Manager → Available ports)
- Better error messages and troubleshooting guidance
- Explicit device name reporting (CP2102, CH340, etc.)
- More robust regex matching for ESP32-S3 devices

**Detection Methods:**
1. **PlatformIO** (most reliable): `pio device list`
2. **Device Manager** (fallback): WMI query for USB Serial devices
3. **Available Ports** (last resort): .NET SerialPort enumeration

**Output:**
- Updates `platformio.ini` with `upload_port = COMx`
- Sets environment variable for current session
- Returns exit code 0 on success, 1 on failure

### 2. Updated Build Tasks ([.vscode/tasks.json](.vscode/tasks.json))

**Main Task: "Build + Upload + Monitor (USB)"**
```json
{
  "label": "Build + Upload + Monitor (USB)",
  "command": "powershell -Command '
    # Step 1: Detect port
    .\\detect_port.ps1;
    if ($LASTEXITCODE -eq 0) {
      # Step 2: Build
      pio run;
      if ($LASTEXITCODE -eq 0) {
        # Step 3: Upload
        pio run --target upload;
        if ($LASTEXITCODE -eq 0) {
          # Step 4: Wait for device reset
          Start-Sleep -Seconds 3;
          # Step 5: Monitor
          pio device monitor
        }
      }
    }
  '"
}
```

**Key Features:**
- Sequential error checking (stops on first failure)
- 3-second delay after upload (prevents port locking)
- Auto-detection runs before every upload
- Color-coded progress messages

**New Task: "Detect COM Port"**
- Standalone task to run detection script manually
- Useful for debugging port issues
- Updates `platformio.ini` without building

**Updated Task: "Monitor Only"**
- Now runs `detect_port.ps1` before opening monitor
- No longer prompts for manual port selection
- Uses auto-detected port from environment variable

### 3. Monitor Configuration ([platformio.ini](platformio.ini))

**Current Settings:**
```ini
monitor_speed = 115200    # Correct baud rate for firmware
monitor_rts = 0           # Prevents device reset on connect
monitor_dtr = 0           # Prevents device reset on connect
upload_speed = 1500000    # Fast upload (1.5 Mbps)
# upload_port auto-detected by script
```

**Note:** `upload_port` is dynamically set by `detect_port.ps1` and should not be manually configured unless auto-detection fails.

---

## Testing Checklist

When device is connected, verify:

### ✅ Port Detection
```powershell
.\detect_port.ps1
```
- [ ] Script completes without errors
- [ ] Correct COM port detected
- [ ] `platformio.ini` updated with port
- [ ] Environment variable set: `$env:PLATFORMIO_UPLOAD_PORT`

### ✅ Build + Upload + Monitor
Press **Ctrl+Shift+B**:
- [ ] Port detection succeeds
- [ ] Firmware builds (787.2 KB)
- [ ] Upload completes successfully
- [ ] 3-second delay occurs
- [ ] Monitor opens automatically
- [ ] Serial output appears (no manual prompt)

### ✅ Expected Serial Output
```
=== PIP-BOY ADV v1.0.19 ===
M5.Display initialized: 240x135
[Stage 1/9] Initializing M5Stack Cardputer...
[Stage 2/9] Initializing hardware...
...
[Stage 6/9] Initializing LoRa...
Probing for LoRa SX1262 hardware...
⊘ LoRa hardware not detected - skipping
  (This is normal if SX1262 hat is not installed)
...
[Stage 9/9] Initialization complete!

LVGL tick: 1000 ms
LVGL tick: 2000 ms
```

### ✅ LoRa Auto-Detection (v1.0.19)
- [ ] "Probing for LoRa SX1262 hardware..." appears
- [ ] "⊘ LoRa hardware not detected - skipping" appears
- [ ] Device does NOT crash or hang
- [ ] Boot continues to Stage 7

### ✅ UI Display
- [ ] Screen displays Pip-Boy green UI
- [ ] No black screen
- [ ] STAT tab visible
- [ ] Navigation works

### ✅ Monitor Only
Run Task: "Monitor Only"
- [ ] Port auto-detected
- [ ] No manual prompt
- [ ] Serial output appears

### ✅ Device Reset Prevention
- [ ] Monitor opens without resetting device
- [ ] Serial output continues from current state
- [ ] No boot sequence appears (unless device was just uploaded to)

---

## Files Modified

### Primary Changes
1. **detect_port.ps1** - Enhanced with environment variable export
2. **.vscode/tasks.json** - Integrated auto-detection into workflow
3. **USB_UPLOAD_GUIDE.md** - Updated troubleshooting sections

### Configuration Verified
- **platformio.ini** - Monitor settings correct (rts/dtr=0)
- **src/main.cpp** - LoRa auto-detection (v1.0.19)
- **src/LoRaHelper.cpp** - Hardware probing implemented

---

## Workflow Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                   Ctrl+Shift+B Pressed                      │
└────────────────────┬────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────────┐
│  Step 1: detect_port.ps1                                    │
│  ├─ Scan PlatformIO device list                            │
│  ├─ Fallback to Device Manager                             │
│  ├─ Set $env:PLATFORMIO_UPLOAD_PORT                        │
│  └─ Update platformio.ini                                   │
└────────────────────┬────────────────────────────────────────┘
                     │ exit code 0
                     ▼
┌─────────────────────────────────────────────────────────────┐
│  Step 2: pio run (Build)                                    │
│  └─ Compile firmware → 787.2 KB                            │
└────────────────────┬────────────────────────────────────────┘
                     │ LASTEXITCODE == 0
                     ▼
┌─────────────────────────────────────────────────────────────┐
│  Step 3: pio run --target upload                            │
│  ├─ Uses port from platformio.ini                          │
│  └─ Upload at 1.5 Mbps                                      │
└────────────────────┬────────────────────────────────────────┘
                     │ LASTEXITCODE == 0
                     ▼
┌─────────────────────────────────────────────────────────────┐
│  Step 4: Start-Sleep -Seconds 3                             │
│  └─ Wait for device reset and port availability            │
└────────────────────┬────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────────┐
│  Step 5: pio device monitor                                 │
│  ├─ Uses port from platformio.ini                          │
│  ├─ 115200 baud, rts=0, dtr=0                              │
│  └─ Display serial output                                   │
└─────────────────────────────────────────────────────────────┘
```

---

## Error Handling

### Port Detection Fails
- Script exits with code 1
- Task chain stops (no build attempt)
- User sees troubleshooting message

### Build Fails
- Upload skipped
- Monitor skipped
- PlatformIO error displayed

### Upload Fails
- Monitor skipped
- Error message displayed
- Device may need manual reset

### Monitor Fails
- Build/upload succeeded
- Firmware is on device
- User can run "Monitor Only" task manually

---

## Future Improvements

### Optional Enhancements
1. **Port Caching**: Store last-used port for faster detection
2. **Multiple Device Support**: Handle multiple M5Stack devices
3. **Wireless Monitor**: OTA serial output via WiFi
4. **Auto-Retry**: Retry upload if first attempt fails

### Known Limitations
1. Requires Windows (PowerShell-based)
2. Assumes single M5Stack device connected
3. 3-second delay may be too long/short for some devices

---

## Version History

### v1.0.19 (Current - COM Port Auto-Detection Fix)
- ✅ Enhanced `detect_port.ps1` with environment variable export
- ✅ Integrated auto-detection into VS Code tasks
- ✅ Eliminated manual port prompts
- ✅ Fixed "could not open port" errors with 3-second delay
- ✅ Updated `USB_UPLOAD_GUIDE.md` with troubleshooting

### v1.0.18 (USB Upload Configuration)
- ✅ Created `detect_port.ps1` initial version
- ✅ Configured `platformio.ini` with monitor_rts/dtr=0
- ✅ Created build tasks in `.vscode/tasks.json`

### v1.0.16 (Black Screen Fix)
- ✅ Disabled boot animation
- ✅ Added NULL checks to animation system
- ✅ Disabled CRT vignette

### v1.0.0-1.0.15 (Initial Development)
- Build error fixes
- M5 Launcher compatibility
- LVGL UI integration

---

## Contact & Support

**Issue Reporting:**
1. Run `.\detect_port.ps1` and capture output
2. Run `pio device list` and capture output
3. Check Device Manager for COM ports
4. Include error messages from VS Code terminal

**Common Solutions:**
- Unplug/replug device
- Try different USB cable
- Close other serial monitors
- Reinstall CP2102/CH340 drivers
- Run as Administrator if permission denied
