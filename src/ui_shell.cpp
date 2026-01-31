// ============================================================================
// Pip-Boy ADV UI Shell - Main UI Container
// ============================================================================
// Coordinates the entire UI system including theme, status bar, and tabs
// Provides update functions called from main firmware and helper modules
// ============================================================================

#include "ui_shell.h"
#include <Arduino.h>
#include "ui.h"

// ============================================================================
// Global UI Objects
// ============================================================================

static lv_obj_t *ui_screen_main = NULL;

// ============================================================================
// UI Shell Initialization
// ============================================================================

/**
 * Initialize the complete UI shell
 * Call this once during firmware boot
 */
void ui_shell_init() {
    Serial.println("UI Shell: Initializing (Pip-Boy Weather UI)...");

    // Initialize the SquareLine UI from Pip-Boy Weather Clock
    ui_init();
    ui_screen_main = ui_Screen1;

    // Start built-in animations from the reference UI
    if (ui_Img_stat) {
        walking_Animation(ui_Img_stat, 0);
    }
    if (ui_Img_data) {
        thumpsup_Animation(ui_Img_data, 0);
    }

    Serial.println("UI Shell: Ready (Pip-Boy Weather UI loaded)");
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
    if (ui_Label_Time) {
        lv_label_set_text(ui_Label_Time, time_str ? time_str : "--:--");
    }
}

void ui_shell_update_date(const char *date_str) {
    if (ui_Label_date) {
        lv_label_set_text(ui_Label_date, date_str ? date_str : "DATE");
    }
}

// ============================================================================
// Weather Updates
// ============================================================================

void ui_shell_update_weather(const char *weather, const char *temp) {
    if (ui_Label_temp) {
        lv_label_set_text(ui_Label_temp, weather ? weather : "T:--");
    }
    if (ui_Label_hum) {
        lv_label_set_text(ui_Label_hum, temp ? temp : "H:--");
    }
}

// ============================================================================
// System Status Updates
// ============================================================================

void ui_shell_update_battery(int percentage, bool charging) {
    (void)charging;
    if (ui_Bar_battery) {
        lv_bar_set_value(ui_Bar_battery, percentage, LV_ANIM_OFF);
    }
}

void ui_shell_update_wifi(bool connected, int rssi) {
    (void)connected;
    (void)rssi;
}

void ui_shell_update_lora(bool active, float rssi) {
    (void)active;
    (void)rssi;
}

// ============================================================================
// System Stats Updates (DATA Tab)
// ============================================================================

void ui_shell_update_cpu(uint8_t freq_mhz) {
    if (ui_Labe2) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%u", freq_mhz);
        lv_label_set_text(ui_Labe2, buf);
    }
}

void ui_shell_update_memory(uint32_t free_heap, uint32_t total_heap) {
    if (total_heap == 0) return;
    uint32_t used = total_heap - free_heap;
    uint32_t percent = (used * 100U) / total_heap;
    if (ui_Bar_FL) {
        lv_bar_set_value(ui_Bar_FL, percent, LV_ANIM_OFF);
    }
    if (ui_Labe5) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%lu", (unsigned long)percent);
        lv_label_set_text(ui_Labe5, buf);
    }
}

void ui_shell_update_psram(uint32_t free_psram, uint32_t total_psram) {
    if (total_psram == 0) return;
    uint32_t used = total_psram - free_psram;
    uint32_t percent = (used * 100U) / total_psram;
    if (ui_Bar_HI) {
        lv_bar_set_value(ui_Bar_HI, percent, LV_ANIM_OFF);
    }
}

void ui_shell_update_uptime(uint32_t seconds) {
    (void)seconds;
}

// ============================================================================
// Animation Control
// ============================================================================

void ui_shell_play_animation(int animation_type) {
    (void)animation_type;
    if (ui_Img_stat) {
        walking_Animation(ui_Img_stat, 0);
    }
}

void ui_shell_stop_animation() {
    // No-op: SquareLine animations loop internally
}

// ============================================================================
// Tab Management
// ============================================================================

void ui_shell_switch_tab(int tab_index) {
    if (ui_Tab_main && tab_index >= 0 && tab_index <= 1) {
        lv_tabview_set_act(ui_Tab_main, tab_index, LV_ANIM_ON);
    }
}

int ui_shell_get_active_tab() {
    if (!ui_Tab_main) return 0;
    return lv_tabview_get_tab_act(ui_Tab_main);
}

// ============================================================================
// Screen Access
// ============================================================================

lv_obj_t* ui_shell_get_screen() {
    return ui_screen_main;
}

lv_obj_t* ui_shell_get_animation_container() {
    return ui_Img_stat;
}

