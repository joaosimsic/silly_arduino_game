#include "Buzzer.h"
#include "Game.h"
#include "HardwareSerial.h"
#include "Jingle.h"
#include "Rhythm.h"
#include "Tuning.h"

constexpr int BUZZER_PIN = 8;
constexpr bool BUZZER_ENABLED = true;
constexpr bool DEBUG_MODE = true;

Buzzer buzzer(BUZZER_PIN, BUZZER_ENABLED);
Tuning tuning;
Jingle jingle(buzzer, tuning);
Rhythm rhythm(tuning);
Game game(buzzer, jingle, rhythm, tuning, DEBUG_MODE);

void setup() {
    Serial.begin(9600);
    buzzer.setup();
    game.setup();
}

void loop() {
    game.update();
}
