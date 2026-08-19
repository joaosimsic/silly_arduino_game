#include "Game.h"
#include "Arduino.h"
#include "HardwareSerial.h"

Game::Game(int buzzerPin, bool buzzerEnabled)
    : buzzer(buzzerPin, buzzerEnabled) {}

void Game::setup() {
  buzzer.setup();
  randomSeed(analogRead(A0));
  Serial.begin(9600);
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

void Game::updateIdle() {
  bool hasPressed = Serial.available() > 0;

  if (!hasPressed)
    return;

  while (hasPressed) {
    Serial.read();
  }

  generatePattern();
  stepIndex = 0;
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

  if (preRollMs > 0) {
    buzzer.stop();
    preRollMs = (elapsed >= preRollMs) ? 0 : preRollMs - elapsed;
    return;
  }

  bool hasPressed = Serial.available() > 0;

  if (remainingPauseMs <= 0)
    return;
}
