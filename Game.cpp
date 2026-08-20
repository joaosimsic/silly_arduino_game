#include "Game.h"
#include <Arduino.h>
#include <string.h>
#include "Buzzer.h"
#include "HardwareSerial.h"
#include "Jingle.h"
#include "KeyInput.h"
#include "Rhythm.h"
#include "RhythmPlayer.h"

namespace {
// Minimal float parser so we don't pull in atof()/strtod() (heavy on Uno).
float parseFloat(const char *s) {
    float sign = 1.0f;
    if (*s == '-') {
        sign = -1.0f;
        s++;
    } else if (*s == '+') {
        s++;
    }
    long whole = 0;
    while (*s >= '0' && *s <= '9') {
        whole = whole * 10 + (*s - '0');
        s++;
    }
    float frac = 0.0f;
    if (*s == '.') {
        s++;
        float div = 0.1f;
        while (*s >= '0' && *s <= '9') {
            frac += (float)(*s - '0') * div;
            div *= 0.1f;
            s++;
        }
    }
    return sign * ((float)whole + frac);
}
} // namespace

Game::Game(Buzzer &buzzer, Jingle &jingle, Rhythm &rhythm, Tuning &tuning,
           bool debugMode)
    : buzzer(buzzer), jingle(jingle), rhythm(rhythm), tuning(tuning),
      player(buzzer, rhythm, tuning), debugEnabled(debugMode) {}

void Game::setup() {
    randomSeed(analogRead(A0));
    toIdle();
}

void Game::toIdle() {
    state = State::Idle;
    buzzer.stop();
    Serial.println();
    Serial.println(F("==  ENDLESS REFLEX BUZZER GAME =="));
    if (debugEnabled)
        Serial.println(F("Press any key to start, or 'd' for debug mode"));
    else
        Serial.println(F("Press any key to start..."));
}

void Game::startGame() {
    rhythm.generate();
    speed = 1.0f;
    loopCount = 0;
    outcome = Outcome::None;
    input.reset();
    player.startLoop(speed, true);
    Serial.println(F("Get ready..."));
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
    case State::Debug:
        updateDebug();
        break;
    }
}

