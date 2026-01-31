// ============================================================================
// Pip-Boy ADV Tab System - Complete Implementation
// ============================================================================
// Five-tab interface: STATUS | DATA | MAP | RADIO | SYSTEM
// Uses LVGL tabview with custom Pip-Boy theme and Vault Boy walking animation
// ============================================================================

#include "ui_tabs.h"
#include "ui_main.h"      // For legacy tab variable declarations
#include "ui_theme.h"
#include "ui_radio.h"
#include "ui_settings.h"
#include "ui_animations.h"
#include "ui_statusbar.h"
#include "VaultBoyAnimation.h"
#include <Arduino.h>

// ============================================================================
// Global Tab Objects
// ============================================================================

lv_obj_t *ui_tabview_main = NULL;
lv_obj_t *ui_tab_status = NULL;
lv_obj_t *ui_tab_data = NULL;
lv_obj_t *ui_tab_map = NULL;
lv_obj_t *ui_tab_system = NULL;

// Legacy tab aliases (ui_tab_radio, ui_animation_container, ui_tab_stats, 
// ui_tab_inventory, ui_tab_settings) are defined in ui_main.cpp
// and declared extern in ui_main.h - DO NOT redefine them here

// ============================================================================
// STATUS Tab UI Elements
// ============================================================================

static lv_obj_t *ui_label_status_time = NULL;
static lv_obj_t *ui_label_status_date = NULL;
static lv_obj_t *ui_label_status_weather = NULL;
static lv_obj_t *ui_label_status_temp = NULL;
static lv_obj_t *ui_bar_battery = NULL;
static lv_obj_t *ui_label_status_battery = NULL;  // Renamed to avoid conflict with ui_main.h

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
// SYSTEM Tab UI Elements & Sound Toggle
// ============================================================================

static lv_obj_t *ui_label_system_version = NULL;
static lv_obj_t *ui_label_system_build = NULL;
static lv_obj_t *ui_switch_sound = NULL;

