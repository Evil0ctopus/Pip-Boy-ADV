#include "SDHelper.h"

bool SDHelper::_ready = false;

bool SDHelper::begin(uint8_t csPin, SPIClass &spi, uint32_t freq) {
    if (_ready) return true;
    _ready = SD.begin(csPin, spi, freq);
    return _ready;
}

bool SDHelper::isReady() {
    return _ready;
}
