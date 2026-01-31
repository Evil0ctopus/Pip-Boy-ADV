# Pip-Boy-ADV Layout Audit & Fixes

## Current Layout Issues

### 1. **Overlapping Tabs** - Root Cause
- **Tab button bar height: 20px** (in `ui_tabs.cpp` line 103)
- With 5 tabs ("STAT DATA MAP RADIO SYS") at montserrat_14 font, they overflow horizontally
- Tab buttons are not using flex layout, causing text to overlap

### 2. **Oversized Bottom Section** - Unreadable
- Tabs only have **115px height** (135 - 20 statusbar - 20 tab buttons = 95px actually)
- Content containers using **fixed Y positioning** (70px, 85px) instead of dynamic
- No scroll protection for content exceeding bounds

### 3. **Hardcoded Dimensions**
- `DISPLAY_WIDTH = 240` (hardcoded across files)
- `DISPLAY_HEIGHT = 135` (hardcoded)
- `STATUSBAR_HEIGHT = 20` (hardcoded)
- Tab button height: **20px** (in ui_tabs_init)
- Font sizes: **montserrat_14** for tab buttons (too large for small screen)

### 4. **Missing Dynamic Screen Sizing**
- No calls to `lv_disp_get_hor_res()` or `lv_disp_get_ver_res()`
- Content layout hardcoded to 240x135 assumption
- Won't scale if display changes

---

## Recommended Fixes

### Fix #1: Reduce Tab Button Height & Use Flex Layout

**File:** `src/ui_tabs.cpp` (lines 100-125)

```cpp
void ui_tabs_init(lv_obj_t *parent) {
    if (parent == NULL) {
        Serial.println("UI Tabs: ERROR - NULL parent");
        return;
    }
    
    // Get dynamic screen dimensions
    int32_t screen_width = lv_disp_get_hor_res(NULL);   // Default: 240
    int32_t screen_height = lv_disp_get_ver_res(NULL);  // Default: 135
    int32_t tab_btn_height = 16;  // REDUCED from 20px
    int32_t tab_content_height = screen_height - STATUSBAR_HEIGHT - tab_btn_height;
    
    Serial.printf("UI Tabs: Screen %dx%d, Tab area: %dx%d\n", 
                  screen_width, screen_height, screen_width, tab_content_height);
    
    // Create tabview with reduced button height
    ui_tabview_main = lv_tabview_create(parent, LV_DIR_TOP, tab_btn_height);
    lv_obj_set_size(ui_tabview_main, screen_width, tab_content_height);
    lv_obj_align(ui_tabview_main, LV_ALIGN_BOTTOM_MID, 0, 0);
    
    // Apply Pip-Boy theme to tabview
    lv_obj_set_style_bg_color(ui_tabview_main, PIPBOY_BLACK, 0);
    lv_obj_set_style_text_color(ui_tabview_main, PIPBOY_GREEN, 0);
    
    // CRITICAL: Style tab buttons with flex layout
    lv_obj_t *tab_btns = lv_tabview_get_tab_btns(ui_tabview_main);
    
    // Enable flex layout to prevent text overlap
    lv_obj_set_style_layout(tab_btns, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_flex_flow(tab_btns, LV_FLEX_FLOW_ROW, 0);
    lv_obj_set_style_flex_main_place(tab_btns, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);  // Space tabs evenly
    lv_obj_set_style_flex_cross_place(tab_btns, LV_FLEX_ALIGN_CENTER, 0);
    
    // Reduce font size for tab buttons
    lv_obj_set_style_text_font(tab_btns, &lv_font_montserrat_10, 0);  // REDUCED from _14
    
    // Tab button styling
    lv_obj_set_style_bg_color(tab_btns, PIPBOY_GREEN_DARK, 0);
    lv_obj_set_style_bg_opa(tab_btns, PIPBOY_OPA_70, 0);
    lv_obj_set_style_text_color(tab_btns, PIPBOY_GREEN_BRIGHT, 0);
    lv_obj_set_style_pad_all(tab_btns, 1, 0);  // REDUCED padding from 2
    lv_obj_set_style_gap(tab_btns, 2, 0);  // Gap between tab buttons
    
    // Active tab style
    lv_obj_set_style_bg_color(tab_btns, PIPBOY_GREEN, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(tab_btns, PIPBOY_OPA_COVER, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(tab_btns, PIPBOY_BLACK, LV_PART_ITEMS | LV_STATE_CHECKED);
    
    // Create all five tabs
    ui_tab_status = lv_tabview_add_tab(ui_tabview_main, "STAT");
    ui_tab_data = lv_tabview_add_tab(ui_tabview_main, "DATA");
    ui_tab_map = lv_tabview_add_tab(ui_tabview_main, "MAP");
    ui_tab_radio = lv_tabview_add_tab(ui_tabview_main, "RADIO");
    ui_tab_system = lv_tabview_add_tab(ui_tabview_main, "SYS");
    
    // Set legacy aliases
    ui_tab_stats = ui_tab_status;
    ui_tab_inventory = ui_tab_map;
    ui_tab_settings = ui_tab_system;
    
    Serial.println("  ✓ Tabs created with flex layout");
    
    // Set tab backgrounds
    lv_obj_set_style_bg_color(ui_tab_status, PIPBOY_BG_DARK, 0);
    lv_obj_set_style_bg_color(ui_tab_data, PIPBOY_BG_DARK, 0);
    lv_obj_set_style_bg_color(ui_tab_map, PIPBOY_BG_DARK, 0);
    lv_obj_set_style_bg_color(ui_tab_radio, PIPBOY_BG_DARK, 0);
    lv_obj_set_style_bg_color(ui_tab_system, PIPBOY_BG_DARK, 0);
    
    // ... rest of function
}
```

