#ifndef UI_ASSETS_H
#define UI_ASSETS_H

#include <lvgl.h>
#include <FS.h>
#include <SD.h>

// Asset paths on SD card
#define WALKING_ASSET_PATH "/assets/Images/walking/walking_%02d.png"
#define THUMBSUP_ASSET_PATH "/assets/Images/thumpsup/thumpsup_%02d.png"
#define BATTERY_ICON_PATH "/assets/Images/battery.png"

// Walking animation frame count
#define WALKING_FRAME_COUNT 21

// Thumbsup animation frame count
#define THUMBSUP_FRAME_COUNT 11

// Image descriptor cache
extern lv_img_dsc_t *walking_frames[WALKING_FRAME_COUNT];
extern lv_img_dsc_t *thumbsup_frames[THUMBSUP_FRAME_COUNT];
extern lv_img_dsc_t *battery_icon;

// Initialize asset loading system
bool ui_assets_init();

// Load specific asset from SD card
lv_img_dsc_t* ui_assets_load_image(const char *path);

// Load animation frames
bool ui_assets_load_walking_frames();
bool ui_assets_load_thumbsup_frames();
bool ui_assets_load_battery_icon();

// Free loaded assets
void ui_assets_free_image(lv_img_dsc_t *img);
void ui_assets_free_all();

// Helper function to convert file to LVGL image descriptor
lv_img_dsc_t* ui_assets_file_to_img_dsc(File &file);

#endif // UI_ASSETS_H
