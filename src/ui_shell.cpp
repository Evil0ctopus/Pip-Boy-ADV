// ============================================================================
// Pip-Boy ADV UI Shell - Main UI Container
// ============================================================================
// Coordinates the entire UI system including theme, status bar, and tabs
// Provides update functions called from main firmware and helper modules
// ============================================================================

#include "ui_shell.h"
#include "ui_theme.h"
#include "ui_statusbar.h"
#include "ui_tabs.h"
#include "ui_animations.h"
#include "HardwareConfig.h"
#include <Arduino.h>

// ============================================================================
// Global UI Objects
// ============================================================================

static lv_obj_t *ui_screen_main = NULL;

// Tab-specific data objects (for DATA tab updates)
static lv_obj_t *ui_label_data_cpu = NULL;
static lv_obj_t *ui_label_data_memory = NULL;
static lv_obj_t *ui_label_data_psram = NULL;
static lv_obj_t *ui_label_data_uptime = NULL;

// ============================================================================
// UI Shell Initialization
// ============================================================================

/**
 * Initialize the complete UI shell
 * Call this once during firmware boot
 */
void ui_shell_init() {
    Serial.println("UI Shell: Initializing...");
    
    // Step 1: Initialize Pip-Boy theme
    ui_theme_init();
    Serial.println("  ✓ Theme initialized");
    
    // Step 2: Create main screen container
    ui_screen_main = lv_obj_create(NULL);
    ui_theme_apply_screen(ui_screen_main);
    Serial.println("  ✓ Main screen created");
    
    // Step 3: Initialize status bar (top of screen)
    ui_statusbar_init(ui_screen_main);
    Serial.println("  ✓ Status bar initialized");
    
    // Step 4: Initialize tab system (below status bar)
    ui_tabs_init(ui_screen_main);
    Serial.println("  ✓ Tab system initialized");
    
    // Step 5: Apply CRT effects (scanlines and vignette)
    ui_theme_apply_crt_effects(ui_screen_main);
    Serial.println("  ✓ CRT effects applied");
    
    // Step 6: Load and display the screen
    lv_scr_load(ui_screen_main);
    Serial.println("  ✓ UI loaded to display");
    
    // Step 7: Boot animation temporarily disabled for initial testing
    // TODO: Re-enable after confirming UI displays correctly
    // ui_theme_create_boot_animation(ui_screen_main);
    Serial.println("  ○ Boot animation disabled (testing mode)");
    
    Serial.println("UI Shell: Ready");
}

/**
 * Start UI (alias for init - for compatibility)
 */
void ui_shell_start() {
    ui_shell_init();
}

/**
 * Clean up UI resources
 */
void ui_shell_deinit() {
    if (ui_screen_main) {
        lv_obj_del(ui_screen_main);
        ui_screen_main = NULL;
    }
}

// ============================================================================
// Time and Date Updates
// ============================================================================

void ui_shell_update_time(const char *time_str) {
    ui_statusbar_update_time(time_str);
    ui_tabs_update_time(time_str);
}

void ui_shell_update_date(const char *date_str) {
    ui_tabs_update_date(date_str);
}

// ============================================================================
// Weather Updates
// ============================================================================

void ui_shell_update_weather(const char *weather, const char *temp) {
    ui_tabs_update_weather(weather, temp);
}

// ============================================================================
// System Status Updates
// ============================================================================

void ui_shell_update_battery(int percentage, bool charging) {
    ui_statusbar_update_battery(percentage, charging);
    ui_tabs_update_battery(percentage);
}

void ui_shell_update_wifi(bool connected, int rssi) {
    ui_statusbar_update_wifi(connected, rssi);
}

void ui_shell_update_lora(bool active, float rssi) {
    ui_statusbar_update_lora(active, rssi);
}

// ============================================================================
// System Stats Updates (DATA Tab)
// ============================================================================

void ui_shell_update_cpu(uint8_t freq_mhz) {
    ui_tabs_update_cpu_freq(freq_mhz);
}

void ui_shell_update_memory(uint32_t free_heap, uint32_t total_heap) {
    ui_tabs_update_memory(free_heap, total_heap);
}

void ui_shell_update_psram(uint32_t free_psram, uint32_t total_psram) {
    ui_tabs_update_psram(free_psram, total_psram);
}

void ui_shell_update_uptime(uint32_t seconds) {
    ui_tabs_update_uptime(seconds);
}

// ============================================================================
// Animation Control
// ============================================================================

void ui_shell_play_animation(int animation_type) {
    ui_animation_play(static_cast<AnimationType>(animation_type));
}

void ui_shell_stop_animation() {
    ui_animation_stop();
}

// ============================================================================
// Tab Management
// ============================================================================

void ui_shell_switch_tab(int tab_index) {
    if (tab_index >= 0 && tab_index < TAB_COUNT) {
        ui_tabs_switch_to(static_cast<TabIndex>(tab_index));
    }
}

int ui_shell_get_active_tab() {
    return static_cast<int>(ui_tabs_get_active());
}

// ============================================================================
// Screen Access
// ============================================================================

lv_obj_t* ui_shell_get_screen() {
    return ui_screen_main;
}

lv_obj_t* ui_shell_get_animation_container() {
    return ui_animation_container;
}

