// ============================================================================
// Pip-Boy ADV Tab System - Complete Implementation
// ============================================================================
// Five-tab interface: STATUS | DATA | MAP | RADIO | SYSTEM
// Uses LVGL tabview with custom Pip-Boy theme
// ============================================================================

#include "ui_tabs.h"
#include "ui_theme.h"
#include "ui_radio.h"
#include "ui_settings.h"
#include "ui_animations.h"
#include "ui_statusbar.h"
#include <Arduino.h>

// ============================================================================
// Global Tab Objects
// ============================================================================

lv_obj_t *ui_tabview_main = NULL;
lv_obj_t *ui_tab_status = NULL;
lv_obj_t *ui_tab_data = NULL;
lv_obj_t *ui_tab_map = NULL;
lv_obj_t *ui_tab_radio = NULL;
lv_obj_t *ui_tab_system = NULL;
lv_obj_t *ui_animation_container = NULL;

// Legacy aliases for compatibility
lv_obj_t *ui_tab_stats = NULL;
lv_obj_t *ui_tab_inventory = NULL;
lv_obj_t *ui_tab_settings = NULL;

// ============================================================================
// STATUS Tab UI Elements
// ============================================================================

static lv_obj_t *ui_label_status_time = NULL;
static lv_obj_t *ui_label_status_date = NULL;
static lv_obj_t *ui_label_status_weather = NULL;
static lv_obj_t *ui_label_status_temp = NULL;
static lv_obj_t *ui_bar_battery = NULL;
static lv_obj_t *ui_label_battery = NULL;

// ============================================================================
// DATA Tab UI Elements
// ============================================================================

static lv_obj_t *ui_label_data_cpu = NULL;
static lv_obj_t *ui_label_data_memory = NULL;
static lv_obj_t *ui_label_data_psram = NULL;
static lv_obj_t *ui_label_data_uptime = NULL;
static lv_obj_t *ui_label_data_tasks = NULL;
static lv_obj_t *ui_bar_memory = NULL;
static lv_obj_t *ui_bar_psram = NULL;

// ============================================================================
// MAP Tab UI Elements
// ============================================================================

static lv_obj_t *ui_label_map_coords = NULL;
static lv_obj_t *ui_label_map_status = NULL;

// ============================================================================
// Tab Change Callback
// ============================================================================

static void (*tab_change_callback)(TabIndex, TabIndex) = NULL;

static void tab_changed_event(lv_event_t *e) {
    lv_obj_t *tabview = lv_event_get_target(e);
    static TabIndex previous_tab = TAB_STATUS;
    TabIndex current_tab = (TabIndex)lv_tabview_get_tab_act(tabview);
    
    if (tab_change_callback && current_tab != previous_tab) {
        tab_change_callback(previous_tab, current_tab);
    }
    
    previous_tab = current_tab;
}

// ============================================================================
// Tab System Initialization
// ============================================================================

