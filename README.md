# Pip-Boy ADV - M5Stack Cardputer ADV Edition

---
<p align="center">
  <span style="color: yellow;">If you like this project, consider supporting it:</span>
</p>

<p align="center">
  <a href="https://www.buymeacoffee.com/nishad2m8" target="_blank">
    <img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Buy Me a Coffee" style="height: 35px;">
  </a>
  <a href="https://www.youtube.com/channel/UCV_35rUyf4N5mHZXaxaFKiQ" target="_blank">
    <img src="https://img.shields.io/badge/Subscribe%20on%20YouTube-FF0000?style=flat&logo=youtube" alt="Subscribe on YouTube" style="height: 35px;">
  </a>
</p>

---

![alt text](Elements/IMG_2923.JPG)

## M5Stack Cardputer ADV ONLY

This project has been optimized exclusively for the **M5Stack Cardputer ADV** with the following hardware:
- **Display:** 240x135 ST7789 TFT
- **MCU:** ESP32-S3 (StampS3)
- **Keyboard:** Full QWERTY keyboard
- **PMIC:** AXP2101 Power Management
- **Sensors:** IMU (accelerometer/gyroscope)
- **Expansion:** LoRa SX1262 Hat support
- **LED:** WS2812B NeoPixel
- **Storage:** SD card support

---

## Features

✅ **Weather Display** - Real-time weather data from WeatherAPI  
✅ **NTP Time Sync** - Automatic time synchronization with timezone support  
✅ **LoRa Communication** - SX1262 LoRa Hat support for wireless messaging  
✅ **Sensor Monitoring** - IMU-based gesture detection (shake, tap)  
✅ **Pip-Boy UI** - Retro-futuristic LVGL interface with animations  
✅ **SD Card Config** - Dynamic configuration loading from SD card  
✅ **Audio Effects** - Built-in beeps and sound effects  
✅ **Multi-Tab Interface** - Stats, Radio, Settings, and more  

---

## PlatformIO Setup

1. Open `Cardputer bin/PIO Cardputer SD Card Configuration Loader/` in PlatformIO

2. Update `config.txt` with your data and copy to the root directory of your SD card:
```
WIFI_SSID=YourSSID
WIFI_PASSWORD=YourPassword
TIME_ZONE=AST-3
API_KEY=YourWeatherAPIKey
LOCATION=YourCity
```

3. Build and upload using PlatformIO:
   - Environment: `m5stack-cardputer-adv`
   - Board: `m5stack-stamps3`
   - Upload speed: 1500000

## Binary Flashing Setup

1. Update `config.txt` with your data and copy to the root directory of your SD card (not inside any folder).

2. Use the [ESP Web Flasher](https://espressif.github.io/esptool-js/) to flash the .bin file to Cardputer ADV.
   - Set flash address to `0x0` when flashing
   - Use the binary from `Cardputer bin/` folder

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