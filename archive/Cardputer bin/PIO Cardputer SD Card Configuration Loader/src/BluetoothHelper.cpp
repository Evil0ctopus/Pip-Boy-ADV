#include "BluetoothHelper.h"

// Global instance
BluetoothHelper bluetooth;

BluetoothHelper::BluetoothHelper() : 
    btSerial(nullptr),
    initialized(false),
    enabled(false),
    mode(BT_MODE_OFF),
    deviceName("Pip-Boy-ADV"),
    discoverable(true),
    connectCallback(nullptr),
    disconnectCallback(nullptr),
    dataCallback(nullptr) {
}

bool BluetoothHelper::begin(const char *deviceName) {
    if (initialized) {
        Serial.println("Bluetooth already initialized");
        return true;
    }
    
    Serial.println("Initializing Bluetooth Classic...");
    
    this->deviceName = String(deviceName);
    
    // Create Bluetooth Serial instance
    btSerial = new BluetoothSerial();
    
    if (!btSerial->begin(deviceName)) {
        Serial.println("Bluetooth initialization failed!");
        delete btSerial;
        btSerial = nullptr;
        return false;
    }
    
    initialized = true;
    enabled = true;
    mode = BT_MODE_CLASSIC;
    
    Serial.print("Bluetooth initialized as: ");
    Serial.println(deviceName);
    Serial.print("Address: ");
    Serial.println(getAddress());
    
    return true;
}

bool BluetoothHelper::beginBLE(const char *deviceName) {
    // BLE initialization - future implementation
    // Would use ESP32 BLE libraries
    Serial.println("BLE mode not yet implemented");
    return false;
}

void BluetoothHelper::end() {
    if (!initialized) return;
    
    if (btSerial) {
        btSerial->end();
        delete btSerial;
        btSerial = nullptr;
    }
    
    initialized = false;
    enabled = false;
    mode = BT_MODE_OFF;
    
    Serial.println("Bluetooth disabled");
}

bool BluetoothHelper::isConnected() {
    if (!initialized || !btSerial) return false;
    return btSerial->hasClient();
}

bool BluetoothHelper::hasClient() {
    return isConnected();
}

String BluetoothHelper::getClientName() {
    if (!initialized || !btSerial) return "";
    
    // Get connected client name - API dependent
    return "Unknown";  // BluetoothSerial doesn't expose client name directly
}

void BluetoothHelper::print(const char *str) {
    if (!initialized || !btSerial) return;
    btSerial->print(str);
}

void BluetoothHelper::println(const char *str) {
    if (!initialized || !btSerial) return;
    btSerial->println(str);
}

void BluetoothHelper::write(uint8_t byte) {
    if (!initialized || !btSerial) return;
    btSerial->write(byte);
}

void BluetoothHelper::write(const uint8_t *buffer, size_t size) {
    if (!initialized || !btSerial) return;
    btSerial->write(buffer, size);
}

int BluetoothHelper::available() {
    if (!initialized || !btSerial) return 0;
    return btSerial->available();
}

int BluetoothHelper::read() {
    if (!initialized || !btSerial) return -1;
    return btSerial->read();
}

int BluetoothHelper::readBytes(uint8_t *buffer, size_t length) {
    if (!initialized || !btSerial) return 0;
    return btSerial->readBytes(buffer, length);
}

String BluetoothHelper::readString() {
    if (!initialized || !btSerial) return "";
    return btSerial->readString();
}

String BluetoothHelper::readStringUntil(char terminator) {
    if (!initialized || !btSerial) return "";
    return btSerial->readStringUntil(terminator);
}

void BluetoothHelper::setDiscoverable(bool discoverable) {
    this->discoverable = discoverable;
    
    if (btSerial) {
        btSerial->enableSSP();  // Simple Secure Pairing
    }
}

bool BluetoothHelper::isDiscoverable() {
    return discoverable;
}

void BluetoothHelper::setDeviceName(const char *name) {
    deviceName = String(name);
    
    // Restart Bluetooth with new name
    if (initialized) {
        end();
        begin(name);
    }
}

String BluetoothHelper::getDeviceName() {
    return deviceName;
}

String BluetoothHelper::getAddress() {
    if (!initialized || !btSerial) return "";
    
    // Get ESP32 Bluetooth MAC address
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);
    
    char macStr[18];
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    
    return String(macStr);
}

bool BluetoothHelper::startScan(uint32_t duration) {
    // Bluetooth scan - future implementation
    Serial.println("Bluetooth scan not yet implemented");
    return false;
}

void BluetoothHelper::stopScan() {
    // Stop scan
}

int BluetoothHelper::getScanResults(BTDevice *devices, int maxDevices) {
    // Return scanned devices
    return 0;
}

bool BluetoothHelper::pair(const char *address) {
    // Pairing - future implementation
    Serial.println("Bluetooth pairing not yet implemented");
    return false;
}

bool BluetoothHelper::unpair(const char *address) {
    // Unpairing
    return false;
}

bool BluetoothHelper::isPaired(const char *address) {
    // Check if device is paired
    return false;
}

int BluetoothHelper::getPairedDevices(BTDevice *devices, int maxDevices) {
    // Get list of paired devices
    return 0;
}

bool BluetoothHelper::isInitialized() {
    return initialized;
}

bool BluetoothHelper::isEnabled() {
    return enabled;
}

BTMode BluetoothHelper::getMode() {
    return mode;
}

void BluetoothHelper::onConnect(void (*callback)(String clientName)) {
    connectCallback = callback;
}

void BluetoothHelper::onDisconnect(void (*callback)()) {
    disconnectCallback = callback;
}

void BluetoothHelper::onDataReceived(void (*callback)(String data)) {
    dataCallback = callback;
}

void BluetoothHelper::sleep() {
    if (!initialized) return;
    
    // Put Bluetooth in low power mode
    // Implementation depends on ESP32 BT API
    enabled = false;
}

void BluetoothHelper::wake() {
    if (!initialized) return;
    
    // Wake Bluetooth from sleep
    enabled = true;
}

void BluetoothHelper::handleEvents() {
    if (!initialized || !btSerial) return;
    
    // Check for incoming data
    if (available() > 0 && dataCallback) {
        String data = readString();
        dataCallback(data);
    }
    
    // Check connection status changes
    static bool wasConnected = false;
    bool nowConnected = isConnected();
    
    if (nowConnected && !wasConnected && connectCallback) {
        connectCallback(getClientName());
    } else if (!nowConnected && wasConnected && disconnectCallback) {
        disconnectCallback();
    }
    
    wasConnected = nowConnected;
}
