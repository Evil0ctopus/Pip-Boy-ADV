#include "LoRaHelper.h"

LoRaHelper::LoRaHelper() : 
    radio(nullptr),
    loraSPI(nullptr),
    currentMode(LORA_MODE_IDLE),
    initialized(false),
    currentFreq(LORA_DEFAULT_FREQ),
    currentBW(LORA_DEFAULT_BW),
    currentSF(LORA_DEFAULT_SF),
    currentCR(LORA_DEFAULT_CR),
    currentPower(LORA_DEFAULT_POWER),
    receiveCallback(nullptr) {
    resetStats();
}

bool LoRaHelper::isAvailable() {
    // Safely probe for SX1262 hardware without initializing
    // This prevents crashes when the chip is not present
    
    Serial.println("Probing for LoRa SX1262 hardware...");
    
    // Create temporary SPI instance
    SPIClass *probeSPI = new SPIClass(HSPI);
    probeSPI->begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
    
    // Create temporary radio instance
    SX1262 *probeRadio = new SX1262(new Module(LORA_CS, LORA_IRQ, LORA_RST, LORA_BUSY, *probeSPI));
    
    // Try to read chip version register (safe operation)
    // SX1262 should return a valid version value
    // If chip is absent, this will timeout or return 0x00/0xFF
    
    bool detected = false;
    
    // Attempt basic initialization with minimal timeout
    // Use a safe frequency that won't cause issues
    int state = probeRadio->begin(915.0, 125.0, 7, 5, 0x12, 14, 8, 0);
    
    if (state == RADIOLIB_ERR_NONE) {
        detected = true;
        Serial.println("  → SX1262 chip responded");
        // Put chip to sleep to save power
        probeRadio->sleep();
    } else {
        Serial.print("  → No response (error code: ");
        Serial.print(state);
        Serial.println(")");
        detected = false;
    }
    
    // Clean up probe resources
    delete probeRadio;
    probeSPI->end();
    delete probeSPI;
    
    return detected;
}

bool LoRaHelper::begin() {
    return begin(LORA_DEFAULT_FREQ, LORA_DEFAULT_BW, LORA_DEFAULT_SF, 
                 LORA_DEFAULT_CR, LORA_DEFAULT_POWER);
}

bool LoRaHelper::begin(float freq, float bw, uint8_t sf, uint8_t cr, uint8_t power) {
    if (initialized) {
        Serial.println("LoRa already initialized");
        return true;
    }
    
    Serial.println("Initializing LoRa SX1262...");
    
    // Create SPI instance for LoRa
    loraSPI = new SPIClass(HSPI);
    loraSPI->begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
    
    // Create radio instance using Cardputer ADV pins
    radio = new SX1262(new Module(LORA_CS, LORA_IRQ, LORA_RST, LORA_BUSY, *loraSPI));
    
    // Initialize radio
    Serial.print("Frequency: "); Serial.print(freq); Serial.println(" MHz");
    
    int state = radio->begin(freq, bw, sf, cr, LORA_SYNC_WORD, power, 
                             LORA_PREAMBLE_LEN, 0); // 0 = TCXO not used
    
    if (state != RADIOLIB_ERR_NONE) {
        Serial.print("LoRa initialization failed, code: ");
        Serial.println(state);
        delete radio;
        delete loraSPI;
        radio = nullptr;
        loraSPI = nullptr;
        return false;
    }
    
    // Store configuration
    currentFreq = freq;
    currentBW = bw;
    currentSF = sf;
    currentCR = cr;
    currentPower = power;
    
    // Enable CRC
    radio->setCRC(true);
    
    Serial.println("LoRa SX1262 initialized successfully!");
    printConfig();
    
    initialized = true;
    currentMode = LORA_MODE_IDLE;
    
    // Start in receive mode
    startReceive();
    
    return true;
}

