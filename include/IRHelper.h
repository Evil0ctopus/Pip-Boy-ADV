// IRHelper.h - M5Stack IR Unit Support (Grove)
// Provides IR capture, replay, save/load functionality
#ifndef IR_HELPER_H
#define IR_HELPER_H

#include <Arduino.h>
#include <vector>

#define IR_PIN_RX 2  // Grove port RX pin
#define IR_PIN_TX 1  // Grove port TX pin
#define MAX_IR_SIGNALS 50
#define MAX_IR_RAWLEN 512

struct IRSignal {
    String name;
    uint16_t rawData[MAX_IR_RAWLEN];
    uint16_t rawLen;
    uint32_t frequency;
};

class IRHelper {
public:
    IRHelper();
    bool begin();
    bool isAvailable();
    
    // Capture
    bool startCapture();
    bool captureInProgress();
    bool captureComplete();
    IRSignal* getCapturedSignal();
    
    // Replay
    bool transmit(IRSignal* signal);
    
    // Storage
    bool saveSignal(IRSignal* signal, const char* filename);
    bool loadSignal(IRSignal* signal, const char* filename);
    bool deleteSignal(const char* filename);
    std::vector<String> listSignals();
    
    // Management
    int getSignalCount();
    IRSignal* getSignal(int index);
    
private:
    bool available;
    bool capturing;
    IRSignal currentCapture;
    std::vector<IRSignal> signals;
    
    void initPins();
    uint32_t pulseInLong(uint8_t pin, uint8_t state, uint32_t timeout);
};

#endif // IR_HELPER_H
