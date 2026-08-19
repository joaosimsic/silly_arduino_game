#include "Game.h"
#include "Arduino.h"
#include "Buzzer.h"
#include "HardwareSerial.h"
#include "Jingle.h"

Game::Game(Buzzer &buzzer, Jingle &jingle) : buzzer(buzzer), jingle(jingle) {}

void Game::setup() {
    randomSeed(analogRead(A0));
    toIdle();
}

int Game::randomIn(int min, int max) { return min + random(max - min + 1); }

void Game::generatePattern() {
    stepCount = randomIn(MIN_STEPS, MAX_STEPS);
    specialIndex = random(stepCount);

    for (int i = 0; i < stepCount; i++) {
        steps[i].freq = (i == specialIndex) ? SPECIAL_FREQ : BASE_FREQ;
        steps[i].duration = randomIn(MIN_TONE_MS, MAX_TONE_MS);
        steps[i].pause = randomIn(MIN_PAUSE_MS, MAX_PAUSE_MS);
    }
}

void Game::toIdle() {
    state = State::Idle;
    buzzer.stop();
    Serial.println();
    Serial.println("==  RANDOM BUZZER PATTERN GAME ==");
    Serial.println("Press any key to start...");
}

void Game::finishRound(bool hit) {
    lastWasHit = hit;
    jingle.play();
    Serial.println(hit ? "HIT!" : "MISS!");
    state = State::Result;
}

void Game::update() {
    switch (state) {
    case State::Idle:
        updateIdle();
        break;
    case State::Playing:
        updatePlaying();
        break;
    case State::Result:
        updateResult();
        break;
    }
}

void Game::updateIdle() {
    if (Serial.available() == 0)
        return;

    while (Serial.available())
        Serial.read();

    generatePattern();
    stepIndex = 0;
    toneStarted = false;
    remainingToneMs = steps[0].duration;
    remainingPauseMs = 0;
    preRollMs = PRE_ROLL_MS;
    lastMillis = millis();
    state = State::Playing;
    Serial.println("Get ready...");
}

void Game::updatePlaying() {
    unsigned long now = millis();
    unsigned long elapsed = now - lastMillis;
    lastMillis = now;

    while (elapsed > 0) {
        if (preRollMs > 0) {
            if (handlePreRoll(elapsed))
                return;
            continue;
        }

        if (remainingPauseMs > 0) {
            if (handlePause(elapsed))
                return;
            continue;
        }

        if (handleTone(elapsed))
            return;
    }
}
void Game::updateResult() {
    jingle.update();
    if (jingle.done())
        toIdle();
}

bool Game::handlePreRoll(unsigned long &elapsed) {
    unsigned long consumed = min(elapsed, preRollMs);

    preRollMs -= consumed;
    elapsed -= consumed;

    buzzer.stop();

    return preRollMs > 0;
}

bool Game::handlePause(unsigned long &elapsed) {
    unsigned long consumed = min(elapsed, remainingPauseMs);

    remainingPauseMs -= consumed;
    elapsed -= consumed;

    if (remainingPauseMs > 0)
        return true;

    if (stepIndex + 1 < stepCount) {
        ++stepIndex;
        remainingToneMs = steps[stepIndex].duration;
        toneStarted = false;
        return false;
    }

    finishRound(false);
    return true;
}

bool Game::handleTone(unsigned long &elapsed) {
    if (!toneStarted) {
        buzzer.tone(steps[stepIndex].freq, steps[stepIndex].duration);
        toneStarted = true;
    }

    if (stepIndex == specialIndex && Serial.available() > 0) {
        while (Serial.available())
            Serial.read();

        finishRound(true);
        return true;
    }

    unsigned long consumed = min(elapsed, remainingToneMs);

    remainingToneMs -= consumed;
    elapsed -= consumed;

    if (remainingToneMs == 0) {
        buzzer.stop();
        remainingPauseMs = steps[stepIndex].pause;
    }

    return remainingToneMs > 0;
}
