#ifndef UI_ANIMATIONS_H
#define UI_ANIMATIONS_H

#include <lvgl.h>

// Animation types
enum AnimationType {
    ANIM_WALKING,
    ANIM_THUMBSUP,
    ANIM_NONE
};

// Animation state
struct AnimationState {
    AnimationType type;
    int current_frame;
    int total_frames;
    bool is_playing;
    lv_obj_t *img_obj;
    lv_timer_t *timer;
};

// Initialize animation system
void ui_animations_init(lv_obj_t *parent);

// Animation control
void ui_animation_play(AnimationType type);
void ui_animation_stop();
void ui_animation_pause();
void ui_animation_resume();

// Frame management
void ui_animation_update();
bool ui_animation_load_frame(AnimationType type, int frame_index);

// Get current animation state
AnimationState* ui_animation_get_state();

#endif // UI_ANIMATIONS_H
