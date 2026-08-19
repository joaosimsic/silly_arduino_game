#pragma once

#include "Buzzer.h"
#include "Jingle.h"
#include "Rhythm.h"

class Game {
  public:
    explicit Game(Buzzer &buzzer, Jingle &jingle, Rhythm &rhythm);

    void setup();
    void update();

  private:
    enum class State {
        Idle,
        Playing,
        Happy,
        GameOver,
    };

    static constexpr unsigned long PRE_ROLL_MS = 1000;
    static constexpr float SPEEDUP_FACTOR = 0.9f;
    static constexpr float MIN_SPEED = 0.3f;

    Buzzer &buzzer;
    Jingle &jingle;
    Rhythm &rhythm;

    State state = State::Idle;
    unsigned long lastMillis = 0;

    float speed = 1.0f;
    int loopCount = 0;
    int stepIndex = 0;
    unsigned long remainingToneMs = 0;
    unsigned long remainingPauseMs = 0;
    bool toneStarted = false;
    unsigned long preRollMs = 0;
    int decided = -1; // -1 none, 0 miss, 1 hit (this loop)
    bool awaitClear = false;

    void toIdle();
    void startGame();
    void startLoop();
    void finishLoop();

    void updateIdle();
    void updatePlaying();
    void updateHappy();
    void updateGameOver();

    bool handlePreRoll(unsigned long &elapsed);
    bool handlePause(unsigned long &elapsed);
    bool handleTone(unsigned long &elapsed);
};
