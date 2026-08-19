#pragma once

#include "Buzzer.h"
#include "Jingle.h"
#include "Rhythm.h"
#include "RhythmPlayer.h"
#include "KeyInput.h"

class Game {
  public:
    explicit Game(Buzzer &buzzer, Jingle &jingle, Rhythm &rhythm);

    void setup();
    void update();

  private:
    enum class State { Idle, Playing, Happy, GameOver };
    enum class Outcome { None, Hit, Miss };

    static constexpr float SPEEDUP_FACTOR = 0.9f;
    static constexpr float MIN_SPEED = 0.3f;

    Buzzer &buzzer;
    Jingle &jingle;
    Rhythm &rhythm;
    RhythmPlayer player;
    KeyInput input;

    State state = State::Idle;
    Outcome outcome = Outcome::None;
    float speed = 1.0f;
    int loopCount = 0;
    unsigned long lastMillis = 0;

    void toIdle();
    void startGame();
    void startNextLoop();

    void updateIdle();
    void updatePlaying();
    void updateHappy();
    void updateGameOver();
};
