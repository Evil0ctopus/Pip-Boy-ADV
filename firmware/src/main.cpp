#include <M5Unified.h>
#include <lvgl.h>

#include "drivers/display_adv.h"
#include "drivers/input_adv.h"
#include "ui.h"

// LVGL tick handler (called every 5ms)
void lv_tick_task(void *arg) {
    lv_tick_inc(5);
}

void setup() {
  delay(2000);
  Serial.begin(115200);
  Serial.println("before M5.begin");

  auto cfg = M5.config();
  cfg.output_power = true;
  cfg.internal_imu = false;
  cfg.internal_rtc = true;
  cfg.internal_spk = false;
  cfg.internal_mic = false;
  M5.begin(cfg);

  Serial.println("after M5.begin");

  Serial.println("before adv_display_init");
  adv_display_init();
  Serial.println("after adv_display_init");

  Serial.println("before adv_input_init");
  adv_input_init();
  Serial.println("after adv_input_init");
}

void loop() {
  delay(1000);
  Serial.println("loop alive");
}
// ADV entry point for PlatformIO Arduino project
#include <M5Unified.h>
#include <lvgl.h>

#include "drivers/display_adv.h"
#include "drivers/input_adv.h"
#include "ui.h"



