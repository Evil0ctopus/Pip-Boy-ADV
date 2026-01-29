#ifndef UI_SHELL_H
#define UI_SHELL_H

#include <lvgl.h>

// ============================================================================
// Pip-Boy ADV UI Shell
// ============================================================================
// Main UI container and coordinator for the Pip-Boy interface
// Manages the overall screen structure, status bar, and tab system
// ============================================================================

// Main UI initialization and lifecycle
void ui_shell_init();                           // Initialize UI system
void ui_shell_start();                          // Start/show UI (alias for init)
void ui_shell_deinit();                         // Clean up UI resources

// Time and date updates
void ui_shell_update_time(const char *time_str);
void ui_shell_update_date(const char *date_str);

// Weather updates
void ui_shell_update_weather(const char *weather, const char *temp);

// System status updates
void ui_shell_update_battery(int percentage, bool charging);
void ui_shell_update_wifi(bool connected, int rssi);
void ui_shell_update_lora(bool active, float rssi);

// System stats updates (for DATA tab)
void ui_shell_update_cpu(uint8_t freq_mhz);
void ui_shell_update_memory(uint32_t free_heap, uint32_t total_heap);
void ui_shell_update_psram(uint32_t free_psram, uint32_t total_psram);
void ui_shell_update_uptime(uint32_t seconds);

// Animation control
void ui_shell_play_animation(int animation_type);
void ui_shell_stop_animation();

// Tab management
void ui_shell_switch_tab(int tab_index);
int ui_shell_get_active_tab();

// Screen management
lv_obj_t* ui_shell_get_screen();
lv_obj_t* ui_shell_get_animation_container();

#endif // UI_SHELL_H

