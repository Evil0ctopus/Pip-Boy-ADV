#include "AudioHelper.h"

AudioHelper::AudioHelper() : 
    currentVolume(128),
    muted(false),
    enabled(true),
    playing(false) {
}

bool AudioHelper::begin() {
    Serial.println("Initializing audio system...");
    
    // M5Unified handles speaker initialization
    // Set initial volume
    M5.Speaker.setVolume(currentVolume);
    
    enabled = true;
    Serial.println("Audio system initialized");
    
    return true;
}

void AudioHelper::end() {
    noTone();
    enabled = false;
    Serial.println("Audio system disabled");
}

void AudioHelper::setVolume(uint8_t volume) {
    currentVolume = volume;
    if (!muted) {
        M5.Speaker.setVolume(volume);
    }
    Serial.print("Volume set to: "); Serial.println(volume);
}

uint8_t AudioHelper::getVolume() {
    return currentVolume;
}

void AudioHelper::mute() {
    muted = true;
    M5.Speaker.setVolume(0);
    Serial.println("Audio muted");
}

void AudioHelper::unmute() {
    muted = false;
    M5.Speaker.setVolume(currentVolume);
    Serial.println("Audio unmuted");
}

bool AudioHelper::isMuted() {
    return muted;
}

void AudioHelper::tone(uint16_t frequency, uint32_t duration) {
    if (!enabled || muted) return;
    
    playing = true;
    
    if (duration > 0) {
        M5.Speaker.tone(frequency, duration);
    } else {
        M5.Speaker.tone(frequency);
    }
}

void AudioHelper::tone(uint16_t frequency, uint32_t duration, uint8_t volume) {
    if (!enabled) return;
    
    uint8_t oldVolume = currentVolume;
    M5.Speaker.setVolume(volume);
    
    tone(frequency, duration);
    
    M5.Speaker.setVolume(oldVolume);
}

void AudioHelper::noTone() {
    M5.Speaker.stop();
    playing = false;
}

void AudioHelper::playEffect(SoundEffect effect) {
    if (!enabled || muted) return;
    
    switch (effect) {
        case SFX_BEEP_SHORT:
            playBeep(TONE_BEEP, 50);
            break;
            
        case SFX_BEEP_LONG:
            playBeep(TONE_BEEP, 200);
            break;
            
        case SFX_ERROR:
            playError();
            break;
            
        case SFX_SUCCESS:
            playSuccess();
            break;
            
        case SFX_BUTTON_PRESS:
            playButtonPress();
            break;
            
        case SFX_TAB_SWITCH:
            playTabSwitch();
            break;
            
        case SFX_RADIO_ON:
            playRadioTune();
            break;
            
        case SFX_RADIO_OFF:
            playBeep(800, 100);
            break;
            
        case SFX_ALERT:
            playAlert();
            break;
            
        case SFX_NOTIFICATION:
            playBeep(1200, 150);
            break;
            
        default:
            break;
    }
}

void AudioHelper::playBeep(uint16_t frequency, uint32_t duration) {
    tone(frequency, duration);
}

void AudioHelper::playButtonPress() {
    tone(TONE_BUTTON, 50);
}

void AudioHelper::playTabSwitch() {
    tone(TONE_BUTTON, 80);
}

void AudioHelper::playError() {
    // Three descending tones
    tone(TONE_ERROR + 200, 100);
    delay(120);
    tone(TONE_ERROR + 100, 100);
    delay(120);
    tone(TONE_ERROR, 150);
}

void AudioHelper::playSuccess() {
    // Two ascending tones
    tone(TONE_SUCCESS - 200, 80);
    delay(100);
    tone(TONE_SUCCESS, 120);
}

void AudioHelper::playAlert() {
    // Alternating tones (Pip-Boy style)
    for (int i = 0; i < 3; i++) {
        tone(TONE_ALERT, 100);
        delay(150);
        tone(TONE_ALERT + 300, 100);
        delay(150);
    }
}

void AudioHelper::playStartupSequence() {
    // Pip-Boy startup sound
    tone(800, 100);
    delay(120);
    tone(1000, 100);
    delay(120);
    tone(1200, 100);
    delay(120);
    tone(1500, 200);
}

void AudioHelper::playShutdownSequence() {
    // Pip-Boy shutdown sound
    tone(1500, 100);
    delay(120);
    tone(1200, 100);
    delay(120);
    tone(1000, 100);
    delay(120);
    tone(800, 200);
}

void AudioHelper::playRadioTune() {
    // Radio tuning sound (Pip-Boy style)
    for (int f = 400; f < 1200; f += 200) {
        tone(f, 50);
        delay(60);
    }
    tone(1500, 150);
}

void AudioHelper::playMelody(const uint16_t *frequencies, const uint32_t *durations, size_t length) {
    if (!enabled || muted) return;
    
    for (size_t i = 0; i < length; i++) {
        if (frequencies[i] > 0) {
            tone(frequencies[i], durations[i]);
        }
        delay(durations[i] + 10);  // Small gap between notes
    }
}

void AudioHelper::playRTTTL(const char *rtttl) {
    // Simple RTTTL parser - future implementation
    // Format: name:duration=4,o=6,b=120:notes
    Serial.println("RTTTL playback not yet implemented");
}

bool AudioHelper::isPlaying() {
    return playing;
}

bool AudioHelper::isEnabled() {
    return enabled;
}

void AudioHelper::enable() {
    enabled = true;
    Serial.println("Audio enabled");
}

void AudioHelper::disable() {
    noTone();
    enabled = false;
    Serial.println("Audio disabled");
}

void AudioHelper::stopCurrentTone() {
    noTone();
}
