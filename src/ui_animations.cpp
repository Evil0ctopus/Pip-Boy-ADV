#include "ui_animations.h"
#include "ui_assets.h"
#include "ui_main.h"

// Global animation state
static AnimationState anim_state = {
    .type = ANIM_NONE,
    .current_frame = 0,
    .total_frames = 0,
    .is_playing = false,
    .img_obj = NULL,
    .timer = NULL
};

// Animation timer callback
static void animation_timer_callback(lv_timer_t *timer) {
    if (!anim_state.is_playing || anim_state.img_obj == NULL) return;
    
    // Update frame
    anim_state.current_frame++;
    if (anim_state.current_frame >= anim_state.total_frames) {
        anim_state.current_frame = 0;  // Loop animation
    }
    
    // Load and display the frame
    if (!ui_animation_load_frame(anim_state.type, anim_state.current_frame)) {
        // If frame fails to load, stop animation to prevent continuous errors
        Serial.println("⚠ Animation frame load failed - stopping animation");
        ui_animation_stop();
    }
}

// Initialize animation system
void ui_animations_init(lv_obj_t *parent) {
    if (parent == NULL) return;
    
    // Create image object for animation
    anim_state.img_obj = lv_img_create(parent);
    lv_obj_align(anim_state.img_obj, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(anim_state.img_obj, LV_OPA_TRANSP, 0);
    
    // Create timer for animation (100ms per frame = 10 FPS)
    anim_state.timer = lv_timer_create(animation_timer_callback, 100, NULL);
    lv_timer_pause(anim_state.timer);
    
    // Initialize with no animation
    anim_state.type = ANIM_NONE;
    anim_state.is_playing = false;
}

// Play animation
void ui_animation_play(AnimationType type) {
    if (anim_state.img_obj == NULL) return;
    
    // Stop current animation if playing
    if (anim_state.is_playing) {
        ui_animation_stop();
    }
    
    // Set new animation type
    anim_state.type = type;
    anim_state.current_frame = 0;
    
    // Set frame count based on type
    switch (type) {
        case ANIM_WALKING:
            anim_state.total_frames = WALKING_FRAME_COUNT;
            break;
        case ANIM_THUMBSUP:
            anim_state.total_frames = THUMBSUP_FRAME_COUNT;
            break;
        default:
            anim_state.total_frames = 0;
            return;
    }
    
    // Load first frame
    if (ui_animation_load_frame(type, 0)) {
        anim_state.is_playing = true;
        lv_timer_resume(anim_state.timer);
        Serial.printf("✓ Animation started: type=%d frames=%d\n", type, anim_state.total_frames);
    } else {
        Serial.printf("⚠ Animation failed to load: type=%d (missing SD assets)\n", type);
        // Don't start animation if frames can't load
        anim_state.is_playing = false;
        anim_state.type = ANIM_NONE;
    }
}

// Stop animation
void ui_animation_stop() {
    anim_state.is_playing = false;
    anim_state.current_frame = 0;
    anim_state.type = ANIM_NONE;
    
    if (anim_state.timer != NULL) {
        lv_timer_pause(anim_state.timer);
    }
    
    // Clear image
    if (anim_state.img_obj != NULL) {
        lv_img_set_src(anim_state.img_obj, NULL);
    }
}

// Pause animation
void ui_animation_pause() {
    anim_state.is_playing = false;
    if (anim_state.timer != NULL) {
        lv_timer_pause(anim_state.timer);
    }
}

// Resume animation
void ui_animation_resume() {
    if (anim_state.type != ANIM_NONE) {
        anim_state.is_playing = true;
        if (anim_state.timer != NULL) {
            lv_timer_resume(anim_state.timer);
        }
    }
}

// Load specific animation frame
bool ui_animation_load_frame(AnimationType type, int frame_index) {
    if (anim_state.img_obj == NULL) return false;
    
    char path[64];
    lv_img_dsc_t *img = NULL;
    
    // Get the image based on type
    switch (type) {
        case ANIM_WALKING:
            if (frame_index < 0 || frame_index >= WALKING_FRAME_COUNT) return false;
            if (walking_frames[frame_index] != NULL) {
                img = walking_frames[frame_index];
            } else {
                // Load on demand if not cached
                snprintf(path, sizeof(path), WALKING_ASSET_PATH, frame_index);
                img = ui_assets_load_image(path);
                walking_frames[frame_index] = img;
            }
            break;
            
        case ANIM_THUMBSUP:
            if (frame_index < 0 || frame_index >= THUMBSUP_FRAME_COUNT) return false;
            if (thumbsup_frames[frame_index] != NULL) {
                img = thumbsup_frames[frame_index];
            } else {
                // Load on demand if not cached
                snprintf(path, sizeof(path), THUMBSUP_ASSET_PATH, frame_index);
                img = ui_assets_load_image(path);
                thumbsup_frames[frame_index] = img;
            }
            break;
            
        default:
            return false;
    }
    
    if (img != NULL) {
        lv_img_set_src(anim_state.img_obj, img);
        return true;
    }
    
    return false;
}

// Update animation (called from main loop if needed)
void ui_animation_update() {
    // Animation is timer-based, this can be used for manual updates if needed
    // Currently not needed as we use LVGL timer
}

// Get current animation state
AnimationState* ui_animation_get_state() {
    return &anim_state;
}
