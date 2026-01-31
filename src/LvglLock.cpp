#include "LvglLock.h"

static SemaphoreHandle_t s_lvglMutex = nullptr;

void lvgl_lock_init() {
    if (s_lvglMutex == nullptr) {
        s_lvglMutex = xSemaphoreCreateRecursiveMutex();
    }
}

void lvgl_lock() {
    if (s_lvglMutex) {
        xSemaphoreTakeRecursive(s_lvglMutex, portMAX_DELAY);
    }
}

void lvgl_unlock() {
    if (s_lvglMutex) {
        xSemaphoreGiveRecursive(s_lvglMutex);
    }
}
