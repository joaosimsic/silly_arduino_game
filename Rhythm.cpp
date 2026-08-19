#include "Rhythm.h"
#include <Arduino.h>

Rhythm::Rhythm() {}

int Rhythm::randomIn(int min, int max) { return min + random(max - min + 1); }

void Rhythm::generate() {
    stepCount = randomIn(MIN_STEPS, MAX_STEPS);
    specialIndex_ = random(stepCount);

    for (int i = 0; i < stepCount; i++) {
        steps[i].freq = (i == specialIndex_) ? SPECIAL_FREQ : BASE_FREQ;
        steps[i].duration = randomIn(MIN_TONE_MS, MAX_TONE_MS);
        steps[i].pause = randomIn(MIN_PAUSE_MS, MAX_PAUSE_MS);
    }
}
