// ============================================================================
// Vault Boy Walking Animation Handler
// Loads and plays PNG frames from SD card
// From: https://github.com/nishad2m8/Pip-Boy-Weather-Clock
// ============================================================================

#ifndef VAULTBOY_ANIMATION_H
#define VAULTBOY_ANIMATION_H

#include <lvgl.h>

// ============================================================================
// Animation Configuration
// ============================================================================

#define WALKING_FRAME_COUNT 21                                    // walking_00.png through walking_20.png
#define WALKING_FRAME_TIME 100                                    // 100ms per frame = 10 FPS
#define WALKING_TOTAL_TIME (WALKING_FRAME_COUNT * WALKING_FRAME_TIME)  // 2100ms per cycle

#define WALKING_ASSET_PATH "/assets/Images/walking/walking_%02d.png"
#define WALKING_ASSET_DIR "/assets/Images/walking"

// ============================================================================
// Animation State
// ============================================================================

typedef struct {
    lv_obj_t *img_obj;           // LVGL image object to display frames
    lv_timer_t *timer;           // Animation timer
    int current_frame;           // Current frame index (0-20)
    int total_frames;            // Total frames in animation
    bool is_playing;             // Is animation currently running
    uint32_t frame_time_ms;      // Milliseconds per frame
} VaultBoyAnimationState;

// ============================================================================
// Public API
// ============================================================================

/**
 * Initialize Vault Boy walking animation
 * @param parent LVGL object to create animation container in
 * @return pointer to animation container object, or NULL on error
 */
lv_obj_t *vault_boy_animation_init(lv_obj_t *parent);

/**
 * Start the walking animation
 */
void vault_boy_animation_start(void);

/**
 * Stop the walking animation
 */
void vault_boy_animation_stop(void);

/**
 * Check if animation assets are available on SD card
 * @return true if walking frames directory exists and is readable
 */
bool vault_boy_assets_available(void);

/**
 * Preload all walking frames into memory for smooth playback
 * @return true if all frames loaded successfully
 */
bool vault_boy_preload_frames(void);

#endif  // VAULTBOY_ANIMATION_H
