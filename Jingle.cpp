#include "Jingle.h"
#include "Buzzer.h"
#include <Arduino.h>

namespace {
const unsigned int HAPPY_NOTES[] = {262, 330, 392, 523, 659, 784, 1047};
const int HAPPY_COUNT = 7;
const unsigned int SAD_NOTES[] = {523, 392, 330, 262};
const int SAD_COUNT = 4;
}

Jingle::Jingle(Buzzer &buzzer) : buzzer(buzzer) {}

void Jingle::playHappy() {
    notes = HAPPY_NOTES;
    noteCount = HAPPY_COUNT;
    noteMs = HAPPY_MS;
    noteIndex = 0;
    noteMsLeft = noteMs;
    lastMillis = millis();
    playing = true;
    buzzer.tone(notes[0], noteMs);
}

void Jingle::playSad() {
    notes = SAD_NOTES;
    noteCount = SAD_COUNT;
    noteMs = SAD_MS;
    noteIndex = 0;
    noteMsLeft = noteMs;
    lastMillis = millis();
    playing = true;
    buzzer.tone(notes[0], noteMs);
}

void Jingle::update() {
    if (!playing)
        return;

    unsigned long now = millis();
    unsigned long elapsed = now - lastMillis;
    lastMillis = now;

    if (elapsed < noteMsLeft) {
        noteMsLeft -= elapsed;
        return;
    }

    noteIndex++;

    if (noteIndex >= noteCount) {
        stop();
        return;
    }

    buzzer.tone(notes[noteIndex], noteMs);
    noteMsLeft = noteMs;
}

void Jingle::stop() {
    buzzer.stop();
    playing = false;
    noteIndex = 0;
    noteMsLeft = 0;
}

bool Jingle::done() const { return !playing; }
