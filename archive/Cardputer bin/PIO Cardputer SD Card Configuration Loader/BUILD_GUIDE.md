# Pip-Boy ADV Build Instructions

## Quick Start Guide for M5Stack Cardputer ADV

### Prerequisites
1. **PlatformIO** installed in VS Code
2. **M5Stack Cardputer ADV** device
3. **Micro SD Card** (formatted as FAT32)
4. **Weather API Key** from weatherapi.com (free tier)

### Step 1: Prepare SD Card

#### Create Folder Structure
```
SD Card Root/
├── config.txt
└── assets/
    └── Images/
        ├── walking/
        │   ├── walking_00.png
        │   ├── walking_01.png
        │   ├── ... (through walking_20.png)
        ├── thumpsup/
        │   ├── thumpsup_00.png
        │   ├── thumpsup_01.png
        │   ├── ... (through thumpsup_10.png)
        └── battery.png
```

#### Create config.txt
In the root of your SD card, create a file named `config.txt` with the following content:

```
WIFI_SSID=YourWiFiName
WIFI_PASSWORD=YourWiFiPassword
TIME_ZONE=EST5EDT
API_KEY=your_weatherapi_key_here
LOCATION=YourCity,YourCountry
```

**Time Zone Examples:**
- `EST5EDT` - Eastern Time (US)
- `PST8PDT` - Pacific Time (US)
- `CST6CDT` - Central Time (US)
- `GMT0` - Greenwich Mean Time
- `CET-1CEST` - Central European Time
- `JST-9` - Japan Standard Time
- `AEST-10AEDT` - Australian Eastern Time

**Get Weather API Key:**
1. Go to https://www.weatherapi.com/
2. Sign up for free account
3. Copy your API key
4. Paste it in `config.txt`

#### Copy Animation Assets
The animation PNG files should already exist in the repository under:
- `SLS Cardputer/assets/Images/walking/`
- `SLS Cardputer/assets/Images/thumpsup/`

Copy these folders to your SD card's `/assets/Images/` directory.

### Step 2: Build the Firmware

#### Using PlatformIO in VS Code

1. **Open the Project**
   - Open VS Code
   - File → Open Folder
   - Navigate to: `Cardputer bin/PIO Cardputer SD Card Configuration Loader`
   - Click "Select Folder"

2. **Select Environment**
   - Look for the PlatformIO toolbar at the bottom
   - Click the environment selector
   - Choose: `env:m5stack-cardputer-adv`

3. **Build the Firmware**
   - Click the checkmark (✓) icon in PlatformIO toolbar, or
   - Press `Ctrl+Alt+B`, or
   - Terminal → Run Task → PlatformIO: Build

4. **Connect Device**
   - Connect M5Stack Cardputer ADV via USB-C
   - Wait for drivers to install (first time only)
   - Note the COM port in Device Manager (Windows) or `ls /dev/tty*` (Linux/Mac)

5. **Upload to Device**
   - Click the arrow (→) icon in PlatformIO toolbar, or
   - Press `Ctrl+Alt+U`, or
   - Terminal → Run Task → PlatformIO: Upload

6. **Monitor Serial Output** (Optional but recommended)
   - Click the plug icon in PlatformIO toolbar, or
   - Terminal → Run Task → PlatformIO: Monitor
   - Baud rate: 115200

### Step 3: First Boot

1. **Insert SD Card**
   - Power off the Cardputer ADV
   - Insert the prepared SD card
   - Power on the device

2. **Watch Boot Sequence**
   - Device initializes M5Unified
   - LVGL UI loads with green Pip-Boy theme
   - SD card is read for config.txt
   - WiFi connection starts
   - NTP time sync occurs
   - Weather data is fetched
   - Walking animation starts

3. **Check Serial Monitor**
   You should see output like:
   ```
   M5Unified initialized
   SD card initialized successfully
   WiFi SSID: YourNetwork
   Connecting to WiFi...
   WiFi connected successfully!
   IP Address: 192.168.1.xxx
   NTP and timezone set.
   Weather data fetched
   ```

### Step 4: Using the Interface

#### Tab Navigation
- **BtnA** (Left side button): Toggle between animations (walking ↔ thumbsup)
- **BtnB** (Middle button): Switch between tabs (STAT → RADIO → ITEMS → DATA → STAT)
- **BtnC** (Right button): Cycle brightness (0 → 64 → 128 → 192 → 255 → 0)

#### Tabs Overview
1. **STAT** - Time, date, weather, battery, and animation
2. **RADIO** - Placeholder for Meshtastic/LoRa (future)
3. **ITEMS** - Placeholder for file browser (future)
4. **DATA** - Settings: brightness, WiFi status, system info

### Troubleshooting

#### SD Card Not Detected
**Error:** `SD card initialization failed`

