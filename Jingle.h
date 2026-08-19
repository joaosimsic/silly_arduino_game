#pragma once

#include "Buzzer.h"

class Jingle {
  public:
    explicit Jingle(Buzzer &buzzer);

    void play();
    void update();
    void stop();
    bool done() const;

  private:
    static constexpr unsigned int NOTES[] = {262, 330, 392, 523,
                                             659, 784, 1047};
    static constexpr int NOTE_COUNT = 7;
    static constexpr unsigned long NOTE_MS = 150;

    Buzzer &buzzer;
    int noteIndex = 0;
    unsigned long noteMsLeft = 0;
    unsigned long lastMillis = 0;
    bool playing = false;
};
