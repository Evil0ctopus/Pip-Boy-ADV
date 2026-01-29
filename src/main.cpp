// ============================================================================
// Pip-Boy ADV - M5Stack Cardputer ADV Firmware
// ============================================================================
// Main firmware file for the Pip-Boy ADV project
// Supports: ESP32-S3, LVGL UI, LoRa, WiFi, Sensors, Audio
// ============================================================================

#include <M5Unified.h>       // M5Unified library for Cardputer ADV support
#include <lvgl.h>            // LVGL graphics library
#include <WiFi.h>            // WiFi connectivity
#include <SD.h>              // SD card filesystem
#include <SPI.h>             // SPI communication
#include <FS.h>              // File system

// ============================================================================
// Includes - Configuration and Helper Modules
// ============================================================================

#include "Config.h"          // Configuration manager (uses Config class)
#include "HardwareConfig.h"  // Hardware pin definitions for Cardputer ADV

// UI System - Modular interface components
#include "ui_shell.h"        // Main UI shell/container
#include "ui_main.h"         // Core UI functions
#include "ui_animations.h"   // Animation controller
#include "ui_settings.h"     // Settings UI
#include "ui_assets.h"       // Asset loading system
#include "ui_radio.h"        // Radio/LoRa UI
#include "ui_statusbar.h"    // Status bar UI
#include "ui_tabs.h"         // Tab navigation UI

// Helper Modules - Hardware and utility abstractions
#include "WiFiHelper.h"      // WiFi management
#include "WeatherHelper.h"   // Weather API integration
#include "TimeHelper.h"      // NTP time synchronization
#include "LEDHelper.h"       // NeoPixel LED control
#include "AudioHelper.h"     // Speaker/audio output
#include "SensorHelper.h"    // IMU sensor integration
#include "LoRaHelper.h"      // LoRa radio communication
#include "BluetoothHelper.h" // Bluetooth functionality
#include "KeyboardHelper.h"  // Keyboard input handling
#include "SystemStats.h"     // System statistics monitoring

// ============================================================================
// Global Variables
// ============================================================================

// LVGL Display Buffers (double buffering for smooth rendering)
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[DISPLAY_WIDTH * 10];  // Buffer 1: 1/10th of screen
static lv_color_t buf2[DISPLAY_WIDTH * 10];  // Buffer 2: for double buffering

// SPI instance for SD card (HSPI)
SPIClass *hspi = new SPIClass(HSPI);

// Configuration Variables (loaded from SD card /config.txt)
String WIFI_SSID = "";
String WIFI_PASSWORD = "";
String TIME_ZONE = "PST8PDT,M3.2.0,M11.1.0";  // Default to Pacific Time
String API_KEY = "";
String LOCATION = "";

// System State Variables
int currentBrightness = 128;           // LCD brightness (0-255)
unsigned long lastNTPSync = 0;         // Last NTP synchronization time
unsigned long lastStatsUpdate = 0;     // Last system stats update
unsigned long lastAnimToggle = 0;      // Last animation toggle time
bool walkingAnimActive = false;        // Animation state flag

// Task Timing Constants
const unsigned long NTP_SYNC_INTERVAL = 300000;    // 5 minutes
const unsigned long STATS_UPDATE_INTERVAL = 5000;  // 5 seconds
const unsigned long UI_UPDATE_INTERVAL = 10000;    // 10 seconds

// Task Handles (for FreeRTOS tasks)
TaskHandle_t wifiTaskHandle = NULL;
TaskHandle_t weatherTaskHandle = NULL;
TaskHandle_t loraTaskHandle = NULL;
TaskHandle_t sensorTaskHandle = NULL;
TaskHandle_t statsTaskHandle = NULL;

// ============================================================================
// LVGL Setup and Display Driver
// ============================================================================

/**
 * LVGL Display Flush Callback
 * Transfers rendered LVGL buffer to M5Cardputer display
 */
void lvgl_display_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;

    M5.Display.startWrite();
    M5.Display.setAddrWindow(area->x1, area->y1, w, h);
    M5.Display.pushColors((uint16_t *)&color_p->full, w * h, true);
    M5.Display.endWrite();

    lv_disp_flush_ready(disp_drv);
}

/**
 * Initialize LVGL Library and Display Driver
 */
void lvgl_setup() {
    Serial.println("Initializing LVGL...");
    
    // Initialize LVGL core
    lv_init();

    // Initialize display buffer with double buffering
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, DISPLAY_WIDTH * 10);

    // Configure display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    
    disp_drv.hor_res = DISPLAY_WIDTH;
    disp_drv.ver_res = DISPLAY_HEIGHT;
    disp_drv.flush_cb = lvgl_display_flush;
    disp_drv.draw_buf = &draw_buf;

    // Register display driver
    lv_disp_drv_register(&disp_drv);
    
    Serial.println("✓ LVGL initialized");
}

