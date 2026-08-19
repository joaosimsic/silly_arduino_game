#include "Jingle.h"
#include "Buzzer.h"
#include <Arduino.h>

Jingle::Jingle(Buzzer &buzzer) : buzzer(buzzer) {}

void Jingle::play() {
    noteIndex = 0;
    noteMsLeft = NOTE_MS;
    lastMillis = millis();
    playing = true;
    buzzer.tone(NOTES[0], NOTE_MS);
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

    if (noteIndex >= NOTE_COUNT) {
        stop();
        return;
    }

    buzzer.tone(NOTES[noteIndex], NOTE_MS);
    noteMsLeft = NOTE_MS;
}

void Jingle::stop() {
    buzzer.stop();
    playing = false;
    noteIndex = 0;
    noteMsLeft = 0;
}

bool Jingle::done() const { return !playing; }
