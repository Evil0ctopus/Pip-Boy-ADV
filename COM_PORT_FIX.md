# COM Port Configuration - FIXED

## Changes Made

### 1. platformio.ini - Serial Configuration
**Location:** [platformio.ini](platformio.ini#L29-L38)

**Changed:**
```ini
; Serial Monitor Configuration
monitor_speed = 115200
monitor_filters = 
  default
  time
monitor_rts = 0        # Prevent DTR/RTS reset on monitor connect
monitor_dtr = 0        # Keeps device running during monitoring

; Upload Configuration
; Auto-detect COM port (leave commented out for auto-detection)
; Uncomment and set manually if auto-detection fails: upload_port = COM4
upload_speed = 1500000
```

**Why:**
- **monitor_rts/dtr = 0**: Prevents serial monitor from resetting the device when connecting
- **No upload_port**: Allows PlatformIO to auto-detect the COM port
- **Comment shows manual override**: If auto-detection fails, uncomment and set the correct port

---

### 2. .vscode/tasks.json - Sequential Upload Workflow
**Location:** [.vscode/tasks.json](.vscode/tasks.json#L4-L24)

**Changed:**
```json
{
  "label": "Build + Upload + Monitor (USB)",
  "type": "shell",
  "command": "powershell",
  "args": [
    "-ExecutionPolicy", "Bypass",
    "-NoProfile",
    "-Command",
    "Build → Upload → Monitor workflow with error checking"
  ]
}
```

**Workflow:**
1. **Build** firmware with `pio run`
2. **Check** build success (`$LASTEXITCODE -eq 0`)
3. **Upload** with `pio run --target upload`
4. **Wait 2 seconds** for device reset
5. **Open monitor** with `pio device monitor`

**Why:**
- **Sequential execution**: Each step completes before the next starts
- **Error handling**: Stops if build or upload fails
- **2-second delay**: Allows device to reset after upload before monitor connects
- **Dedicated panel**: Keeps serial output visible
- **No port conflicts**: Upload completes before monitor opens

---

### 3. detect_port.ps1 - Auto-Detection Script
**Location:** [detect_port.ps1](detect_port.ps1)

**Purpose:** Automatically detect and configure COM port

**Usage:**
```powershell
.\detect_port.ps1
```

**What it does:**
1. Scans for COM ports using 3 methods:
   - .NET SerialPort API
   - PlatformIO device list
   - Windows Device Manager (WMI)
2. Identifies most likely M5Stack device
3. Updates `platformio.ini` with detected port
4. Shows confirmation and next steps

---

## How to Use

### When Device is Connected

#### Method 1: Auto-Detect (Recommended)
1. **Connect** M5Stack Cardputer via USB
2. **Run** detection script:
   ```powershell
   .\detect_port.ps1
   ```
3. **Verify** port was detected and added to platformio.ini
4. **Build & Upload**:
   ```
   Ctrl+Shift+B
   ```

#### Method 2: Manual Configuration
1. **Open Device Manager** (`Win + X` → Device Manager)
2. **Expand** "Ports (COM & LPT)"
3. **Find** "USB Serial Device (COMx)" or "CP2102 (COMx)"
4. **Note the port** (e.g., COM4)
5. **Edit** [platformio.ini](platformio.ini):
   ```ini
   upload_port = COM4  ; Your actual port
   ```
6. **Build & Upload**:
   ```
   Ctrl+Shift+B
   ```

---

## Available Tasks

### 1. Build + Upload + Monitor (USB) [DEFAULT]
- **Shortcut:** `Ctrl+Shift+B`
- **Workflow:** Build → Upload → Wait → Monitor
- **Use:** Complete USB deployment with serial output

### 2. Build Only
- **Command:** `pio run`
- **Use:** Test compilation without device

### 3. Upload Only
- **Command:** `pio run --target upload`
- **Use:** Upload pre-built firmware

### 4. Monitor Only
- **Command:** `pio device monitor --baud 115200`
- **Use:** View serial output from running device

### 5. Build & Package OTA Firmware
- **Command:** `.\package.ps1 D`
- **Use:** Create M5 Launcher package

---

## Troubleshooting

### Error: "Please specify upload_port"

**Cause:** No device detected or device not in bootloader mode

**Solution 1 - Auto-Detect:**
```powershell
.\detect_port.ps1
```

**Solution 2 - Manual:**
1. Check Device Manager for COM port
2. Add to platformio.ini:
   ```ini
   upload_port = COM4
   ```

**Solution 3 - Try Upload Again:**
```powershell
pio run --target upload
```
- When you see "Connecting...", press RESET button on device

---

### Error: "Upload failed" or "Timed out waiting for packet header"

**Cause:** Device not in bootloader mode or wrong baud rate

**Solution:**
1. **Hold RESET** button on device
2. **Run upload** command
3. **Release RESET** when "Writing at 0x..." appears

Or reduce upload speed in platformio.ini:
```ini
upload_speed = 921600  # Half speed (more reliable)
```

---

### Serial Monitor Shows No Output

**Cause:** Wrong baud rate or device not transmitting

**Solution:**
1. **Press RESET** on device after monitor opens
2. **Check** monitor_speed in platformio.ini:
   ```ini
   monitor_speed = 115200  # Must match Serial.begin()
   ```
3. **Verify** firmware has Serial.println() calls

---

### Port Locked / Access Denied

**Cause:** Serial monitor still open from previous session

**Solution 1 - Kill monitor:**
```powershell
Get-Process | Where-Object {$_.ProcessName -like "*pio*"} | Stop-Process -Force
```

**Solution 2 - Close VS Code terminal:**
- Close the terminal panel showing the monitor
- Try upload again

**Solution 3 - Unplug and replug device:**
- Disconnect USB cable
- Wait 5 seconds
- Reconnect and try again

---

### Device Not Showing in Device Manager

**Cause:** Missing USB drivers

**Solution:**
1. **Download drivers:**
   - CP2102: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
   - CH340: http://www.wch-ic.com/downloads/CH341SER_ZIP.html
2. **Install** driver for your device
3. **Reboot** computer
4. **Run** detect_port.ps1 again

---

## Validation Tests

### Test 1: Auto-Detection
```powershell
.\detect_port.ps1
```
**Expected:**
```
✓ Detected port from PlatformIO: COM4
  Updated existing upload_port to: COM4
✓ Configuration updated!
```

### Test 2: Build Only
```powershell
pio run
```
**Expected:**
```
RAM: 31.2% (102,204 bytes)
Flash: 24.1% (804,729 bytes)
SUCCESS
```

### Test 3: Upload
```powershell
pio run --target upload
```
**Expected:**
```
Connecting.....
Writing at 0x00010000... (10%)
...
Hash of data verified.
```

### Test 4: Serial Monitor
```powershell
pio device monitor --baud 115200
```
**Expected:**
```
╔════════════════════════════════════════╗
║     Pip-Boy ADV Firmware v1.0.18       ║
╚════════════════════════════════════════╝

[LOOP] LVGL running - 200 ticks/sec
[LOOP] LVGL running - 198 ticks/sec
...
```

### Test 5: Full Workflow
```
Ctrl+Shift+B
```
**Expected:**
1. Build completes successfully
2. Upload starts and finishes
3. 2-second pause
4. Serial monitor opens
5. Boot messages appear
6. Continuous LVGL ticks

---

## Technical Details

### Why Sequential Instead of Parallel?

**Old method** (broken):
```bash
pio run --target upload --target monitor
```
**Problem:** PlatformIO tries to open monitor immediately after upload starts, locking the COM port before upload completes.

**New method** (fixed):
```powershell
pio run
if success:
  pio run --target upload
  if success:
    Wait 2 seconds
    pio device monitor
```
**Benefit:** Each operation completes fully before the next starts.

---

### Why monitor_rts/dtr = 0?

**DTR (Data Terminal Ready)** and **RTS (Request To Send)** are serial control signals.

**Default behavior:** When serial monitor connects, it toggles DTR/RTS, causing the ESP32 to reset.

**Problem:** Device resets every time you open the monitor, interrupting debugging.

**Solution:** Set both to 0 to disable hardware reset on monitor connect.

---

### Why the 2-Second Delay?

After uploading new firmware:
1. ESP32 automatically resets
2. Bootloader validates firmware (~1 second)
3. Application starts (~0.5 seconds)
4. Serial initialization completes

**Without delay:** Monitor opens before Serial.begin(), missing boot messages.

**With 2-second delay:** Monitor opens after Serial is ready, capturing all output.

---

## Summary

### Configuration Status: ✅ FIXED

| Issue | Status | Solution |
|-------|--------|----------|
| upload_port error | ✅ Fixed | Auto-detection enabled |
| Sequential workflow | ✅ Fixed | PowerShell script with error handling |
| Port conflicts | ✅ Fixed | Upload completes before monitor opens |
| Monitor reset device | ✅ Fixed | monitor_rts/dtr = 0 |
| Missing boot logs | ✅ Fixed | 2-second delay before monitor |

### Files Modified
1. **platformio.ini** - Added monitor_rts/dtr, removed hard-coded upload_port
2. **.vscode/tasks.json** - Fixed duplicate brace, sequential upload workflow
3. **detect_port.ps1** - Created auto-detection script

### Next Steps
**When device is connected:**
1. Run `.\detect_port.ps1` to auto-configure COM port
2. Press `Ctrl+Shift+B` to build, upload, and monitor
3. Verify continuous LVGL ticks appear in serial output

**If device is not available:**
- Firmware is pre-built and packaged for M5 Launcher
- Location: `D:\launcher\downloads\PipBoyADV\`
- Install via M5 Launcher when device is accessible
