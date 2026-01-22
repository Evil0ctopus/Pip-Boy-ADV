# Pip-Boy ADV - M5Stack Cardputer ADV Edition

A Fallout-inspired Pip-Boy interface exclusively for the **M5Stack Cardputer ADV** featuring real-time weather, NTP time sync, LoRa communication, and retro UI with LVGL.

---

## 🎯 Project Status

✅ **Clean PlatformIO Structure**  
✅ **Single Device Support:** M5Stack Cardputer ADV Only  
✅ **Production Ready:** Verified pin mappings and build configuration  

---

## 📋 Hardware Requirements

- **Device:** M5Stack Cardputer ADV
- **MCU:** ESP32-S3 (Dual Core, 240MHz, 8MB PSRAM)
- **Display:** 240x135 ST7789 TFT
- **Keyboard:** Full QWERTY (TCA8418 controller)
- **Storage:** MicroSD card (FAT32)
- **Optional:** LoRa SX1262 Hat for wireless communication

---

## ✨ Features

- ✅ **Retro Pip-Boy UI** - Classic green phosphor CRT aesthetic
- ✅ **Weather Integration** - Real-time data via WeatherAPI
- ✅ **NTP Time Sync** - Automatic timezone-aware clock
- ✅ **LoRa Radio** - SX1262 hat support for wireless messaging
- ✅ **IMU Gestures** - Shake detection and orientation sensing
- ✅ **Animations** - Sprite-based character animations
- ✅ **Modular Design** - Clean separation of UI and hardware layers
- ✅ **SD Card Config** - Dynamic configuration loading

---

## 🚀 Quick Start

### 1. Prerequisites

