#ifndef SD_HELPER_H
#define SD_HELPER_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

class SDHelper {
public:
    // Initialize SD once with explicit SPI pins and frequency.
    static bool begin(uint8_t csPin, SPIClass &spi, uint32_t freq);
    static bool isReady();

private:
    static bool _ready;
};

#endif // SD_HELPER_H
