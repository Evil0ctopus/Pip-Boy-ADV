#ifndef LORA_HELPER_H
#define LORA_HELPER_H

#include <Arduino.h>
#include <RadioLib.h>
#include "HardwareConfig.h"

// SX1262 LoRa Hat Pin Definitions for Cardputer ADV
// Defined in HardwareConfig.h:
// LORA_SCK=36, LORA_MISO=37, LORA_MOSI=35,
// LORA_CS=34, LORA_RST=33, LORA_IRQ=39, LORA_BUSY=38

// LoRa Configuration
#define LORA_DEFAULT_FREQ 915.0      // MHz (change based on region: 433/868/915)
#define LORA_DEFAULT_BW 125.0        // kHz
#define LORA_DEFAULT_SF 7            // Spreading Factor (7-12)
#define LORA_DEFAULT_CR 5            // Coding Rate (5-8)
#define LORA_DEFAULT_POWER 14        // dBm (2-20)
#define LORA_PREAMBLE_LEN 8          // Preamble length
#define LORA_SYNC_WORD 0x12          // Sync word (0x12 = LoRaWAN public, 0x34 = private)

// Packet size limits
#define LORA_MAX_PACKET_SIZE 255
#define LORA_BUFFER_SIZE 512

// LoRa operating modes
enum LoRaMode {
    LORA_MODE_IDLE = 0,
    LORA_MODE_TX,
    LORA_MODE_RX,
    LORA_MODE_SLEEP
};

// LoRa packet structure
struct LoRaPacket {
    uint8_t data[LORA_MAX_PACKET_SIZE];
    uint8_t length;
    float rssi;
    float snr;
    uint32_t timestamp;
    bool valid;
};

// LoRa statistics
struct LoRaStats {
    uint32_t packets_sent;
    uint32_t packets_received;
    uint32_t packets_failed;
    uint32_t crc_errors;
    float last_rssi;
    float last_snr;
};

class LoRaHelper {
public:
    LoRaHelper();
    
    // Initialization
    bool begin();
    bool begin(float freq, float bw, uint8_t sf, uint8_t cr, uint8_t power);
    void end();
    
    // Configuration
    bool setFrequency(float freq);
    bool setBandwidth(float bw);
    bool setSpreadingFactor(uint8_t sf);
    bool setCodingRate(uint8_t cr);
    bool setOutputPower(int8_t power);
    bool setSyncWord(uint8_t sw);
    
    // Operating modes
    bool setMode(LoRaMode mode);
    LoRaMode getMode();
    
    // Transmit
    bool send(const uint8_t *data, size_t len);
    bool send(const char *str);
    bool sendJson(const char *json);
    bool isSending();
    
    // Receive
    bool available();
    LoRaPacket receive();
    int16_t receiveRaw(uint8_t *buffer, size_t maxLen);
    void startReceive();
    void stopReceive();
    
    // Status and diagnostics
    LoRaStats getStats();
    float getLastRSSI();
    float getLastSNR();
    bool isInitialized();
    void printConfig();
    
    // Advanced features
    void setCAD(bool enable);  // Channel Activity Detection
    bool channelFree();
    void setLowDataRateOptimize(bool enable);
    void setCRC(bool enable);
    
    // Callback for received packets
    void setReceiveCallback(void (*callback)(LoRaPacket));
    
private:
    SX1262 *radio;
    SPIClass *loraSPI;
    
    LoRaMode currentMode;
    LoRaStats stats;
    bool initialized;
    
    float currentFreq;
    float currentBW;
    uint8_t currentSF;
    uint8_t currentCR;
    int8_t currentPower;
    
    void (*receiveCallback)(LoRaPacket);
    
    // Internal helpers
    void resetStats();
    void updateStats(bool success, bool isRx);
};

// Global LoRa helper instance
extern LoRaHelper lora;

#endif // LORA_HELPER_H
