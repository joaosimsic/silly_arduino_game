#include "Game.h"
#include "Arduino.h"
#include "Buzzer.h"
#include "HardwareSerial.h"
#include "Jingle.h"
#include "Rhythm.h"

Game::Game(Buzzer &buzzer, Jingle &jingle, Rhythm &rhythm)
    : buzzer(buzzer), jingle(jingle), rhythm(rhythm) {}

void Game::setup() {
    randomSeed(analogRead(A0));
    toIdle();
}

void Game::toIdle() {
    state = State::Idle;
    buzzer.stop();
    Serial.println();
    Serial.println("==  ENDLESS REFLEX BUZZER GAME ==");
    Serial.println("Press any key to start...");
}

void Game::startGame() {
    rhythm.generate();
    speed = 1.0f;
    loopCount = 0;
    preRollMs = PRE_ROLL_MS;
    startLoop();
    Serial.println("Get ready...");
}

void Game::startLoop() {
    stepIndex = 0;
    toneStarted = false;
    remainingToneMs = (unsigned long)(rhythm.step(0).duration * speed);
    remainingPauseMs = 0;
    decided = -1;
    awaitClear = true;
    lastMillis = millis();
    state = State::Playing;
}

void Game::finishLoop() {
    if (decided == 1) {
        loopCount++;
        speed *= SPEEDUP_FACTOR;
        if (speed < MIN_SPEED)
            speed = MIN_SPEED;
        jingle.playHappy();
        state = State::Happy;
        Serial.print("HIT! Loop ");
        Serial.println(loopCount);
    } else {
        jingle.playSad();
        state = State::GameOver;
        Serial.print("MISS! Loops cleared: ");
        Serial.println(loopCount);
    }
}

void Game::update() {
    switch (state) {
    case State::Idle:
        updateIdle();
        break;
    case State::Playing:
        updatePlaying();
        break;
    case State::Happy:
        updateHappy();
        break;
    case State::GameOver:
        updateGameOver();
        break;
    }
}

void Game::updateIdle() {
    if (Serial.available() == 0)
        return;

    while (Serial.available())
        Serial.read();

    startGame();
}

void Game::updatePlaying() {
    unsigned long now = millis();
    unsigned long elapsed = now - lastMillis;
    lastMillis = now;

    if (awaitClear) {
        if (Serial.available() > 0) {
            while (Serial.available())
                Serial.read();
            return;
        }
        awaitClear = false;
    } else if (decided == -1 && preRollMs == 0 && Serial.available() > 0) {
        while (Serial.available())
            Serial.read();
        decided = (stepIndex == rhythm.specialIndex()) ? 1 : 0;
    }

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

void Game::updateHappy() {
    jingle.update();
    if (jingle.done())
        startLoop();
}

void Game::updateGameOver() {
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

    if (remainingPauseMs == 0) {
        if (stepIndex + 1 < rhythm.count()) {
            ++stepIndex;
            remainingToneMs =
                (unsigned long)(rhythm.step(stepIndex).duration * speed);
            toneStarted = false;
            return false;
        }

        finishLoop();
        return true;
    }

    return true;
}

bool Game::handleTone(unsigned long &elapsed) {
    if (!toneStarted) {
        unsigned long dur =
            (unsigned long)(rhythm.step(stepIndex).duration * speed);
        buzzer.tone(rhythm.step(stepIndex).freq, dur);
        remainingToneMs = dur;
        toneStarted = true;
    }

    unsigned long consumed = min(elapsed, remainingToneMs);

    remainingToneMs -= consumed;
    elapsed -= consumed;

    if (remainingToneMs == 0) {
        buzzer.stop();

        if (stepIndex == rhythm.specialIndex() && decided != 1)
            decided = 0;

        remainingPauseMs =
            (unsigned long)(rhythm.step(stepIndex).pause * speed);
    }

    return remainingToneMs > 0;
}
