#pragma once

#include "Buzzer.h"
#include "Jingle.h"
#include "Rhythm.h"
#include "RhythmPlayer.h"
#include "KeyInput.h"
#include "Tuning.h"

class Game {
  public:
    Game(Buzzer &buzzer, Jingle &jingle, Rhythm &rhythm, Tuning &tuning,
        bool debugMode);

    void setup();
    void update();

  private:
    enum class State { Idle, Playing, Happy, GameOver, Debug };
    enum class Outcome { None, Hit, Miss };

    static constexpr unsigned long DEBUG_TONE_MS = 400;
    static constexpr unsigned long DEBUG_SWEEP_GAP_MS = 60;

    Buzzer &buzzer;
    Jingle &jingle;
    Rhythm &rhythm;
    Tuning &tuning;
    RhythmPlayer player;
    KeyInput input;

    State state = State::Idle;
    Outcome outcome = Outcome::None;
    float speed = 1.0f;
    int loopCount = 0;
    unsigned long lastMillis = 0;
    const bool debugEnabled;

    void toIdle();
    void startGame();
    void startNextLoop();

    void updateIdle();
    void updatePlaying();
    void updateHappy();
    void updateGameOver();

    void enterDebug();
    void updateDebug();
    void handleDebugCommand(char *line);
    void printDebugHelp();

    static int tokenize(char *s, char **tok, int maxTok);
};
