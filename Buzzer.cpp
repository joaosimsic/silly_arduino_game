#include "Buzzer.h"
#include <Arduino.h>

Buzzer::Buzzer(const uint8_t buzzerPin, bool buzzerEnabled)
    : pin(buzzerPin), enabled(buzzerEnabled) {}

void Buzzer::setup() {
    pinMode(pin, OUTPUT);
    stop();
}

void Buzzer::stop() { ::noTone(pin); }

bool Buzzer::tone(unsigned int frequency, unsigned long duration) {
    if (!enabled)
        return false;

    if (frequency < 31 || frequency > 65535 || duration == 0)
        return false;

    ::tone(pin, frequency, duration);
    return true;
}