void ui_tabs_init(lv_obj_t *parent) {
    if (parent == NULL) {
        Serial.println("UI Tabs: ERROR - NULL parent");
        return;
    }
    
    Serial.println("UI Tabs: Creating tabview...");
    
    // Create tabview (positioned below status bar)
    ui_tabview_main = lv_tabview_create(parent, LV_DIR_TOP, 20);
    lv_obj_set_size(ui_tabview_main, 240, 135 - STATUSBAR_HEIGHT);
    lv_obj_align(ui_tabview_main, LV_ALIGN_BOTTOM_MID, 0, 0);
    
    // Apply Pip-Boy theme to tabview
    lv_obj_set_style_bg_color(ui_tabview_main, PIPBOY_BLACK, 0);
    lv_obj_set_style_text_color(ui_tabview_main, PIPBOY_GREEN, 0);
    
    // Style the tab buttons
    lv_obj_t *tab_btns = lv_tabview_get_tab_btns(ui_tabview_main);
    lv_obj_set_style_bg_color(tab_btns, PIPBOY_GREEN_DARK, 0);
    lv_obj_set_style_bg_opa(tab_btns, PIPBOY_OPA_70, 0);
    lv_obj_set_style_text_color(tab_btns, PIPBOY_GREEN_BRIGHT, 0);
    lv_obj_set_style_text_font(tab_btns, &lv_font_montserrat_14, 0);
    lv_obj_set_style_pad_all(tab_btns, 2, 0);
    
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
    
    Serial.println("  ✓ Tabs created");
    
    // Set tab backgrounds
    lv_obj_set_style_bg_color(ui_tab_status, PIPBOY_BG_DARK, 0);
    lv_obj_set_style_bg_color(ui_tab_data, PIPBOY_BG_DARK, 0);
    lv_obj_set_style_bg_color(ui_tab_map, PIPBOY_BG_DARK, 0);
    lv_obj_set_style_bg_color(ui_tab_radio, PIPBOY_BG_DARK, 0);
    lv_obj_set_style_bg_color(ui_tab_system, PIPBOY_BG_DARK, 0);
    
    // Create tab contents
    ui_tabs_create_status(ui_tab_status);
    ui_tabs_create_data(ui_tab_data);
    ui_tabs_create_map(ui_tab_map);
    ui_tabs_create_radio(ui_tab_radio);
    ui_tabs_create_system(ui_tab_system);
    
    Serial.println("  ✓ Tab contents populated");
    
    // Add tab change event
    lv_obj_add_event_cb(ui_tabview_main, tab_changed_event, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Start with STATUS tab
    lv_tabview_set_act(ui_tabview_main, TAB_STATUS, LV_ANIM_OFF);
    
    Serial.println("UI Tabs: Initialization complete");
}

// Legacy compatibility function
void ui_tabs_create_stats(lv_obj_t *parent) {
    ui_tabs_create_status(parent);
}

void ui_tabs_create_inventory(lv_obj_t *parent) {
    ui_tabs_create_map(parent);
}

void ui_tabs_create_settings(lv_obj_t *parent) {
    ui_tabs_create_system(parent);
}

// ============================================================================
// STATUS Tab Creation
// ============================================================================

void ui_tabs_create_status(lv_obj_t *parent) {
    if (!parent) return;
    
    // Title
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "STATUS");
    ui_theme_apply_label_title(title);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 1);
    
    // ========================================================================
    // Left Column: Time, Date, Weather
    // ========================================================================
    
    lv_obj_t *left_container = lv_obj_create(parent);
    lv_obj_set_size(left_container, 140, 85);
    lv_obj_align(left_container, LV_ALIGN_TOP_LEFT, 2, 18);
    ui_theme_apply_container(left_container);
    lv_obj_clear_flag(left_container, LV_OBJ_FLAG_SCROLLABLE);
    
    // Time display (large)
    ui_label_status_time = lv_label_create(left_container);
    lv_label_set_text(ui_label_status_time, "12:00 PM");
    lv_obj_set_style_text_color(ui_label_status_time, PIPBOY_GREEN_BRIGHT, 0);
    lv_obj_set_style_text_font(ui_label_status_time, &lv_font_montserrat_16, 0);
    lv_obj_align(ui_label_status_time, LV_ALIGN_TOP_MID, 0, 3);
    
    // Date display
    ui_label_status_date = lv_label_create(left_container);
    lv_label_set_text(ui_label_status_date, "Mon, Jan 28, 2026");
    ui_theme_apply_label_normal(ui_label_status_date);
    lv_obj_align(ui_label_status_date, LV_ALIGN_TOP_MID, 0, 26);
    
    // Weather section
    ui_label_status_weather = lv_label_create(left_container);
    lv_label_set_text(ui_label_status_weather, "Weather: --");
    ui_theme_apply_label_small(ui_label_status_weather);
    lv_obj_align(ui_label_status_weather, LV_ALIGN_TOP_LEFT, 3, 48);
    
    ui_label_status_temp = lv_label_create(left_container);
    lv_label_set_text(ui_label_status_temp, "Temp: --°C");
    ui_theme_apply_label_small(ui_label_status_temp);
    lv_obj_align(ui_label_status_temp, LV_ALIGN_TOP_LEFT, 3, 62);
    
    // ========================================================================
    // Right Column: Animation Container
    // ========================================================================
    
    ui_animation_container = lv_obj_create(parent);
    lv_obj_set_size(ui_animation_container, 85, 85);
    lv_obj_align(ui_animation_container, LV_ALIGN_TOP_RIGHT, -2, 18);
    ui_theme_apply_container(ui_animation_container);
    lv_obj_clear_flag(ui_animation_container, LV_OBJ_FLAG_SCROLLABLE);
    
    // Initialize animation system
    ui_animations_init(ui_animation_container);
    
    // ========================================================================
    // Bottom: Battery Status
    // ========================================================================
    
    ui_label_battery = lv_label_create(parent);
    lv_label_set_text(ui_label_battery, "POWER:");
    ui_theme_apply_label_small(ui_label_battery);
    lv_obj_align(ui_label_battery, LV_ALIGN_BOTTOM_LEFT, 3, -2);
    
    ui_bar_battery = lv_bar_create(parent);
    lv_obj_set_size(ui_bar_battery, 140, 10);
    lv_obj_align(ui_bar_battery, LV_ALIGN_BOTTOM_LEFT, 45, -2);
    lv_bar_set_range(ui_bar_battery, 0, 100);
    lv_bar_set_value(ui_bar_battery, 75, LV_ANIM_OFF);
    ui_theme_apply_bar(ui_bar_battery);
    lv_obj_set_style_bg_color(ui_bar_battery, PIPBOY_GREEN, LV_PART_INDICATOR);
    
    // Battery percentage label
    lv_obj_t *bat_pct = lv_label_create(parent);
    lv_label_set_text(bat_pct, "75%");
    ui_theme_apply_label_small(bat_pct);
    lv_obj_align(bat_pct, LV_ALIGN_BOTTOM_RIGHT, -3, -2);
}

