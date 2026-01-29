#ifndef UI_TABS_H
#define UI_TABS_H

#include <lvgl.h>

// ============================================================================
// Pip-Boy ADV Tab System
// ============================================================================
// Implements the multi-screen tab interface
// Tabs: STATUS | DATA | MAP | RADIO | SYSTEM
// ============================================================================

// Tab indices for the Pip-Boy interface
enum TabIndex {
    TAB_STATUS = 0,      // Status tab: clock, battery, weather
    TAB_DATA,            // Data tab: system stats, RAM, CPU
    TAB_MAP,             // Map tab: placeholder for future GPS
    TAB_RADIO,           // Radio tab: LoRa communication
    TAB_SYSTEM,          // System tab: settings, theme, WiFi
    TAB_COUNT            // Total number of tabs
};

// Legacy compatibility aliases
#define TAB_INDEX_STATS     TAB_STATUS
#define TAB_INDEX_RADIO     TAB_RADIO
#define TAB_INDEX_INVENTORY TAB_MAP
#define TAB_INDEX_SETTINGS  TAB_SYSTEM
#define TAB_INDEX_COUNT     TAB_COUNT
#define TAB_STATS           TAB_STATUS

// Tab objects (global access for updates)
extern lv_obj_t *ui_tabview_main;
extern lv_obj_t *ui_tab_status;
extern lv_obj_t *ui_tab_data;
extern lv_obj_t *ui_tab_map;
extern lv_obj_t *ui_tab_radio;
extern lv_obj_t *ui_tab_system;

// Animation container (in STATUS tab)
extern lv_obj_t *ui_animation_container;

// ============================================================================
// Tab System Functions
// ============================================================================

// Initialize tabview and create all tabs
void ui_tabs_init(lv_obj_t *parent);

// Tab content creation functions (modular design)
void ui_tabs_create_status(lv_obj_t *parent);   // STATUS tab: time, weather, battery
void ui_tabs_create_data(lv_obj_t *parent);     // DATA tab: system stats
void ui_tabs_create_map(lv_obj_t *parent);      // MAP tab: placeholder
void ui_tabs_create_radio(lv_obj_t *parent);    // RADIO tab: LoRa/IR
void ui_tabs_create_system(lv_obj_t *parent);   // SYSTEM tab: settings

// Tab switching and query
void ui_tabs_switch_to(TabIndex index);
TabIndex ui_tabs_get_active();

// Tab change callback (called when user switches tabs)
void ui_tabs_set_change_callback(void (*callback)(TabIndex old_tab, TabIndex new_tab));

// ============================================================================
// Tab Update Functions (called from main/helpers)
// ============================================================================

// STATUS tab updates
void ui_tabs_update_time(const char *time_str);
void ui_tabs_update_date(const char *date_str);
void ui_tabs_update_weather(const char *weather_str, const char *temp_str);
void ui_tabs_update_battery(int percentage);

// DATA tab updates
void ui_tabs_update_cpu_freq(uint8_t freq_mhz);
void ui_tabs_update_memory(uint32_t free_heap, uint32_t total_heap);
void ui_tabs_update_psram(uint32_t free_psram, uint32_t total_psram);
void ui_tabs_update_uptime(uint32_t seconds);
void ui_tabs_update_task_count(uint8_t count);

// MAP tab updates (future)
void ui_tabs_update_gps(float lat, float lon);

#endif // UI_TABS_H

