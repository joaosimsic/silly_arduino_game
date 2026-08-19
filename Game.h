#pragma once

#include "Buzzer.h"
#include "Jingle.h"

class Game {
  public:
    explicit Game(Buzzer &buzzer, Jingle &jingle);

    void setup();
    void update();

  private:
    struct Step {
        unsigned int freq;
        unsigned long duration;
        unsigned long pause;
    };

    enum class State {
        Idle,
        Playing,
        Result,
    };

    static constexpr unsigned int BASE_FREQ = 300;
    static constexpr unsigned int SPECIAL_FREQ = 900;
    static constexpr int MIN_STEPS = 4;
    static constexpr int MAX_STEPS = 9;
    static constexpr unsigned long MIN_TONE_MS = 150;
    static constexpr unsigned long MAX_TONE_MS = 400;
    static constexpr unsigned long MIN_PAUSE_MS = 150;
    static constexpr unsigned long MAX_PAUSE_MS = 700;
    static constexpr unsigned long PRE_ROLL_MS = 1000;

    Buzzer &buzzer;
    Jingle &jingle;
    Step steps[MAX_STEPS];
    int stepCount = 0;
    int stepIndex = 0;
    int specialIndex = -1;
    unsigned long remainingToneMs = 0;
    unsigned long remainingPauseMs = 0;
    bool toneStarted = false;

    unsigned long lastMillis = 0;
    unsigned long preRollMs = 0;

    State state = State::Idle;
    bool lastWasHit = false;

    int randomIn(int min, int max);
    void generatePattern();
    void finishRound(bool hit);
    void toIdle();

    void updateIdle();
    void updatePlaying();
    void updateResult();

    bool handlePreRoll(unsigned long &elapsed);
    bool handlePause(unsigned long &elapsed);
    bool handleTone(unsigned long &elapsed);
};