void Game::updateIdle() {
    if (Serial.available() == 0)
        return;

    char c = Serial.read();
    if (debugEnabled && (c == 'd' || c == 'D')) {
        while (Serial.available())
            Serial.read();
        enterDebug();
        return;
    }

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
        speed *= tuning.speedupFactor;
        if (speed < tuning.minSpeed)
            speed = tuning.minSpeed;
        jingle.playHappy();
        Serial.print(F("HIT! Loop "));
        Serial.println(loopCount);
        state = State::Happy;
        return;
    }

    if (player.loopCompleted()) {
        jingle.playSad();
        Serial.print(F("MISS! Loops cleared: "));
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

void Game::enterDebug() {
    state = State::Debug;
    buzzer.stop();
    Serial.println();
    Serial.println(F("== DEBUG / TUNING MODE =="));
    printDebugHelp();
}

void Game::printDebugHelp() {
    Serial.println(F("Commands:"));
    Serial.println(F("  ?                  show this help & all values"));
    Serial.println(F("  base <hz>          set base frequency"));
    Serial.println(F("  special <hz>       set target frequency"));
    Serial.println(F("  tone <min> <max>   set tone duration range (ms)"));
    Serial.println(F("  pause <min> <max>  set pause duration range (ms)"));
    Serial.println(F("  steps <min> <max>  set step count range"));
    Serial.println(F("  speedup <f>        set speed-up factor per loop"));
    Serial.println(F("  minspeed <f>       set min speed floor"));
    Serial.println(F("  preroll <ms>       set pre-roll (ms)"));
    Serial.println(F("  htone <ms>         set happy jingle note duration"));
    Serial.println(F("  stone <ms>         set sad jingle note duration"));
    Serial.println(F("  happy <n...>       set happy melody notes (Hz)"));
    Serial.println(F("  sad <n...>         set sad melody notes (Hz)"));
    Serial.println(F("  play <hz>/p <hz>   play a single tone"));
    Serial.println(F("  sweep <a> <b> <c>  sweep a..b Hz in steps of c"));
    Serial.println(F("  start              start game with current values"));
    Serial.println(F("  reset              restore defaults"));
    Serial.println(F("  exit               back to idle"));
}

void Game::updateDebug() {
    if (Serial.available() == 0)
        return;

    static char buf[64];
    int len = Serial.readBytesUntil('\n', buf, sizeof(buf) - 1);
    if (len <= 0)
        return;
    buf[len] = '\0';
    if (len > 0 && (buf[len - 1] == '\r' || buf[len - 1] == '\n'))
        buf[len - 1] = '\0';
    // drain any leftover bytes from an over-long line
    while (Serial.available() && Serial.peek() != '\n')
        Serial.read();
    if (Serial.available() && Serial.peek() == '\n')
        Serial.read();

    handleDebugCommand(buf);
}

int Game::tokenize(char *s, char **tok, int maxTok) {
    int count = 0;
    char *save = nullptr;
    char *p = strtok_r(s, " ", &save);
    while (p && count < maxTok) {
        tok[count++] = p;
        p = strtok_r(nullptr, " ", &save);
    }
    return count;
}

void Game::handleDebugCommand(char *line) {
    char *tok[16];
    int n = tokenize(line, tok, 16);
    if (n == 0)
        return;
    const char *cmd = tok[0];

    if (strcmp(cmd, "?") == 0 || strcmp(cmd, "help") == 0) {
        printDebugHelp();
        tuning.printTo(Serial);
        return;
    }
    if (strcmp(cmd, "base") == 0) {
        if (n < 2) {
            Serial.println(F("usage: base <hz>"));
            return;
        }
        unsigned long f = (unsigned long)atol(tok[1]);
        if (!Tuning::freqValid(f)) {
            Serial.println(F("freq out of range 31..65535"));
            return;
        }
        tuning.baseFreq = f;
        Serial.print(F("base="));
        Serial.println(f);
        return;
    }
    if (strcmp(cmd, "special") == 0) {
        if (n < 2) {
            Serial.println(F("usage: special <hz>"));
            return;
        }
        unsigned long f = (unsigned long)atol(tok[1]);
        if (!Tuning::freqValid(f)) {
            Serial.println(F("freq out of range 31..65535"));
            return;
        }
        tuning.specialFreq = f;
        Serial.print(F("special="));
        Serial.println(f);
        return;
    }
    if (strcmp(cmd, "tone") == 0) {
        if (n < 3) {
            Serial.println(F("usage: tone <min> <max>"));
            return;
        }
        tuning.minToneMs = (unsigned long)atol(tok[1]);
        tuning.maxToneMs = (unsigned long)atol(tok[2]);
        Serial.print(F("tone="));
        Serial.print(tuning.minToneMs);
        Serial.print(F(".."));
        Serial.println(tuning.maxToneMs);
        return;
    }
    if (strcmp(cmd, "pause") == 0) {
        if (n < 3) {
            Serial.println(F("usage: pause <min> <max>"));
            return;
        }
        tuning.minPauseMs = (unsigned long)atol(tok[1]);
        tuning.maxPauseMs = (unsigned long)atol(tok[2]);
        Serial.print(F("pause="));
        Serial.print(tuning.minPauseMs);
        Serial.print(F(".."));
        Serial.println(tuning.maxPauseMs);
        return;
    }
    if (strcmp(cmd, "steps") == 0) {
        if (n < 3) {
            Serial.println(F("usage: steps <min> <max>"));
            return;
        }
        tuning.minSteps = atoi(tok[1]);
        tuning.maxSteps = atoi(tok[2]);
        Serial.print(F("steps="));
        Serial.print(tuning.minSteps);
        Serial.print(F(".."));
        Serial.println(tuning.maxSteps);
        return;
    }
    if (strcmp(cmd, "speedup") == 0) {
        if (n < 2) {
            Serial.println(F("usage: speedup <f>"));
            return;
        }
        tuning.speedupFactor = parseFloat(tok[1]);
        Serial.print(F("speedup="));
        Tuning::printFactor(Serial, tuning.speedupFactor);
        return;
    }
    if (strcmp(cmd, "minspeed") == 0) {
        if (n < 2) {
            Serial.println(F("usage: minspeed <f>"));
            return;
        }
        tuning.minSpeed = parseFloat(tok[1]);
        Serial.print(F("minspeed="));
        Tuning::printFactor(Serial, tuning.minSpeed);
        return;
    }
    if (strcmp(cmd, "preroll") == 0) {
        if (n < 2) {
            Serial.println(F("usage: preroll <ms>"));
            return;
        }
        tuning.preRollMs = (unsigned long)atol(tok[1]);
        Serial.print(F("preRoll="));
        Serial.println(tuning.preRollMs);
        return;
    }
    if (strcmp(cmd, "htone") == 0) {
        if (n < 2) {
            Serial.println(F("usage: htone <ms>"));
            return;
        }
        tuning.happyMs = (unsigned long)atol(tok[1]);
        Serial.print(F("happyMs="));
        Serial.println(tuning.happyMs);
        return;
    }
    if (strcmp(cmd, "stone") == 0) {
        if (n < 2) {
            Serial.println(F("usage: stone <ms>"));
            return;
        }
        tuning.sadMs = (unsigned long)atol(tok[1]);
        Serial.print(F("sadMs="));
        Serial.println(tuning.sadMs);
        return;
    }
    if (strcmp(cmd, "happy") == 0) {
        int cnt = 0;
        for (int i = 1; i < n && cnt < Tuning::MAX_NOTES; i++) {
            unsigned long f = (unsigned long)atol(tok[i]);
            if (!Tuning::freqValid(f)) {
                Serial.println(F("bad note Hz (31..65535)"));
                return;
            }
            tuning.happyNotes[cnt++] = f;
        }
        if (cnt == 0) {
            Serial.println(F("usage: happy <hz> ..."));
            return;
        }
        tuning.happyCount = cnt;
        Serial.print(F("happyNotes="));
        for (int i = 0; i < cnt; i++) {
            Serial.print(tuning.happyNotes[i]);
            Serial.print(' ');
        }
        Serial.println();
        return;
    }
    if (strcmp(cmd, "sad") == 0) {
        int cnt = 0;
        for (int i = 1; i < n && cnt < Tuning::MAX_NOTES; i++) {
            unsigned long f = (unsigned long)atol(tok[i]);
            if (!Tuning::freqValid(f)) {
                Serial.println(F("bad note Hz (31..65535)"));
                return;
            }
            tuning.sadNotes[cnt++] = f;
        }
        if (cnt == 0) {
            Serial.println(F("usage: sad <hz> ..."));
            return;
        }
        tuning.sadCount = cnt;
        Serial.print(F("sadNotes="));
        for (int i = 0; i < cnt; i++) {
            Serial.print(tuning.sadNotes[i]);
            Serial.print(' ');
        }
        Serial.println();
        return;
    }
    if (strcmp(cmd, "play") == 0 || strcmp(cmd, "p") == 0) {
        if (n < 2) {
            Serial.println(F("usage: play <hz>"));
            return;
        }
        unsigned long f = (unsigned long)atol(tok[1]);
        if (!Tuning::freqValid(f)) {
            Serial.println(F("freq out of range 31..65535"));
            return;
        }
        if (!buzzer.tone(f, DEBUG_TONE_MS))
            Serial.println(F("buzzer rejected tone"));
        else {
            Serial.print(F("playing "));
            Serial.print(f);
            Serial.println(F(" Hz"));
        }
        return;
    }
    if (strcmp(cmd, "sweep") == 0) {
        if (n < 4) {
            Serial.println(F("usage: sweep <a> <b> <c>"));
            return;
        }
        long a = atol(tok[1]);
        long b = atol(tok[2]);
        long c = atol(tok[3]);
        if (c <= 0) {
            Serial.println(F("step must be > 0"));
            return;
        }
        Serial.println(F("sweeping..."));
        for (long f = a; f <= b; f += c) {
            if (!Tuning::freqValid((unsigned int)f))
                continue;
            buzzer.tone((unsigned int)f, DEBUG_TONE_MS);
            Serial.println(f);
            delay(DEBUG_TONE_MS + DEBUG_SWEEP_GAP_MS);
        }
        buzzer.stop();
        Serial.println(F("sweep done"));
        return;
    }
    if (strcmp(cmd, "start") == 0) {
        startGame();
        return;
    }
    if (strcmp(cmd, "reset") == 0) {
        tuning.setDefaults();
        Serial.println(F("defaults restored"));
        return;
    }
    if (strcmp(cmd, "exit") == 0) {
        toIdle();
        return;
    }

    Serial.println(F("unknown command (try ?)"));
}
