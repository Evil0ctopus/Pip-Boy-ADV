# Pip-Boy-ADV UI Architecture Diagram

## System Overview

```
┌─────────────────────────────────────────────────────────────┐
│                        main.cpp                             │
│  ┌────────────┐  ┌──────────────┐  ┌──────────────────┐   │
│  │ M5.begin() │→ │ lvgl_setup() │→ │ ui_shell_init()  │   │
│  └────────────┘  └──────────────┘  └──────────────────┘   │
│                                             ↓               │
│  ┌────────────────────────────────────────────────────┐    │
│  │ ui_shell_update_*() calls from tasks/helpers      │    │
│  └────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                      ui_shell.cpp                           │
│   ┌──────────────────────────────────────────────────┐     │
│   │  Coordinator - Routes updates to sub-modules     │     │
│   └──────────────────────────────────────────────────┘     │
│                    ↓         ↓          ↓                   │
│     ┌──────────────┴──┬──────┴───────┬──┴────────────┐     │
│     ↓                 ↓              ↓               ↓     │
│ ui_theme         ui_statusbar    ui_tabs      ui_animations│
└─────────────────────────────────────────────────────────────┘
```

## Module Hierarchy

```
ui_shell (Main Coordinator)
│
├── ui_theme (Styling & Colors)
│   ├── PIPBOY_GREEN_BRIGHT (#33FF33)
│   ├── PIPBOY_GREEN        (#00FF00)
│   ├── PIPBOY_GREEN_DARK   (#006600)
│   ├── PIPBOY_BG_DARK      (#002200)
│   └── 14 LVGL style structs
│
├── ui_statusbar (Top Status Bar - 18px)
│   ├── Time Display (left)
│   ├── WiFi Indicator [W]
│   ├── LoRa Indicator [L]
│   └── Battery % (right)
│
├── ui_tabs (Tabview - 117px)
│   ├── Tab Buttons (22px)
│   │   ├── [STAT]
│   │   ├── [RADIO]
│   │   ├── [ITEMS]
│   │   └── [DATA]
│   │
│   └── Tab Content (95px)
│       ├── STAT Tab
│       │   ├── Time/Date labels
│       │   ├── Weather info
│       │   ├── Battery bar
│       │   └── Animation Container
│       │       └── ui_animations
│       │
│       ├── RADIO Tab
│       │   └── ui_radio
│       │       ├── RSSI/SNR display
│       │       ├── Message history
│       │       └── Send controls
│       │
│       ├── ITEMS Tab
│       │   └── File browser (placeholder)
│       │
│       └── DATA Tab
│           └── ui_settings
│               ├── Brightness slider
│               ├── WiFi status
│               └── System info
│
└── ui_assets (SD Card Loader)
    ├── PNG decoder
    ├── Image cache
    └── Sprite frame loader
```

## Data Flow

### Update Time Example

```
WiFiHelper.cpp
     │
     │ getCurrentTime()
     ↓
ui_shell_update_time("12:34 PM")
     │
     ├──→ ui_statusbar_update_time()
     │         │
     │         └──→ lv_label_set_text(ui_statusbar_time, "12:34 PM")
     │
     └──→ ui_tabs_update_time()
               │
               └──→ lv_label_set_text(ui_label_stats_time, "12:34 PM")
```

### LoRa Status Update Example

```
LoRaHelper.cpp
     │
     │ receive() → packet.rssi = -75.5
     ↓
ui_shell_update_lora(true, -75.5)
     │
     └──→ ui_statusbar_update_lora(true, -75.5)
               │
               ├──→ lv_label_set_text(ui_statusbar_lora, "L")
               │
               └──→ Set color based on RSSI:
                    RSSI > -80:  PIPBOY_GREEN_BRIGHT
                    RSSI > -100: PIPBOY_GREEN
                    else:        PIPBOY_GREEN_DARK
```

## Screen Layout Detail

