# Idea: Random buzzer pattern game

## Concept

Procedurally generate a random buzzer pattern the user must react to.

- Play a continuous stream of base-frequency tones with random durations and pauses.
- At one random position in the stream, a clearly different frequency plays.
- The user must press when they hear the different-frequency tone.
- Everything is random: number of tones, each duration, each pause, and the special tone's position.

## Input / output

- Input: terminal keys (no physical button).
- Output: buzzer tones + terminal messages.

## Game flow

- Idle — serial prints "Press any key to start". A key press generates a fresh random
  pattern and starts playback (small pre-roll of silence so the user is ready and stale
  serial input is drained).
- Playing — buzzer emits a continuous stream of base-frequency (300 Hz) tones with random
  durations and random pauses. One random step is set to a special frequency (900 Hz).
- Result — hit/miss evaluation, then a cheerful melody on the buzzer (same tune for win
  and loss) and a HIT/MISS message on the serial port. Back to Idle.

## Scoring

- Press during the special tone's window = HIT.
- Press during any distractor tone or pause = MISS (immediate).
- No press by the time the special tone ends = MISS.

## Timing (configurable constants, placeholders)

- steps: 4–9
- tone duration: 150–400 ms
- pause: 150–700 ms
- special frequency: 900 Hz, base frequency: 300 Hz
- result melody: happy ascending arpeggio (C4-E4-G4-C5-E5-G5-C6), 150 ms each

## Implementation

- `game.cpp` / `game.h` — implement `Game` class:
  - own a `Buzzer` member; `setup()` runs buzzer init + `randomSeed(analogRead(A0))`;
    keep `start()` / `update()`.
  - `generatePattern()` builds a `Step {freq, duration, pause}` array with the ranges above.
  - `update()` drives everything with `millis()` (non-blocking): tone playback, pause gaps,
    serial input detection, state transitions, and a non-blocking melody sequencer.
- `silly_arduino_game.ino` — `Serial.begin(9600)`, construct `Game`, call `setup()`/`update()`
  from `loop()`; remove the hardcoded test tone.
- `Buzzer.h` / `Buzzer.cpp` — no changes needed.
- `Makefile` — add a `monitor` target for the serial terminal.

## Verification

- `make build` compiles for the Uno.
- Play via `make up` then `make monitor`.
