# Pip-Boy ADV - Complete Hardware Integration Guide

## Overview
This guide covers all integrated hardware features of the M5Stack Cardputer ADV, including the official LoRa Hat (SX1262).

---

## 🔌 Hardware Components

### 1. LoRa Hat (SX1262)
**Status**: ✅ Fully Integrated

#### Pin Configuration
```cpp
LORA_MISO = 13
LORA_MOSI = 15
LORA_SCK  = 11
LORA_CS   = 12
LORA_RST  = 8
LORA_DIO1 = 3
LORA_BUSY = 46
```

#### Default Settings
- **Frequency**: 915.0 MHz (configurable: 433/868/915)
- **Bandwidth**: 125.0 kHz
- **Spreading Factor**: 7 (range: 7-12)
- **Coding Rate**: 5 (range: 5-8)
- **TX Power**: 14 dBm (range: 2-20)
- **Sync Word**: 0x12 (LoRaWAN public)

#### Usage Example
```cpp
#include "LoRaHelper.h"

// Initialize LoRa
if (lora.begin()) {
    Serial.println("LoRa ready");
}

// Send a message
lora.send("Hello World");

// Receive (in loop or task)
if (lora.available()) {
    LoRaPacket packet = lora.receive();
    Serial.print("Received: ");
    Serial.println((char*)packet.data);
    Serial.printf("RSSI: %.0f dBm, SNR: %.1f dB\n", packet.rssi, packet.snr);
}

// Change frequency
lora.setFrequency(868.0);  // EU frequency

// Adjust TX power
lora.setOutputPower(20);  // Max power

// Get statistics
LoRaStats stats = lora.getStats();
Serial.printf("RX: %lu, TX: %lu\n", stats.packets_received, stats.packets_sent);
```

#### Advanced Features
```cpp
// Channel Activity Detection
if (lora.channelFree()) {
    lora.send("Safe to transmit");
}

// Custom configuration
lora.setSpreadingFactor(12);  // Long range
lora.setBandwidth(62.5);      // Narrow bandwidth
lora.setCodingRate(8);        // Maximum error correction

// Callback for automatic packet handling
lora.setReceiveCallback([](LoRaPacket packet) {
    ui_radio_add_message((char*)packet.data, false);
});
```

---

### 2. Audio System (Speaker)
**Status**: ✅ Fully Integrated

#### Features
- Tone generation
- Sound effects library
- Volume control (0-255)
- Mute functionality
- Pip-Boy themed sounds

#### Usage Example
```cpp
#include "AudioHelper.h"

// Initialize
audio.begin();

// Basic tones
audio.playBeep();                  // Default beep
audio.playBeep(2000, 100);         // 2kHz for 100ms
audio.tone(1000, 500);             // 1kHz for 500ms

// Sound effects
audio.playButtonPress();           // Button click sound
audio.playTabSwitch();             // Tab change sound
audio.playError();                 // Error sequence
audio.playSuccess();               // Success sequence
audio.playAlert();                 // Alert sequence

// Volume control
audio.setVolume(200);              // Set volume (0-255)
audio.mute();                      // Mute all sounds
audio.unmute();                    // Restore volume

// Sequences
audio.playStartupSequence();       // Boot sound
audio.playShutdownSequence();      // Shutdown sound
audio.playRadioTune();             // Radio tuning sound

// Custom melodies
uint16_t frequencies[] = {523, 587, 659, 698};  // C, D, E, F
uint32_t durations[] = {200, 200, 200, 400};
audio.playMelody(frequencies, durations, 4);
```

#### Available Sound Effects
```cpp
SFX_BEEP_SHORT       // Quick beep
SFX_BEEP_LONG        // Long beep
SFX_ERROR            // Error indication
SFX_SUCCESS          // Success indication
SFX_BUTTON_PRESS     // Button feedback
SFX_TAB_SWITCH       // Tab navigation
SFX_RADIO_ON         // Radio activation
SFX_RADIO_OFF        // Radio deactivation
SFX_ALERT            // Alert notification
SFX_NOTIFICATION     // Message received
```

---

### 3. Sensors (IMU, Temperature, Battery)
**Status**: ✅ Fully Integrated

