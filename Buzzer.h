#pragma once

#include <Arduino.h>

class Buzzer {
  public:
    explicit Buzzer(const uint8_t buzzerPin, bool buzzerEnabled = true);

    void setup();
    bool tone(unsigned int frequency, unsigned long duration = 0);
    void stop();

  private:
    const uint8_t pin;
    bool enabled;
};