/**
 * LVGL Tick Task
 * Provides timing for LVGL animations and timers
 */
void lvgl_tick_task(void *pvParameters) {
    while (true) {
        lv_tick_inc(5);  // Increment LVGL tick by 5ms
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

// ============================================================================
// SD Card and Configuration Management
// ============================================================================

/**
 * Initialize SD Card
 * Configures HSPI and mounts SD card filesystem
 * @return true if successful, false otherwise
 */
bool initializeSDCard() {
    Serial.println("Initializing SD card...");
    
    // Configure HSPI for SD card
    hspi->begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    
    if (!SD.begin(SD_CS, *hspi, SD_SPI_FREQ)) {
        Serial.println("✗ SD card initialization failed");
        return false;
    }
    
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("✓ SD card initialized: %llu MB\n", cardSize);
    return true;
}

/**
 * Load Configuration from SD Card
 * Reads /config.txt and populates global configuration variables
 */
void loadConfigFromSD() {
    Serial.println("Loading configuration from /config.txt...");
    
    File configFile = SD.open("/config.txt", FILE_READ);
    if (!configFile) {
        Serial.println("✗ Failed to open /config.txt");
        Serial.println("Please create config.txt on SD card with:");
        Serial.println("  WIFI_SSID=YourSSID");
        Serial.println("  WIFI_PASSWORD=YourPassword");
        Serial.println("  TIME_ZONE=PST8PDT,M3.2.0,M11.1.0");
        Serial.println("  API_KEY=YourWeatherAPIKey");
        Serial.println("  LOCATION=YourCity");
        return;
    }

    // Parse configuration file line by line
    char line[256];
    while (configFile.available()) {
        memset(line, 0, sizeof(line));
        configFile.readBytesUntil('\n', line, sizeof(line) - 1);
        
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\r\n");

        if (key && value) {
            // Trim whitespace
            while (*value == ' ') value++;
            
            if (strcmp(key, "WIFI_SSID") == 0) {
                WIFI_SSID = String(value);
            } else if (strcmp(key, "WIFI_PASSWORD") == 0) {
                WIFI_PASSWORD = String(value);
            } else if (strcmp(key, "TIME_ZONE") == 0) {
                TIME_ZONE = String(value);
            } else if (strcmp(key, "API_KEY") == 0) {
                API_KEY = String(value);
            } else if (strcmp(key, "LOCATION") == 0) {
                LOCATION = String(value);
            }
        }
    }
    
    configFile.close();
    
    // Print loaded configuration (mask sensitive data)
    Serial.println("✓ Configuration loaded:");
    Serial.printf("  WiFi SSID: %s\n", WIFI_SSID.c_str());
    Serial.printf("  WiFi Password: %s\n", WIFI_PASSWORD.length() > 0 ? "********" : "NOT SET");
    Serial.printf("  Time Zone: %s\n", TIME_ZONE.c_str());
    Serial.printf("  API Key: %s\n", API_KEY.length() > 0 ? "********" : "NOT SET");
    Serial.printf("  Location: %s\n", LOCATION.c_str());
}

// ============================================================================
// FreeRTOS Background Tasks
// ============================================================================

/**
 * WiFi Connection Task
 * Handles WiFi connection and NTP synchronization
 */
void wifiConnectionTask(void *pvParameters) {
    Serial.println("Starting WiFi connection...");
    
    LEDHelper led;
    led.begin();
    led.setLedColor(LedColor::RED);  // Red while connecting
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID.c_str(), WIFI_PASSWORD.c_str());
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\n✓ WiFi connected: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("  RSSI: %d dBm\n", WiFi.RSSI());
        
        led.setLedColor(LedColor::BLUE);  // Blue when connected
        
        // Initialize NTP time synchronization
        if (timeHelper.begin(TIME_ZONE.c_str())) {
            Serial.println("✓ NTP time synchronized");
            led.setLedColor(LedColor::GREEN);  // Green when fully ready
        }
        
        // Update UI with WiFi status
        ui_shell_update_wifi(true, WiFi.RSSI());
        
        // Start weather task after WiFi is connected
        xTaskCreatePinnedToCore(
            weatherTask,
            "WeatherTask",
            8192,
            NULL,
            1,
            &weatherTaskHandle,
            1
        );
    } else {
        Serial.println("\n✗ WiFi connection failed");
        ui_shell_update_wifi(false, -100);
    }
    
    // Task complete, delete itself
    vTaskDelete(NULL);
}

