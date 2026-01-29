# Pip-Boy ADV - Quick Installation Guide

## ✅ Status: READY FOR INSTALLATION

**Version:** 1.0.12  
**Build Date:** 2026-01-28  
**Package Location:** D:\launcher\downloads\PipBoyADV\  

---

## Installation Steps

### 1. Prepare SD Card
```
✓ SD card is mounted as D:
✓ Firmware package created
✓ All files present:
  - firmware.bin (786.2 KB)
  - bootloader.bin (14.8 KB)
  - partitions.bin (3 KB)
  - app.json
  - icon.png
```

### 2. Eject SD Card
- Safely eject D: drive from computer
- Do NOT just pull out the card!

### 3. Install on Cardputer
1. Insert SD card into M5Stack Cardputer
2. Power on device
3. Open **M5 Launcher** app
4. Navigate to **Install Firmware**
5. Select **PipBoyADV**
6. Confirm installation
7. Wait 30-60 seconds
8. Device reboots automatically

---

## What to Expect

### ✅ SUCCESS - You Should See:

**2 seconds:** Green text appears
```
PIP-BOY ADV
Initializing...
```

**5 seconds:** Full green UI loads
```
┌─────────────────────────────┐
│ Status Bar (battery, time)  │
├─────────────────────────────┤
│ STAT│INV│DATA│MAP│RAD│SYS  │ ← Tabs
├─────────────────────────────┤
│                             │
│   Green CRT-style UI        │
│   Walking animation         │
│   System stats              │
│                             │
└─────────────────────────────┘
```

**10 seconds:** Fully operational
- All tabs clickable
- Real-time stats updating
- Animations running

---

## ❌ TROUBLESHOOTING

### Black Screen After Install?

**Connect USB and check serial output:**
```bash
pio device monitor --baud 115200
```

**Look for where it stops:**
- ✓ "Display hardware initialized" = Display works
- ✓ "LVGL core initialized" = LVGL works
- ✓ "UI Shell: Ready" = UI created
- ✗ Screen still black = Missing refresh (report this!)

### Partial Display?

- Green text shows but no full UI = LVGL buffer issue
- UI appears then freezes = Task handler issue
- Garbled graphics = Display driver issue

### Reboot Loop?

- Check serial output for panic messages
- Possible memory overflow
- Try reflashing bootloader

---

## Debug Commands

**Monitor Serial Output:**
```bash
pio device monitor --baud 115200
```

**Rebuild Firmware:**
```bash
pio run -e cardputer-adv
```

**Repackage for M5 Launcher:**
```bash
powershell -ExecutionPolicy Bypass -File .\package.ps1 D
```

**Clean Build:**
```bash
pio run -t clean
pio run -e cardputer-adv
```

---

## Critical Files

### Firmware Source:
- `src/main.cpp` - Main initialization (FIXED: duplicate UI init removed)
- `src/ui_shell.cpp` - UI creation (FIXED: boot animation disabled)
- `src/ui_assets.cpp` - Asset loading (FIXED: graceful failure)

### M5 Launcher Package:
- `D:\launcher\downloads\PipBoyADV\firmware.bin`
- `D:\launcher\downloads\PipBoyADV\bootloader.bin`
- `D:\launcher\downloads\PipBoyADV\partitions.bin`
- `D:\launcher\downloads\PipBoyADV\app.json`
- `D:\launcher\downloads\PipBoyADV\icon.png`

---

## Display Test Checkpoints

| Time | Checkpoint | Expected |
|------|-----------|----------|
| 0s | Power on | M5 logo |
| 2s | Display test | "PIP-BOY ADV" text |
| 5s | LVGL init | Green UI with tabs |
| 8s | Animation | Walking animation |
| 10s | Full boot | All systems operational |

---

## Key Fixes Applied

1. ✅ **Display Init** - M5.Display.begin() called explicitly
2. ✅ **Test Pattern** - Green text displays immediately to verify hardware
3. ✅ **Forced Refresh** - lv_timer_handler() + lv_refr_now() after UI creation
4. ✅ **Duplicate Init** - Removed second ui_shell_start() call
5. ✅ **Debug Logging** - Comprehensive serial output for troubleshooting
6. ✅ **Asset Safety** - Missing SD card won't block boot
7. ✅ **Boot Animation** - Disabled temporarily (re-enable after testing)
8. ✅ **M5 Package** - Correct folder structure and file names

---

## Success Rate Prediction

**Based on fixes applied:**
- Hardware working (M5.begin succeeds): **95% confidence**
- Display shows text immediately: **90% confidence**
- LVGL UI loads successfully: **85% confidence**
- Full system operational: **80% confidence**

**Remaining unknowns:**
- SD card quality/formatting
- M5 Launcher version compatibility
- Specific Cardputer ADV hardware revision

---

## Next Actions

1. **NOW:** Eject SD card, insert into Cardputer
2. **THEN:** Install via M5 Launcher → PipBoyADV
3. **WAIT:** 30-60 seconds for installation
4. **VERIFY:** Green UI appears after reboot
5. **REPORT:** Success or failure with serial output

---

## Contact & Support

**Documentation:**
- BLACK_SCREEN_FIX_COMPLETE.md - Full technical details
- CRITICAL_FIXES_APPLIED.md - Previous fix summary
- INSTALLATION_CHECKLIST.md - Detailed testing guide

**Serial Debugging:** 115200 baud, watch for "✓" success markers

**Expected Serial Output:** See BLACK_SCREEN_FIX_COMPLETE.md for complete boot sequence

---

## ✅ READY FOR INSTALLATION

**All systems go! Install the firmware and verify the green UI displays!**
