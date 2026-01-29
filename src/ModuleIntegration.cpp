#include "ModuleIntegration.h"
#include "ui_shell.h"

// External module instances (defined in main.cpp)
extern IRHelper irHelper;
extern GPSHelper gpsHelper;
extern FileBrowser fileBrowser;
extern TerminalHelper terminalHelper;
extern WiFiManager wifiManager;
extern MapRenderer mapRenderer;
extern PluginSystem pluginSystem;
extern LoRaHelper loraHelper;

ModuleIntegration::ModuleIntegration() :
    _lastGPSUpdate(0),
    _lastLoRaUpdate(0),
    _updateInterval(100) {  // 100ms update interval
    memset(&_status, 0, sizeof(ModuleStatus));
}

void ModuleIntegration::init() {
    Serial.println("[ModuleIntegration] Initializing integration layer...");
    
    // Check module availability
    _checkModuleStatus();
    
    Serial.println("[ModuleIntegration] Integration layer ready");
}

void ModuleIntegration::update() {
    uint32_t now = millis();
    
    // Update GPS at regular interval
    if (now - _lastGPSUpdate >= _updateInterval) {
        _updateGPS();
        _lastGPSUpdate = now;
    }
    
    // Update LoRa (check for incoming packets)
    if (now - _lastLoRaUpdate >= _updateInterval) {
        _updateLoRa();
        _lastLoRaUpdate = now;
    }
}

ModuleStatus ModuleIntegration::getStatus() {
    return _status;
}

bool ModuleIntegration::isModuleAvailable(const char* moduleName) {
    if (strcmp(moduleName, "ir") == 0) return _status.ir_available;
    if (strcmp(moduleName, "gps") == 0) return _status.gps_available;
    if (strcmp(moduleName, "lora") == 0) return _status.lora_available;
    if (strcmp(moduleName, "wifi") == 0) return _status.wifi_available;
    if (strcmp(moduleName, "sd") == 0) return _status.sd_available;
    if (strcmp(moduleName, "map") == 0) return _status.map_available;
    if (strcmp(moduleName, "plugins") == 0) return _status.plugins_available;
    if (strcmp(moduleName, "terminal") == 0) return _status.terminal_available;
    return false;
}

// ============================================================================
// Event Handlers
// ============================================================================

void ModuleIntegration::onLoRaSend(const char* message) {
    if (!_status.lora_available) return;
    
    Serial.printf("[ModuleIntegration] Sending LoRa message: %s\n", message);
    loraHelper.send(message);
}

void ModuleIntegration::onLoRaFrequencyChange(float freq) {
    if (!_status.lora_available) return;
    
    Serial.printf("[ModuleIntegration] Changing LoRa frequency to %.1f MHz\n", freq);
    loraHelper.setFrequency(freq);
}

void ModuleIntegration::onIRCapture() {
    if (!_status.ir_available) return;
    
    Serial.println("[ModuleIntegration] Starting IR capture...");
    irHelper.startCapture();
}

void ModuleIntegration::onIRTransmit(const char* signalName) {
    if (!_status.ir_available) return;
    
    Serial.printf("[ModuleIntegration] Transmitting IR signal: %s\n", signalName);
    IRSignal signal;
    if (irHelper.loadSignal(&signal, signalName)) {
        irHelper.transmit(&signal);
    }
}

void ModuleIntegration::onWiFiScan() {
    if (!_status.wifi_available) return;
    
    Serial.println("[ModuleIntegration] Starting WiFi scan...");
    wifiManager.startScan();
}

void ModuleIntegration::onWiFiConnect(const char* ssid, const char* password) {
    if (!_status.wifi_available) return;
    
    Serial.printf("[ModuleIntegration] Connecting to WiFi: %s\n", ssid);
    wifiManager.connect(ssid, password);
}

void ModuleIntegration::onMapZoom(int delta) {
    if (!_status.map_available) return;
    
    // MapRenderer doesn't expose zoom level getter, skip for now
    // mapRenderer.setZoom(currentZoom + delta);
}