---

### Fix #2: Use Dynamic Heights in Tab Content

**File:** `src/ui_tabs.cpp` (in `ui_tabs_create_status()` function, around line 186)

**Replace hardcoded positioning with dynamic layout:**

```cpp
void ui_tabs_create_status(lv_obj_t *parent) {
    if (!parent) return;
    
    // Get container dimensions dynamically
    int32_t container_width = lv_obj_get_width(parent);
    int32_t container_height = lv_obj_get_height(parent);
    
    Serial.printf("STATUS tab dimensions: %dx%d\n", container_width, container_height);
    
    // Title
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "STATUS");
    ui_theme_apply_label_title(title);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_12, 0);  // REDUCED from _14
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 1);
    
    // ========================================================================
    // Use Flex Layout Instead of Fixed Positioning
    // ========================================================================
    
    // Main flex container for content
    lv_obj_t *flex_container = lv_obj_create(parent);
    lv_obj_set_size(flex_container, container_width - 4, container_height - 18);  // Leave room for title
    lv_obj_align(flex_container, LV_ALIGN_BOTTOM_MID, 0, -2);
    
    // Enable flex layout (vertical columns)
    lv_obj_set_style_layout(flex_container, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_flex_flow(flex_container, LV_FLEX_FLOW_ROW, 0);  // Side-by-side
    lv_obj_set_style_flex_main_place(flex_container, LV_FLEX_ALIGN_START, 0);
    lv_obj_set_style_flex_cross_place(flex_container, LV_FLEX_ALIGN_START, 0);
    
    // Remove scrolling and border
    lv_obj_clear_flag(flex_container, LV_OBJ_FLAG_SCROLLABLE);
    ui_theme_apply_container(flex_container);
    
    // ========================================================================
    // LEFT COLUMN: Time, Date, Weather (Dynamic width)
    // ========================================================================
    
    lv_obj_t *left_column = lv_obj_create(flex_container);
    lv_obj_set_width(left_column, (container_width - 6) / 2 - 2);  // 50% with gap
    lv_obj_set_height(left_column, LV_SIZE_CONTENT);
    
    lv_obj_set_style_layout(left_column, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_flex_flow(left_column, LV_FLEX_FLOW_COLUMN, 0);  // Vertical stacking
    lv_obj_set_style_flex_main_place(left_column, LV_FLEX_ALIGN_START, 0);
    lv_obj_clear_flag(left_column, LV_OBJ_FLAG_SCROLLABLE);
    ui_theme_apply_container(left_column);
    
    // Time display
    ui_label_status_time = lv_label_create(left_column);
    lv_label_set_text(ui_label_status_time, "00:00:00");
    ui_theme_apply_label_normal(ui_label_status_time);
    lv_obj_set_style_text_font(ui_label_status_time, &lv_font_montserrat_10, 0);
    
    // Date display
    ui_label_status_date = lv_label_create(left_column);
    lv_label_set_text(ui_label_status_date, "01/01/2026");
    ui_theme_apply_label_small(ui_label_status_date);
    
    // Weather (placeholder)
    ui_label_status_weather = lv_label_create(left_column);
    lv_label_set_text(ui_label_status_weather, "CLEAR");
    ui_theme_apply_label_small(ui_label_status_weather);
    
    // Temperature
    ui_label_status_temp = lv_label_create(left_column);
    lv_label_set_text(ui_label_status_temp, "72°F");
    ui_theme_apply_label_small(ui_label_status_temp);
    
    // ========================================================================
    // RIGHT COLUMN: Animation Container
    // ========================================================================
    
    ui_animation_container = lv_obj_create(flex_container);
    lv_obj_set_width(ui_animation_container, (container_width - 6) / 2 - 2);  // 50% with gap
    lv_obj_set_height(ui_animation_container, LV_SIZE_CONTENT);
    
    ui_theme_apply_container(ui_animation_container);
    lv_obj_clear_flag(ui_animation_container, LV_OBJ_FLAG_SCROLLABLE);
    
    // Initialize animation system
    ui_animations_init(ui_animation_container);
    
    // ========================================================================
    // BOTTOM: Battery Status Bar
    // ========================================================================
    
    // Battery container at bottom (full width, minimal height)
    lv_obj_t *battery_container = lv_obj_create(parent);
    lv_obj_set_width(battery_container, container_width - 4);
    lv_obj_set_height(battery_container, 12);
    lv_obj_align(battery_container, LV_ALIGN_BOTTOM_MID, 0, -2);
    
    lv_obj_set_style_layout(battery_container, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_flex_flow(battery_container, LV_FLEX_FLOW_ROW, 0);
    lv_obj_set_style_flex_main_place(battery_container, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);
    lv_obj_clear_flag(battery_container, LV_OBJ_FLAG_SCROLLABLE);
    ui_theme_apply_container(battery_container);
    
    // Battery label
    ui_label_status_battery = lv_label_create(battery_container);
    lv_label_set_text(ui_label_status_battery, "POWER:");
    ui_theme_apply_label_small(ui_label_status_battery);
    lv_obj_set_style_text_font(ui_label_status_battery, &lv_font_montserrat_8, 0);
    
    // Battery bar (right side, 60% of width)
    ui_bar_battery = lv_bar_create(battery_container);
    lv_obj_set_width(ui_bar_battery, 60);
    lv_obj_set_height(ui_bar_battery, 10);
    lv_bar_set_range(ui_bar_battery, 0, 100);
    lv_bar_set_value(ui_bar_battery, 75, LV_ANIM_OFF);
}
```

