# UI Shell Integration Guide

## Migrating from ui_main to ui_shell

The new **ui_shell** system provides a more modular and organized UI architecture. Here's how to integrate it:

## Quick Start

### Option 1: Use ui_shell (Recommended - New Modular System)

Replace the existing UI initialization in `main.cpp`:

**OLD CODE:**
```cpp
#include "ui_main.h"

void setup() {
    // ...
    lvgl_setup();
    ui_main_init();  // Old initialization
    // ...
}
```

**NEW CODE:**
```cpp
#include "ui_shell.h"  // Single include for complete UI system

void setup() {
    // ...
    lvgl_setup();
    ui_shell_init();  // New modular initialization
    // ...
}
```

### Option 2: Keep Existing ui_main (Current System)

If you prefer the current system, no changes needed. The new modules are additions, not replacements.

## File Comparison

### Current System (ui_main-based)
```
src/
├── ui_main.h/cpp          - Monolithic UI with all tabs
├── ui_animations.h/cpp    - Sprite animations
├── ui_settings.h/cpp      - Settings tab
├── ui_radio.h/cpp         - Radio tab
└── ui_assets.h/cpp        - Asset loading
```

### New System (ui_shell-based)
```
src/
├── ui_shell.h/cpp         - Main coordinator (thin wrapper)
├── ui_theme.h/cpp         - Centralized theming
├── ui_statusbar.h/cpp     - Dedicated status bar
├── ui_tabs.h/cpp          - Tabview management
├── ui_animations.h/cpp    - Sprite animations (shared)
├── ui_radio.h/cpp         - Radio tab (shared)
├── ui_settings.h/cpp      - Settings tab (shared)
└── ui_assets.h/cpp        - Asset loading (shared)
```

## Benefits of ui_shell System

1. **Separation of Concerns** - Each module has a single responsibility
2. **Reusable Theme** - Centralized color scheme and styles
3. **Independent Status Bar** - Can be updated without affecting tabs
4. **Easier Customization** - Modify one aspect without touching others
5. **Better Testing** - Each module can be tested independently

## API Comparison

### Update Time Display

**ui_main approach:**
```cpp
// Direct access to LVGL objects (tightly coupled)
extern lv_obj_t *ui_label_time;
if (ui_label_time) {
    lv_label_set_text(ui_label_time, "12:34");
}
```

**ui_shell approach:**
```cpp
// Clean API (loosely coupled)
ui_shell_update_time("12:34");
```

### Update Battery

**ui_main approach:**
```cpp
// Update function with direct LVGL calls
ui_update_battery(85);  // Updates bar directly
```

**ui_shell approach:**
```cpp
// Updates both status bar and stats tab
ui_shell_update_battery(85, false);  // percentage, charging
```

## Complete Integration Example

Here's a full example showing both systems side-by-side:

### Using ui_main (Current)

```cpp
#include "ui_main.h"
#include "ui_animations.h"

void setup() {
    M5.begin();
    lvgl_setup();
    
    // Initialize UI
    ui_main_init();
    ui_animations_init(/* parent container */);
    
    // Update elements
    ui_update_time("12:34 PM");
    ui_update_date("Jan 21, 2026");
    ui_update_weather("Sunny");
    ui_update_battery(85);
    
    // Start animation
    ui_animation_play(ANIM_WALKING);
}
```

### Using ui_shell (New)

```cpp
#include "ui_shell.h"

void setup() {
    M5.begin();
    lvgl_setup();
    
    // Single initialization - all modules loaded
    ui_shell_init();
    
    // Update elements through unified API
    ui_shell_update_time("12:34 PM");
    ui_shell_update_date("Jan 21, 2026");
    ui_shell_update_weather("Sunny", "22°C");
    ui_shell_update_battery(85, false);
    ui_shell_update_wifi(true, -45);
    ui_shell_update_lora(true, -75.5);
    
    // Start animation
    ui_shell_play_animation(ANIM_WALKING);
}
```

## Coexistence

Both systems can coexist in the same project:

```cpp
// You can use either approach
#include "ui_main.h"    // Current system
#include "ui_shell.h"   // New system

void setup() {
    lvgl_setup();
    
    // Choose one:
    ui_main_init();   // OR
    ui_shell_init();  // <-- Recommended
}
```

