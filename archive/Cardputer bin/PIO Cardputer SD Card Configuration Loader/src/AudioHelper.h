#ifndef AUDIO_HELPER_H
#define AUDIO_HELPER_H

#include <M5Unified.h>

// Audio configuration for Cardputer ADV
#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_BUFFER_SIZE 512

// Pip-Boy sound effects
enum SoundEffect {
    SFX_NONE = 0,
    SFX_BEEP_SHORT,
    SFX_BEEP_LONG,
    SFX_ERROR,
    SFX_SUCCESS,
    SFX_BUTTON_PRESS,
    SFX_TAB_SWITCH,
    SFX_RADIO_ON,
    SFX_RADIO_OFF,
    SFX_ALERT,
    SFX_NOTIFICATION
};

// Pip-Boy classic tones (frequencies in Hz)
#define TONE_BEEP       2000
#define TONE_BUTTON     4000
#define TONE_ERROR      300
#define TONE_SUCCESS    1500
#define TONE_ALERT      1000

class AudioHelper {
public:
    AudioHelper();
    
    // Initialization
    bool begin();
    void end();
    
    // Volume control
    void setVolume(uint8_t volume);  // 0-255
    uint8_t getVolume();
    void mute();
    void unmute();
    bool isMuted();
    
    // Tone generation
    void tone(uint16_t frequency, uint32_t duration = 0);  // duration 0 = continuous
    void tone(uint16_t frequency, uint32_t duration, uint8_t volume);
    void noTone();
    
    // Sound effects
    void playEffect(SoundEffect effect);
    void playBeep(uint16_t frequency = TONE_BEEP, uint32_t duration = 100);
    void playButtonPress();
    void playTabSwitch();
    void playError();
    void playSuccess();
    void playAlert();
    
    // Multi-tone sequences
    void playStartupSequence();
    void playShutdownSequence();
    void playRadioTune();
    
    // Advanced features
    void playMelody(const uint16_t *frequencies, const uint32_t *durations, size_t length);
    void playRTTTL(const char *rtttl);  // Ring Tone Text Transfer Language
    
    // Status
    bool isPlaying();
    bool isEnabled();
    void enable();
    void disable();
    
private:
    uint8_t currentVolume;
    bool muted;
    bool enabled;
    bool playing;
    
    void stopCurrentTone();
};

// Global audio helper instance
extern AudioHelper audio;

#endif // AUDIO_HELPER_H
