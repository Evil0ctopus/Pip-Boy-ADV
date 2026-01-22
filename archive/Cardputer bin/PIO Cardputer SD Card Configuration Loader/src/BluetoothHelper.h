#ifndef BLUETOOTH_HELPER_H
#define BLUETOOTH_HELPER_H

#include <Arduino.h>
#include <BluetoothSerial.h>

// Bluetooth modes
enum BTMode {
    BT_MODE_OFF = 0,
    BT_MODE_CLASSIC,
    BT_MODE_BLE,
    BT_MODE_DUAL
};

// Bluetooth device info
struct BTDevice {
    String name;
    String address;
    int8_t rssi;
    bool paired;
};

class BluetoothHelper {
public:
    BluetoothHelper();
    
    // Initialization
    bool begin(const char *deviceName = "Pip-Boy-ADV");
    bool beginBLE(const char *deviceName = "Pip-Boy-ADV");
    void end();
    
    // Classic Bluetooth (Serial)
    bool isConnected();
    bool hasClient();
    String getClientName();
    
    // Send/Receive
    void print(const char *str);
    void println(const char *str);
    void write(uint8_t byte);
    void write(const uint8_t *buffer, size_t size);
    
    int available();
    int read();
    int readBytes(uint8_t *buffer, size_t length);
    String readString();
    String readStringUntil(char terminator);
    
    // Device management
    void setDiscoverable(bool discoverable);
    bool isDiscoverable();
    void setDeviceName(const char *name);
    String getDeviceName();
    String getAddress();
    
    // Scanning (for master mode)
    bool startScan(uint32_t duration = 10000);
    void stopScan();
    int getScanResults(BTDevice *devices, int maxDevices);
    
    // Pairing
    bool pair(const char *address);
    bool unpair(const char *address);
    bool isPaired(const char *address);
    int getPairedDevices(BTDevice *devices, int maxDevices);
    
    // Status
    bool isInitialized();
    bool isEnabled();
    BTMode getMode();
    
    // Callbacks
    void onConnect(void (*callback)(String clientName));
    void onDisconnect(void (*callback)());
    void onDataReceived(void (*callback)(String data));
    
    // Power management
    void sleep();
    void wake();
    
private:
    BluetoothSerial *btSerial;
    
    bool initialized;
    bool enabled;
    BTMode mode;
    String deviceName;
    bool discoverable;
    
    void (*connectCallback)(String);
    void (*disconnectCallback)();
    void (*dataCallback)(String);
    
    // Internal helpers
    void handleEvents();
};

// Global Bluetooth helper instance
extern BluetoothHelper bluetooth;

#endif // BLUETOOTH_HELPER_H