#### IMU (Inertial Measurement Unit)
```cpp
#include "SensorHelper.h"

// Initialize
sensors.begin();

// Read IMU data
IMUData imu = sensors.readIMU();
Serial.printf("Accel: X=%.2f Y=%.2f Z=%.2f m/s²\n", 
              imu.accelX, imu.accelY, imu.accelZ);
Serial.printf("Gyro: X=%.2f Y=%.2f Z=%.2f °/s\n",
              imu.gyroX, imu.gyroY, imu.gyroZ);
Serial.printf("Orientation: Pitch=%.1f° Roll=%.1f°\n",
              imu.pitch, imu.roll);

// Calibrate IMU (device must be still)
sensors.calibrateIMU();

// Motion detection
if (sensors.detectShake()) {
    Serial.println("Shake detected!");
}

if (sensors.detectTap()) {
    Serial.println("Tap detected!");
}

if (sensors.isMoving()) {
    Serial.println("Device is moving");
}

// Orientation
float pitch, roll, yaw;
sensors.getOrientation(pitch, roll, yaw);

if (sensors.isLevelHorizontal(5.0)) {  // Within 5 degrees
    Serial.println("Device is level");
}
```

#### Temperature Monitoring
```cpp
float temp = sensors.getTemperature();
Serial.printf("Temperature: %.1f°C\n", temp);
```

#### Battery Monitoring
```cpp
float voltage = sensors.getBatteryVoltage();
int level = sensors.getBatteryLevel();  // 0-100%
bool charging = sensors.isCharging();
float current = sensors.getBatteryCurrent();  // mA

Serial.printf("Battery: %.2fV (%d%%) %s\n", 
              voltage, level, charging ? "Charging" : "Discharging");
Serial.printf("Current: %.1f mA\n", current);

// Update UI
ui_update_battery(level);
```

---

### 4. Bluetooth
**Status**: ✅ Integrated (Classic BT)

#### Usage Example
```cpp
#include "BluetoothHelper.h"

// Initialize
if (bluetooth.begin("Pip-Boy-ADV")) {
    Serial.println("Bluetooth ready");
    Serial.println("Address: " + bluetooth.getAddress());
}

// Send data
bluetooth.println("Hello from Pip-Boy!");
bluetooth.write((uint8_t*)"Binary data", 11);

// Receive data
if (bluetooth.available()) {
    String received = bluetooth.readString();
    Serial.println("Received: " + received);
}

// Check connection
if (bluetooth.isConnected()) {
    Serial.println("Client connected");
}

// Callbacks
bluetooth.onConnect([](String clientName) {
    Serial.println("Connected: " + clientName);
    audio.playSuccess();
});

bluetooth.onDisconnect([]() {
    Serial.println("Disconnected");
    audio.playError();
});

bluetooth.onDataReceived([](String data) {
    Serial.println("Data: " + data);
});

// Discoverable mode
bluetooth.setDiscoverable(true);

// Power management
bluetooth.sleep();  // Low power
bluetooth.wake();   // Resume
```

---

### 5. Display (LVGL)
**Status**: ✅ Fully Integrated

#### Specifications
- **Resolution**: 240×135 pixels
- **Library**: LVGL 8.3.11
- **Rotation**: 1 (landscape)
- **Color Depth**: RGB565

#### Brightness Control
```cpp
M5.Display.setBrightness(128);  // 0-255
```

#### LVGL Integration
All UI is managed through the modular UI system:
- `ui_main.cpp` - Main tab interface
- `ui_radio.cpp` - LoRa radio tab
- `ui_settings.cpp` - Settings tab
- `ui_animations.cpp` - Sprite animations

---

### 6. SD Card
**Status**: ✅ Fully Integrated

#### Pin Configuration
```cpp
SD_SCK  = 40
SD_MISO = 39
SD_MOSI = 14
SD_SS   = 12
```

#### Usage
```cpp
// Already initialized in main.cpp
if (SD.exists("/config.txt")) {
    File file = SD.open("/config.txt", FILE_READ);
    // Read configuration
    file.close();
}

// Store LoRa logs
File log = SD.open("/lora_log.txt", FILE_APPEND);
log.println("Packet received at " + String(millis()));
log.close();

// Load animation assets
lv_img_dsc_t* image = ui_assets_load_image("/assets/Images/custom.png");
```

---

### 7. WiFi 6
**Status**: ✅ Fully Integrated

#### Features
- WiFi 802.11ax (Wi-Fi 6)
- NTP time synchronization
- Weather API integration
- OTA updates (future)