```
┌─────────────────────────────────────────────┐ ─┐
│ 12:34 PM       [L][W]              85% ⚡   │  │ 18px
│                                             │  │ Status Bar
└─────────────────────────────────────────────┘ ─┘
┌─────────────────────────────────────────────┐ ─┐
│ [STAT] [RADIO] [ITEMS] [DATA]               │  │ 22px
│                                             │  │ Tab Buttons
└─────────────────────────────────────────────┘ ─┘
┌─────────────────────────────────────────────┐ ─┐
│              STAT                           │  │
│           12:00 PM                          │  │
│       Mon, Jan 21, 2026                     │  │
│                                             │  │
│  Weather: Sunny          ┌────────────┐    │  │
│  Temp: 22°C              │            │    │  │ 95px
│                          │  WALKING   │    │  │ Content
│                          │  SPRITE    │    │  │ Area
│                          │  (64×64)   │    │  │
│                          └────────────┘    │  │
│  PWR: [██████████░░░] 75%                  │  │
└─────────────────────────────────────────────┘ ─┘
      Total Height: 135px (18+22+95)
      Total Width:  240px
```

## Component Sizing

```
Status Bar Layout (240×18px):
┌────────────────────────────────────────────┐
│ 12:34  [L][W]                         85%  │
│ ←5px→  ←15px→                        ←30px→│
└────────────────────────────────────────────┘

STAT Tab Layout (240×95px):
┌────────────────────────────────────────────┐
│              STAT        ← 18pt font       │ Y=2
│           12:00 PM      ← 16pt font       │ Y=22
│       Mon, Jan 21       ← 12pt font       │ Y=42
│                                            │
│  Weather: Sunny  ← 10pt  ┌──────────┐     │ Y=60
│  Temp: 22°C              │  70×70px │     │ Y=73
│                          │  Sprite  │     │
│                          └──────────┘     │
│  PWR: [120px bar] 75%                     │ Y=77
└────────────────────────────────────────────┘
```

## Memory Layout

```
PSRAM (ESP32-S3)
├── LVGL Display Buffers
│   ├── buf1: 240×10×2 = 4.8 KB
│   └── buf2: 240×10×2 = 4.8 KB
│
├── UI Object Tree (~8 KB)
│   ├── ui_screen_main
│   │   ├── ui_statusbar
│   │   │   ├── ui_statusbar_time
│   │   │   ├── ui_statusbar_wifi
│   │   │   ├── ui_statusbar_lora
│   │   │   └── ui_statusbar_battery
│   │   │
│   │   └── ui_tabview_main
│   │       ├── ui_tab_stats
│   │       ├── ui_tab_radio
│   │       ├── ui_tab_inventory
│   │       └── ui_tab_settings
│   │
│   └── Animation Container
│       └── Current frame image
│
├── Theme Styles (~1 KB)
│   ├── style_screen
│   ├── style_container
│   ├── style_label_title/normal/small
│   ├── style_button/button_pressed
│   ├── style_slider/bar/textarea
│   └── style_tabview/tab_active/inactive
│
└── Animation Cache (~8 KB per frame)
    ├── walking_01.png decoded
    ├── walking_02.png decoded
    └── ... (cached on demand)

Total: ~30 KB for UI system
```

## Task Integration

```
FreeRTOS Tasks → UI Updates
│
├── wifiTask (Core 1, Priority 1)
│   └── Every 10s → ui_shell_update_wifi()
│                   ui_shell_update_time()
│
├── weatherTask (Core 1, Priority 1)
│   └── Every 5min → ui_shell_update_weather()
│
├── batteryTask (Core 1, Priority 2)
│   └── Every 30s → ui_shell_update_battery()
│
├── loraReceiveTask (Core 1, Priority 3)
│   └── On packet → ui_shell_update_lora()
│                   ui_radio_add_message()
│
└── sensorMonitorTask (Core 1, Priority 2)
    └── Every 50ms → ui_shell_play_animation() (on shake)

Main Loop (Core 0)
└── Every 5ms → lv_timer_handler() (LVGL rendering)
```