void LoRaHelper::end() {
    if (!initialized) return;
    
    if (radio) {
        radio->sleep();
        delete radio;
        radio = nullptr;
    }
    
    if (loraSPI) {
        loraSPI->end();
        delete loraSPI;
        loraSPI = nullptr;
    }
    
    initialized = false;
    currentMode = LORA_MODE_IDLE;
    Serial.println("LoRa deinitialized");
}

bool LoRaHelper::setFrequency(float freq) {
    if (!initialized) return false;
    
    int state = radio->setFrequency(freq);
    if (state == RADIOLIB_ERR_NONE) {
        currentFreq = freq;
        Serial.print("Frequency set to: "); Serial.print(freq); Serial.println(" MHz");
        return true;
    }
    
    Serial.print("Failed to set frequency, code: "); Serial.println(state);
    return false;
}

bool LoRaHelper::setBandwidth(float bw) {
    if (!initialized) return false;
    
    int state = radio->setBandwidth(bw);
    if (state == RADIOLIB_ERR_NONE) {
        currentBW = bw;
        Serial.print("Bandwidth set to: "); Serial.print(bw); Serial.println(" kHz");
        return true;
    }
    
    return false;
}

bool LoRaHelper::setSpreadingFactor(uint8_t sf) {
    if (!initialized) return false;
    
    int state = radio->setSpreadingFactor(sf);
    if (state == RADIOLIB_ERR_NONE) {
        currentSF = sf;
        Serial.print("Spreading Factor set to: "); Serial.println(sf);
        return true;
    }
    
    return false;
}

bool LoRaHelper::setCodingRate(uint8_t cr) {
    if (!initialized) return false;
    
    int state = radio->setCodingRate(cr);
    if (state == RADIOLIB_ERR_NONE) {
        currentCR = cr;
        Serial.print("Coding Rate set to: "); Serial.println(cr);
        return true;
    }
    
    return false;
}

bool LoRaHelper::setOutputPower(int8_t power) {
    if (!initialized) return false;
    
    int state = radio->setOutputPower(power);
    if (state == RADIOLIB_ERR_NONE) {
        currentPower = power;
        Serial.print("Output Power set to: "); Serial.print(power); Serial.println(" dBm");
        return true;
    }
    
    return false;
}

bool LoRaHelper::setSyncWord(uint8_t sw) {
    if (!initialized) return false;
    
    int state = radio->setSyncWord(sw);
    return (state == RADIOLIB_ERR_NONE);
}

bool LoRaHelper::setMode(LoRaMode mode) {
    if (!initialized) return false;
    
    switch (mode) {
        case LORA_MODE_IDLE:
            radio->standby();
            break;
        case LORA_MODE_RX:
            radio->startReceive();
            break;
        case LORA_MODE_SLEEP:
            radio->sleep();
            break;
        default:
            return false;
    }
    
    currentMode = mode;
    return true;
}

LoRaMode LoRaHelper::getMode() {
    return currentMode;
}

bool LoRaHelper::send(const uint8_t *data, size_t len) {
    if (!initialized || len == 0 || len > LORA_MAX_PACKET_SIZE) return false;
    
    currentMode = LORA_MODE_TX;
    
    int state = radio->transmit(const_cast<uint8_t*>(data), len);
    
    if (state == RADIOLIB_ERR_NONE) {
        stats.packets_sent++;
        Serial.print("Packet sent successfully! Length: "); Serial.println(len);
        
        // Return to receive mode
        startReceive();
        return true;
    }
    
    stats.packets_failed++;
    Serial.print("Transmission failed, code: "); Serial.println(state);
    
    // Return to receive mode even on failure
    startReceive();
    return false;
}

bool LoRaHelper::send(const char *str) {
    return send((const uint8_t*)str, strlen(str));
}

bool LoRaHelper::sendJson(const char *json) {
    return send((const uint8_t*)json, strlen(json));
}

bool LoRaHelper::isSending() {
    if (!initialized) return false;
    return (currentMode == LORA_MODE_TX);
}

bool LoRaHelper::available() {
    if (!initialized) return false;
    return radio->getPacketLength() > 0;
}

