#ifndef KEYBOARD_HELPER_H
#define KEYBOARD_HELPER_H

#include <M5Unified.h>

// Keyboard helper for Cardputer ADV
// This provides a wrapper for keyboard input handling

class KeyboardHelper {
public:
    void init();
    void update();
    bool isKeyPressed(char key);
    String getInputString();
    void clearInput();
    
private:
    String inputBuffer;
    unsigned long lastKeyTime;
};

#endif // KEYBOARD_HELPER_H
