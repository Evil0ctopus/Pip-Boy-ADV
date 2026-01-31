// ============================================================================
// Vault Boy Walking Animation Implementation
// Loads PNG frames from SD card and plays them in sequence
// ============================================================================

#include "VaultBoyAnimation.h"
#include <Arduino.h>

// Global animation state
static VaultBoyAnimationState g_anim_state = {
    .img_obj = NULL,
    .timer = NULL,
    .current_frame = 0,
    .total_frames = WALKING_FRAME_COUNT,
    .is_playing = false,
    .frame_time_ms = WALKING_FRAME_TIME
};

// Cached frame image descriptors
static lv_img_dsc_t *g_walking_frames[WALKING_FRAME_COUNT] = {NULL};
static bool g_frames_preloaded = false;

// ============================================================================
// Frame Loading
// ============================================================================

/**
 * Load a PNG frame from SD card
 * Uses LVGL's image loading with file path
 */
static bool _load_frame(int frame_idx) {
    if (frame_idx < 0 || frame_idx >= WALKING_FRAME_COUNT) {
        return false;
    }
    
    // Return cached frame if available
    if (g_walking_frames[frame_idx] != NULL) {
        return true;
    }
    
    // Build path: /assets/Images/walking/walking_00.png
    char path[64];
    snprintf(path, sizeof(path), WALKING_ASSET_PATH, frame_idx);
    
    // Check if file exists
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        Serial.printf("⚠ Vault Boy frame not found: %s\n", path);
        return false;
    }
    fclose(f);
    
    // Note: LVGL will load the PNG directly from file path using lv_img_set_src()
    // This is more efficient than pre-converting to C arrays
    
    Serial.printf("✓ Loaded Vault Boy frame %d: %s\n", frame_idx, path);
    return true;
}

/**
 * Animate the walking frames
 * Called by timer every 100ms
 */
static void _animation_timer_callback(lv_timer_t *timer) {
    if (!g_anim_state.is_playing || g_anim_state.img_obj == NULL) {
        return;
    }
    
    // Advance to next frame
    g_anim_state.current_frame++;
    if (g_anim_state.current_frame >= g_anim_state.total_frames) {
        g_anim_state.current_frame = 0;  // Loop back to start
    }
    
    // Build path and set image
    char path[64];
    snprintf(path, sizeof(path), WALKING_ASSET_PATH, g_anim_state.current_frame);
    
    // Set the image source - LVGL will load and cache it
    lv_img_set_src(g_anim_state.img_obj, path);
    
    // Debug output every 10 frames to avoid spam
    if (g_anim_state.current_frame % 10 == 0) {
        Serial.printf("Vault Boy walking frame: %d/%d\n", 
                     g_anim_state.current_frame, g_anim_state.total_frames);
    }
}

// ============================================================================
// Public API Implementation
// ============================================================================

lv_obj_t *vault_boy_animation_init(lv_obj_t *parent) {
    if (parent == NULL) {
        Serial.println("✗ Vault Boy init failed: NULL parent");
        return NULL;
    }
    
    Serial.println("Initializing Vault Boy walking animation...");
    
    // Create container for animation
    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_set_size(container, 100, 70);
    lv_obj_set_style_bg_color(container, lv_color_make(0x00, 0x11, 0x00), 0);  // CRT black
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_pad_all(container, 0, 0);
    lv_obj_clear_flag(container, LV_OBJ_FLAG_SCROLLABLE);
    
    // Create image object for displaying frames
    lv_obj_t *img_obj = lv_img_create(container);
    lv_obj_set_size(img_obj, 64, 64);
    lv_obj_align(img_obj, LV_ALIGN_CENTER, 0, 0);
    
    // Store in global state
    g_anim_state.img_obj = img_obj;
    g_anim_state.current_frame = 0;
    g_anim_state.is_playing = false;
    
    // Try to load first frame to verify assets exist
    if (vault_boy_assets_available()) {
        Serial.println("✓ Vault Boy assets found on SD card");
        
        // Set initial frame
        char path[64];
        snprintf(path, sizeof(path), WALKING_ASSET_PATH, 0);
        lv_img_set_src(img_obj, path);
        
        // Create animation timer
        g_anim_state.timer = lv_timer_create(
            _animation_timer_callback,
            g_anim_state.frame_time_ms,
            NULL
        );
        
        if (g_anim_state.timer == NULL) {
            Serial.println("✗ Failed to create animation timer");
            return container;
        }
        
        // Auto-start animation
        g_anim_state.is_playing = true;
        Serial.println("✓ Vault Boy animation ready and playing");
    } else {
        Serial.println("⚠ Vault Boy assets not found - using placeholder");
        // Display placeholder text
        lv_obj_t *lbl = lv_label_create(container);
        lv_label_set_text(lbl, "VAULT BOY\nWALKING\nANIMATION");
        lv_obj_set_style_text_color(lbl, lv_color_make(0x00, 0xFF, 0x33), 0);
        lv_obj_center(lbl);
    }
    
    return container;
}

void vault_boy_animation_start(void) {
    if (g_anim_state.timer == NULL) {
        Serial.println("⚠ Animation timer not initialized");
        return;
    }
    
    g_anim_state.is_playing = true;
    lv_timer_resume(g_anim_state.timer);
    Serial.println("▶ Vault Boy animation started");
}

void vault_boy_animation_stop(void) {
    if (g_anim_state.timer == NULL) {
        return;
    }
    
    g_anim_state.is_playing = false;
    lv_timer_pause(g_anim_state.timer);
    Serial.println("⏸ Vault Boy animation stopped");
}

bool vault_boy_assets_available(void) {
    // Check if walking frames directory exists
    FILE *f = fopen(WALKING_ASSET_DIR, "r");
    if (f == NULL) {
        // Directory doesn't exist or can't be opened
        return false;
    }
    fclose(f);
    
    // Check if at least the first frame exists
    char path[64];
    snprintf(path, sizeof(path), WALKING_ASSET_PATH, 0);
    f = fopen(path, "r");
    if (f == NULL) {
        return false;
    }
    fclose(f);
    
    return true;
}

bool vault_boy_preload_frames(void) {
    if (g_frames_preloaded) {
        return true;
    }
    
    Serial.println("Preloading Vault Boy walking frames...");
    int loaded = 0;
    
    for (int i = 0; i < WALKING_FRAME_COUNT; i++) {
        if (_load_frame(i)) {
            loaded++;
        }
    }
    
    g_frames_preloaded = (loaded == WALKING_FRAME_COUNT);
    
    Serial.printf("Preloaded %d/%d frames\n", loaded, WALKING_FRAME_COUNT);
    return g_frames_preloaded;
}
