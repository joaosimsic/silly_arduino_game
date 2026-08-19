#pragma once

#include <Arduino.h>

class KeyInput {
  public:
    KeyInput();

    void reset();
    void update();
    bool consumePress();

  private:
    enum class State { AwaitingIdle, Armed, Latched };

    State state;
    bool pending;
};
