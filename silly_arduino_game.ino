#define BUZZER_ENABLED 0

const int BUZZER = 8;

void setup() {
  pinMode(BUZZER, OUTPUT);
}

void loop() {
#if BUZZER_ENABLED
  tone(BUZZER, 500);
  delay(500);

  noTone(BUZZER);
  delay(500);
#endif
}