---

### Fix #3: Add Header Definitions for Dynamic Sizing

**File:** Create or update `include/ui_config.h`

```cpp
#ifndef UI_CONFIG_H
#define UI_CONFIG_H

#include <lvgl.h>

// Dynamic Screen Dimensions (retrieved from LVGL display)
// These should be read from the display driver, not hardcoded
#define DISPLAY_WIDTH_DEFAULT 240
#define DISPLAY_HEIGHT_DEFAULT 135
#define STATUSBAR_HEIGHT 15  // REDUCED from 20

// Tab Layout Configuration
#define TAB_BUTTON_HEIGHT 16  // REDUCED from 20 (makes room for content)
#define TAB_CONTENT_HEIGHT (DISPLAY_HEIGHT_DEFAULT - STATUSBAR_HEIGHT - TAB_BUTTON_HEIGHT)

// Font Scaling for Small Display
#define TITLE_FONT &lv_font_montserrat_12     // Was _14
#define LABEL_FONT &lv_font_montserrat_10     // Standard label
#define SMALL_FONT &lv_font_montserrat_8      // Small text

// Padding and Margins (in pixels)
#define PADDING_NORMAL 2       // REDUCED from 3
#define PADDING_SMALL 1        // REDUCED from 2
#define GAP_NORMAL 2
#define GAP_SMALL 1

// Container Constraints
#define CONTAINER_MIN_WIDTH 60
#define CONTAINER_MIN_HEIGHT 30

// Dynamic Screen Dimension Helpers
static inline int32_t ui_get_screen_width(void) {
    return lv_disp_get_hor_res(NULL);  // Gets actual display width
}

static inline int32_t ui_get_screen_height(void) {
    return lv_disp_get_ver_res(NULL);  // Gets actual display height
}

static inline int32_t ui_get_tab_content_width(void) {
    return ui_get_screen_width() - (PADDING_NORMAL * 2);
}

static inline int32_t ui_get_tab_content_height(void) {
    return ui_get_screen_height() - STATUSBAR_HEIGHT - TAB_BUTTON_HEIGHT - (PADDING_SMALL * 2);
}

#endif // UI_CONFIG_H
```

