# Pip-Boy-ADV UI Shell - API Quick Reference

## Include Statement
```cpp
#include "ui_shell.h"  // Single include for complete UI
```

## Initialization
```cpp
void setup() {
    M5.begin();
    lvgl_setup();
    ui_shell_init();  // Initialize complete UI system
}
```

## Update Functions

### Time & Date
```cpp
ui_shell_update_time("12:34 PM");
ui_shell_update_date("Mon, Jan 21, 2026");
```

### Weather
```cpp
ui_shell_update_weather("Sunny", "22°C");
```

### Battery
```cpp
ui_shell_update_battery(85, false);  // 85%, not charging
ui_shell_update_battery(42, true);   // 42%, charging (shows ⚡)
```

### WiFi Status
```cpp
ui_shell_update_wifi(true, -45);   // Connected, -45 dBm RSSI
ui_shell_update_wifi(false, 0);    // Disconnected
```

### LoRa Status
```cpp
ui_shell_update_lora(true, -75.5);  // Active, -75.5 dBm RSSI
ui_shell_update_lora(false, 0);     // Inactive
```

### Animations
```cpp
ui_shell_play_animation(ANIM_WALKING);   // Play walking (21 frames)
ui_shell_play_animation(ANIM_THUMBSUP);  // Play thumbsup (11 frames)
ui_shell_stop_animation();               // Stop current animation
```

## Color Constants (ui_theme.h)

```cpp
PIPBOY_GREEN_BRIGHT    // #33FF33 - Highlights
PIPBOY_GREEN           // #00FF00 - Standard text
PIPBOY_GREEN_MED       // #00CC00 - Secondary text
PIPBOY_GREEN_DARK      // #006600 - Borders
PIPBOY_BG_DARK         // #002200 - Containers
PIPBOY_BLACK           // #000000 - Background
```

## Tab Navigation (ui_tabs.h)

```cpp
#include "ui_tabs.h"

ui_tabs_switch_to(TAB_INDEX_STATS);      // Switch to STAT tab
ui_tabs_switch_to(TAB_INDEX_RADIO);      // Switch to RADIO tab
ui_tabs_switch_to(TAB_INDEX_INVENTORY);  // Switch to ITEMS tab
ui_tabs_switch_to(TAB_INDEX_SETTINGS);   // Switch to DATA tab

TabIndex current = ui_tabs_get_active();  // Get current tab (0-3)
```

## Tab Change Callback

```cpp
ui_tabs_set_change_callback([](TabIndex old_tab, TabIndex new_tab) {
    Serial.printf("Tab changed: %d -> %d\n", old_tab, new_tab);
    audio.playTabSwitch();  // Play sound on tab change
});
```

## Status Bar Features (ui_statusbar.h)

```cpp
#include "ui_statusbar.h"

// Blink LoRa indicator during transmission
ui_statusbar_blink_lora(true);   // Start blinking
ui_statusbar_blink_lora(false);  // Stop blinking
```

## Theme Application (ui_theme.h)

```cpp
#include "ui_theme.h"

lv_obj_t *label = lv_label_create(parent);
ui_theme_apply_label_title(label);   // Large title font
ui_theme_apply_label_normal(label);  // Standard font
ui_theme_apply_label_small(label);   // Small font

lv_obj_t *btn = lv_btn_create(parent);
ui_theme_apply_button(btn);  // Pip-Boy button style

lv_obj_t *container = lv_obj_create(parent);
ui_theme_apply_container(container);  // Dark green container

ui_theme_apply_glow(label);  // Add green glow effect
```

## Screen Layout

```
┌────────────────────────────────────────┐
│ [Time]         [L][W]   [Battery%]     │ ← Status Bar (18px)
├────────────────────────────────────────┤
│ [STAT] [RADIO] [ITEMS] [DATA]          │ ← Tabs (22px)
├────────────────────────────────────────┤
│                                        │
│         Tab Content (95px)             │
│                                        │
└────────────────────────────────────────┘
Total: 240×135 pixels
```

## Color Coding Reference

### WiFi/LoRa Signal
- **Bright Green** - Excellent signal
- **Green** - Good signal
- **Dark Green** - Weak signal
- **Darkest Green** - Disconnected

### Battery Level
- **Green** - Above 60%
- **Medium Green** - 30-60%
- **Yellow** - 15-30%
- **Red** - Below 15%

## Complete Example

```cpp
#include <M5Unified.h>
#include <lvgl.h>
#include "ui_shell.h"

void setup() {
    // Hardware init
    M5.begin();
    
    // LVGL init (existing function)
    lvgl_setup();
    
    // Initialize complete UI
    ui_shell_init();
    
    // Set initial values
    ui_shell_update_time("12:00");
    ui_shell_update_date("Mon, Jan 21");
    ui_shell_update_battery(100, false);
    ui_shell_update_wifi(false, 0);
    ui_shell_update_lora(false, 0);
    
    // Start animation
    ui_shell_play_animation(ANIM_WALKING);
}

void loop() {
    M5.update();
    lv_timer_handler();  // LVGL update
    delay(5);
    
    // Update time every second
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 1000) {
        lastUpdate = millis();
        
        // Get current time (example)
        char timeStr[16];
        sprintf(timeStr, "%02d:%02d", hour, minute);
        ui_shell_update_time(timeStr);
        
        // Update battery
        int battLevel = M5.Power.getBatteryLevel();
        bool charging = M5.Power.isCharging();
        ui_shell_update_battery(battLevel, charging);
    }
}
```

## Module Files

- `ui_shell.h/cpp` - Main API
- `ui_theme.h/cpp` - Color theme
- `ui_statusbar.h/cpp` - Status bar
- `ui_tabs.h/cpp` - Tab management
- `ui_animations.h/cpp` - Sprite animations
- `ui_radio.h/cpp` - Radio tab
- `ui_settings.h/cpp` - Settings tab
- `ui_assets.h/cpp` - Asset loader

## Documentation

- `UI_SYSTEM_README.md` - Complete system documentation
- `UI_INTEGRATION_GUIDE.md` - Migration from ui_main
- `IMPLEMENTATION_SUMMARY.md` - Implementation details
- This file - Quick API reference

## Build Command

```bash
cd "Cardputer bin/PIO Cardputer SD Card Configuration Loader"
pio run
```

## Hardware Target

- **Device**: M5Stack Cardputer ADV
- **Display**: 240×135 ST7789 LCD
- **MCU**: ESP32-S3
- **Framework**: Arduino + M5Unified
- **Graphics**: LVGL 8.3.11

---
*For full documentation, see UI_SYSTEM_README.md*
