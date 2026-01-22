# Pip-Boy-ADV UI Shell System

## Overview
This is a modular LVGL UI system for the M5Stack Cardputer ADV, implementing a Pip-Boy themed interface with green phosphor display aesthetics.

## Architecture

The UI system consists of these modules:

### 1. **ui_shell** - Main UI Coordinator
- **Purpose**: High-level API for initializing and updating the UI
- **Functions**:
  - `ui_shell_init()` - Initialize entire UI system
  - `ui_shell_update_time(time_str)` - Update time display
  - `ui_shell_update_date(date_str)` - Update date display
  - `ui_shell_update_weather(weather, temp)` - Update weather info
  - `ui_shell_update_battery(percentage, charging)` - Update battery status
  - `ui_shell_update_wifi(connected, rssi)` - Update WiFi status
  - `ui_shell_update_lora(active, rssi)` - Update LoRa status
  - `ui_shell_play_animation(type)` - Play sprite animation
  - `ui_shell_stop_animation()` - Stop current animation

### 2. **ui_theme** - Pip-Boy Visual Theme
- **Purpose**: Centralized color scheme and styling
- **Colors**:
  - `PIPBOY_GREEN_BRIGHT` - #33FF33 (highlights, active text)
  - `PIPBOY_GREEN` - #00FF00 (standard text)
  - `PIPBOY_GREEN_MED` - #00CC00 (secondary text)
  - `PIPBOY_GREEN_DARK` - #006600 (borders, inactive)
  - `PIPBOY_BG_DARK` - #002200 (container backgrounds)
  - `PIPBOY_BLACK` - #000000 (screen background)

- **Styles**:
  - Screen, container, labels (title/normal/small)
  - Buttons, sliders, bars, textareas
  - Tabview and tab states

- **Functions**:
  - `ui_theme_init()` - Initialize all theme styles
  - `ui_theme_apply_*()` - Apply specific styles to objects
  - `ui_theme_apply_glow()` - Add green glow effect to text

### 3. **ui_statusbar** - Status Bar Display
- **Purpose**: Always-visible status indicators at screen top
- **Height**: 18 pixels
- **Elements**:
  - Time display (left)
  - WiFi indicator "W" (signal strength color-coded)
  - LoRa indicator "L" (signal strength color-coded)
  - Battery percentage (right, color-coded by level)

- **Functions**:
  - `ui_statusbar_init(parent)` - Create status bar
  - `ui_statusbar_update_time(time_str)` - Update time
  - `ui_statusbar_update_battery(level, charging)` - Update battery (⚡ when charging)
  - `ui_statusbar_update_wifi(connected, rssi)` - Update WiFi (color by signal)
  - `ui_statusbar_update_lora(active, rssi)` - Update LoRa (color by signal)
  - `ui_statusbar_blink_lora(enable)` - Enable blinking for LoRa activity

### 4. **ui_tabs** - Tabbed Navigation System
- **Purpose**: Main content organization with 4 tabs
- **Tabs**:
  1. **STAT** - Statistics and system info
  2. **RADIO** - LoRa communication
  3. **ITEMS** - Inventory/file browser (placeholder)
  4. **DATA** - Settings and configuration

- **Functions**:
  - `ui_tabs_init(parent)` - Create tabview below status bar
  - `ui_tabs_create_stats()` - Create Stats tab content
  - `ui_tabs_create_radio()` - Create Radio tab content
  - `ui_tabs_create_inventory()` - Create Inventory tab content
  - `ui_tabs_create_settings()` - Create Settings tab content
  - `ui_tabs_switch_to(TabIndex)` - Switch to specific tab
  - `ui_tabs_get_active()` - Get current active tab
  - `ui_tabs_set_change_callback()` - Register tab change callback

### 5. **Integration with Existing Modules**
- **ui_animations** - Sprite animation engine (walking/thumbsup)
- **ui_radio** - LoRa radio tab UI
- **ui_settings** - Settings tab UI
- **ui_assets** - SD card image loader

## Screen Layout

```
┌─────────────────────────────────────┐
│ [Time]         [L][W]   [Battery%]  │  ← Status Bar (18px)
├─────────────────────────────────────┤
│ [STAT] [RADIO] [ITEMS] [DATA]       │  ← Tab Buttons (22px)
├─────────────────────────────────────┤
│                                     │
│                                     │
│         Tab Content Area            │  ← Content (95px)
│                                     │
│                                     │
└─────────────────────────────────────┘
Total: 240×135 pixels
```

## Stats Tab Layout