// ============================================================================
// DATA Tab Creation - System Statistics
// ============================================================================

void ui_tabs_create_data(lv_obj_t *parent) {
    if (!parent) return;
    
    // Title
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "SYSTEM DATA");
    ui_theme_apply_label_title(title);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 1);
    
    // ========================================================================
    // CPU Information
    // ========================================================================
    
    lv_obj_t *cpu_label = lv_label_create(parent);
    lv_label_set_text(cpu_label, "CPU:");
    ui_theme_apply_label_normal(cpu_label);
    lv_obj_align(cpu_label, LV_ALIGN_TOP_LEFT, 3, 18);
    
    ui_label_data_cpu = lv_label_create(parent);
    lv_label_set_text(ui_label_data_cpu, "ESP32-S3 @ 240MHz");
    ui_theme_apply_label_small(ui_label_data_cpu);
    lv_obj_set_style_text_color(ui_label_data_cpu, PIPBOY_GREEN_BRIGHT, 0);
    lv_obj_align(ui_label_data_cpu, LV_ALIGN_TOP_LEFT, 45, 18);
    
    // ========================================================================
    // Memory (Heap) Information
    // ========================================================================
    
    lv_obj_t *mem_label = lv_label_create(parent);
    lv_label_set_text(mem_label, "HEAP:");
    ui_theme_apply_label_normal(mem_label);
    lv_obj_align(mem_label, LV_ALIGN_TOP_LEFT, 3, 33);
    
    ui_label_data_memory = lv_label_create(parent);
    lv_label_set_text(ui_label_data_memory, "256KB / 320KB");
    ui_theme_apply_label_small(ui_label_data_memory);
    lv_obj_align(ui_label_data_memory, LV_ALIGN_TOP_LEFT, 45, 33);
    
    ui_bar_memory = lv_bar_create(parent);
    lv_obj_set_size(ui_bar_memory, 180, 8);
    lv_obj_align(ui_bar_memory, LV_ALIGN_TOP_LEFT, 45, 46);
    lv_bar_set_range(ui_bar_memory, 0, 100);
    lv_bar_set_value(ui_bar_memory, 80, LV_ANIM_OFF);
    ui_theme_apply_bar(ui_bar_memory);
    lv_obj_set_style_bg_color(ui_bar_memory, PIPBOY_GREEN, LV_PART_INDICATOR);
    
    // ========================================================================
    // PSRAM Information
    // ========================================================================
    
    lv_obj_t *psram_label = lv_label_create(parent);
    lv_label_set_text(psram_label, "PSRAM:");
    ui_theme_apply_label_normal(psram_label);
    lv_obj_align(psram_label, LV_ALIGN_TOP_LEFT, 3, 58);
    
    ui_label_data_psram = lv_label_create(parent);
    lv_label_set_text(ui_label_data_psram, "6MB / 8MB");
    ui_theme_apply_label_small(ui_label_data_psram);
    lv_obj_align(ui_label_data_psram, LV_ALIGN_TOP_LEFT, 45, 58);
    
    ui_bar_psram = lv_bar_create(parent);
    lv_obj_set_size(ui_bar_psram, 180, 8);
    lv_obj_align(ui_bar_psram, LV_ALIGN_TOP_LEFT, 45, 71);
    lv_bar_set_range(ui_bar_psram, 0, 100);
    lv_bar_set_value(ui_bar_psram, 75, LV_ANIM_OFF);
    ui_theme_apply_bar(ui_bar_psram);
    lv_obj_set_style_bg_color(ui_bar_psram, PIPBOY_GREEN, LV_PART_INDICATOR);
    
    // ========================================================================
    // Uptime
    // ========================================================================
    
    lv_obj_t *uptime_label = lv_label_create(parent);
    lv_label_set_text(uptime_label, "UPTIME:");
    ui_theme_apply_label_normal(uptime_label);
    lv_obj_align(uptime_label, LV_ALIGN_TOP_LEFT, 3, 83);
    
    ui_label_data_uptime = lv_label_create(parent);
    lv_label_set_text(ui_label_data_uptime, "0d 00:00:00");
    ui_theme_apply_label_small(ui_label_data_uptime);
    lv_obj_set_style_text_color(ui_label_data_uptime, PIPBOY_GREEN_BRIGHT, 0);
    lv_obj_align(ui_label_data_uptime, LV_ALIGN_TOP_LEFT, 60, 83);
    
    // ========================================================================
    // FreeRTOS Tasks
    // ========================================================================
    
    lv_obj_t *task_label = lv_label_create(parent);
    lv_label_set_text(task_label, "TASKS:");
    ui_theme_apply_label_normal(task_label);
    lv_obj_align(task_label, LV_ALIGN_TOP_LEFT, 3, 96);
    
    ui_label_data_tasks = lv_label_create(parent);
    lv_label_set_text(ui_label_data_tasks, "8 running");
    ui_theme_apply_label_small(ui_label_data_tasks);
    lv_obj_align(ui_label_data_tasks, LV_ALIGN_TOP_LEFT, 50, 96);
}

