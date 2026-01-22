# Pip-Boy ADV Quick Reference

## 🚀 Quick Start (30 seconds)

1. **Prepare SD Card**
   - Format as FAT32
   - Copy `/assets/Images/` folders from `SLS Cardputer/assets/Images/`
   - Create `config.txt` with WiFi credentials

2. **Build & Flash**
   ```bash
   cd "Cardputer bin/PIO Cardputer SD Card Configuration Loader"
   pio run --target upload
   ```

3. **Insert SD card and power on!**

---

## 📋 config.txt Template

```
WIFI_SSID=YourWiFiName
WIFI_PASSWORD=YourPassword
TIME_ZONE=EST5EDT
API_KEY=get_from_weatherapi.com
LOCATION=YourCity
```

---

## 🎮 Controls

| Button | Action |
|--------|--------|
| **BtnA** | Toggle animation (walk ↔ thumbsup) |
| **BtnB** | Next tab (STAT → RADIO → ITEMS → DATA) |
| **BtnC** | Brightness cycle |

---

## 📱 Tabs

1. **STAT** - Time, weather, battery, animation
2. **RADIO** - Future: Meshtastic/LoRa
3. **ITEMS** - Future: File browser
4. **DATA** - Settings & system info

---

## 🎨 Theme Colors

```cpp
#define PIPBOY_GREEN       0x33FF33
#define PIPBOY_DARK_GREEN  0x006600
#define PIPBOY_BG          0x002200
```

---

## 🔧 Key Files

| File | Purpose |
|------|---------|
| `main.cpp` | Main entry, M5Unified init |
| `ui_main.cpp` | UI tabs & layout |
| `ui_animations.cpp` | Sprite engine |
| `ui_settings.cpp` | Settings tab |
| `ui_assets.cpp` | SD card loading |
| `WiFiHelper.cpp` | WiFi & NTP |
| `WeatherHelper.cpp` | Weather API |

---

## 📐 Display Specs

- **Resolution**: 240×135 pixels
- **Rotation**: 1 (landscape)
- **Colors**: RGB565
- **Buffers**: 2× (240×10)

---

## 🔌 API Quick Reference

### UI Updates
```cpp
ui_update_time("12:30 PM");
ui_update_date("Mon, Jan 21, 2026");
ui_update_weather("Sunny", "25°C");
ui_update_battery(75);
```

### Animations
```cpp
ui_animation_play(ANIM_WALKING);
ui_animation_play(ANIM_THUMBSUP);
ui_animation_stop();
```

### Settings
```cpp
ui_settings_update_brightness(128);
ui_settings_update_wifi_status(true, "MyNetwork");
```

### Navigation
```cpp
ui_switch_tab(TAB_STATS);     // 0
ui_switch_tab(TAB_RADIO);     // 1
ui_switch_tab(TAB_INVENTORY); // 2
ui_switch_tab(TAB_SETTINGS);  // 3
```

---

## ⚡ Common Issues

| Problem | Solution |
|---------|----------|
| SD not detected | Reformat as FAT32, reinsert |
| WiFi fails | Check SSID/password, no spaces |
| No animation | Verify PNG files on SD |
| Blank screen | Re-upload firmware |
| Time not syncing | Check internet, TIME_ZONE |

---

## 📊 Update Intervals

- LVGL: 5ms
- NTP: 10s
- Weather: 5min
- Battery: 10s
- Animation: 100ms/frame

---

## 🎯 To-Do Integration Examples

### Add Meshtastic to RADIO tab
```cpp
// 1. Add to platformio.ini
lib_deps = meshtastic/Meshtastic @ ^2.0.0

// 2. Create MeshtasticHelper.h/.cpp
// 3. Update ui_main.cpp RADIO tab
// 4. Initialize in main.cpp setup()
```

### Add File Browser to ITEMS tab
```cpp
// 1. Create ui_filebrowser.h/.cpp
// 2. Implement SD directory listing
// 3. Add LVGL list widget
// 4. Handle file selection events
```

---

## 🛠️ Build Flags

```ini
-DARDUINO_USB_CDC_ON_BOOT=1  # USB serial
-DBOARD_HAS_PSRAM            # Enable PSRAM
-DLV_CONF_INCLUDE_SIMPLE     # LVGL config
-DCORE_DEBUG_LEVEL=0         # Debug (0-5)
```

---

## 📁 SD Card Layout

```
/
├── config.txt
└── assets/
    └── Images/
        ├── walking/
        │   └── walking_00.png ... walking_20.png
        ├── thumpsup/
        │   └── thumpsup_00.png ... thumpsup_10.png
        └── battery.png
```

---

## 🔍 Debug Commands

```bash
# Build only
pio run

# Upload
pio run --target upload

# Monitor serial
pio device monitor --baud 115200

# Clean build
pio run --target clean

# Full rebuild
pio run --target clean && pio run --target upload
```

---

## 💡 Pro Tips

1. **PSRAM**: Always enabled for Cardputer ADV
2. **Serial Debug**: Set CORE_DEBUG_LEVEL=3 for verbose output
3. **Animation Speed**: Edit timer in `ui_animations.cpp` line 52
4. **Custom Colors**: Modify `ui_main.h` lines 6-9
5. **Memory**: Load animations on-demand to save RAM

---

## 📚 Documentation Files

- `README_ADV.md` - Full documentation
- `BUILD_GUIDE.md` - Step-by-step build
- `DEVELOPMENT_SUMMARY.md` - Technical details
- `QUICK_REFERENCE.md` - This file

---

## 🌐 Resources

- **Weather API**: https://www.weatherapi.com/
- **M5Unified Docs**: https://github.com/m5stack/M5Unified
- **LVGL Docs**: https://docs.lvgl.io/8.3/
- **PlatformIO**: https://platformio.org/

---

## ✅ Pre-Flight Checklist

- [ ] SD card formatted (FAT32)
- [ ] Animation PNGs copied
- [ ] config.txt created with WiFi creds
- [ ] Weather API key obtained
- [ ] PlatformIO installed
- [ ] USB cable connected
- [ ] Ready to build!

---

**Status**: Production Ready  
**Version**: 1.0  
**Platform**: M5Stack Cardputer ADV (ESP32-S3)  
**Date**: January 2026
