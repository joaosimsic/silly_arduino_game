#pragma once

#include "Buzzer.h"

class Jingle {
  public:
    explicit Jingle(Buzzer &buzzer);

    void playHappy();
    void playSad();
    void update();
    void stop();
    bool done() const;

  private:
    static constexpr unsigned long HAPPY_MS = 150;
    static constexpr unsigned long SAD_MS = 250;

    Buzzer &buzzer;
    const unsigned int *notes = nullptr;
    int noteCount = 0;
    unsigned long noteMs = 0;
    int noteIndex = 0;
    unsigned long noteMsLeft = 0;
    unsigned long lastMillis = 0;
    bool playing = false;
};