// ============================================================================
// MAP Tab Creation - GPS Placeholder
// ============================================================================

void ui_tabs_create_map(lv_obj_t *parent) {
    if (!parent) return;
    
    // Title
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "LOCAL MAP");
    ui_theme_apply_label_title(title);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 1);
    
    // Map canvas placeholder (future: actual map rendering)
    lv_obj_t *map_canvas = lv_obj_create(parent);
    lv_obj_set_size(map_canvas, 220, 70);
    lv_obj_align(map_canvas, LV_ALIGN_TOP_MID, 0, 18);
    lv_obj_set_style_bg_color(map_canvas, PIPBOY_BG_DARKER, 0);
    lv_obj_set_style_border_color(map_canvas, PIPBOY_GREEN_DARK, 0);
    lv_obj_set_style_border_width(map_canvas, 1, 0);
    lv_obj_clear_flag(map_canvas, LV_OBJ_FLAG_SCROLLABLE);
    
    // Placeholder text
    lv_obj_t *placeholder = lv_label_create(map_canvas);
    lv_label_set_text(placeholder, "[GPS Module Required]\n\nFuture Features:\n• Real-time location\n• Waypoint marking\n• Distance tracking");
    ui_theme_apply_label_small(placeholder);
    lv_obj_set_style_text_align(placeholder, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(placeholder);
    
    // Status at bottom
    ui_label_map_status = lv_label_create(parent);
    lv_label_set_text(ui_label_map_status, "GPS: Not Available");
    ui_theme_apply_label_small(ui_label_map_status);
    lv_obj_set_style_text_color(ui_label_map_status, PIPBOY_GREEN_DARK, 0);
    lv_obj_align(ui_label_map_status, LV_ALIGN_BOTTOM_MID, 0, -2);
    
    // Coordinates display
    ui_label_map_coords = lv_label_create(parent);
    lv_label_set_text(ui_label_map_coords, "LAT: --  LON: --");
    ui_theme_apply_label_small(ui_label_map_coords);
    lv_obj_align(ui_label_map_coords, LV_ALIGN_BOTTOM_MID, 0, -14);
}

// ============================================================================
// RADIO Tab Creation - LoRa Communication
// ============================================================================

void ui_tabs_create_radio(lv_obj_t *parent) {
    if (!parent) return;
    
    // Use the existing ui_radio module
    ui_radio_init(parent);
}

// ============================================================================
// SYSTEM Tab Creation - Settings and Configuration
// ============================================================================

void ui_tabs_create_system(lv_obj_t *parent) {
    if (!parent) return;
    
    // Use the existing ui_settings module
    ui_settings_init(parent);
}

