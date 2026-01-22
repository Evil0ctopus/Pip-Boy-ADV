# Build and Upload Guide - Pip-Boy ADV

This guide covers building and uploading the Pip-Boy ADV firmware to your M5Stack Cardputer ADV.

---

## Prerequisites

### Required Software
- **PlatformIO** (VS Code extension or CLI)
- **Git** (for cloning the repository)

### Required Hardware
- **M5Stack Cardputer ADV**
- **USB-C cable**
- **MicroSD card** (FAT32 formatted)

---

## Quick Start (VS Code)

### 1. Open Project
```bash
git clone <your-repo>
cd Pip-Boy-ADV
code .
```

### 2. Install Dependencies
PlatformIO will automatically install dependencies on first build:
- M5Unified
- LVGL
- ArduinoJson
- Adafruit NeoPixel
- RadioLib

### 3. Build
Press `Ctrl+Alt+B` or:
```bash
pio run
```

### 4. Upload
Press `Ctrl+Alt+U` or:
```bash
pio run -t upload
```

### 5. Monitor
Press `Ctrl+Alt+S` or:
```bash
pio device monitor -b 115200
```

---

## Command Line (PlatformIO CLI)

### Install PlatformIO Core
```bash
pip install platformio
```

### Build Project
```bash
cd Pip-Boy-ADV
pio run
```

### Upload Firmware
```bash
pio run -t upload
```

### Serial Monitor
```bash
pio device monitor -b 115200
```

### Clean Build
```bash
pio run -t clean
```

### Update Libraries
```bash
pio lib update
```

---

## Build Output

Successful build will show:
```
Environment    Status    Duration
-------------  --------  ------------
cardputer-adv  SUCCESS   00:00:45.123
```

Binary location: `.pio/build/cardputer-adv/firmware.bin`

---

## Upload Methods

### Method 1: Direct USB Upload (Recommended)
```bash
pio run -t upload
```

PlatformIO will automatically:
1. Detect the Cardputer ADV
2. Upload firmware via USB
3. Reset the device

### Method 2: Manual Upload (Troubleshooting)

If automatic upload fails:

1. **Hold BOOT button** (if available) while connecting USB
2. Run: `pio run -t upload`
3. Release BOOT button
4. Press RESET button

### Method 3: ESP Web Flasher

For users without PlatformIO:

1. Build firmware: `pio run`
2. Locate: `.pio/build/cardputer-adv/firmware.bin`
3. Visit: https://espressif.github.io/esptool-js/
4. Connect device
5. Flash at address `0x0`

---

## Configuration

### platformio.ini
```ini
[env:cardputer-adv]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

build_flags =
  -I include
  -DARDUINO_USB_MODE=1
  -DARDUINO_USB_CDC_ON_BOOT=1
  -DBOARD_HAS_PSRAM
  -DLV_CONF_INCLUDE_SIMPLE
  -DCORE_DEBUG_LEVEL=0
  -DRADIOLIB_GODMODE
```

### Changing Build Options

**Debug Level:**
```ini
-DCORE_DEBUG_LEVEL=3  # Verbose logging
-DCORE_DEBUG_LEVEL=0  # No logging (faster)
```

**Upload Speed:**
```ini
upload_speed = 1500000   # Fast
upload_speed = 921600    # Medium
upload_speed = 115200    # Slow but reliable
```

---

## SD Card Setup

### 1. Format Card
- **File System:** FAT32
- **Allocation Unit:** 4096 or default
- **Volume Label:** (optional)

### 2. Create Structure
```
SD Card:
├── config.txt
└── assets/
    └── Images/
        ├── walking/
        ├── thumpsup/
        └── battery.png
```

### 3. Edit config.txt
Copy from `data/config.txt.example`:
```
WIFI_SSID=YourNetwork
WIFI_PASSWORD=YourPassword
TIME_ZONE=AST-3
API_KEY=YourWeatherAPIKey
LOCATION=YourCity
```

### 4. Insert Card
Power off Cardputer, insert SD card, power on.

---

## Troubleshooting

### Build Errors

**"fatal error: Arduino.h: No such file"**
- PlatformIO installing dependencies
- Wait for completion or run: `pio lib install`

**"LVGL config error"**
- Verify `include/lv_conf.h` exists
- Check `-DLV_CONF_INCLUDE_SIMPLE` in platformio.ini

**"Multiple definition of..."**
- Clean build: `pio run -t clean`
- Rebuild: `pio run`

### Upload Errors

**"No serial port detected"**
- Check USB cable (must support data)
- Install CP210x drivers if needed
- Try different USB port

**"Timed out waiting for packet header"**
- Hold BOOT button during upload
- Reduce upload_speed to 921600
- Try manual reset

**"A fatal error occurred: MD5 of file does not match"**
- Clean build and retry
- Check USB cable quality
- Try slower upload speed

### Runtime Errors

**"SD card initialization failed"**
- Verify FAT32 format
- Check SD card pins in HardwareConfig.h
- Try different SD card
- Verify card is inserted correctly

**"WiFi connection failed"**
- Check credentials in config.txt
- Verify 2.4GHz network (5GHz not supported)
- Move closer to router

**"Serial output garbled"**
- Set monitor_speed to 115200
- Wait 2-3 seconds after upload for USB CDC
- Verify `-DARDUINO_USB_CDC_ON_BOOT=1` flag

---

## Advanced Options

### Custom Build Flags

Add to `platformio.ini`:
```ini
build_flags =
  -DDEBUG_MODE=1              # Enable debug output
  -DDISABLE_LORA=1            # Disable LoRa
  -DSD_DETECT_PIN=10          # Custom SD detect pin
```

### OTA Updates (Future)

For wireless updates:
```ini
upload_protocol = espota
upload_port = cardputer-adv.local
```

### Partition Scheme

For larger programs:
```ini
board_build.partitions = huge_app.csv
```

---

## Verification

After upload, serial output should show:
```
Initializing M5Unified...
SD Card initialized successfully
WiFi SSID: YourNetwork
Sensors initialized
LoRa initialized
```

---

## Build Time Estimates

| Task | Duration |
|------|----------|
| First build (dependencies) | 2-5 minutes |
| Incremental build | 10-30 seconds |
| Upload | 5-15 seconds |
| Full clean build | 30-60 seconds |

---

## Support

If issues persist:
1. Check [CARDPUTER_ADV_CONFIG.md](CARDPUTER_ADV_CONFIG.md)
2. Verify hardware connections
3. Review serial output for errors
4. Check GitHub issues

---

**Last Updated:** January 21, 2026  
**Platform:** PlatformIO  
**Board:** esp32-s3-devkitc-1
