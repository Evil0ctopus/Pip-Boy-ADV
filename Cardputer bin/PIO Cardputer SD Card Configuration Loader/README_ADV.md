# Pip-Boy ADV - M5Stack Cardputer ADV Implementation

## Overview
This is the M5Stack Cardputer ADV implementation of the Pip-Boy interface using M5Unified and LVGL with a modular architecture.

## Hardware Requirements
- **Device**: M5Stack Cardputer ADV
- **Display**: 240×135 pixels
- **Chip**: ESP32-S3
- **Storage**: SD Card (for assets and configuration)

## Features
- ✅ LVGL UI with tabbed interface (Stats, Radio, Inventory, Settings)
- ✅ Green Pip-Boy retro theme
- ✅ Animation engine with sprite support
- ✅ Modular code structure
- ✅ WiFi connectivity with NTP time sync
- ✅ Weather data integration
- ✅ SD card configuration loading
- ✅ Battery monitoring
- ✅ LED status indicators
- 🔧 Expandable for Meshtastic, IR tools, and radio modules

## Project Structure

### Source Files
```
src/
├── main.cpp              # Main application entry point
├── ui_main.cpp/.h        # Main UI shell with tabs
├── ui_animations.cpp/.h  # Animation engine for sprites
├── ui_settings.cpp/.h    # Settings tab UI
├── ui_assets.cpp/.h      # SD card asset loading
├── Config.h              # Configuration declarations
├── WiFiHelper.cpp/.h     # WiFi and NTP functionality
├── WeatherHelper.cpp/.h  # Weather API integration
└── LEDHelper.cpp/.h      # NeoPixel LED control
```

### Assets on SD Card
```
/assets/
  /Images/
    /walking/           # Walking animation frames (00-20)
    /thumpsup/          # Thumbs up animation frames (00-10)
    battery.png         # Battery icon
```

## Configuration

### platformio.ini
```ini
[env:m5stack-cardputer-adv]
platform = espressif32
board = m5stack-stamps3
framework = arduino

build_flags =
  -I include
  -DARDUINO_USB_CDC_ON_BOOT=1
  -DBOARD_HAS_PSRAM
  -DLV_CONF_INCLUDE_SIMPLE
  -DCORE_DEBUG_LEVEL=0

lib_deps = 
  m5stack/M5Unified@^0.1.16
  lvgl/lvgl@^8.3.11
  bblanchon/ArduinoJson@^6.21.3
  adafruit/Adafruit NeoPixel @ ^1.12.3
```

### SD Card config.txt
Place this file in the root of your SD card:
```
WIFI_SSID=YourWiFiSSID
WIFI_PASSWORD=YourPassword
TIME_ZONE=EST5EDT
API_KEY=YourWeatherAPIKey
LOCATION=YourCity
```

## UI Tabs

### 1. STAT (Stats)
- Current time (from NTP)
- Current date
- Weather conditions
- Temperature
- Battery level with bar
- Animated sprite (walking/thumbsup)

### 2. RADIO
- Placeholder for future Meshtastic integration
- Expandable for LoRa radio features
- IR tools integration placeholder

### 3. ITEMS (Inventory)
- Placeholder for SD card file browser
- Future: File manager and data viewer

### 4. DATA (Settings)
- Brightness control slider
- WiFi status display
- System information
- Device specs

## Controls (Current Implementation)

### Side Buttons (M5Unified)
- **BtnA**: Toggle between walking/thumbsup animations
- **BtnB**: Switch between tabs
- **BtnC**: Cycle brightness levels

### Future Keyboard Support
The firmware is designed to support the Cardputer ADV keyboard for:
- Tab switching with specific keys
- Brightness adjustment
- Menu navigation
- Text input for settings

## Color Scheme (Pip-Boy Green)
```cpp
#define PIPBOY_GREEN       0x33FF33  // Bright green text
#define PIPBOY_DARK_GREEN  0x006600  // Dark green backgrounds
#define PIPBOY_BG          0x002200  // Very dark green background
#define PIPBOY_BLACK       0x000000  // Black
```

## Modular API

### UI Main Functions
```cpp
void ui_main_init();                                    // Initialize main UI
void ui_update_time(const char *time_str);             // Update time display
void ui_update_date(const char *date_str);             // Update date display
void ui_update_weather(const char *weather, const char *temp);  // Update weather
void ui_update_battery(int percentage);                // Update battery level
void ui_switch_tab(int tab_index);                     // Switch tabs programmatically
```

### Animation Functions
```cpp
void ui_animations_init(lv_obj_t *parent);             // Initialize animation system
void ui_animation_play(AnimationType type);            // Play animation (ANIM_WALKING, ANIM_THUMBSUP)
void ui_animation_stop();                              // Stop current animation
void ui_animation_pause();                             // Pause animation
void ui_animation_resume();                            // Resume animation
```

### Settings Functions
```cpp
void ui_settings_init(lv_obj_t *parent);              // Initialize settings UI
void ui_settings_update_brightness(int value);        // Update brightness display
void ui_settings_update_wifi_status(bool connected, const char *ssid);  // Update WiFi status
```

### Asset Functions
```cpp
bool ui_assets_init();                                 // Initialize asset system
lv_img_dsc_t* ui_assets_load_image(const char *path); // Load image from SD
bool ui_assets_load_walking_frames();                 // Pre-load walking frames
bool ui_assets_load_thumbsup_frames();                // Pre-load thumbsup frames
void ui_assets_free_all();                            // Free all loaded assets
```

## Building and Flashing

### Using PlatformIO
1. Open project in VS Code with PlatformIO
2. Select environment: `m5stack-cardputer-adv`
3. Build: `pio run`
4. Upload: `pio run --target upload`

### Prepare SD Card
1. Format SD card (FAT32)
2. Create folder structure: `/assets/Images/walking/` and `/assets/Images/thumpsup/`
3. Copy animation PNG files to respective folders
4. Create `config.txt` in root with your WiFi and weather API credentials
5. Insert SD card into Cardputer ADV

## Future Expansion Points

### Radio Module Integration
The Radio tab is designed for:
- Meshtastic messaging
- LoRa communication
- Frequency scanning
- Packet monitoring

### Inventory/File Manager
The Items tab will support:
- SD card file browsing
- Configuration file editing
- Data logging viewer
- Asset management

### Additional Features
- IR remote control functions
- Keyboard macro system
- Custom animations
- Plugin system for external modules

## Debug Output
Monitor serial output at 115200 baud to see:
- Initialization status
- WiFi connection progress
- NTP time sync
- Weather API responses
- Button presses and UI events

## Troubleshooting

### SD Card Not Detected
- Ensure SD card is formatted as FAT32
- Check SPI pin connections
- Verify SD card is inserted properly

### Display Issues
- Confirm M5Unified is properly initialized
- Check LVGL buffer configuration
- Verify display rotation setting (should be 1 for landscape)

### Animation Not Showing
- Ensure PNG files are in correct folders on SD card
- Check file naming convention (e.g., `walking_00.png`)
- Monitor serial output for asset loading errors

### WiFi Not Connecting
- Verify `config.txt` has correct SSID/password
- Check WiFi credentials have no trailing spaces
- Ensure 2.4GHz WiFi network is available

## License
See main repository for license information.

## Credits
Based on the original Pip-Boy Weather Clock project, adapted for M5Stack Cardputer ADV with modular architecture.