#### Usage
```cpp
// Configured via config.txt on SD card
WIFI_SSID=YourNetwork
WIFI_PASSWORD=YourPassword

// Automatically connects in wifiTask
// Status shown in Settings tab
ui_settings_update_wifi_status(WiFi.isConnected(), WIFI_SSID.c_str());
```

---

### 8. Keyboard & Buttons
**Status**: ✅ Partial Integration

#### Side Buttons
```cpp
// BtnA - Toggle animations
if (M5.BtnA.wasPressed()) {
    // Switch between walking/thumbsup
}

// BtnB - Switch tabs
if (M5.BtnB.wasPressed()) {
    // Cycle through STAT/RADIO/ITEMS/DATA
}

// BtnC - Adjust brightness
if (M5.BtnC.wasPressed()) {
    // Increase brightness
}
```

#### Full Keyboard (Future Enhancement)
The Cardputer ADV's QWERTY keyboard requires additional integration work. Currently using side buttons for primary navigation.

---

## 🎮 Complete Hardware API Reference

### LoRa Module
```cpp
lora.begin()                                    // Initialize with defaults
lora.begin(freq, bw, sf, cr, power)            // Custom initialization
lora.send(data, length)                        // Send bytes
lora.send(string)                              // Send string
lora.available()                               // Check for data
lora.receive()                                 // Get packet
lora.setFrequency(freq)                        // Set frequency
lora.setOutputPower(power)                     // Set TX power
lora.getStats()                                // Get statistics
```

### Audio Module
```cpp
audio.begin()                                  // Initialize
audio.playBeep(freq, duration)                 // Play tone
audio.playEffect(effect)                       // Play sound effect
audio.setVolume(volume)                        // Set volume
audio.mute() / unmute()                        // Mute control
```

### Sensor Module
```cpp
sensors.begin()                                // Initialize
sensors.readIMU()                              // Get IMU data
sensors.calibrateIMU()                         // Calibrate
sensors.detectShake()                          // Shake detection
sensors.getBatteryLevel()                      // Battery %
sensors.getTemperature()                       // Temperature
```

### Bluetooth Module
```cpp
bluetooth.begin(name)                          // Initialize
bluetooth.send(data)                           // Send data
bluetooth.available()                          // Check data
bluetooth.read()                               // Read data
bluetooth.onConnect(callback)                  // Connection callback
```

---

## 🔧 FreeRTOS Tasks

### Active Tasks
1. **wifiTask** - WiFi connection and NTP sync
2. **weatherTask** - Weather data updates
3. **batteryTask** - Battery monitoring
4. **loraReceiveTask** - LoRa packet reception
5. **sensorMonitorTask** - IMU and gesture detection
6. **lv_tick_task** - LVGL timer handler

### Task Pinning
- Core 0: Main loop, LVGL handler
- Core 1: Background tasks (WiFi, sensors, LoRa)

---

## 📊 Resource Usage

### Memory
- **PSRAM**: Enabled for large allocations
- **LVGL Buffers**: 2× 240×10 pixels
- **LoRa Buffer**: 512 bytes
- **Task Stacks**: 4KB each

### Power Consumption
- **Idle**: ~80mA
- **WiFi Active**: ~150mA
- **LoRa TX (20dBm)**: ~120mA
- **LoRa RX**: ~15mA
- **Display**: ~40mA (varies with brightness)

---

## 🚀 Getting Started

### 1. Hardware Setup
- Attach LoRa Hat to Cardputer ADV GPIO expansion
- Insert SD card with config.txt and assets
- Power on device

### 2. First Boot
- Device initializes all hardware
- Connects to WiFi (from config)
- Starts LoRa receiver
- Displays Stats tab

### 3. Using LoRa
- Navigate to RADIO tab (BtnB)
- Click "Send Test" to transmit
- Received packets appear automatically
- RSSI/SNR shown for each packet

### 4. Gestures
- **Shake device** - Toggle animations
- **BtnA** - Animation control
- **BtnB** - Tab navigation
- **BtnC** - Brightness adjustment

---

## 🔍 Troubleshooting

### LoRa Not Initializing
**Symptoms**: "LoRa init failed" message

**Solutions**:
1. Check LoRa Hat is securely attached
2. Verify SPI pin connections
3. Try different frequency (433/868/915 MHz)
4. Check antenna is connected
5. Monitor serial output for error codes

