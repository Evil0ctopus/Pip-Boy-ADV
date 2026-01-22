#ifndef UI_STATUSBAR_H
#define UI_STATUSBAR_H

#include <lvgl.h>

// Status bar height
#define STATUSBAR_HEIGHT 18

// Status bar elements
extern lv_obj_t *ui_statusbar;
extern lv_obj_t *ui_statusbar_time;
extern lv_obj_t *ui_statusbar_battery;
extern lv_obj_t *ui_statusbar_wifi;
extern lv_obj_t *ui_statusbar_lora;
extern lv_obj_t *ui_statusbar_battery_icon;

// Status icons (using text symbols)
#define ICON_BATTERY_FULL    "🔋"
#define ICON_BATTERY_HIGH    "🔋"
#define ICON_BATTERY_MED     "🔋"
#define ICON_BATTERY_LOW     "🔋"
#define ICON_BATTERY_EMPTY   "🔋"
#define ICON_BATTERY_CHARGE  "⚡"
#define ICON_WIFI_ON         "📶"
#define ICON_WIFI_OFF        "📡"
#define ICON_LORA_ON         "📡"
#define ICON_LORA_OFF        "📡"

// Initialize status bar
void ui_statusbar_init(lv_obj_t *parent);

// Update functions
void ui_statusbar_update_time(const char *time_str);
void ui_statusbar_update_battery(int level, bool charging);
void ui_statusbar_update_wifi(bool connected, int rssi);
void ui_statusbar_update_lora(bool active, float rssi);

// Animation
void ui_statusbar_blink_lora(bool enable);

#endif // UI_STATUSBAR_H