## Migration Checklist

To migrate from ui_main to ui_shell:

- [ ] **Step 1**: Test build with new files (no code changes)
  ```bash
  pio run
  ```

- [ ] **Step 2**: Replace `#include "ui_main.h"` with `#include "ui_shell.h"`

- [ ] **Step 3**: Replace `ui_main_init()` with `ui_shell_init()`

- [ ] **Step 4**: Update all `ui_update_*()` calls to `ui_shell_update_*()`

- [ ] **Step 5**: Replace animation calls:
  ```cpp
  ui_animation_play(ANIM_WALKING);  // Keep as-is
  // OR use wrapper:
  ui_shell_play_animation(ANIM_WALKING);
  ```

- [ ] **Step 6**: Test on hardware

- [ ] **Step 7**: Remove unused ui_main.h/cpp (optional)

## Theme Customization

One of the biggest advantages of ui_shell is centralized theming:

### Change Colors Globally

Edit `src/ui_theme.h`:

```cpp
// Original Pip-Boy green
#define PIPBOY_GREEN lv_color_make(0x00, 0xFF, 0x00)

// Change to amber (like Pip-Boy 3000 Mk IV)
#define PIPBOY_GREEN lv_color_make(0xFF, 0xA5, 0x00)

// Or blue (like Vault-Tec Security)
#define PIPBOY_GREEN lv_color_make(0x00, 0x99, 0xFF)
```

All UI elements update automatically!

### Add Custom Styles

In `src/ui_theme.cpp`:

```cpp
// Define new style
lv_style_t style_my_widget;

void ui_theme_init() {
    // ... existing styles ...
    
    // Your custom style
    lv_style_init(&style_my_widget);
    lv_style_set_bg_color(&style_my_widget, PIPBOY_GREEN_DARK);
    lv_style_set_border_width(&style_my_widget, 2);
    // ... etc
}

// Apply function
void ui_theme_apply_my_widget(lv_obj_t *obj) {
    if (obj) lv_obj_add_style(obj, &style_my_widget, 0);
}
```

## Status Bar Indicators

The new system includes a persistent status bar:

```cpp
// Update indicators at any time
ui_shell_update_wifi(true, -45);    // Connected, -45 dBm
ui_shell_update_lora(false, 0);     // Not active
ui_shell_update_battery(42, true);  // 42%, charging
```

Indicators automatically change color based on signal strength and battery level!

## Advanced: Tab Management

Access tab functions directly:

```cpp
#include "ui_tabs.h"

// Switch to Radio tab
ui_tabs_switch_to(TAB_INDEX_RADIO);

// Get current tab
TabIndex current = ui_tabs_get_active();

// Register callback for tab changes
ui_tabs_set_change_callback([](TabIndex old_tab, TabIndex new_tab) {
    Serial.printf("Switched from tab %d to %d\n", old_tab, new_tab);
    
    // Play tab switch sound
    audio.playTabSwitch();
});
```

## Troubleshooting

### Build Errors

**Error**: `undefined reference to ui_shell_init`
- **Solution**: Make sure `ui_shell.cpp` is in your `src/` folder

**Error**: `ui_theme.h: No such file or directory`
- **Solution**: Verify all new files are present:
  - ui_shell.h/cpp
  - ui_theme.h/cpp
  - ui_statusbar.h/cpp
  - ui_tabs.h/cpp

### Display Issues

**Problem**: Status bar overlaps tabs
- **Solution**: Ensure status bar height = 18px and tabs start at Y=18

**Problem**: Wrong colors displayed
- **Solution**: Call `ui_theme_init()` before creating any UI objects

**Problem**: Tabs not switching
- **Solution**: Check LVGL is ticking in main loop: `lv_timer_handler()`

## Performance Notes

- **ui_main**: ~25 KB RAM, single large file
- **ui_shell**: ~30 KB RAM, modular files
- Trade-off: +5 KB RAM for better code organization

## Recommendation

**For new projects**: Use ui_shell for cleaner architecture

**For existing projects**: Can migrate gradually or keep ui_main

Both systems are fully functional and maintained!

## Support

Questions? Check:
- `UI_SYSTEM_README.md` - Full system documentation
- `src/ui_shell.h` - Main API reference
- `src/ui_theme.h` - Color constants and styles