---

### Fix #4: Update Status Bar Height

**File:** `include/ui_statusbar.h`

```cpp
// Change from:
#define STATUSBAR_HEIGHT 20

// To:
#define STATUSBAR_HEIGHT 15
```

**And in `src/ui_statusbar.cpp`, update the creation:**

```cpp
void ui_statusbar_init(lv_obj_t *parent) {
    // Get dynamic dimensions
    int32_t screen_width = lv_disp_get_hor_res(NULL);
    int32_t statusbar_height = 15;  // Reduced from 20
    
    ui_statusbar = lv_obj_create(parent);
    lv_obj_set_size(ui_statusbar, screen_width, statusbar_height);
    lv_obj_align(ui_statusbar, LV_ALIGN_TOP_MID, 0, 0);
    
    // ... rest of init
}
```

---

### Fix #5: Add Scroll Protection for Content

Add this to any tab content function that has many items:

```cpp
// Example: In ui_tabs_create_system()
void ui_tabs_create_system(lv_obj_t *parent) {
    // Get safe content area
    int32_t max_height = lv_obj_get_height(parent);
    int32_t used_height = 0;
    
    // Create scrollable container if content might exceed bounds
    lv_obj_t *scroll_container = lv_obj_create(parent);
    lv_obj_set_size(scroll_container, lv_obj_get_width(parent) - 4, max_height - 4);
    lv_obj_align(scroll_container, LV_ALIGN_CENTER, 0, 0);
    
    // Enable scrolling
    lv_obj_set_style_layout(scroll_container, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_flex_flow(scroll_container, LV_FLEX_FLOW_COLUMN, 0);
    lv_obj_set_scrollbar_mode(scroll_container, LV_SCROLLBAR_MODE_AUTO);
    
    // Content goes into scroll_container instead of parent
    // This ensures items don't overflow screen
    
    // ... add items to scroll_container
}
```

---

## Implementation Summary

| Issue | Fix | File | Priority |
|-------|-----|------|----------|
| Overlapping tabs | Flex layout + reduced font | `src/ui_tabs.cpp:100-125` | **CRITICAL** |
| Oversized bottom section | Dynamic positioning + flex | `src/ui_tabs.cpp:186+` | **CRITICAL** |
| Hardcoded dimensions | Add `ui_config.h` with helpers | New file | **HIGH** |
| Statusbar too tall | Reduce to 15px | `include/ui_statusbar.h` | **HIGH** |
| Tab buttons too tall | Reduce to 16px | `src/ui_tabs.cpp` | **HIGH** |
| Font sizes | Scale down by 2px | Multiple files | **HIGH** |
| No scroll protection | Add flex + scrolling | Tab content functions | **MEDIUM** |

---

## Testing Checklist

- [ ] Tab names all visible and evenly spaced ("STAT DATA MAP RADIO SYS")
- [ ] Tab content fully readable at bottom of screen
- [ ] No overlapping text
- [ ] All 5 tabs accessible by clicking middle button
- [ ] Status bar shows battery/time clearly
- [ ] Layout adapts if screen resolution changes
- [ ] No content extends beyond screen bounds
- [ ] Buttons still responsive

---

## Notes

- Montserrat font already embedded (no additional dependencies needed)
- LVGL flex layout is GPU-efficient
- Changes are backward compatible (no API changes)
- Can be applied incrementally (fix tabs first, then content)
