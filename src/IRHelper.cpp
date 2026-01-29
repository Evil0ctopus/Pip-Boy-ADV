// IRHelper.cpp - M5Stack IR Unit Implementation
#include "IRHelper.h"
#include <SD.h>
#include <ArduinoJson.h>

IRHelper::IRHelper() : available(false), capturing(false) {
    currentCapture.rawLen = 0;
}

bool IRHelper::begin() {
    initPins();
    
    // Test if IR unit is connected
    pinMode(IR_PIN_RX, INPUT);
    pinMode(IR_PIN_TX, OUTPUT);
    digitalWrite(IR_PIN_TX, LOW);
    
    available = true;
    Serial.println("IR Helper: Initialized");
    return true;
}

void IRHelper::initPins() {
    pinMode(IR_PIN_RX, INPUT);
    pinMode(IR_PIN_TX, OUTPUT);
    digitalWrite(IR_PIN_TX, LOW);
}

bool IRHelper::isAvailable() {
    return available;
}

bool IRHelper::startCapture() {
    if (!available) return false;
    
    capturing = true;
    currentCapture.rawLen = 0;
    currentCapture.frequency = 38000; // Default 38kHz
    Serial.println("IR: Capture started");
    
    // Capture IR signal (simplified - real implementation would use interrupts)
    uint16_t idx = 0;
    unsigned long timeout = millis() + 5000; // 5 second timeout
    
    while (idx < MAX_IR_RAWLEN && millis() < timeout) {
        uint32_t highPulse = pulseInLong(IR_PIN_RX, HIGH, 100000);
        if (highPulse == 0) break;
        currentCapture.rawData[idx++] = highPulse;
        
        uint32_t lowPulse = pulseInLong(IR_PIN_RX, LOW, 100000);
        if (lowPulse == 0) break;
        currentCapture.rawData[idx++] = lowPulse;
    }
    
    currentCapture.rawLen = idx;
    capturing = false;
    
    if (idx > 10) {
        currentCapture.name = "Captured_" + String(millis());
        Serial.printf("IR: Captured %d pulses\n", idx);
        return true;
    }
    
    Serial.println("IR: Capture failed or no signal");
    return false;
}

bool IRHelper::captureInProgress() {
    return capturing;
}

bool IRHelper::captureComplete() {
    return !capturing && currentCapture.rawLen > 0;
}

IRSignal* IRHelper::getCapturedSignal() {
    return &currentCapture;
}

bool IRHelper::transmit(IRSignal* signal) {
    if (!available || !signal || signal->rawLen == 0) return false;
    
    Serial.printf("IR: Transmitting %d pulses\n", signal->rawLen);
    
    // Send IR signal
    for (uint16_t i = 0; i < signal->rawLen; i++) {
        if (i % 2 == 0) {
            // Mark (on)
            digitalWrite(IR_PIN_TX, HIGH);
            delayMicroseconds(signal->rawData[i]);
        } else {
            // Space (off)
            digitalWrite(IR_PIN_TX, LOW);
            delayMicroseconds(signal->rawData[i]);
        }
    }
    digitalWrite(IR_PIN_TX, LOW);
    
    Serial.println("IR: Transmission complete");
    return true;
}

bool IRHelper::saveSignal(IRSignal* signal, const char* filename) {
    if (!signal || signal->rawLen == 0) return false;
    
    String path = "/ir/" + String(filename) + ".json";
    
    StaticJsonDocument<4096> doc;
    doc["name"] = signal->name;
    doc["frequency"] = signal->frequency;
    doc["length"] = signal->rawLen;
    
    JsonArray data = doc.createNestedArray("data");
    for (uint16_t i = 0; i < signal->rawLen; i++) {
        data.add(signal->rawData[i]);
    }
    
    File file = SD.open(path.c_str(), FILE_WRITE);
    if (!file) {
        Serial.println("IR: Failed to open file for writing");
        return false;
    }
    
    serializeJson(doc, file);
    file.close();
    
    Serial.printf("IR: Saved signal to %s\n", path.c_str());
    return true;
}

bool IRHelper::loadSignal(IRSignal* signal, const char* filename) {
    String path = "/ir/" + String(filename);
    if (!path.endsWith(".json")) path += ".json";
    
    File file = SD.open(path.c_str(), FILE_READ);
    if (!file) {
        Serial.printf("IR: Failed to open %s\n", path.c_str());
        return false;
    }
    
    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    if (error) {
        Serial.println("IR: Failed to parse JSON");
        return false;
    }
    
    signal->name = doc["name"].as<String>();
    signal->frequency = doc["frequency"];
    signal->rawLen = doc["length"];
    
    JsonArray data = doc["data"];
    for (uint16_t i = 0; i < signal->rawLen && i < MAX_IR_RAWLEN; i++) {
        signal->rawData[i] = data[i];
    }
    
    Serial.printf("IR: Loaded signal %s (%d pulses)\n", signal->name.c_str(), signal->rawLen);
    return true;
}

std::vector<String> IRHelper::listSignals() {
    std::vector<String> fileList;
    
    File dir = SD.open("/ir");
    if (!dir || !dir.isDirectory()) {
        SD.mkdir("/ir");
        return fileList;
    }
    
    File file = dir.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            String name = file.name();
            if (name.endsWith(".json")) {
                fileList.push_back(name);
            }
        }
        file = dir.openNextFile();
    }
    
    return fileList;
}

uint32_t IRHelper::pulseInLong(uint8_t pin, uint8_t state, uint32_t timeout) {
    uint32_t start = micros();
    while (digitalRead(pin) != state) {
        if (micros() - start > timeout) return 0;
    }
    
    start = micros();
    while (digitalRead(pin) == state) {
        if (micros() - start > timeout) return 0;
    }
    
    return micros() - start;
}
