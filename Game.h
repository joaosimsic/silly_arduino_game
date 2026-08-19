#pragma once

#include "Buzzer.h"

class Game {
public:
  explicit Game(int buzzerPin, bool buzzerEnabled);

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
  static constexpr unsigned int NOTE_C4 = 262;
  static constexpr unsigned int NOTE_E4 = 330;
  static constexpr unsigned int NOTE_G4 = 392;
  static constexpr unsigned int NOTE_C5 = 523;
  static constexpr unsigned int NOTE_E5 = 659;
  static constexpr unsigned int NOTE_G5 = 784;
  static constexpr unsigned int NOTE_C6 = 1047;
  static constexpr unsigned int MELODY[] = {
      NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6,
  };
  static constexpr int MELODY_LEN = 7;
  static constexpr unsigned long NOTE_MS = 150;

  Buzzer buzzer;
  Step steps[MAX_STEPS];
  int stepCount = 0;
  int stepIndex = 0;
  int specialIndex = -1;
  unsigned long remainingToneMs = 0;
  unsigned long remainingPauseMs = 0;

  unsigned long lastMillis = 0;
  unsigned long preRollMs = 0;
  int melodyIndex = 0;
  unsigned long melodyMsLeft = 0;

  State state = State::Idle;
  bool lastWasHit = false;

  int randomIn(int min, int max);
  void generatePattern();
  void finishRound(bool hit);
  void toIdle();
  void updateIdle();
  void updatePlaying();
  void updateResult();
};
