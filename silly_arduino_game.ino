#include "Buzzer.h"
#include "Game.h"
#include "HardwareSerial.h"
#include "Jingle.h"
#include "Rhythm.h"

constexpr int BUZZER_PIN = 8;
constexpr bool BUZZER_ENABLED = true;

Buzzer buzzer(BUZZER_PIN, BUZZER_ENABLED);
Jingle jingle(buzzer);
Rhythm rhythm;
Game game(buzzer, jingle, rhythm);

void setup() {
    Serial.begin(9600);
    buzzer.setup();
    game.setup();
}

void loop() { game.update(); }
