#include "Jingle.h"
#include "Buzzer.h"
#include <Arduino.h>

Jingle::Jingle(Buzzer &buzzer, Tuning &tuning) : buzzer(buzzer), tuning(tuning) {}

void Jingle::playHappy() {
    notes = tuning.happyNotes;
    noteCount = tuning.happyCount;
    noteMs = tuning.happyMs;
    noteIndex = 0;
    noteMsLeft = noteMs;
    lastMillis = millis();
    playing = true;
    if (noteCount > 0)
        buzzer.tone(notes[0], noteMs);
}

void Jingle::playSad() {
    notes = tuning.sadNotes;
    noteCount = tuning.sadCount;
    noteMs = tuning.sadMs;
    noteIndex = 0;
    noteMsLeft = noteMs;
    lastMillis = millis();
    playing = true;
    if (noteCount > 0)
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