```
┌─────────────────────────────────────┐
│              STAT                   │
│           12:00 PM                  │
│       Mon, Jan 21, 2026             │
│                                     │
│  Weather: Sunny          ┌────────┐ │
│  Temp: 22°C              │ SPRITE │ │
│                          │  ANIM  │ │
│                          └────────┘ │
│  PWR: [██████████░░░] 75%          │
└─────────────────────────────────────┘
```

## Color Coding

### WiFi/LoRa Signal Strength
- **Bright Green** - Excellent (WiFi > -50dBm, LoRa > -80dBm)
- **Green** - Good (WiFi > -70dBm, LoRa > -100dBm)
- **Dark Green** - Weak (below thresholds)
- **Darkest Green** - Disconnected/Inactive

### Battery Level
- **Green** - Above 60%
- **Medium Green** - 30-60%
- **Yellow** - 15-30%
- **Red** - Below 15%
- **⚡ Symbol** - Charging

## Usage Example

```cpp
#include "ui_shell.h"

void setup() {
    // ... M5 and LVGL initialization ...
    
    // Initialize complete UI system
    ui_shell_init();
    
    // Update UI elements
    ui_shell_update_time("12:34 PM");
    ui_shell_update_date("Mon, Jan 21, 2026");
    ui_shell_update_battery(85, false);
    ui_shell_update_wifi(true, -45);
    ui_shell_update_lora(true, -75.5);
    ui_shell_update_weather("Sunny", "22°C");
    
    // Play walking animation
    ui_shell_play_animation(ANIM_WALKING);
}
```

## Sprite Animation

The animation container in the Stats tab displays animated sprites from SD card:

### Supported Animations
- **ANIM_WALKING** - 21 frames walking cycle
  - Files: `/walking/walking_01.png` through `/walking/walking_21.png`
  - Frame rate: 10 FPS (100ms per frame)
  
- **ANIM_THUMBSUP** - 11 frames thumbs-up gesture
  - Files: `/thumpsup/thumpsup_01.png` through `/thumpsup/thumpsup_11.png`
  - Frame rate: 10 FPS (100ms per frame)

### Image Requirements
- Format: PNG with transparency
- Size: Recommended 64×64 pixels (fits in animation container)
- Naming: Sequential with zero-padded numbers (`_01`, `_02`, etc.)

## Customization

### Adding New Colors
Edit [ui_theme.h](src/ui_theme.h) to define new color constants:
```cpp
#define MY_CUSTOM_COLOR lv_color_make(0xRR, 0xGG, 0xBB)
```

### Adding New Tabs
1. Add tab index to `TabIndex` enum in [ui_tabs.h](src/ui_tabs.h)
2. Create tab in `ui_tabs_init()` using `lv_tabview_add_tab()`
3. Implement `ui_tabs_create_mytab()` function
4. Call from `ui_tabs_init()`

### Modifying Status Bar
Edit [ui_statusbar.cpp](src/ui_statusbar.cpp) to add new indicators:
- Create label/icon object
- Add update function
- Position in status bar container

## File Structure

```
src/
├── ui_shell.h/cpp         - Main UI coordinator
├── ui_theme.h/cpp         - Pip-Boy theme system
├── ui_statusbar.h/cpp     - Top status bar
├── ui_tabs.h/cpp          - Tabview and tab contents
├── ui_animations.h/cpp    - Sprite animation engine
├── ui_radio.h/cpp         - Radio tab (LoRa)
├── ui_settings.h/cpp      - Settings tab
└── ui_assets.h/cpp        - SD card asset loader
```

## Dependencies

- **LVGL 8.3.11** - Graphics library
- **M5Unified** - Hardware abstraction
- **SD Card** - Sprite images storage

## Memory Usage

- **LVGL Buffers**: 2× 240×10 pixels = 9.6 KB
- **Theme Styles**: ~1 KB (14 style structs)
- **UI Objects**: ~8 KB (tabs, labels, containers)
- **Animations**: 64×64×2 bytes = 8 KB per frame (cached)

**Total Estimated**: ~30 KB for UI system

## Performance

- **Frame Rate**: 30 FPS (LVGL tick every 33ms)
- **Tab Switching**: Instant with smooth animation
- **Animation Playback**: 10 FPS sprite animation
- **Status Updates**: Non-blocking, updates within 1 frame

## Future Enhancements

- [ ] Scanline/CRT effect overlay
- [ ] Boot-up sequence animation
- [ ] Terminal-style text typing effect
- [ ] Custom bitmap fonts (Fallout-style)
- [ ] More animation types (combat, radiation, etc.)
- [ ] Inventory file browser implementation
- [ ] Multi-page settings with scrolling

## License

Part of the Pip-Boy-ADV project for M5Stack Cardputer ADV.
