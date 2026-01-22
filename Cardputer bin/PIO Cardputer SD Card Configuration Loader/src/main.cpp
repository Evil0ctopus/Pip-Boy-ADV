#include <M5Unified.h>    // Use M5Unified instead of M5Cardputer for ADV support
#include <lvgl.h>         // Include LVGL library

#include "Config.h"       // Config file with external declarations
#include "HardwareConfig.h"  // Hardware pin definitions for Cardputer ADV
#include <SD.h>           // SD card library for reading config
#include <SPI.h>
#include <FS.h>

#include "WiFiHelper.h"
#include "WeatherHelper.h"
#include "LEDHelper.h"

// New modular UI includes
#include "ui_main.h"
#include "ui_animations.h"
#include "ui_settings.h"
#include "ui_assets.h"
#include "ui_radio.h"

// Hardware modules
#include "LoRaHelper.h"
#include "AudioHelper.h"
#include "SensorHelper.h"
#include "BluetoothHelper.h"

// Create buffers for LVGL display
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[240 * 10];  // Buffer size for 1/10th of screen (240 width)
static lv_color_t buf2[240 * 10];  // Second buffer for double buffering

// Function to set up the LVGL environment
void lvgl_setup() {
    // Initialize the LVGL library
    lv_init();

    // Initialize the display buffer with two buffers for smoother rendering
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, 240 * 10);

    // Set up the display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    // Flush callback to send rendered buffer to the M5Cardputer display
    disp_drv.flush_cb = [](lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
        uint32_t w = area->x2 - area->x1 + 1;
        uint32_t h = area->y2 - area->y1 + 1;

        M5.Display.startWrite();
        M5.Display.setAddrWindow(area->x1, area->y1, w, h);
        M5.Display.pushColors((uint16_t *)&color_p->full, w * h, true);
        M5.Display.endWrite();

        lv_disp_flush_ready(disp_drv);  // Inform LVGL that flushing is done
    };

    // Set the screen resolution
    disp_drv.hor_res = 240;  // Horizontal resolution
    disp_drv.ver_res = 135;  // Vertical resolution
    disp_drv.draw_buf = &draw_buf;

    // Register the display driver
    lv_disp_drv_register(&disp_drv);
}

// SD card configuration using hardware definitions
// Definitions from HardwareConfig.h:
// SD_SCK=40, SD_MISO=39, SD_MOSI=14, SD_CS=12

SPIClass *hspi = new SPIClass(HSPI);  // SPI for SD card

// Global variables for configuration
String WIFI_SSID = "";
String WIFI_PASSWORD = "";
String TIME_ZONE = "";
String API_KEY = "";
String LOCATION = "";

// Global variable to store the current LCD brightness
int currentBrightness = 128;  // Initialize brightness to half (range 0-255)

// Declare global variables for NTP and weather checks
unsigned long lastNTPTimeCheck = 0;  // To track when NTP was last checked
const unsigned long ntpTimeCheckInterval = 10000;  // 10 seconds interval for NTP time check

// Animation state
static bool walking_anim_active = false;
static unsigned long last_anim_toggle = 0;

// LoRa receive task
void loraReceiveTask(void *pvParameters) {
    while (true) {
        if (lora.isInitialized() && lora.available()) {
            LoRaPacket packet = lora.receive();
            
            if (packet.valid) {
                // Convert data to string
                char msg[256];
                size_t len = min(packet.length, (uint8_t)255);
                memcpy(msg, packet.data, len);
                msg[len] = '\0';
                
                // Update UI
                ui_radio_add_message(msg, false);
                ui_radio_update_rssi(packet.rssi);
                ui_radio_update_snr(packet.snr);
                
                // Update stats
                LoRaStats stats = lora.getStats();
                ui_radio_update_stats(stats.packets_received, stats.packets_sent);
                
                // Play notification sound
                audio.playEffect(SFX_NOTIFICATION);
            }
        }
        
        vTaskDelay(100 / portTICK_PERIOD_MS);  // Check every 100ms
    }
}