Install [PlatformIO](https://platformio.org/):
- **VS Code:** Install PlatformIO IDE extension
- **CLI:** `pip install platformio`

### 2. Clone and Build

```bash
git clone <your-repo-url>
cd Pip-Boy-ADV
pio run
```

### 3. Prepare SD Card

Format SD card as FAT32 and create this structure:
```
/config.txt              # Required: WiFi and API credentials
/assets/Images/          # Optional: Animation frames
  walking/
  thumpsup/
  battery.png
```

Copy `data/config.txt.example` to your SD card as `config.txt` and edit with your credentials:
```
WIFI_SSID=YourNetwork
WIFI_PASSWORD=YourPassword
TIME_ZONE=AST-3
API_KEY=YourWeatherAPIKey
LOCATION=YourCity
```

See [data/SD_CARD_STRUCTURE.md](data/SD_CARD_STRUCTURE.md) for details.

### 4. Upload

```bash
pio run -t upload
pio device monitor -b 115200
```

---

## 📁 Repository Structure

```
Pip-Boy-ADV/
├── platformio.ini           # PlatformIO configuration
├── src/                     # Source code (.cpp)
│   ├── main.cpp
│   ├── ui_*.cpp            # UI modules
│   ├── *Helper.cpp         # Hardware helpers
│   └── ...
├── include/                 # Headers (.h)
│   ├── HardwareConfig.h    # Pin definitions
│   ├── Config.h
│   ├── lv_conf.h           # LVGL configuration
│   └── ...
├── lib/                     # Custom libraries (empty - using registry)
├── data/                    # SD card filesystem content
│   ├── assets/             # Place assets here for upload
│   ├── themes/             # Optional theme files
│   ├── config.txt.example  # Sample configuration
│   └── SD_CARD_STRUCTURE.md
├── docs/                    # Archived documentation
│   ├── CARDPUTER_ADV_CONFIG.md
│   ├── QUICK_REFERENCE_ADV.md
│   └── ...
└── README.md
```

---

## 🔧 Hardware Pin Configuration

All pin definitions are in [include/HardwareConfig.h](include/HardwareConfig.h)

### Critical Pins
| Function | Pin | Notes |
|----------|-----|-------|
| I2C SDA | GPIO 12 | Keyboard, IMU, PMIC |
| I2C SCL | GPIO 11 | 400kHz |
| SD SCK | GPIO 40 | HSPI bus |
| SD MISO | GPIO 39 | |
| SD MOSI | GPIO 14 | |
| SD CS | GPIO 12 | |
| NeoPixel | GPIO 21 | Status LED |
| LoRa SCK | GPIO 36 | SX1262 hat |
| LoRa MISO | GPIO 37 | |
| LoRa MOSI | GPIO 35 | |
| LoRa CS | GPIO 34 | |

See [QUICK_REFERENCE_ADV.md](QUICK_REFERENCE_ADV.md) for complete pinout.

---

## 📚 Documentation

- **[README_ADV.md](README_ADV.md)** - Detailed ADV implementation guide
- **[CARDPUTER_ADV_CONFIG.md](CARDPUTER_ADV_CONFIG.md)** - Complete configuration reference
- **[QUICK_REFERENCE_ADV.md](QUICK_REFERENCE_ADV.md)** - Quick pin reference
- **[HARDWARE_GUIDE.md](HARDWARE_GUIDE.md)** - Hardware integration guide
- **[BUILD_GUIDE.md](BUILD_GUIDE.md)** - Build and upload instructions
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Software architecture overview

---

## 🛠️ Development

### Build Commands
```bash
pio run                      # Build
pio run -t upload            # Upload to device
pio device monitor           # Serial monitor
pio run -t clean             # Clean build
```

### Environment
```ini
[env:cardputer-adv]
board = esp32-s3-devkitc-1
platform = espressif32
framework = arduino
```

---

## 🎨 Customization

### Themes
Create `/themes/pipboy.theme` on SD card for custom colors (optional).

### Assets
Place PNG images in `/assets/Images/` on SD card:
- Walking animation: `walking/walking_00.png` through `walking_20.png`
- Thumbs up: `thumpsup/thumpsup_00.png` through `thumpsup_10.png`
- Battery icon: `battery.png`

---

## 📡 LoRa Communication

The firmware supports the M5Stack LoRa SX1262 Hat:
- **Frequency:** 915 MHz (configurable)
- **Bandwidth:** 125 kHz
- **Spreading Factor:** 7
- **Coding Rate:** 4/5

Modify `LoRaHelper.h` to change LoRa parameters.

---

## 🐛 Troubleshooting

### Serial Not Working
- Verify `-DARDUINO_USB_CDC_ON_BOOT=1` in platformio.ini
- Wait 2-3 seconds after upload for USB enumeration

### SD Card Fails
- Format as FAT32
- Check connections: SCK=40, MISO=39, MOSI=14, CS=12
- Verify files are in root directory

### Display Issues
- Display managed by M5Unified
- Verify LVGL config in `include/lv_conf.h`

### LoRa Not Working
- Check hat connection
- Verify pins in `include/HardwareConfig.h`
- Ensure `RADIOLIB_GODMODE` flag is set

---

## 📦 Dependencies

- **M5Unified** ^0.1.16 - Hardware abstraction for Cardputer ADV
- **LVGL** ^8.3.11 - Graphics library
- **ArduinoJson** ^6.21.3 - JSON parsing
- **Adafruit NeoPixel** ^1.12.3 - LED control
- **RadioLib** ^6.4.0 - LoRa communication

---

## 📄 License

This project is provided as-is for educational and personal use.

---

## 🙏 Acknowledgments

- M5Stack for the Cardputer ADV hardware
- LVGL project for the UI framework
- RadioLib for LoRa support
- Original Pip-Boy design © Bethesda Softworks

---

## 🔗 Links

- **Hardware:** [M5Stack Cardputer ADV](https://shop.m5stack.com/)
- **PlatformIO:** [platformio.org](https://platformio.org/)
- **LVGL:** [lvgl.io](https://lvgl.io/)

## Time Zone Configuration for NTP

This project uses NTP (Network Time Protocol) to synchronize time over the internet and adjusts the local time based on the configured time zone. The time zone is set using POSIX-style strings, allowing you to handle both standard and daylight saving time automatically.

### Example Time Zones:

*  PST8PDT  - Pacific Standard Time with Daylight Saving Time
*  EST5EDT  - Eastern Standard Time with Daylight Saving Time
*  UTC      - Coordinated Universal Time
*  GMT      - Greenwich Mean Time
*  CET-1CEST- Central European Time with Daylight Saving Time
*  IST-5:30 - Indian Standard Time (UTC+5:30)
*  JST-9    - Japan Standard Time (UTC+9)
*  CST6CDT  - Central Standard Time with Daylight Saving Time (US)
*  MST7MDT  - Mountain Standard Time with Daylight Saving Time (US)
*  PHT-8    - Philippine Time (UTC+8)
*  SGT-8    - Singapore Time (UTC+8)
*  MSK-3    - Moscow Time (UTC+3)
*  GMT+4    - UAE Time (UTC+4)
*  AST-3    - Arabian Standard Time (Bahrain, UTC+3)