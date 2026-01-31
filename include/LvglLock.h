#ifndef LVGL_LOCK_H
#define LVGL_LOCK_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Initialize LVGL mutex. Call once after lv_init().
void lvgl_lock_init();

// Lock/unlock LVGL from non-LVGL tasks.
void lvgl_lock();
void lvgl_unlock();

#endif // LVGL_LOCK_H
