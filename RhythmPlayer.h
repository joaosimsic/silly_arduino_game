#pragma once

#include "Buzzer.h"
#include "Rhythm.h"

class RhythmPlayer {
  public:
    RhythmPlayer(Buzzer &buzzer, Rhythm &rhythm);

    void startLoop(float speed, bool withPreRoll);
    void update(unsigned long dt);

    bool toneStarted() const { return evToneStarted; }
    bool targetToneStarted() const { return evTargetToneStarted; }
    bool targetToneEnded() const { return evTargetToneEnded; }
    bool loopCompleted() const { return evLoopCompleted; }

    bool isPreRoll() const { return seg == Seg::PreRoll; }

    bool targetWindowActive() const {
        return (seg == Seg::Tone && stepIndex == rhythm.specialIndex()) ||
               (seg == Seg::Pause && stepIndex == rhythm.specialIndex());
    }

  private:
    enum class Seg { PreRoll, Tone, Pause };

    static constexpr unsigned long PRE_ROLL_MS = 1000;

    Buzzer &buzzer;
    Rhythm &rhythm;

    float speed = 1.0f;
    Seg seg = Seg::PreRoll;
    int stepIndex = 0;
    unsigned long segRemaining = 0;

    bool evToneStarted = false;
    bool evTargetToneStarted = false;
    bool evTargetToneEnded = false;
    bool evLoopCompleted = false;

    void startTone();
    void advanceSegment();
};