**Solutions:**
- Remove and reinsert SD card
- Format SD card as FAT32 (not exFAT)
- Try a different SD card (some cards are incompatible)
- Check SD card is not write-protected

#### WiFi Connection Failed
**Error:** WiFi keeps showing "Connecting..."

**Solutions:**
- Verify SSID and password in config.txt
- Ensure no extra spaces in config.txt
- Check 2.4GHz WiFi (5GHz not supported)
- Move closer to router
- Check if network has special characters in password

#### Animations Not Showing
**Error:** Blank animation area or frozen frame

**Solutions:**
- Verify PNG files are in correct folders
- Check file naming: `walking_00.png` (not `walking_0.png`)
- Ensure all 21 walking frames and 11 thumbsup frames exist
- Check PNG files are valid (try opening on PC)

#### Display Shows Garbage or Blank
**Solutions:**
- Re-upload firmware
- Check M5Unified library version
- Verify platformio.ini settings
- Try power cycle (full shutdown and restart)

#### Time Not Updating
**Solutions:**
- Ensure WiFi is connected
- Check internet connection on your network
- Verify TIME_ZONE format in config.txt
- NTP servers might be temporarily down (wait and retry)

#### Weather Data Not Showing
**Solutions:**
- Verify API key is correct
- Check location format: "City,Country" or "Latitude,Longitude"
- Ensure you haven't exceeded free tier limits (500 calls/month)
- Check weatherapi.com service status

### Advanced Configuration

#### Custom Animation FPS
Edit `ui_animations.cpp`, line ~52:
```cpp
// Change 100ms to desired delay (lower = faster)
anim_state.timer = lv_timer_create(animation_timer_callback, 100, NULL);
```

#### Custom Pip-Boy Colors
Edit `ui_main.h`, lines 6-9:
```cpp
#define PIPBOY_GREEN lv_color_make(0x33, 0xFF, 0x33)
#define PIPBOY_DARK_GREEN lv_color_make(0x00, 0x66, 0x00)
#define PIPBOY_BG lv_color_make(0x00, 0x22, 0x00)
```

#### Adjust Update Intervals
Edit `main.cpp`:
```cpp
// NTP time check interval (default: 10 seconds)
const unsigned long ntpTimeCheckInterval = 10000;
```

Edit `WeatherHelper.cpp`:
```cpp
// Weather update interval (default: 5 minutes)
vTaskDelay(300000 / portTICK_PERIOD_MS);
```

#### Battery Update Rate
Edit `main.cpp`:
```cpp
void batteryTask(void *pvParameters) {
    while (true) {
        updateBatteryBar();
        vTaskDelay(10000 / portTICK_PERIOD_MS);  // Change 10000 to desired ms
    }
}
```

### Development Notes

#### Adding New UI Elements
1. Declare in appropriate header file (`ui_*.h`)
2. Implement in corresponding `.cpp` file
3. Follow Pip-Boy green color scheme
4. Update `ui_main_init()` if adding to main UI

#### Adding New Tabs
1. Add tab enum in `ui_main.h`
2. Create content function in `ui_main.cpp`
3. Call from `ui_main_init()`
4. Update tab switching logic

#### Integrating New Modules
Example: Adding Meshtastic to Radio tab
1. Add library to `platformio.ini`
2. Create `MeshtasticHelper.h/.cpp`
3. Update Radio tab UI in `ui_main.cpp`
4. Add initialization in `setup()`

### Compilation Flags

Current build flags in `platformio.ini`:
```ini
build_flags =
  -I include                      # Include directory
  -DARDUINO_USB_CDC_ON_BOOT=1    # USB serial
  -DBOARD_HAS_PSRAM              # Enable PSRAM
  -DLV_CONF_INCLUDE_SIMPLE       # LVGL config
  -DCORE_DEBUG_LEVEL=0           # Debug level (0-5)
```

Change `CORE_DEBUG_LEVEL` to 3 or 4 for verbose debugging.

### Memory Optimization

If you encounter memory issues:
1. Reduce LVGL buffer size in `main.cpp`
2. Load animations on-demand instead of pre-loading
3. Reduce weather update frequency
4. Enable PSRAM (already enabled)

### Next Steps

**Planned Features:**
- [ ] Full keyboard input support
- [ ] Meshtastic integration (Radio tab)
- [ ] SD card file browser (Items tab)
- [ ] IR remote control
- [ ] Custom widget system
- [ ] Theme editor
- [ ] Config file editor on-device

**Contributing:**
To add features, follow the modular architecture:
- Create new `ui_<feature>.h/.cpp` files
- Keep functions focused and reusable
- Document public APIs
- Test on actual hardware
- Submit PRs with clear descriptions

## Support

For issues, refer to:
- Main README.md for general project info
- Serial monitor output for debugging
- PlatformIO build output for compilation errors
- GitHub issues for community support
