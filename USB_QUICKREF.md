# Quick Reference: USB Upload & Monitor

## 🚀 One-Command Workflow

Press **Ctrl+Shift+B** for complete automation:
1. Auto-detect COM port
2. Build firmware (787.2 KB)
3. Upload to device
4. Open serial monitor

No configuration needed!

---

## 📋 Common Tasks

| Task | Command | Description |
|------|---------|-------------|
| **Full Workflow** | `Ctrl+Shift+B` | Build → Upload → Monitor |
| **Detect Port** | Tasks → "Detect COM Port" | Find device and update config |
| **Build Only** | Tasks → "Build Only" | Compile without uploading |
| **Monitor Only** | Tasks → "Monitor Only" | Open serial monitor |
| **Package OTA** | Tasks → "Build & Package OTA" | Create M5 Launcher package |

---

## 🔧 Troubleshooting

### No COM port detected
```powershell
.\detect_port.ps1
```
- Check USB cable (must support data)
- Try different USB port
- Install CP2102/CH340 drivers

### Monitor won't open
```powershell
# Close other serial monitors, then:
.\detect_port.ps1
pio device monitor
```

### Port locked error
- Unplug USB cable
- Wait 5 seconds
- Reconnect and run `.\detect_port.ps1`

---

## ✅ Expected Output

### Serial Monitor (v1.0.19)
```
=== PIP-BOY ADV v1.0.19 ===
[Stage 1/9] Initializing M5Stack Cardputer...
[Stage 6/9] Initializing LoRa...
⊘ LoRa hardware not detected - skipping
[Stage 9/9] Initialization complete!

LVGL tick: 1000 ms
LVGL tick: 2000 ms
```

### Build Output
```
RAM:   [===       ]  31.4% (used 103368 bytes)
Flash: [==        ]  24.8% (used 787248 bytes)
✓ Firmware built successfully
```

---

## 📁 Key Files

- **detect_port.ps1** - Auto-detection script
- **.vscode/tasks.json** - Build automation
- **platformio.ini** - Serial configuration
- **USB_UPLOAD_GUIDE.md** - Full documentation
- **SERIAL_MONITOR_FIX.md** - Implementation details

---

## 🎯 Version Info

**Current:** v1.0.19
- ✅ LoRa auto-detection
- ✅ COM port auto-detection
- ✅ Black screen fixed
- ✅ USB workflow automated

---

For detailed troubleshooting, see [USB_UPLOAD_GUIDE.md](USB_UPLOAD_GUIDE.md)
