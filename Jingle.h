#pragma once

#include "Buzzer.h"
#include "Tuning.h"

class Jingle {
  public:
    explicit Jingle(Buzzer &buzzer, Tuning &tuning);

    void playHappy();
    void playSad();
    void update();
    void stop();
    bool done() const;

  private:
    Buzzer &buzzer;
    Tuning &tuning;
    const unsigned int *notes = nullptr;
    int noteCount = 0;
    unsigned long noteMs = 0;
    int noteIndex = 0;
    unsigned long noteMsLeft = 0;
    unsigned long lastMillis = 0;
    bool playing = false;
};
