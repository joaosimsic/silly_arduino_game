#include "KeyInput.h"
#include <Arduino.h>

KeyInput::KeyInput() : state(State::AwaitingIdle), pending(false) {}

void KeyInput::reset() {
    state = State::AwaitingIdle;
    pending = false;
}

void KeyInput::update() {
    if (Serial.available() == 0) {
        if (state == State::AwaitingIdle || state == State::Latched)
            state = State::Armed;
        return;
    }

    while (Serial.available())
        Serial.read();

    if (state == State::Armed) {
        pending = true;
        state = State::Latched;
    }
}

bool KeyInput::consumePress() {
    bool p = pending;
    pending = false;
    return p;
}
