// ============================================================================
// Pip-Boy ADV - M5Stack Cardputer ADV Firmware
// ============================================================================
// Main firmware file for the Pip-Boy ADV project
// Supports: ESP32-S3, LVGL UI, LoRa, WiFi, Sensors, Audio
// ============================================================================

#include <M5Unified.h>       // M5Unified library for Cardputer ADV support
#include <lvgl.h>            // LVGL graphics library
#include <SD.h>              // SD card filesystem
#include <SPI.h>             // SPI communication
#include <FS.h>              // File system

// ============================================================================
// Includes - Configuration and Helper Modules
// ============================================================================

#include "HardwareConfig.h"  // Hardware pin definitions for Cardputer ADV
#include "ProjectConfig.h"   // Feature flags for WiFi/Weather/Config
#include "SDHelper.h"        // Centralized SD initialization
#include "LvglLock.h"        // LVGL mutex

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
#include "LEDHelper.h"       // NeoPixel LED control
#include "AudioHelper.h"     // Speaker/audio output
#include "SensorHelper.h"    // IMU sensor integration
#include "LoRaHelper.h"      // LoRa radio communication
#include "BluetoothHelper.h" // Bluetooth functionality
#include "KeyboardHelper.h"  // Keyboard input handling
#include "SystemStats.h"     // System statistics monitoring

// New Optional Modules
#include "IRHelper.h"        // IR capture/replay (M5 IR Unit)
#include "GPSHelper.h"       // GPS module (NEO-6M/NEO-M8N)
#include "FileBrowser.h"     // SD card file browser
#include "TerminalHelper.h"  // Shell terminal
#include "WiFiManager.h"     // Wi-Fi management (stubbed when disabled)
#include "MapRenderer.h"     // Map rendering
#include "PluginSystem.h"    // Plugin loader

// Module integration layer
#include "ModuleIntegration.h"

// ============================================================================
// Global Variables
// ============================================================================

// LVGL Display Buffers (double buffering for smooth rendering)
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[DISPLAY_WIDTH * 10];  // Buffer 1: 1/10th of screen
static lv_color_t buf2[DISPLAY_WIDTH * 10];  // Buffer 2: for double buffering

// SPI instance for SD card (HSPI)
SPIClass *hspi = new SPIClass(HSPI);

// Configuration Variables are defined in ConfigHelper.cpp and declared extern in Config.h
// No need to redefine them here - just include Config.h to use them

// System State Variables
int currentBrightness = 128;           // LCD brightness (0-255)
bool walkingAnimActive = false;        // Animation state flag

// Task Handles (for FreeRTOS tasks)
TaskHandle_t loraTaskHandle = NULL;
TaskHandle_t sensorTaskHandle = NULL;

// Global helper instances
AudioHelper audio;
SensorHelper sensorHelper;
SystemStats systemStats;
LoRaHelper loraHelper;

// Optional module instances (gracefully handle missing hardware)
IRHelper irHelper;
GPSHelper gpsHelper;
FileBrowser fileBrowser;
TerminalHelper terminalHelper;
WiFiManager wifiManager;
MapRenderer mapRenderer;
PluginSystem pluginSystem;

// Module integration layer (after all module instances)
ModuleIntegration modules;

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

    // Ensure display is awake during write
    M5.Display.wakeup();
    
    M5.Display.startWrite();
    M5.Display.setAddrWindow(area->x1, area->y1, w, h);
    
    // Write pixels with proper color format handling
    uint16_t *pixels = (uint16_t *)color_p;
    for (uint32_t i = 0; i < w * h; i++) {
        M5.Display.writeData(pixels[i]);
    }
    
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
    lvgl_lock_init();
    Serial.println("  ✓ LVGL core initialized (v8.4.0)");

    // Initialize display buffer with double buffering
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, DISPLAY_WIDTH * 10);
    Serial.printf("  ✓ Display buffers allocated (2x %d bytes)\n", DISPLAY_WIDTH * 10 * sizeof(lv_color_t));

    // Configure display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    
    disp_drv.hor_res = DISPLAY_WIDTH;
    disp_drv.ver_res = DISPLAY_HEIGHT;
    disp_drv.flush_cb = lvgl_display_flush;
    disp_drv.draw_buf = &draw_buf;
    Serial.printf("  ✓ Display driver configured (%dx%d)\n", DISPLAY_WIDTH, DISPLAY_HEIGHT);

    // Register display driver
    lv_disp_drv_register(&disp_drv);
    Serial.println("  ✓ Display driver registered");
    
    Serial.println("✓ LVGL initialization complete");
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

    if (!SDHelper::begin(SD_CS, *hspi, SD_SPI_FREQ)) {
        Serial.println("✗ SD card initialization failed");
        return false;
    }
    
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("✓ SD card initialized: %llu MB\n", cardSize);
    return true;
}