// Sound toggle callback
static void sound_toggle_cb(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    bool enabled = lv_obj_has_state(sw, LV_STATE_CHECKED);
    ui_theme_set_sound_enabled(enabled);
    Serial.printf("Sound effects %s\n", enabled ? "enabled" : "disabled");
}

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
    
    // Get dynamic screen dimensions
    int32_t screen_width = 240;
    int32_t screen_height = 135;
    int32_t content_height = screen_height - STATUSBAR_HEIGHT;  // 117 px
    
    Serial.printf("UI Tabs: Screen %dx%d, Content area: %dx%d\n", 
                  screen_width, screen_height, screen_width, content_height);
    
    // Create main container - FULL SCREEN tabs with bottom button bar
    ui_tabview_main = lv_obj_create(parent);
    lv_obj_set_size(ui_tabview_main, screen_width, content_height);
    lv_obj_align(ui_tabview_main, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(ui_tabview_main, PIPBOY_CRT_BLACK, 0);
    lv_obj_set_style_pad_all(ui_tabview_main, 0, 0);
    lv_obj_clear_flag(ui_tabview_main, LV_OBJ_FLAG_SCROLLABLE);
    
    // ========================================================================
    // Create 5 full-screen tab containers (all same position, hidden by default)
    // ========================================================================
    
    ui_tab_status = lv_obj_create(ui_tabview_main);
    ui_tab_data = lv_obj_create(ui_tabview_main);
    ui_tab_map = lv_obj_create(ui_tabview_main);
    ui_tab_radio = lv_obj_create(ui_tabview_main);
    ui_tab_system = lv_obj_create(ui_tabview_main);
    
    // Configure all tab containers for full screen display
    lv_obj_t *tabs[] = {ui_tab_status, ui_tab_data, ui_tab_map, ui_tab_radio, ui_tab_system};
    for (int i = 0; i < 5; i++) {
        lv_obj_set_size(tabs[i], screen_width, content_height - 18);
        lv_obj_set_pos(tabs[i], 0, 0);
        lv_obj_set_style_bg_color(tabs[i], PIPBOY_CRT_BLACK, 0);
        lv_obj_set_style_border_width(tabs[i], 0, 0);
        lv_obj_set_style_pad_all(tabs[i], 4, 0);
        lv_obj_clear_flag(tabs[i], LV_OBJ_FLAG_SCROLLABLE);
        if (i > 0) lv_obj_add_flag(tabs[i], LV_OBJ_FLAG_HIDDEN);  // Hide all except STATUS
    }
    
    // ========================================================================
    // Bottom Tab Button Bar
    // ========================================================================
    
    lv_obj_t *tab_bar = lv_obj_create(ui_tabview_main);
    lv_obj_set_size(tab_bar, screen_width, 16);
    lv_obj_set_pos(tab_bar, 0, content_height - 16);
    lv_obj_set_style_bg_color(tab_bar, PIPBOY_CRT_BLACK, 0);
    lv_obj_set_style_border_color(tab_bar, PIPBOY_PHOSPHOR, 0);
    lv_obj_set_style_border_width(tab_bar, 1, 0);
    lv_obj_set_style_border_side(tab_bar, LV_BORDER_SIDE_TOP, 0);
    lv_obj_set_style_pad_all(tab_bar, 1, 0);
    lv_obj_set_style_layout(tab_bar, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_flex_flow(tab_bar, LV_FLEX_FLOW_ROW, 0);
    lv_obj_set_style_flex_main_place(tab_bar, LV_FLEX_ALIGN_SPACE_AROUND, 0);
    lv_obj_clear_flag(tab_bar, LV_OBJ_FLAG_SCROLLABLE);
    
    // ========================================================================
    // Create Tab Buttons with Click Handlers
    // ========================================================================
    
    // Lambda function to switch tabs
    auto switch_tab = [](lv_obj_t *new_tab) {
        lv_obj_add_flag(ui_tab_status, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_tab_data, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_tab_map, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_tab_radio, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_tab_system, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(new_tab, LV_OBJ_FLAG_HIDDEN);
    };
    
    // Helper to create tab button with click handler
    auto create_tab_btn = [&](const char *label, lv_obj_t *target_tab) {
        lv_obj_t *btn = lv_btn_create(tab_bar);
        lv_obj_set_size(btn, 45, 14);
        lv_obj_set_style_bg_color(btn, PIPBOY_PHOSPHOR_DARK, 0);
        lv_obj_set_style_border_width(btn, 1, 0);
        lv_obj_set_style_border_color(btn, PIPBOY_PHOSPHOR, 0);
        lv_obj_set_style_radius(btn, 0, 0);
        
        lv_obj_t *lbl = lv_label_create(btn);
        lv_label_set_text(lbl, label);
        lv_obj_set_style_text_font(lbl, &lv_font_unscii_8, 0);
        lv_obj_set_style_text_color(lbl, PIPBOY_PHOSPHOR, 0);
        lv_obj_center(lbl);
        
        // Add click event handler
        lv_obj_add_event_cb(btn, [](lv_event_t *e) {
            if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
                lv_obj_t *target = (lv_obj_t *)lv_event_get_user_data(e);
                
                // Hide all tabs
                lv_obj_add_flag(ui_tab_status, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(ui_tab_data, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(ui_tab_map, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(ui_tab_radio, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(ui_tab_system, LV_OBJ_FLAG_HIDDEN);
                
                // Show clicked tab
                lv_obj_clear_flag(target, LV_OBJ_FLAG_HIDDEN);
            }
        }, LV_EVENT_CLICKED, target_tab);
    };
    
    // Create all tab buttons
    create_tab_btn("STAT", ui_tab_status);
    create_tab_btn("DATA", ui_tab_data);
    create_tab_btn("MAP", ui_tab_map);
    create_tab_btn("RADIO", ui_tab_radio);
    create_tab_btn("SYS", ui_tab_system);
    
    // Set legacy aliases
    ui_tab_stats = ui_tab_status;
    ui_tab_inventory = ui_tab_map;
    ui_tab_settings = ui_tab_system;
    
    Serial.println("  ✓ Full-screen tab system with button bar created");
    
    // Create tab contents
    ui_tabs_create_status(ui_tab_status);
    ui_tabs_create_data(ui_tab_data);
    ui_tabs_create_map(ui_tab_map);
    ui_tabs_create_radio(ui_tab_radio);
    ui_tabs_create_system(ui_tab_system);
    
    Serial.println("  ✓ Tab contents populated");
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
    
    // Title with authentic monospace terminal font
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "- STATUS -");
    ui_theme_apply_label_title(title);
    lv_obj_set_style_text_font(title, &lv_font_unscii_8, 0);
    lv_obj_set_style_text_color(title, PIPBOY_PHOSPHOR_BRIGHT, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 1);
    
    // ========================================================================
    // Left Column: Time, Date, Weather
    // ========================================================================
    
    lv_obj_t *left_container = lv_obj_create(parent);
    lv_obj_set_size(left_container, 140, 85);
    lv_obj_align(left_container, LV_ALIGN_TOP_LEFT, 2, 18);
    ui_theme_apply_container(left_container);
    lv_obj_clear_flag(left_container, LV_OBJ_FLAG_SCROLLABLE);
    
    // Time display (show uptime)
    ui_label_status_time = lv_label_create(left_container);
    lv_label_set_text(ui_label_status_time, "00:00:00");
    lv_obj_set_style_text_color(ui_label_status_time, PIPBOY_GREEN_BRIGHT, 0);
    lv_obj_set_style_text_font(ui_label_status_time, &lv_font_montserrat_16, 0);
    lv_obj_align(ui_label_status_time, LV_ALIGN_TOP_MID, 0, 3);
    
    // Date display (system status)
    ui_label_status_date = lv_label_create(left_container);
    lv_label_set_text(ui_label_status_date, "SYSTEM READY");
    ui_theme_apply_label_normal(ui_label_status_date);
    lv_obj_align(ui_label_status_date, LV_ALIGN_TOP_MID, 0, 26);
    
    // CPU Info
    ui_label_status_weather = lv_label_create(left_container);
    lv_label_set_text(ui_label_status_weather, "CPU: 240 MHz");
    ui_theme_apply_label_small(ui_label_status_weather);
    lv_obj_align(ui_label_status_weather, LV_ALIGN_TOP_LEFT, 3, 48);
    
    // Memory Info
    ui_label_status_temp = lv_label_create(left_container);
    lv_label_set_text(ui_label_status_temp, "RAM: --KB");
    ui_theme_apply_label_small(ui_label_status_temp);
    lv_obj_align(ui_label_status_temp, LV_ALIGN_TOP_LEFT, 3, 62);
    
    // ========================================================================
    // Right Column: Vault Boy Walking Animation (from Pip-Boy Weather Clock)
    // ========================================================================
    
    // Initialize the Vault Boy animation with PNG frames from SD card
    lv_obj_t *vault_boy_container = vault_boy_animation_init(parent);
    lv_obj_align(vault_boy_container, LV_ALIGN_TOP_RIGHT, -4, 20);
    
    // ========================================================================
    // Bottom: Battery Status with Flex Layout (prevents overlap)
    // ========================================================================
    
    // Bottom status container (full width at bottom)
    lv_obj_t *bottom_container = lv_obj_create(parent);
    lv_obj_set_size(bottom_container, lv_obj_get_width(parent) - 4, 14);
    lv_obj_align(bottom_container, LV_ALIGN_BOTTOM_MID, 0, -2);
    ui_theme_apply_container(bottom_container);
    lv_obj_clear_flag(bottom_container, LV_OBJ_FLAG_SCROLLABLE);
    
    // Enable flex layout (horizontal row)
    lv_obj_set_style_layout(bottom_container, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_flex_flow(bottom_container, LV_FLEX_FLOW_ROW, 0);
    lv_obj_set_style_flex_main_place(bottom_container, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);
    lv_obj_set_style_flex_cross_place(bottom_container, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_all(bottom_container, 2, 0);
    
    // POWER label
    ui_label_status_battery = lv_label_create(bottom_container);
    lv_label_set_text(ui_label_status_battery, "PWR:");
    ui_theme_apply_label_small(ui_label_status_battery);
    
    // Battery bar (wider to fill space)
    ui_bar_battery = lv_bar_create(bottom_container);
    lv_obj_set_size(ui_bar_battery, 130, 10);
    lv_bar_set_range(ui_bar_battery, 0, 100);
    lv_bar_set_value(ui_bar_battery, 75, LV_ANIM_OFF);
    ui_theme_apply_bar(ui_bar_battery);
    lv_obj_set_style_bg_color(ui_bar_battery, PIPBOY_GREEN, LV_PART_INDICATOR);
    
    // Battery percentage label
    lv_obj_t *bat_pct = lv_label_create(bottom_container);
    lv_label_set_text(bat_pct, "75%");
    ui_theme_apply_label_small(bat_pct);
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
// SYSTEM Tab Creation - Firmware and Device Information
// ============================================================================

void ui_tabs_create_system(lv_obj_t *parent) {
    if (!parent) return;
    
    // Title
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "SYSTEM INFO");
    ui_theme_apply_label_title(title);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 1);
    
    // Sound toggle
    lv_obj_t *sound_label = lv_label_create(parent);
    lv_label_set_text(sound_label, "SOUND:");
    ui_theme_apply_label_normal(sound_label);
    lv_obj_align(sound_label, LV_ALIGN_TOP_RIGHT, -50, 1);
    
    ui_switch_sound = lv_switch_create(parent);
    lv_obj_set_size(ui_switch_sound, 35, 18);
    lv_obj_align(ui_switch_sound, LV_ALIGN_TOP_RIGHT, -5, 1);
    lv_obj_add_state(ui_switch_sound, LV_STATE_CHECKED);
    lv_obj_add_event_cb(ui_switch_sound, sound_toggle_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_style_bg_color(ui_switch_sound, PIPBOY_GREEN, LV_PART_INDICATOR | LV_STATE_CHECKED);
    
    // Firmware Version
    lv_obj_t *fw_label = lv_label_create(parent);
    lv_label_set_text(fw_label, "FIRMWARE:");
    ui_theme_apply_label_normal(fw_label);
    lv_obj_align(fw_label, LV_ALIGN_TOP_LEFT, 3, 25);
    
    ui_label_system_version = lv_label_create(parent);
    lv_label_set_text(ui_label_system_version, "v" FIRMWARE_VERSION);
    ui_theme_apply_label_small(ui_label_system_version);
    lv_obj_set_style_text_color(ui_label_system_version, PIPBOY_GREEN_BRIGHT, 0);
    lv_obj_align(ui_label_system_version, LV_ALIGN_TOP_LEFT, 75, 25);
    
    // Build Date
    lv_obj_t *build_label = lv_label_create(parent);
    lv_label_set_text(build_label, "BUILD:");
    ui_theme_apply_label_normal(build_label);
    lv_obj_align(build_label, LV_ALIGN_TOP_LEFT, 3, 40);
    
    ui_label_system_build = lv_label_create(parent);
    lv_label_set_text(ui_label_system_build, BUILD_DATE);
    ui_theme_apply_label_small(ui_label_system_build);
    lv_obj_align(ui_label_system_build, LV_ALIGN_TOP_LEFT, 75, 40);
    
    // Device Model
    lv_obj_t *model_label = lv_label_create(parent);
    lv_label_set_text(model_label, "DEVICE:");
    ui_theme_apply_label_normal(model_label);
    lv_obj_align(model_label, LV_ALIGN_TOP_LEFT, 3, 55);
    
    lv_obj_t *model_value = lv_label_create(parent);
    lv_label_set_text(model_value, "Cardputer ADV");
    ui_theme_apply_label_small(model_value);
    lv_obj_align(model_value, LV_ALIGN_TOP_LEFT, 75, 55);
    
    // Chip Model
    lv_obj_t *chip_label = lv_label_create(parent);
    lv_label_set_text(chip_label, "CHIP:");
    ui_theme_apply_label_normal(chip_label);
    lv_obj_align(chip_label, LV_ALIGN_TOP_LEFT, 3, 70);
    
    lv_obj_t *chip_value = lv_label_create(parent);
    lv_label_set_text(chip_value, "ESP32-S3 240MHz");
    ui_theme_apply_label_small(chip_value);
    lv_obj_align(chip_value, LV_ALIGN_TOP_LEFT, 75, 70);
    
    // Flash Size
    lv_obj_t *flash_label = lv_label_create(parent);
    lv_label_set_text(flash_label, "FLASH:");
    ui_theme_apply_label_normal(flash_label);
    lv_obj_align(flash_label, LV_ALIGN_TOP_LEFT, 3, 85);
    
    lv_obj_t *flash_value = lv_label_create(parent);
    lv_label_set_text(flash_value, "16MB");
    ui_theme_apply_label_small(flash_value);
    lv_obj_align(flash_value, LV_ALIGN_TOP_LEFT, 75, 85);
    
    // Features list
    lv_obj_t *features = lv_label_create(parent);
    lv_label_set_text(features, "FEATURES:\nLoRa SX1262 | IMU QMI8658\nSD Card | NeoPixel LED\nSpeaker | Keyboard");
    ui_theme_apply_label_small(features);
    lv_obj_set_style_text_color(features, PIPBOY_GREEN_DARK, 0);
    lv_obj_align(features, LV_ALIGN_BOTTOM_LEFT, 3, -2);
}

// ============================================================================
// Tab Switching Functions - Enhanced with sound effects
// ============================================================================

void ui_tabs_switch_to(TabIndex index) {
    if (ui_tabview_main && index >= 0 && index < TAB_COUNT) {
        TabIndex old_index = ui_tabs_get_active();
        lv_tabview_set_act(ui_tabview_main, index, LV_ANIM_ON);
        
        // Play tab change sound effect
        if (old_index != index) {
            ui_theme_play_tab_change();
        }
        
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
    // STATUS tab shows uptime, update it
    if (ui_label_status_time && time_str) {
        lv_label_set_text(ui_label_status_time, time_str);
    }
}

void ui_tabs_update_date(const char *date_str) {
    if (ui_label_status_date && date_str) {
        lv_label_set_text(ui_label_status_date, date_str);
    }
}

void ui_tabs_update_weather(const char *weather_str, const char *temp_str) {
    // Repurpose for CPU frequency
    if (ui_label_status_weather && weather_str) {
        lv_label_set_text(ui_label_status_weather, weather_str);
    }
    // Repurpose for memory info
    if (ui_label_status_temp && temp_str) {
        lv_label_set_text(ui_label_status_temp, temp_str);
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