/**
 * System Statistics Monitoring Task
 * Updates system stats and battery status periodically
 */
void systemStatsTask(void *pvParameters) {
    Serial.println("System stats task started");
    
    while (true) {
        // Update system statistics
        systemStats.update();
        
        // Update battery UI
        int batteryLevel = systemStats.getBatteryLevel();
        bool charging = systemStats.isCharging();
        ui_shell_update_battery(batteryLevel, charging);
        
        // Update WiFi RSSI if connected
        if (systemStats.isWiFiConnected()) {
            ui_shell_update_wifi(true, systemStats.getWiFiRSSI());
        }
        
        // Update DATA tab information
        ui_shell_update_cpu(systemStats.getCPUFreqMHz());
        ui_shell_update_memory(systemStats.getFreeHeap(), 320000);
        ui_shell_update_psram(systemStats.getPSRAMFree(), systemStats.getPSRAMSize());
        ui_shell_update_uptime(systemStats.getUptime());
        
        // Print detailed stats every minute
        static unsigned long lastPrint = 0;
        if (millis() - lastPrint > 60000) {
            systemStats.printStats();
            lastPrint = millis();
        }
        
        // Update every 5 seconds
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

/**
 * LoRa Receive Task
 * Continuously monitors for incoming LoRa messages
 */
void loraReceiveTask(void *pvParameters) {
    Serial.println("LoRa receive task started");
    
    while (true) {
        if (lora.isInitialized() && lora.available()) {
            LoRaPacket packet = lora.receive();
            
            if (packet.valid) {
                // Convert packet data to string
                char msg[256];
                size_t len = min(packet.length, (uint8_t)255);
                memcpy(msg, packet.data, len);
                msg[len] = '\0';
                
                // Update radio UI
                ui_radio_add_message(msg, false);
                ui_radio_update_rssi(packet.rssi);
                ui_radio_update_snr(packet.snr);
                
                // Update statistics
                LoRaStats stats = lora.getStats();
                ui_radio_update_stats(stats.packets_received, stats.packets_sent);
                
                // Update status bar
                ui_shell_update_lora(true, packet.rssi);
                
                // Play notification sound
                audio.playEffect(SFX_NOTIFICATION);
                
                Serial.printf("LoRa RX: %s [RSSI: %.0f dBm, SNR: %.1f dB]\n", 
                              msg, packet.rssi, packet.snr);
            }
        }
        
        // Check every 100ms
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * Sensor Monitoring Task
 * Monitors IMU sensors for gestures and orientation
 */
void sensorMonitorTask(void *pvParameters) {
    Serial.println("Sensor monitor task started");
    
    while (true) {
        if (sensors.isInitialized()) {
            sensors.updateIMU();
            
            // Detect shake gesture to toggle animation
            if (sensors.detectShake()) {
                if (walkingAnimActive) {
                    ui_animation_stop();
                    ui_animation_play(ANIM_THUMBSUP);
                    walkingAnimActive = false;
                    audio.playEffect(SFX_NONE);
                } else {
                    ui_animation_stop();
                    ui_animation_play(ANIM_WALKING);
                    walkingAnimActive = true;
                    audio.playBeep(2000, 100);
                }
                
                Serial.println("Gesture detected: Animation toggled");
            }
        }
        
        // Update every 50ms for responsive gesture detection
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// ============================================================================
// Time Update Functions
// ============================================================================

/**
 * Update Time Display from NTP
 * Called periodically to refresh time/date on UI
 */
void updateTimeDisplay() {
    if (!timeHelper.isTimeSynced()) {
        return;
    }
    
    char timeStr[16];
    char dateStr[32];
    
    if (timeHelper.getFormattedTime(timeStr, sizeof(timeStr))) {
        ui_shell_update_time(timeStr);
    }
    
    if (timeHelper.getFormattedDate(dateStr, sizeof(dateStr))) {
        ui_shell_update_date(dateStr);
    }
}

/**
 * Periodic NTP Sync
 * Re-synchronizes time with NTP server
 */
void periodicNTPSync() {
    unsigned long now = millis();
    
    if (now - lastNTPSync >= NTP_SYNC_INTERVAL) {
        if (WiFi.status() == WL_CONNECTED && timeHelper.syncNTP()) {
            Serial.println("✓ NTP time re-synchronized");
        }
        lastNTPSync = now;
    }
}

// ============================================================================
// Input Handling
// ============================================================================

/**
 * Handle Button and Keyboard Input
 * Processes M5Unified button events
 */
void handleInput() {
    M5.update();
    
    // Button A: Toggle Animation
    if (M5.BtnA.wasPressed()) {
        if (walkingAnimActive) {
            ui_animation_stop();
            ui_animation_play(ANIM_THUMBSUP);
            walkingAnimActive = false;
        } else {
            ui_animation_stop();
            ui_animation_play(ANIM_WALKING);
            walkingAnimActive = true;
        }
        audio.playButtonPress();
        Serial.println("Button A: Animation toggled");
    }
    
    // Button B: Switch Tab
    if (M5.BtnB.wasPressed()) {
        static int currentTab = TAB_STATS;
        currentTab = (currentTab + 1) % 4;  // Cycle through 4 tabs
        ui_switch_tab(currentTab);
        audio.playTabSwitch();
        Serial.printf("Button B: Switched to tab %d\n", currentTab);
    }
    
    // Button C: Brightness Control
    if (M5.BtnC.wasPressed()) {
        currentBrightness = (currentBrightness + 64) % 256;
        if (currentBrightness == 0) currentBrightness = 64;  // Avoid complete darkness
        M5.Display.setBrightness(currentBrightness);
        ui_settings_update_brightness(currentBrightness);
        audio.playBeep(3000, 50);
        Serial.printf("Button C: Brightness = %d\n", currentBrightness);
    }
}

// ============================================================================
// Arduino Setup
// ============================================================================

void setup() {
    // Initialize Serial
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n");
    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║     Pip-Boy ADV Firmware v1.0.0       ║");
    Serial.println("║   M5Stack Cardputer ADV (ESP32-S3)    ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.println();

    // ========================================================================
    // Stage 1: M5Unified Initialization
    // ========================================================================
    Serial.println("=== Stage 1: M5Unified Initialization ===");
    
    auto cfg = M5.config();
    cfg.clear_display = true;
    cfg.output_power = true;
    cfg.internal_imu = true;
    cfg.internal_rtc = true;
    cfg.internal_spk = true;
    cfg.internal_mic = false;  // Cardputer ADV may not have mic
    
    M5.begin(cfg);
    M5.Display.setRotation(DISPLAY_ROTATION);
    M5.Display.setBrightness(currentBrightness);
    
    Serial.println("✓ M5Unified initialized");

    // ========================================================================
    // Stage 2: LVGL and UI Initialization
    // ========================================================================
    Serial.println("\n=== Stage 2: LVGL Initialization ===");
    
    lvgl_setup();
    
    // Initialize UI shell (creates main screen, status bar, tabs)
    ui_shell_init();
    
    Serial.println("✓ UI shell loaded");

    // ========================================================================
    // Stage 3: Hardware Module Initialization
    // ========================================================================
    Serial.println("\n=== Stage 3: Hardware Initialization ===");
    
    // Initialize Audio
    audio.begin();
    audio.playStartupSequence();
    Serial.println("✓ Audio initialized");
    
    // Initialize LED
    LEDHelper led;
    led.begin();
    led.setLedColor(LedColor::GREEN);  // Green during boot
    Serial.println("✓ LED initialized");
    
    // Initialize System Stats
    systemStats.begin();
    Serial.println("✓ System stats initialized");

    // ========================================================================
    // Stage 4: SD Card and Configuration
    // ========================================================================
    Serial.println("\n=== Stage 4: SD Card & Configuration ===");
    
    if (!initializeSDCard()) {
        ui_shell_update_date("SD CARD ERROR");
        Serial.println("⚠ Continuing without SD card...");
    } else {
        // Load configuration using Config class
        if (Config::begin()) {
            Serial.println("✓ Configuration loaded");
        } else {
            Serial.printf("⚠ Config load failed: %s\n", Config::getLastError().c_str());
        }
        
        // Initialize asset loading system
        if (ui_assets_init()) {
            Serial.println("✓ Asset system initialized");
        }
    }

    // ========================================================================
    // Stage 5: Sensor Initialization
    // ========================================================================
    Serial.println("\n=== Stage 5: Sensor Initialization ===");
    
    if (sensors.begin()) {
        Serial.println("✓ Sensors initialized");
        sensors.printSensorInfo();
        
        // Start sensor monitoring task
        xTaskCreatePinnedToCore(
            sensorMonitorTask,
            "SensorTask",
            4096,
            NULL,
            2,
            &sensorTaskHandle,
            1
        );
    } else {
        Serial.println("⚠ Sensor initialization failed");
    }

    // ========================================================================
    // Stage 6: LoRa Radio Initialization
    // ========================================================================
    Serial.println("\n=== Stage 6: LoRa Initialization ===");
    
    if (lora.begin()) {
        Serial.printf("✓ LoRa initialized @ %.1f MHz\n", LORA_DEFAULT_FREQ);
        ui_radio_update_status(true, true);
        ui_radio_update_frequency(LORA_DEFAULT_FREQ);
        ui_shell_update_lora(true, 0);
        
        // Start LoRa receive task
        xTaskCreatePinnedToCore(
            loraReceiveTask,
            "LoRaTask",
            4096,
            NULL,
            2,
            &loraTaskHandle,
            1
        );
    } else {
        Serial.println("⚠ LoRa initialization failed - check hat connection");
        ui_radio_update_status(false, false);
        ui_radio_add_message("LoRa init failed - Check SX1262 hat", false);
    }

    // ========================================================================
    // Stage 7: Optional Bluetooth
    // ========================================================================
    Serial.println("\n=== Stage 7: Bluetooth (Optional) ===");
    
    // Uncomment to enable Bluetooth (uses significant resources)
    /*
    if (bluetooth.begin("Pip-Boy-ADV")) {
        Serial.println("✓ Bluetooth initialized");
    } else {
        Serial.println("⚠ Bluetooth initialization failed");
    }
    */
    Serial.println("⊘ Bluetooth disabled (save resources)");

    // ========================================================================
    // Stage 8: Background Tasks
    // ========================================================================
    Serial.println("\n=== Stage 8: Starting Background Tasks ===");
    
    // LVGL tick task (highest priority)
    xTaskCreatePinnedToCore(
        lvgl_tick_task,
        "LVGL_Tick",
        4096,
        NULL,
        5,
        NULL,
        1
    );
    Serial.println("✓ LVGL tick task started");
    
    // WiFi and dependent services
    if (Config::getWiFiSSID().length() > 0) {
        // Initialize WiFi helper
        WiFiHelper::begin();
        
        // Create WiFi connection task
        xTaskCreatePinnedToCore(
            [](void* param) {
                if (WiFiHelper::connect()) {
                    // WiFi connected, start dependent services
                    
                    // Initialize and start TimeHelper
                    if (timeHelper.begin(Config::getTimeZone().c_str())) {
                        Serial.println("✓ TimeHelper initialized");
                    }
                    
                    // Initialize and start WeatherHelper
                    if (WeatherHelper::begin()) {
                        WeatherHelper::startUpdateTask();
                        Serial.println("✓ WeatherHelper initialized");
                    }
                    
                    // Start WiFi monitoring
                    WiFiHelper::startMonitorTask();
                }
                vTaskDelete(NULL);
            },
            "WiFiInit",
            8192,
            NULL,
            1,
            &wifiTaskHandle,
            1
        );
        Serial.println("✓ WiFi initialization task started");
    } else {
        Serial.println("⚠ WiFi disabled - no SSID configured");
        ui_shell_update_wifi(false, -100);
    }
    
    // System stats monitoring task
    systemStats.startMonitorTask();
    Serial.println("✓ System stats task started");

    // ========================================================================
    // Stage 9: Launch UI
    // ========================================================================
    Serial.println("\n=== Stage 9: Launching UI ===");
    
    // Start the UI shell (initializes and loads the interface)
    ui_shell_start();
    
    // Start walking animation
    ui_animation_play(ANIM_WALKING);
    walkingAnimActive = true;
    Serial.println("✓ Animation started");
    
    // Update initial UI state
    ui_shell_update_battery(systemStats.getBatteryLevel(), systemStats.isCharging());
    ui_shell_update_cpu(systemStats.getCPUFreqMHz());
    ui_shell_update_memory(systemStats.getFreeHeap(), 320000); // ESP32-S3 has ~320KB RAM
    ui_shell_update_psram(systemStats.getPSRAMFree(), systemStats.getPSRAMSize());
    ui_shell_update_uptime(0);
    
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║      Boot Complete - System Ready     ║");
    Serial.println("╚════════════════════════════════════════╝\n");
    
    audio.playEffect(SFX_NONE);
}

// ============================================================================
// Arduino Main Loop
// ============================================================================

void loop() {
    // Update LVGL (core UI rendering)
    lv_timer_handler();
    
    // Handle button/keyboard input
    handleInput();
    
    // Update time display periodically
    static unsigned long lastUIUpdate = 0;
    if (millis() - lastUIUpdate >= UI_UPDATE_INTERVAL) {
        updateTimeDisplay();
        lastUIUpdate = millis();
    }
    
    // Periodic NTP synchronization
    periodicNTPSync();
    
    // Small delay to prevent CPU overload (5ms matches LVGL tick)
    delay(5);
}