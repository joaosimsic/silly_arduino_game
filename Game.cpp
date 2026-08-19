#include "Game.h"
#include "Arduino.h"
#include "Buzzer.h"
#include "HardwareSerial.h"
#include "Jingle.h"
#include "KeyInput.h"
#include "Rhythm.h"
#include "RhythmPlayer.h"

Game::Game(Buzzer &buzzer, Jingle &jingle, Rhythm &rhythm)
    : buzzer(buzzer), jingle(jingle), rhythm(rhythm), player(buzzer, rhythm) {}

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
    outcome = Outcome::None;
    input.reset();
    player.startLoop(speed, true);
    Serial.println("Get ready...");
    state = State::Playing;
    lastMillis = millis();
}

void Game::startNextLoop() {
    outcome = Outcome::None;
    input.reset();
    player.startLoop(speed, false);
    lastMillis = millis();
    state = State::Playing;
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
    unsigned long dt = now - lastMillis;
    lastMillis = now;

    bool inTarget = player.targetWindowActive();

    if (!player.isPreRoll())
        input.update();
    player.update(dt);

    if (outcome == Outcome::None && input.consumePress())
        outcome = inTarget ? Outcome::Hit : Outcome::Miss;
    if (outcome == Outcome::None && player.targetToneEnded())
        outcome = Outcome::Miss;

    if (player.loopCompleted() && outcome == Outcome::Hit) {
        loopCount++;
        speed *= SPEEDUP_FACTOR;
        if (speed < MIN_SPEED)
            speed = MIN_SPEED;
        jingle.playHappy();
        Serial.print("HIT! Loop ");
        Serial.println(loopCount);
        state = State::Happy;
        return;
    }

    if (player.loopCompleted()) {
        jingle.playSad();
        Serial.print("MISS! Loops cleared: ");
        Serial.println(loopCount);
        state = State::GameOver;
    }
}

void Game::updateHappy() {
    jingle.update();
    if (jingle.done())
        startNextLoop();
}

void Game::updateGameOver() {
    jingle.update();
    if (jingle.done())
        toIdle();
}
