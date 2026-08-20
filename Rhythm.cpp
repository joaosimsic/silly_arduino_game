#include "Rhythm.h"
#include <Arduino.h>

Rhythm::Rhythm(Tuning &tuning) : tuning(tuning) {}

int Rhythm::randomIn(int min, int max) {
    if (min > max) {
        int t = min;
        min = max;
        max = t;
    }
    return min + random(max - min + 1);
}

void Rhythm::generate() {
    stepCount = randomIn(tuning.minSteps, tuning.maxSteps);
    if (stepCount > MAX_STEPS)
        stepCount = MAX_STEPS;
    if (stepCount < 1)
        stepCount = 1;

    specialIndex_ = random(stepCount);

    for (int i = 0; i < stepCount; i++) {
        steps[i].freq = (i == specialIndex_) ? tuning.specialFreq : tuning.baseFreq;
        steps[i].duration = randomIn(tuning.minToneMs, tuning.maxToneMs);
        steps[i].pause = randomIn(tuning.minPauseMs, tuning.maxPauseMs);
    }
}
