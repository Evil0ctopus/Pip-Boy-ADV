# Pip-Boy ADV - Project Overview

```
┌────────────────────────────────────────────────────────────────┐
│                    PIP-BOY ADV PROJECT                         │
│           M5Stack Cardputer ADV Exclusive Build                │
└────────────────────────────────────────────────────────────────┘

📁 Repository Structure
═══════════════════════════════════════════════════════════════

Pip-Boy-ADV/
│
├── 🔨 BUILD SYSTEM
│   ├── platformio.ini              # PlatformIO configuration
│   └── .gitignore                   # Git ignore rules
│
├── 💻 SOURCE CODE
│   ├── src/                         # C++ implementation files
│   │   ├── main.cpp                 # Entry point
│   │   ├── ui_*.cpp                 # User interface modules
│   │   └── *Helper.cpp              # Hardware abstraction
│   │
│   └── include/                     # Header files
│       ├── HardwareConfig.h         # Pin definitions
│       ├── Config.h                 # App configuration
│       ├── lv_conf.h                # LVGL settings
│       └── *.h                      # Module headers
│
├── 📦 DEPENDENCIES
│   └── lib/                         # Custom libraries (empty)
│       # Uses PlatformIO library registry
│
├── 💾 DATA FILES
│   └── data/                        # SD card content templates
│       ├── config.txt.example       # WiFi/API config template
│       ├── SD_CARD_STRUCTURE.md     # SD card documentation
│       ├── assets/                  # Image storage (user adds)
│       └── themes/                  # Theme files (optional)
│
├── 📚 DOCUMENTATION
│   ├── README.md                    # Main project readme
│   ├── BUILD_INSTRUCTIONS.md        # How to build & upload
│   ├── RESTRUCTURE_COMPLETE.md      # Restructure summary
│   ├── README_ADV.md                # ADV implementation guide
│   ├── CARDPUTER_ADV_CONFIG.md      # Hardware configuration
│   ├── QUICK_REFERENCE_ADV.md       # Pin quick reference
│   ├── HARDWARE_GUIDE.md            # Hardware integration
│   ├── ARCHITECTURE.md              # Software architecture
│   └── [other *.md files]           # Additional documentation
│
├── 🗄️ ARCHIVE
│   └── archive/                     # Legacy files (preserved)
│       ├── Cardputer bin/           # Original firmware folder
│       ├── Elements/                # Original image assets
│       └── README.md                # Archive documentation
│
└── 🔧 VS CODE
    └── .vscode/                     # Editor configuration
        └── [settings]


═══════════════════════════════════════════════════════════════
🎯 Quick Commands
═══════════════════════════════════════════════════════════════

Build:          pio run
Upload:         pio run -t upload
Monitor:        pio device monitor -b 115200
Clean:          pio run -t clean


═══════════════════════════════════════════════════════════════
🔌 Hardware Configuration
═══════════════════════════════════════════════════════════════

Board:          esp32-s3-devkitc-1
Platform:       espressif32
Framework:      arduino

Key Pins:
  I2C SDA:      GPIO 12
  I2C SCL:      GPIO 11
  SD Card CS:   GPIO 12
  NeoPixel:     GPIO 21
  LoRa CS:      GPIO 34


═══════════════════════════════════════════════════════════════
📱 Features
═══════════════════════════════════════════════════════════════

✅ Retro Pip-Boy UI (LVGL)
✅ Real-time Weather (WeatherAPI)
✅ NTP Time Synchronization
✅ LoRa SX1262 Support
✅ IMU Gesture Detection
✅ Sprite Animations
✅ SD Card Configuration
✅ Multi-tab Interface


═══════════════════════════════════════════════════════════════
🚀 Getting Started
═══════════════════════════════════════════════════════════════

1. Clone repository
   git clone <repo-url>
   cd Pip-Boy-ADV

2. Open in VS Code
   code .

3. Prepare SD card
   - Format as FAT32
   - Copy data/config.txt.example as config.txt
   - Edit with your WiFi/API credentials

4. Build & Upload
   pio run -t upload

5. Monitor
   pio device monitor -b 115200


═══════════════════════════════════════════════════════════════
📦 Dependencies (Auto-installed)
═══════════════════════════════════════════════════════════════

✅ M5Unified          ^0.1.16   # Cardputer ADV support
✅ LVGL               ^8.3.11   # Graphics library
✅ ArduinoJson        ^6.21.3   # JSON parsing
✅ Adafruit NeoPixel  ^1.12.3   # LED control
✅ RadioLib           ^6.4.0    # LoRa communication


═══════════════════════════════════════════════════════════════
📋 File Count Summary
═══════════════════════════════════════════════════════════════

Source Files:       18 .cpp
Header Files:       20 .h
Documentation:      16 .md
Configuration:      1  .ini
Templates:          1  .example


═══════════════════════════════════════════════════════════════
✅ Status: PRODUCTION READY
═══════════════════════════════════════════════════════════════

The project is fully configured and ready to build for the
M5Stack Cardputer ADV with no additional setup required.

Last Updated:   January 21, 2026
Build System:   PlatformIO 6.x
Target Device:  M5Stack Cardputer ADV (ESP32-S3)
```
