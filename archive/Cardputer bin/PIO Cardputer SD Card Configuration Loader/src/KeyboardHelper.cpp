#include "KeyboardHelper.h"

void KeyboardHelper::init() {
    inputBuffer = "";
    lastKeyTime = 0;
}

void KeyboardHelper::update() {
    // M5Unified keyboard handling for Cardputer ADV
    // This is a placeholder - actual implementation depends on
    // how M5Unified exposes keyboard on the ADV model
    
    // For now, we'll rely on the side buttons (BtnA, BtnB, BtnC)
    // Full keyboard integration would require additional work
}

bool KeyboardHelper::isKeyPressed(char key) {
    // Placeholder for specific key detection
    // Would need access to keyboard matrix
    return false;
}

String KeyboardHelper::getInputString() {
    return inputBuffer;
}

void KeyboardHelper::clearInput() {
    inputBuffer = "";
}
