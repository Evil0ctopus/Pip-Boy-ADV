#include "ui_assets.h"
#include <Arduino.h>

// Image descriptor caches
lv_img_dsc_t *walking_frames[WALKING_FRAME_COUNT] = {NULL};
lv_img_dsc_t *thumbsup_frames[THUMBSUP_FRAME_COUNT] = {NULL};
lv_img_dsc_t *battery_icon = NULL;

// PNG decoder helper (simplified - for production, use a proper PNG library)
// This is a placeholder - LVGL can work with raw data or you'd need to decode PNG
static lv_img_dsc_t* create_img_dsc_from_file(const char *path) {
    File file = SD.open(path);
    if (!file) {
        Serial.printf("Failed to open image: %s\n", path);
        return NULL;
    }
    
    size_t file_size = file.size();
    
    // Allocate memory for image descriptor
    lv_img_dsc_t *img_dsc = (lv_img_dsc_t*)malloc(sizeof(lv_img_dsc_t));
    if (!img_dsc) {
        file.close();
        return NULL;
    }
    
    // For PNG files, we'd normally decode them
    // For simplicity, this assumes the images are already in a compatible format
    // or you're using LVGL's built-in image decoder
    
    // Read file data
    uint8_t *img_data = (uint8_t*)malloc(file_size);
    if (!img_data) {
        free(img_dsc);
        file.close();
        return NULL;
    }
    
    file.read(img_data, file_size);
    file.close();
    
    // Set up image descriptor
    // Note: This is simplified - proper implementation would decode PNG
    img_dsc->header.always_zero = 0;
    img_dsc->header.w = 64;  // Default size - should be read from PNG header
    img_dsc->header.h = 64;
    img_dsc->data_size = file_size;
    img_dsc->header.cf = LV_IMG_CF_RAW;  // Would be different for decoded PNG
    img_dsc->data = img_data;
    
    return img_dsc;
}

// Initialize asset loading system
bool ui_assets_init() {
    // Attempt SD card initialization, but don't fail if unavailable
    if (!SD.begin()) {
        Serial.println("⚠ SD Card not available - assets will be unavailable");
        Serial.println("  UI will run without boot animation and custom images");
        return false;  // Return false but don't block boot
    }
    
    Serial.println("✓ SD Card initialized - assets available");
    
    // Pre-load some key assets (optional - can also load on-demand)
    // For now, we'll load on-demand to save memory
    
    return true;
}

// Load specific image from SD card
lv_img_dsc_t* ui_assets_load_image(const char *path) {
    // Check if SD is available first
    if (!SD.begin()) {
        Serial.println("⚠ Cannot load asset - SD not initialized");
        return NULL;
    }
    
    // Check if file exists
    if (!SD.exists(path)) {
        Serial.printf("⚠ Image not found: %s (will skip)\n", path);
        return NULL;  // Return NULL but don't crash
    }
    
    Serial.printf("  → Loading: %s\n", path);
    
    // For LVGL to load PNG directly, we can use the file path
    // LVGL 8.x has built-in PNG decoder support if enabled
    // Alternative: return the path as a string for LVGL to handle
    
    // Using LVGL's file system interface (requires proper setup)
    // For now, return a descriptor that points to the file
    static char file_path_buf[64];
    snprintf(file_path_buf, sizeof(file_path_buf), "S:%s", path);
    
    // Note: This requires LVGL to be configured with file system support
    // and the 'S:' drive letter mapped to SD card
    // For a simpler approach without FS support, use create_img_dsc_from_file
    
    return create_img_dsc_from_file(path);
}

// Load walking animation frames
bool ui_assets_load_walking_frames() {
    char path[64];
    bool all_loaded = true;
    
    for (int i = 0; i < WALKING_FRAME_COUNT; i++) {
        snprintf(path, sizeof(path), WALKING_ASSET_PATH, i);
        walking_frames[i] = ui_assets_load_image(path);
        if (walking_frames[i] == NULL) {
            all_loaded = false;
            Serial.printf("Failed to load walking frame %d\n", i);
        }
    }
    
    return all_loaded;
}

// Load thumbsup animation frames
bool ui_assets_load_thumbsup_frames() {
    char path[64];
    bool all_loaded = true;
    
    for (int i = 0; i < THUMBSUP_FRAME_COUNT; i++) {
        snprintf(path, sizeof(path), THUMBSUP_ASSET_PATH, i);
        thumbsup_frames[i] = ui_assets_load_image(path);
        if (thumbsup_frames[i] == NULL) {
            all_loaded = false;
            Serial.printf("Failed to load thumbsup frame %d\n", i);
        }
    }
    
    return all_loaded;
}

// Load battery icon
bool ui_assets_load_battery_icon() {
    battery_icon = ui_assets_load_image(BATTERY_ICON_PATH);
    return (battery_icon != NULL);
}

// Free a loaded image
void ui_assets_free_image(lv_img_dsc_t *img) {
    if (img != NULL) {
        if (img->data != NULL) {
            free((void*)img->data);
        }
        free(img);
    }
}

// Free all loaded assets
void ui_assets_free_all() {
    // Free walking frames
    for (int i = 0; i < WALKING_FRAME_COUNT; i++) {
        if (walking_frames[i] != NULL) {
            ui_assets_free_image(walking_frames[i]);
            walking_frames[i] = NULL;
        }
    }
    
    // Free thumbsup frames
    for (int i = 0; i < THUMBSUP_FRAME_COUNT; i++) {
        if (thumbsup_frames[i] != NULL) {
            ui_assets_free_image(thumbsup_frames[i]);
            thumbsup_frames[i] = NULL;
        }
    }
    
    // Free battery icon
    if (battery_icon != NULL) {
        ui_assets_free_image(battery_icon);
        battery_icon = NULL;
    }
}

// Helper to convert file to LVGL image descriptor
lv_img_dsc_t* ui_assets_file_to_img_dsc(File &file) {
    if (!file) return NULL;
    
    size_t file_size = file.size();
    
    // Allocate memory for image descriptor
    lv_img_dsc_t *img_dsc = (lv_img_dsc_t*)malloc(sizeof(lv_img_dsc_t));
    if (!img_dsc) return NULL;
    
    // Allocate memory for image data
    uint8_t *img_data = (uint8_t*)malloc(file_size);
    if (!img_data) {
        free(img_dsc);
        return NULL;
    }
    
    // Read file data
    file.read(img_data, file_size);
    
    // Set up image descriptor (simplified)
    img_dsc->header.always_zero = 0;
    img_dsc->header.w = 64;
    img_dsc->header.h = 64;
    img_dsc->data_size = file_size;
    img_dsc->header.cf = LV_IMG_CF_RAW;
    img_dsc->data = img_data;
    
    return img_dsc;
}