### No Received Packets
**Symptoms**: Send test works, but no packets received

**Solutions**:
1. Ensure two devices on same frequency
2. Check spreading factor matches
3. Verify bandwidth setting
4. Check sync word (0x12 default)
5. Increase TX power if distance is large

### IMU Not Responding
**Solutions**:
1. Run `sensors.calibrateIMU()` when device is still
2. Check sensor info with `sensors.printSensorInfo()`
3. Ensure M5Unified is properly initialized

### Bluetooth Connection Issues
**Solutions**:
1. Check Bluetooth is enabled in code
2. Verify device name is set
3. Ensure client device is paired
4. Check serial output for errors

---

## 📡 LoRa Communication Examples

### Point-to-Point
```cpp
// Device A
lora.send("Ping");

// Device B (in task or loop)
if (lora.available()) {
    LoRaPacket packet = lora.receive();
    if (strcmp((char*)packet.data, "Ping") == 0) {
        lora.send("Pong");
    }
}
```

### Broadcast
```cpp
// Beacon every 10 seconds
static unsigned long lastBeacon = 0;
if (millis() - lastBeacon > 10000) {
    lora.send("BEACON:Pip-Boy-ADV");
    lastBeacon = millis();
}
```

### JSON Messages
```cpp
// Send structured data
const char* json = "{\"temp\":25.5,\"battery\":85}";
lora.sendJson(json);

// Receive and parse
if (lora.available()) {
    LoRaPacket packet = lora.receive();
    DynamicJsonDocument doc(256);
    deserializeJson(doc, (char*)packet.data);
    float temp = doc["temp"];
    int battery = doc["battery"];
}
```

---

## 🎯 Future Enhancements

### LoRa Features
- [ ] Frequency hopping
- [ ] Encryption (AES)
- [ ] Meshtastic protocol
- [ ] APRS integration
- [ ] Packet logging to SD
- [ ] Channel scanning

### Sensor Features
- [ ] Step counter
- [ ] Fall detection
- [ ] Activity recognition
- [ ] Compass (magnetometer)
- [ ] Gesture commands

### Bluetooth Features
- [ ] BLE mode
- [ ] Device scanning
- [ ] File transfer
- [ ] Remote control

### Audio Features
- [ ] WAV playback from SD
- [ ] RTTTL melodies
- [ ] Voice alerts
- [ ] Morse code

---

## 📝 Configuration Files

### config.txt (SD Card)
```
WIFI_SSID=YourNetwork
WIFI_PASSWORD=YourPassword
TIME_ZONE=EST5EDT
API_KEY=weatherapi_key
LOCATION=YourCity
LORA_FREQ=915.0
LORA_POWER=14
LORA_SF=7
```

### Future: lora_config.json
```json
{
  "frequency": 915.0,
  "bandwidth": 125.0,
  "spreading_factor": 7,
  "coding_rate": 5,
  "tx_power": 14,
  "sync_word": 18,
  "enable_crc": true,
  "auto_receive": true
}
```

---

## 🏆 Complete Feature Matrix

| Feature | Status | Module | Notes |
|---------|--------|--------|-------|
| LoRa SX1262 | ✅ | LoRaHelper | Full send/receive |
| Audio/Speaker | ✅ | AudioHelper | Tones & effects |
| IMU Sensors | ✅ | SensorHelper | Accel/Gyro/Temp |
| Battery Monitor | ✅ | SensorHelper | Voltage/Level |
| Bluetooth Classic | ✅ | BluetoothHelper | Serial comms |
| WiFi 6 | ✅ | WiFiHelper | NTP/Weather |
| SD Card | ✅ | ui_assets | Config/Assets |
| LVGL Display | ✅ | ui_main | 240×135 UI |
| Side Buttons | ✅ | main.cpp | A/B/C control |
| QWERTY Keyboard | 🔧 | Future | Partial |
| BLE | 🔧 | Future | Classic only |
| Magnetometer | ❌ | N/A | Not present |

✅ Fully Implemented | 🔧 Partial/Future | ❌ Not Available

---

**Last Updated**: January 21, 2026  
**Firmware Version**: Pip-Boy ADV v2.0  
**Platform**: M5Stack Cardputer ADV (ESP32-S3)
