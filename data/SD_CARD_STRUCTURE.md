# SD Card Structure for Pip-Boy ADV

This file documents the required SD card structure for the Pip-Boy ADV firmware.

## Required Files

```
SD Card Root:
├── config.txt              # WiFi and API configuration
├── assets/                 # UI assets and images
│   └── Images/
│       ├── walking/        # Walking animation frames
│       │   ├── walking_00.png
│       │   ├── walking_01.png
│       │   ├── ...
│       │   └── walking_20.png (21 frames total)
│       ├── thumpsup/       # Thumbs up animation frames
│       │   ├── thumpsup_00.png
│       │   ├── thumpsup_01.png
│       │   ├── ...
│       │   └── thumpsup_10.png (11 frames total)
│       └── battery.png     # Battery icon
└── themes/                 # Optional theme files
    └── pipboy.theme        # Pip-Boy color scheme (optional)
```

## config.txt Format

```
WIFI_SSID=YourNetworkName
WIFI_PASSWORD=YourPassword
TIME_ZONE=AST-3
API_KEY=YourWeatherAPIKey
LOCATION=YourCity
```

## Image Requirements

- **Format:** PNG (8-bit or 24-bit)
- **Walking frames:** 64x64 pixels recommended
- **Thumbs up frames:** 64x64 pixels recommended
- **Battery icon:** 24x24 pixels recommended

## Theme File (Optional)

If you want to customize colors, create `/themes/pipboy.theme` with custom color values.
The firmware uses hardcoded Pip-Boy green theme by default.

## Notes

- All paths in the firmware use absolute paths starting with "/"
- SD card must be formatted as FAT32
- Image files are loaded on-demand to save RAM
- Missing images will not crash the firmware, but animations won't display