// ============================================================================
// Keyboard/Input Handler
// ============================================================================

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
        lvgl_lock();
        ui_shell_update_battery(batteryLevel, charging);
        
        // WiFi status update removed for standalone operation
        
        // Update STATUS tab with uptime and memory
        uint32_t uptime = systemStats.getUptime();
        uint32_t hours = (uptime % 86400) / 3600;
        uint32_t minutes = (uptime % 3600) / 60;
        uint32_t secs = uptime % 60;
        char timeStr[16];
        snprintf(timeStr, sizeof(timeStr), "%02lu:%02lu:%02lu", hours, minutes, secs);
        ui_shell_update_time(timeStr);
        
        char memStr[32];
        uint32_t freeHeap = systemStats.getFreeHeap();
        snprintf(memStr, sizeof(memStr), "RAM: %luKB", freeHeap / 1024);
        ui_shell_update_weather("CPU: 240 MHz", memStr);
        
        // Update DATA tab information
        ui_shell_update_cpu(systemStats.getCPUFreqMHz());
        ui_shell_update_memory(systemStats.getFreeHeap(), 320000);
        ui_shell_update_psram(systemStats.getPSRAMFree(), systemStats.getPSRAMSize());
        ui_shell_update_uptime(systemStats.getUptime());
        lvgl_unlock();
        
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
        if (loraHelper.isInitialized() && loraHelper.available()) {
            LoRaPacket packet = loraHelper.receive();
            
            if (packet.valid) {
                // Convert packet data to string
                char msg[256];
                size_t len = min(packet.length, (uint8_t)255);
                memcpy(msg, packet.data, len);
                msg[len] = '\0';
                
                // Update radio UI
                lvgl_lock();
                ui_radio_add_message(msg, false);
                ui_radio_update_rssi(packet.rssi);
                ui_radio_update_snr(packet.snr);
                
                // Update statistics
                LoRaStats stats = loraHelper.getStats();
                ui_radio_update_stats(stats.packets_received, stats.packets_sent);
                
                // Update status bar
                ui_shell_update_lora(true, packet.rssi);
                lvgl_unlock();
                
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
                    lvgl_lock();
                    ui_animation_stop();
                    ui_animation_play(ANIM_THUMBSUP);
                    lvgl_unlock();
                    walkingAnimActive = false;
                    audio.playEffect(SFX_NONE);
                } else {
                    lvgl_lock();
                    ui_animation_stop();
                    ui_animation_play(ANIM_WALKING);
                    lvgl_unlock();
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

// ============================================================================
// Input Handling
// ============================================================================

/**
 * Handle Button and Keyboard Input
 * Processes M5Unified button events
 */
void handleInput() {
    M5.update();
    
    // Debug button state every second
    static unsigned long lastDebug = 0;
    if (millis() - lastDebug >= 1000) {
        Serial.printf("[BTN] A=%d B=%d C=%d\n", M5.BtnA.isPressed(), M5.BtnB.isPressed(), M5.BtnC.isPressed());
        lastDebug = millis();
    }
    
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
    
    // Explicitly initialize display
    M5.Display.begin();
    Serial.println("  ✓ Display hardware initialized");
    
    M5.Display.setRotation(DISPLAY_ROTATION);
    Serial.println("  ✓ Display rotation set to landscape");
    
    M5.Display.setBrightness(currentBrightness);
    M5.Display.wakeup();
    Serial.println("  ✓ Display brightness configured");
    
    // Draw test pattern to verify display is working
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.setTextColor(TFT_GREEN);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(10, 50);
    M5.Display.println("PIP-BOY ADV");
    M5.Display.setTextSize(1);
    M5.Display.setCursor(10, 80);
    M5.Display.println("Initializing...");
    Serial.println("  ✓ Test pattern displayed");
    
    Serial.println("✓ M5Unified initialized");
    Serial.println("  Display: 240x135 @ Rotation 1");

    // ========================================================================
    // Stage 2: LVGL and UI Initialization
    // ========================================================================
    Serial.println("\n=== Stage 2: LVGL Initialization ===");
    
    lvgl_setup();
    Serial.println("  → LVGL ready for UI creation");
    
    // Initialize UI shell (creates main screen, status bar, tabs)
    Serial.println("  → Creating UI shell...");
    ui_shell_init();
    
    // Force LVGL to render immediately
    lv_timer_handler();
    lv_task_handler();
    Serial.println("  ✓ Screen refreshed");
    
    Serial.println("✓ UI shell loaded and displayed");
    Serial.println("  → Screen should be visible now!");

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
    // Stage 3.5: Optional Module Initialization
    // ========================================================================
    Serial.println("\n=== Optional Modules ===");
    
    // Initialize IR Helper (optional)
    if (irHelper.begin()) {
        Serial.println("✓ IR Helper available");
    } else {
        Serial.println("○ IR Helper not available (optional)");
    }
    
    // Initialize GPS Helper (optional)
    if (gpsHelper.begin()) {
        Serial.println("✓ GPS Helper available");
    } else {
        Serial.println("○ GPS Helper not available (optional)");
    }
    
    // Initialize Map Renderer
    mapRenderer.begin();
    Serial.println("✓ Map Renderer initialized");
    
    Serial.println("○ WiFi disabled (enable in SYSTEM tab)");

    // ========================================================================
    // Stage 4: SD Card and Configuration
    // ========================================================================
    Serial.println("\n=== Stage 4: SD Card & Configuration ===");
    
    if (!initializeSDCard()) {
        lvgl_lock();
        ui_shell_update_date("NO SD CARD");
        lvgl_unlock();
        Serial.println("⚠ Continuing without SD card...");
    } else {
        Serial.println("✓ SD card detected");
        
        // Initialize asset loading system
        if (ui_assets_init()) {
            Serial.println("✓ Asset system initialized");
        }

        // Initialize File Browser (SD dependent)
        if (fileBrowser.begin()) {
            Serial.println("✓ File Browser initialized");
        } else {
            Serial.println("○ File Browser unavailable");
        }

        // Initialize Plugin System (SD dependent)
        if (pluginSystem.begin()) {
            Serial.println("✓ Plugin System initialized");
        } else {
            Serial.println("○ Plugin System unavailable");
        }
    }

    // Initialize Module Integration Layer (after SD availability known)
    modules.init();
    Serial.println("✓ Module integration layer initialized");

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
    
    // Check if LoRa hardware is present before initializing
    if (loraHelper.isAvailable()) {
        Serial.println("✓ LoRa hardware detected - initializing...");
        
        if (loraHelper.begin()) {
            Serial.printf("✓ LoRa initialized @ %.1f MHz\n", LORA_DEFAULT_FREQ);
            lvgl_lock();
            ui_radio_update_status(true, true);
            ui_radio_update_frequency(LORA_DEFAULT_FREQ);
            ui_shell_update_lora(true, 0);
            lvgl_unlock();
            
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
            Serial.println("⚠ LoRa initialization failed - check configuration");
            lvgl_lock();
            ui_radio_update_status(false, false);
            ui_radio_add_message("LoRa init failed - Check config", false);
            lvgl_unlock();
        }
    } else {
        Serial.println("⊘ LoRa hardware not detected - skipping");
        Serial.println("  (This is normal if SX1262 hat is not installed)");
        lvgl_lock();
        ui_radio_update_status(false, false);
        ui_radio_add_message("No LoRa hardware detected", false);
        lvgl_unlock();
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
    
    // Network disabled for standalone operation
    Serial.println("WiFi/Weather/NTP: Disabled (standalone mode)");
    lvgl_lock();
    ui_shell_update_wifi(false, 0);
    ui_shell_update_date("OFFLINE");
    ui_shell_update_time("--:--");
    lvgl_unlock();
    
    // System stats monitoring task
    systemStats.startMonitorTask();
    Serial.println("✓ System stats task started");

    // ========================================================================
    // Stage 9: Launch UI
    // ========================================================================
    Serial.println("\n=== Stage 9: Final UI Setup ===");
    
    // UI shell already initialized in Stage 2 - just start animations
    // (ui_shell_start() was already called, don't call it again!)
    
    // DISABLED: Walking animation crashes LVGL when SD card assets missing
    // ui_animation_play(ANIM_WALKING);
    // walkingAnimActive = true;
    Serial.println("⊘ Animation disabled (prevents crash without SD assets)");
    
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
    static unsigned long lastDebugTime = 0;
    static unsigned long loopCount = 0;
    static unsigned long lastDisplayUpdate = 0;
    
    // Keep display awake
    unsigned long now = millis();
    if (now - lastDisplayUpdate > 100) {
        M5.Display.wakeup();
        M5.Display.setBrightness(currentBrightness);
        lastDisplayUpdate = now;
    }
    
    // Update LVGL (core UI rendering) - NO MUTEX HERE, this must run freely
    lv_timer_handler();
    lv_task_handler();
    loopCount++;
    
    // CRITICAL: Update M5 hardware state (buttons, sensors, etc)
    M5.update();
    
    // Debug: Print LVGL tick every second to confirm loop is running
    if (millis() - lastDebugTime >= 1000) {
        Serial.printf("[LOOP] LVGL running - %lu ticks/sec\n", loopCount);
        loopCount = 0;
        lastDebugTime = millis();
    }
    
    // Handle button/keyboard input
    handleInput();
    
    // Update all modules via integration layer
    modules.update();
    
    // Small delay to prevent CPU overload (5ms matches LVGL tick)
    delay(5);
}