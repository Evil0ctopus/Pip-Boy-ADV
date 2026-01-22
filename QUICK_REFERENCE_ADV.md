# Cardputer ADV Quick Reference

## Pin Mapping Summary

### Core Peripherals
| Function | Pin(s) | Notes |
|----------|--------|-------|
| **I2C SDA** | GPIO 12 | Keyboard, IMU, PMIC |
| **I2C SCL** | GPIO 11 | 400kHz |
| **Display** | M5Unified managed | 240x135 ST7789 |
| **NeoPixel LED** | GPIO 21 | WS2812B |
| **Speaker** | GPIO 41 | PWM buzzer |

### SD Card (HSPI)
| Function | Pin | 
|----------|-----|
| **SCK** | GPIO 40 |
| **MISO** | GPIO 39 |
| **MOSI** | GPIO 14 |
| **CS** | GPIO 12 |

### LoRa SX1262 Hat
| Function | Pin |
|----------|-----|
| **SCK** | GPIO 36 |
| **MISO** | GPIO 37 |
| **MOSI** | GPIO 35 |
| **CS** | GPIO 34 |
| **RST** | GPIO 33 |
| **IRQ** | GPIO 39 |
| **BUSY** | GPIO 38 |

### I2C Devices
| Device | Address | Description |
|--------|---------|-------------|
| **TCA8418** | 0x34 | Keyboard controller |
| **QMI8658** | 0x6B | IMU (accel/gyro) |
| **AXP2101** | 0x34 | Power management |

---

## Build Configuration

**Board:** `esp32-s3-devkitc-1`  
**Platform:** `espressif32`  
**Framework:** `arduino`

**Essential Flags:**
```
-DARDUINO_USB_MODE=1
-DARDUINO_USB_CDC_ON_BOOT=1
-DBOARD_HAS_PSRAM
-DLV_CONF_INCLUDE_SIMPLE
```

---

## Project Structure

```
src/
├── HardwareConfig.h      ← ALL pin definitions
├── main.cpp              ← Entry point
├── Config.h              ← WiFi/API config
├── ui_*.cpp/h            ← UI modules
├── *Helper.cpp/h         ← Hardware helpers
└── lv_conf.h             ← LVGL config
```

---

## Common Tasks

### Build
```bash
pio run -e m5stack-cardputer-adv
```

### Upload
```bash
pio run -e m5stack-cardputer-adv -t upload
```

### Monitor
```bash
pio device monitor -b 115200
```

### Clean
```bash
pio run -t clean
```

---

## SD Card Setup

**File:** `/config.txt`
```
WIFI_SSID=YourSSID
WIFI_PASSWORD=YourPassword
TIME_ZONE=AST-3
API_KEY=YourWeatherAPIKey
LOCATION=YourCity
```

**Assets:**
- `/assets/Images/walking/walking_00.png` through `walking_20.png`
- `/assets/Images/thumpsup/thumpsup_00.png` through `thumpsup_10.png`
- `/assets/Images/battery.png`

---

## Troubleshooting

### Serial Not Working
- Ensure `-DARDUINO_USB_CDC_ON_BOOT=1` is in build flags
- Wait 2-3 seconds after upload for USB to initialize
- Use 115200 baud rate

### SD Card Fails
- Check wiring: SCK=40, MISO=39, MOSI=14, CS=12
- Format as FAT32
- Verify files are in root directory

### LoRa Not Working
- Verify hat is properly connected
- Check pin definitions in HardwareConfig.h
- Ensure RADIOLIB_GODMODE flag is set

### Display Issues
- Display is managed by M5Unified
- Resolution: 240x135
- Rotation: 1 (landscape)

---

## Hardware Documentation

**Created:** January 21, 2026  
**Device:** M5Stack Cardputer ADV  
**Chip:** ESP32-S3 (Dual Core, 240MHz)  
**PSRAM:** 8MB  
**Flash:** 16MB