void ModuleIntegration::onMapPan(int dx, int dy) {
    if (!_status.map_available) return;
    
    // MapRenderer doesn't expose setPanOffset, skip for now
    // mapRenderer.setPan(dx, dy);
}

void ModuleIntegration::onTerminalCommand(const char* command) {
    if (!_status.terminal_available) return;
    
    Serial.printf("[ModuleIntegration] Executing terminal command: %s\n", command);
    // Terminal doesn't expose executeCommand publicly, skip for now
}

void ModuleIntegration::onFileBrowserNavigate(const char* path) {
    if (!_status.sd_available) return;
    
    Serial.printf("[ModuleIntegration] Navigating to: %s\n", path);
    fileBrowser.changeDirectory(path);
}

// ============================================================================
// Data Accessors
// ============================================================================

LoRaStats ModuleIntegration::getLoRaStats() {
    if (!_status.lora_available) {
        LoRaStats empty;
        memset(&empty, 0, sizeof(LoRaStats));
        return empty;
    }
    return loraHelper.getStats();
}

GPSData* ModuleIntegration::getGPSData() {
    if (!_status.gps_available) return nullptr;
    return gpsHelper.getData();
}

std::vector<LoRaPacket>* ModuleIntegration::getLoRaPackets() {
    if (!_status.lora_available) return nullptr;
    // Note: This would need a getPacketLog() method added to LoRaHelper
    return nullptr;  // Placeholder
}

std::vector<WiFiNetwork>* ModuleIntegration::getWiFiNetworks() {
    if (!_status.wifi_available) return nullptr;
    // WiFiManager.getNetworks() returns by value, cannot return pointer safely
    return nullptr;
}

std::vector<FileEntry>* ModuleIntegration::getCurrentDirectory() {
    if (!_status.sd_available) return nullptr;
    // FileBrowser uses listDirectory() which returns a new vector
    // Would need refactoring to return pointer
    return nullptr;
}

// ============================================================================
// Private Helpers
// ============================================================================

void ModuleIntegration::_updateGPS() {
    if (!_status.gps_available) return;
    
    gpsHelper.update();
    
    // Update map with GPS position if valid
    if (gpsHelper.hasValidFix()) {
        GPSData* data = gpsHelper.getData();
        mapRenderer.setMarkerPosition(data->latitude, data->longitude);
        mapRenderer.setMarkerVisible(true);
    }
}

void ModuleIntegration::_updateLoRa() {
    if (!_status.lora_available) return;
    
    // LoRa operates in interrupt mode
    // Packets are handled via callback
    // No explicit polling needed
}

void ModuleIntegration::_checkModuleStatus() {
    _status.ir_available = irHelper.isAvailable();
    _status.gps_available = gpsHelper.isAvailable();
    _status.lora_available = loraHelper.isInitialized();
    _status.wifi_available = true;  // WiFi always available on ESP32-S3
    _status.sd_available = true;  // Check SD.begin() status
    _status.map_available = true;   // Map renderer always available
    _status.plugins_available = true;  // Always available
    _status.terminal_available = true; // Terminal always available
    
    Serial.println("[ModuleIntegration] Module Status:");
    Serial.printf("  IR:       %s\n", _status.ir_available ? "✓" : "○");
    Serial.printf("  GPS:      %s\n", _status.gps_available ? "✓" : "○");
    Serial.printf("  LoRa:     %s\n", _status.lora_available ? "✓" : "○");
    Serial.printf("  WiFi:     %s\n", _status.wifi_available ? "✓" : "○");
    Serial.printf("  SD Card:  %s\n", _status.sd_available ? "✓" : "○");
    Serial.printf("  Map:      %s\n", _status.map_available ? "✓" : "○");
    Serial.printf("  Plugins:  %s\n", _status.plugins_available ? "✓" : "○");
    Serial.printf("  Terminal: %s\n", _status.terminal_available ? "✓" : "○");
}
