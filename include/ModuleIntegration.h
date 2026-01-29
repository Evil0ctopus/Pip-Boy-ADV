// ============================================================================
// Module Integration - Central UI/Backend Integration Layer
// ============================================================================
// Coordinates all subsystem modules with the UI shell
// Handles non-blocking updates for GPS, LoRa, IR, WiFi, etc.
// Provides a unified interface for module initialization and event handling
// ============================================================================

#ifndef MODULE_INTEGRATION_H
#define MODULE_INTEGRATION_H

#include <Arduino.h>
#include "IRHelper.h"
#include "GPSHelper.h"
#include "FileBrowser.h"
#include "TerminalHelper.h"
#include "WiFiManager.h"
#include "MapRenderer.h"
#include "PluginSystem.h"
#include "LoRaHelper.h"

// Module initialization flags
struct ModuleStatus {
    bool ir_available;
    bool gps_available;
    bool lora_available;
    bool wifi_available;
    bool sd_available;
    bool map_available;
    bool plugins_available;
    bool terminal_available;
};

class ModuleIntegration {
public:
    ModuleIntegration();
    
    // Initialization
    void init();
    
    // Main loop update - calls all module update functions
    void update();
    
    // Module status
    ModuleStatus getStatus();
    bool isModuleAvailable(const char* moduleName);
    
    // Event handlers (called from UI)
    void onLoRaSend(const char* message);
    void onLoRaFrequencyChange(float freq);
    void onIRCapture();
    void onIRTransmit(const char* signalName);
    void onWiFiScan();
    void onWiFiConnect(const char* ssid, const char* password);
    void onMapZoom(int delta);
    void onMapPan(int dx, int dy);
    void onTerminalCommand(const char* command);
    void onFileBrowserNavigate(const char* path);
    
    // Data accessors for UI
    LoRaStats getLoRaStats();
    GPSData* getGPSData();
    std::vector<LoRaPacket>* getLoRaPackets();
    std::vector<WiFiNetwork>* getWiFiNetworks();
    std::vector<FileEntry>* getCurrentDirectory();
    
private:
    ModuleStatus _status;
    uint32_t _lastGPSUpdate;
    uint32_t _lastLoRaUpdate;
    uint32_t _updateInterval;
    
    // Helper functions
    void _updateGPS();
    void _updateLoRa();
    void _checkModuleStatus();
};

// Global integration instance
extern ModuleIntegration modules;

#endif // MODULE_INTEGRATION_H