// ============================================================================
// Tab Switching Functions
// ============================================================================

void ui_tabs_switch_to(TabIndex index) {
    if (ui_tabview_main && index >= 0 && index < TAB_COUNT) {
        lv_tabview_set_act(ui_tabview_main, index, LV_ANIM_ON);
        Serial.printf("Tab switched to: %d\n", index);
    }
}

TabIndex ui_tabs_get_active() {
    if (ui_tabview_main) {
        return (TabIndex)lv_tabview_get_tab_act(ui_tabview_main);
    }
    return TAB_STATUS;
}

void ui_tabs_set_change_callback(void (*callback)(TabIndex old_tab, TabIndex new_tab)) {
    tab_change_callback = callback;
}

// ============================================================================
// STATUS Tab Update Functions
// ============================================================================

void ui_tabs_update_time(const char *time_str) {
    if (ui_label_status_time) {
        lv_label_set_text(ui_label_status_time, time_str);
    }
}

void ui_tabs_update_date(const char *date_str) {
    if (ui_label_status_date) {
        lv_label_set_text(ui_label_status_date, date_str);
    }
}

void ui_tabs_update_weather(const char *weather_str, const char *temp_str) {
    if (ui_label_status_weather) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Weather: %s", weather_str);
        lv_label_set_text(ui_label_status_weather, buf);
    }
    if (ui_label_status_temp) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Temp: %s", temp_str);
        lv_label_set_text(ui_label_status_temp, buf);
    }
}

void ui_tabs_update_battery(int percentage) {
    if (ui_bar_battery) {
        lv_bar_set_value(ui_bar_battery, percentage, LV_ANIM_ON);
    }
}

// ============================================================================
// DATA Tab Update Functions
// ============================================================================

void ui_tabs_update_cpu_freq(uint8_t freq_mhz) {
    if (ui_label_data_cpu) {
        char buf[32];
        snprintf(buf, sizeof(buf), "ESP32-S3 @ %dMHz", freq_mhz);
        lv_label_set_text(ui_label_data_cpu, buf);
    }
}

void ui_tabs_update_memory(uint32_t free_heap, uint32_t total_heap) {
    if (ui_label_data_memory) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%luKB / %luKB", free_heap / 1024, total_heap / 1024);
        lv_label_set_text(ui_label_data_memory, buf);
    }
    if (ui_bar_memory && total_heap > 0) {
        uint32_t percentage = (free_heap * 100) / total_heap;
        lv_bar_set_value(ui_bar_memory, percentage, LV_ANIM_ON);
    }
}

void ui_tabs_update_psram(uint32_t free_psram, uint32_t total_psram) {
    if (ui_label_data_psram) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%luMB / %luMB", free_psram / (1024*1024), total_psram / (1024*1024));
        lv_label_set_text(ui_label_data_psram, buf);
    }
    if (ui_bar_psram && total_psram > 0) {
        uint32_t percentage = (free_psram * 100) / total_psram;
        lv_bar_set_value(ui_bar_psram, percentage, LV_ANIM_ON);
    }
}

void ui_tabs_update_uptime(uint32_t seconds) {
    if (ui_label_data_uptime) {
        uint32_t days = seconds / 86400;
        uint32_t hours = (seconds % 86400) / 3600;
        uint32_t minutes = (seconds % 3600) / 60;
        uint32_t secs = seconds % 60;
        
        char buf[32];
        snprintf(buf, sizeof(buf), "%lud %02lu:%02lu:%02lu", days, hours, minutes, secs);
        lv_label_set_text(ui_label_data_uptime, buf);
    }
}

void ui_tabs_update_task_count(uint8_t count) {
    if (ui_label_data_tasks) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d running", count);
        lv_label_set_text(ui_label_data_tasks, buf);
    }
}

// ============================================================================
// MAP Tab Update Functions (Future)
// ============================================================================

void ui_tabs_update_gps(float lat, float lon) {
    if (ui_label_map_coords) {
        char buf[64];
        snprintf(buf, sizeof(buf), "LAT: %.4f  LON: %.4f", lat, lon);
        lv_label_set_text(ui_label_map_coords, buf);
    }
    if (ui_label_map_status) {
        lv_label_set_text(ui_label_map_status, "GPS: Active");
        lv_obj_set_style_text_color(ui_label_map_status, PIPBOY_GREEN_BRIGHT, 0);
    }
}
