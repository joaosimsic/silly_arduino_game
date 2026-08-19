#include "Buzzer.h"

const int BUZZER_PIN = 8;
constexpr bool BUZZER_ENABLED = false;

Buzzer buzzer(BUZZER_PIN, BUZZER_ENABLED);

void setup() {
  buzzer.setup();
}

void loop() {
  buzzer.tone(500, 1);
}