## Color Palette Visual

```
Status Indicator Colors:

WiFi/LoRa Signal Strength:
┌──────────────┬──────────────┬──────────────┬──────────────┐
│ Excellent    │ Good         │ Weak         │ Disconnected │
│ #33FF33      │ #00FF00      │ #006600      │ #003300      │
│ Bright Green │ Green        │ Dark Green   │ Darker Green │
└──────────────┴──────────────┴──────────────┴──────────────┘

Battery Level:
┌────────┬────────┬────────┬────────┐
│ >60%   │ 30-60% │ 15-30% │ <15%   │
│ #00FF00│ #00CC00│ #FFCC00│ #FF3333│
│ Green  │ Med Grn│ Yellow │ Red    │
└────────┴────────┴────────┴────────┘

Theme Colors:
┌─────────────────┬─────────────────┐
│ Text/Borders    │ Backgrounds     │
├─────────────────┼─────────────────┤
│ #33FF33 Bright  │ #002200 Dark    │
│ #00FF00 Normal  │ #001100 Darker  │
│ #00CC00 Medium  │ #000000 Black   │
│ #006600 Dark    │                 │
└─────────────────┴─────────────────┘
```

## File Dependencies

```
main.cpp
├─ ui_shell.h
│  ├─ ui_theme.h
│  ├─ ui_statusbar.h
│  │  └─ ui_theme.h
│  ├─ ui_tabs.h
│  │  ├─ ui_theme.h
│  │  ├─ ui_radio.h
│  │  ├─ ui_settings.h
│  │  └─ ui_animations.h
│  │     └─ ui_assets.h
│  │        └─ SD.h
│  └─ ui_animations.h
│
├─ WiFiHelper.h
├─ WeatherHelper.h
├─ LoRaHelper.h
├─ AudioHelper.h
├─ SensorHelper.h
└─ BluetoothHelper.h
```

## Event Flow

```
User Input (Shake) → Sensor Event → Animation Change
────────────────────────────────────────────────────

1. User shakes Cardputer
      ↓
2. IMU detects acceleration > threshold
      ↓
3. sensors.detectShake() returns true
      ↓
4. sensorMonitorTask() calls ui_shell_play_animation()
      ↓
5. ui_animation_play(ANIM_THUMBSUP)
      ↓
6. Timer loads frames from SD card
      ↓
7. lv_timer_handler() renders new frame
      ↓
8. Display shows animation (10 FPS)
```

## Build Process

```
PlatformIO Build
├─ platformio.ini
│  ├─ platform = espressif32
│  ├─ board = m5stack-cardputer
│  ├─ framework = arduino
│  └─ lib_deps
│     ├─ M5Unified@^0.1.16
│     ├─ lvgl@^8.3.11
│     ├─ RadioLib@^6.4.0
│     └─ ArduinoJson@^6.21.3
│
├─ Compile src/*.cpp
│  ├─ main.cpp
│  ├─ ui_shell.cpp
│  ├─ ui_theme.cpp
│  ├─ ui_statusbar.cpp
│  ├─ ui_tabs.cpp
│  ├─ ui_animations.cpp
│  ├─ ui_radio.cpp
│  ├─ ui_settings.cpp
│  ├─ ui_assets.cpp
│  ├─ WiFiHelper.cpp
│  ├─ WeatherHelper.cpp
│  ├─ LoRaHelper.cpp
│  ├─ AudioHelper.cpp
│  ├─ SensorHelper.cpp
│  └─ BluetoothHelper.cpp
│
├─ Link with libraries
└─ Generate firmware.bin
```

---

This architecture provides:
- ✅ Clear separation of concerns
- ✅ Modular and testable components
- ✅ Easy customization and theming
- ✅ Efficient memory usage
- ✅ Scalable for future features
