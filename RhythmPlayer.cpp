#include "RhythmPlayer.h"
#include "Buzzer.h"
#include "Rhythm.h"
#include <Arduino.h>

RhythmPlayer::RhythmPlayer(Buzzer &buzzer, Rhythm &rhythm)
    : buzzer(buzzer), rhythm(rhythm) {}

void RhythmPlayer::startLoop(float spd, bool withPreRoll) {
    speed = spd;
    stepIndex = 0;
    evToneStarted = false;
    evTargetToneStarted = false;
    evTargetToneEnded = false;
    evLoopCompleted = false;

    if (withPreRoll) {
        seg = Seg::PreRoll;
        segRemaining = PRE_ROLL_MS;
        buzzer.stop();
        return;
    }

    seg = Seg::Tone;
    startTone();
}

void RhythmPlayer::startTone() {
    unsigned long dur =
        (unsigned long)(rhythm.step(stepIndex).duration * speed);
    buzzer.tone(rhythm.step(stepIndex).freq, dur);
    segRemaining = dur;
    evToneStarted = true;
    if (stepIndex == rhythm.specialIndex())
        evTargetToneStarted = true;
}

void RhythmPlayer::advanceSegment() {
    switch (seg) {
    case Seg::PreRoll:
        seg = Seg::Tone;
        startTone();
        return;
    case Seg::Tone:
        buzzer.stop();
        if (stepIndex == rhythm.specialIndex())
            evTargetToneEnded = true;
        seg = Seg::Pause;
        segRemaining = (unsigned long)(rhythm.step(stepIndex).pause * speed);
        return;
    case Seg::Pause:
        if (stepIndex + 1 < rhythm.count()) {
            ++stepIndex;
            seg = Seg::Tone;
            startTone();
            return;
        }
        evLoopCompleted = true;
        return;
    }
}

void RhythmPlayer::update(unsigned long dt) {
    evToneStarted = false;
    evTargetToneStarted = false;
    evTargetToneEnded = false;
    evLoopCompleted = false;

    while (dt > 0 && !evLoopCompleted) {
        unsigned long consumed = min(dt, segRemaining);
        segRemaining -= consumed;
        dt -= consumed;

        if (segRemaining == 0)
            advanceSegment();
    }
}
