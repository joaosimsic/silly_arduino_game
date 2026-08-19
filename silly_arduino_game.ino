#include "Buzzer.h"

constexpr int BUZZER_PIN = 8;
constexpr bool BUZZER_ENABLED = true;

Buzzer buzzer(BUZZER_PIN, BUZZER_ENABLED);

void setup() {
    buzzer.setup();
    buzzer.tone(300, 1000);
}

void loop() {}