// Sensor monitoring task
void sensorMonitorTask(void *pvParameters) {
    while (true) {
        if (sensors.isInitialized()) {
            sensors.updateIMU();
            
            // Check for shake gesture
            if (sensors.detectShake()) {
                // Toggle animation on shake
                if (walking_anim_active) {
                    ui_animation_stop();
                    ui_animation_play(ANIM_THUMBSUP);
                    walking_anim_active = false;
                } else {
                    ui_animation_stop();
                    ui_animation_play(ANIM_WALKING);
                    walking_anim_active = true;
                }
                audio.playBeep();
            }
        }
        
        vTaskDelay(50 / portTICK_PERIOD_MS);  // Update every 50ms
    }
}

// Function prototypes for controlling the green LED and stopping the LED
void blinkGreenLED();
void stopLED();
void connectToWiFi();
void loadConfigFromSD();
bool inPlay startup sound
    audio.begin();
    audio.playStartupSequence();
    
    // Initialize asset loading system
    if (!ui_assets_init()) {
        Serial.println("Failed to initialize asset system");
    }

    delay(500);

    if (!initializeSDCard()) {
        Serial.println("Failed to initialize SD card");
        ui_update_date("SD Failed!");
        return;
    }

    loadConfigFromSD();  // Load config from SD card

    // Output loaded configuration values
    Serial.println("WiFi SSID: " + WIFI_SSID);
    Serial.println("WiFi Password: " + WIFI_PASSWORD);
    Serial.println("Time Zone: " + TIME_ZONE);
    Serial.println("API Key: " + API_KEY);
    Serial.println("Location: " + LOCATION);

    // Initialize hardware modules
    Serial.println("\n=== Initializing Hardware ===");
    
    // Initialize sensors
    if (sensors.begin()) {
        Serial.println("✓ Sensors initialized");
        sensors.printSensorInfo();
    } else {
        Serial.println("✗ Sensor initialization failed");
    }
    
    // Initialize LoRa
    if (lora.begin()) {
        Serial.println("✓ LoRa initialized");
        ui_radio_update_status(true, true);
        ui_radio_update_frequency(LORA_DEFAULT_FREQ);
    } else {
        Serial.println("✗ LoRa initialization failed");
        ui_radio_update_status(false, false);
        ui_radio_add_message("LoRa init failed - Check hat connection", false);
    }
    
    // Initialize Bluetooth (optional - comment out to save resources)
    // if (bluetooth.begin("Pip-Boy-ADV")) {
    //     Serial.println("✓ Bluetooth initialized");
    // } else {
    //     Serial.println("✗ Bluetooth initialization failed");
    // }
    
    Serial.println("=============================\n");

    // Start walking animation
    ui_animation_play(ANIM_WALKING);
    walking_anim_active = true;

    // Create FreeRTOS tasks for WiFi, Weather, and Battery in the background
    xTaskCreatePinnedToCore(wifiTask, "wifiTask", 4096, NULL, 1, NULL, 1);
    xTasaudio.playButtonPress();
        Serial.println("Animation toggled");
    }
    
    // Tab switching with side buttons
    if (M5.BtnB.wasPressed()) {
        // Switch to next tab
        static int current_tab = TAB_STATS;
        current_tab = (current_tab + 1) % 4;
        ui_switch_tab(current_tab);
        audio.playTabSwitch();
        Serial.printf("Switched to tab %d\n", current_tab);
    }
    
    // Brightness control with BtnC
    if (M5.BtnC.wasPressed()) {
        currentBrightness = (currentBrightness + 64) % 256;
        M5.Display.setBrightness(currentBrightness);
        ui_settings_update_brightness(currentBrightness);
        audio.playBeep(3000, 50
    M5.begin(cfg);

    M5.Display.setRotation(1); // Rotate the screen for 240x135 landscape

    // Set the initial brightness
    M5.Display.setBrightness(currentBrightness);

    // Initialize LVGL and the display setup
    lvgl_setup();

    // Initialize the new modular UI
    ui_main_init();
    
    // Initialize asset loading system
    if (!ui_assets_init()) {
        Serial.println("Failed to initialize asset system");
    }

    delay(500);

    if (!initializeSDCard()) {
        Serial.println("Failed to initialize SD card");
        ui_update_date("SD Failed!");
        return;
    }

    loadConfigFromSD();  // Load config from SD card

    // Output loaded configuration values
    Serial.println("WiFi SSID: " + WIFI_SSID);
    Serial.println("WiFi Password: " + WIFI_PASSWORD);
    Serial.println("Time Zone: " + TIME_ZONE);
    Serial.println("API Key: " + API_KEY);
    Serial.println("Location: " + LOCATION);

    // Start walking animation
    ui_animation_play(ANIM_WALKING);

    // Create FreeRTOS tasks for WiFi, Weather, and Battery in the background
    xTaskCreatePinnedToCore(wifiTask, "wifiTask", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(batteryTask, "batteryTask", 4096, NULL, 1, NULL, 1);

    // Create a FreeRTOS task for handling LVGL ticks
    xTaskCreatePinnedToCore(lv_tick_task, "lv_tick_task", 4096, NULL, 5, NULL, 1);
}

void loop() {
    // Handle LVGL tasks (this should be called regularly to update the screen)
    lv_timer_handler();
    delay(5);  // Small delay to prevent overloading the CPU

    // Periodically fetch time from NTP server (non-blocking)
    if (millis() - lastNTPTimeCheck >= ntpTimeCheckInterval) {
        fetchNTPTime();  // Fetch time every 10 seconds
        lastNTPTimeCheck = millis();  // Reset NTP time check timer
    }

    // Handle keyboard input using M5Unified
    M5.update();
    
    // Check for keyboard input (M5Unified provides BtnA, BtnB, BtnC for side buttons)
    // For Cardputer keyboard, we need to use the Keyboard interface
    // Note: M5Unified may handle keyboard differently than M5Cardputer
    
    // Side button example (if available on ADV)
    if (M5.BtnA.wasPressed()) {
        // Toggle animation
        if (walking_anim_active) {
            ui_animation_stop();
            ui_animation_play(ANIM_THUMBSUP);
            walking_anim_active = false;
        } else {
            ui_animation_stop();
            ui_animation_play(ANIM_WALKING);
            walking_anim_active = true;
        }
        Serial.println("Animation toggled");
    }
    
    // For full keyboard support on ADV, you may need to use M5Cardputer's keyboard
    // even with M5Unified, or implement custom keyboard handling
    // This is a placeholder for future keyboard integration
    
    // Tab switching with side buttons (example)
    if (M5.BtnB.wasPressed()) {
        // Switch to next tab
        static int current_tab = TAB_STATS;
        current_tab = (current_tab + 1) % 4;
        ui_switch_tab(current_tab);
        Serial.printf("Switched to tab %d\n", current_tab);
    }
    
    // Brightness control with BtnC (example)
    if (M5.BtnC.wasPressed()) {
        currentBrightness = (currentBrightness + 64) % 256;
        M5.Display.setBrightness(currentBrightness);
        ui_settings_update_brightness(currentBrightness);
        Serial.printf("Brightness: %d\n", currentBrightness);
    }
}

// Function to initialize SD card
bool initializeSDCard() {
    hspi->begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS, *hspi, SD_SPI_FREQ)) {
        Serial.println("SD card initialization failed");
        return false;
    }
    Serial.println("SD card initialized successfully");
    return true;
}


// Function to load configuration from SD card
void loadConfigFromSD() {
    File configFile = SD.open("/config.txt", FILE_READ);
    if (!configFile) {
        Serial.println("Failed to open config file");
        return;
    }

    char line[128];  // Buffer to store each line
    while (configFile.available()) {
        memset(line, 0, sizeof(line));
        configFile.readBytesUntil('\n', line, sizeof(line) - 1);
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\r\n");

        if (key && value) {
            if (strcmp(key, "WIFI_SSID") == 0) WIFI_SSID = String(value);
            else if (strcmp(key, "WIFI_PASSWORD") == 0) WIFI_PASSWORD = String(value);
            else if (strcmp(key, "TIME_ZONE") == 0) TIME_ZONE = String(value);
            else if (strcmp(key, "API_KEY") == 0) API_KEY = String(value);
            else if (strcmp(key, "LOCATION") == 0) LOCATION = String(value);
        }
    }
    configFile.close();
    Serial.println("Configuration loaded from SD card.");
}