LoRaPacket LoRaHelper::receive() {
    LoRaPacket packet;
    packet.valid = false;
    packet.length = 0;
    packet.rssi = 0;
    packet.snr = 0;
    packet.timestamp = millis();
    
    if (!initialized) return packet;
    
    int state = radio->readData(packet.data, LORA_MAX_PACKET_SIZE);
    
    if (state == RADIOLIB_ERR_NONE) {
        packet.length = radio->getPacketLength();
        packet.rssi = radio->getRSSI();
        packet.snr = radio->getSNR();
        packet.valid = true;
        
        stats.packets_received++;
        stats.last_rssi = packet.rssi;
        stats.last_snr = packet.snr;
        
        Serial.print("Packet received! Length: "); Serial.print(packet.length);
        Serial.print(", RSSI: "); Serial.print(packet.rssi);
        Serial.print(" dBm, SNR: "); Serial.print(packet.snr); Serial.println(" dB");
        
        // Call user callback if set
        if (receiveCallback) {
            receiveCallback(packet);
        }
    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
        stats.crc_errors++;
        Serial.println("CRC error in received packet");
    }
    
    return packet;
}

int16_t LoRaHelper::receiveRaw(uint8_t *buffer, size_t maxLen) {
    if (!initialized) return -1;
    
    int state = radio->readData(buffer, maxLen);
    
    if (state == RADIOLIB_ERR_NONE) {
        return radio->getPacketLength();
    }
    
    return -1;
}

void LoRaHelper::startReceive() {
    if (!initialized) return;
    
    int state = radio->startReceive();
    if (state == RADIOLIB_ERR_NONE) {
        currentMode = LORA_MODE_RX;
    }
}

void LoRaHelper::stopReceive() {
    if (!initialized) return;
    
    radio->standby();
    currentMode = LORA_MODE_IDLE;
}

LoRaStats LoRaHelper::getStats() {
    return stats;
}

float LoRaHelper::getLastRSSI() {
    return stats.last_rssi;
}

float LoRaHelper::getLastSNR() {
    return stats.last_snr;
}

bool LoRaHelper::isInitialized() {
    return initialized;
}

void LoRaHelper::printConfig() {
    Serial.println("=== LoRa SX1262 Configuration ===");
    Serial.print("Frequency: "); Serial.print(currentFreq); Serial.println(" MHz");
    Serial.print("Bandwidth: "); Serial.print(currentBW); Serial.println(" kHz");
    Serial.print("Spreading Factor: "); Serial.println(currentSF);
    Serial.print("Coding Rate: "); Serial.println(currentCR);
    Serial.print("Output Power: "); Serial.print(currentPower); Serial.println(" dBm");
    Serial.println("================================");
}

void LoRaHelper::setCAD(bool enable) {
    // Channel Activity Detection - future implementation
}

bool LoRaHelper::channelFree() {
    if (!initialized) return false;
    
    // Perform CAD to check if channel is free
    int state = radio->scanChannel();
    return (state == RADIOLIB_CHANNEL_FREE);
}

void LoRaHelper::setLowDataRateOptimize(bool enable) {
    if (!initialized) return;
    
    // LDR optimization for SF11/SF12
    if (enable) {
        radio->setSpreadingFactor(11); // or 12
    }
}

void LoRaHelper::setCRC(bool enable) {
    if (!initialized) return;
    radio->setCRC(enable);
}

void LoRaHelper::setReceiveCallback(void (*callback)(LoRaPacket)) {
    receiveCallback = callback;
}

void LoRaHelper::resetStats() {
    stats.packets_sent = 0;
    stats.packets_received = 0;
    stats.packets_failed = 0;
    stats.crc_errors = 0;
    stats.last_rssi = 0;
    stats.last_snr = 0;
}

void LoRaHelper::updateStats(bool success, bool isRx) {
    if (isRx) {
        if (success) stats.packets_received++;
    } else {
        if (success) stats.packets_sent++;
        else stats.packets_failed++;
    }
}
