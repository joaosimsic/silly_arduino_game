# Idea: Endless reflex/rhythm buzzer game

## Concept

- One **randomly generated rhythm** (random tone durations + pauses, exactly one random
  "target" tone at a distinct frequency) is created **once per game**, *outside* the game
  logic.
- That rhythm **loops continuously**; every completed loop **speeds up** (all durations &
  pauses scaled down), so the target window tightens over time.
- The player presses a terminal key **precisely while the target tone sounds**.
- **Hit** = press during the target tone → the loop keeps playing to the end (suspense) →
  **happy** melody → next loop, faster.
- **Miss** = press during any other tone/pause, *or* no press before the target tone ends →
  the loop still finishes (suspense) → **sad** melody → game over.
- **Endless**: the first miss ends the game. Score = number of loops cleared (per-session
  only, no persistence).

## Input / output

- Input: terminal keys (no physical button), Serial @ 9600.
- Output: buzzer tones + serial messages.

## Rhythm generation (decoupled from Game)

- New `Rhythm` class (`rhythm.h` / `rhythm.cpp`):
  - owns `Step {unsigned int freq; unsigned long duration; unsigned long pause}[]`,
    `stepCount`, `specialIndex`.
  - `generate()` builds a random pattern (ranges below); called on game start, **not**
    inside the game-update loop.
  - exposes `count()`, `step(i)`, `specialIndex()`.
- `Game` **consumes** a `Rhythm`; it never generates randomness itself.

## Game flow (states)

- **Idle** — serial prints `Press any key to start`. A key press → `rhythm.generate()`,
  `speed = 1.0`, `loopCount = 0`, begin `Playing` with a short pre-roll (first loop only,
  to drain stale serial and let the player get ready).
- **Playing** — loop the rhythm:
  - base tones at 300 Hz; target tone at `specialIndex` = 900 Hz.
  - press **during target tone (or its grace pause)** → mark HIT, lock input for the rest
    of the loop.
  - press **during any other tone or pause** → mark MISS (loop still finishes).
  - **target tone ends with no press** → mark MISS.
  - when the whole loop finishes, reveal the outcome:
    - HIT → `Happy`; MISS → `GameOver`.
- **Happy** (reward) — play **happy** melody; on `done()` → `loopCount++`,
  `speed *= SPEEDUP_FACTOR` (clamped to `MIN_SPEED`), restart `Playing` (faster loop).
- **GameOver** — play **sad** melody; print `MISS! Loops cleared: N`; on `done()` → `Idle`.

Serial debounce: at the start of every loop `awaitClear` drains any leftover/held key and
requires the serial to be empty once before a press can be counted, so key auto-repeat
cannot cause an instant double decision. The first press each loop decides the outcome;
further presses are ignored until the next loop.

## Scoring

- `loopCount` = loops fully cleared this game (per session only, no EEPROM).
- Miss ends the game; score printed to serial.

## Timing constants

- steps: 4–9 · tone: 150–400 ms · pause: 150–700 ms · base 300 Hz · target 900 Hz.
- `SPEEDUP_FACTOR` ≈ 0.9 per loop; `MIN_SPEED` floor ≈ 0.3 (keeps tones ≥ ~20 ms so
  `tone()` still works and stays humanly hittable).
- pre-roll (first loop only) ≈ 1000 ms.
- happy melody: ascending arpeggio C4-E4-G4-C5-E5-G5-C6, ~150 ms each.
- sad melody: descending C5-G4-E4-C4, ~250 ms each, lower register.

## Implementation

- **`rhythm.h` / `rhythm.cpp`** — `Rhythm` class + `generate()`; pure data, no timing.
- **`rhythm_player.h` / `rhythm_player.cpp`** (new) — the looping **timing engine**: a
  segment cursor (`PreRoll / Tone / Pause`) over the `Rhythm`, speed scaling applied to
  durations on the fly, drives `Buzzer` playback, and exposes one-shot event flags
  (`toneStarted`, `targetToneStarted`, `targetToneEnded`, `loopCompleted`) plus
  `isPreRoll()` / `targetWindowActive()`.
- **`key_input.h` / `key_input.cpp`** (new) — debounced terminal-key detection: latches a
  single press only after the serial line has been idle (`reset()` disarms at loop start to
  drain stale/held keys), so auto-repeat can't double-fire.
- **`jingle.h` / `jingle.cpp`** — two melodies via `playHappy()` / `playSad()`; keeps the
  non-blocking `update()` / `done()` sequencer.
- **`game.h` / `game.cpp`** — thin orchestrator. Owns `Buzzer` + `Jingle` + `Rhythm` and
  composes a `RhythmPlayer` + `KeyInput`. States `Idle / Playing / Happy / GameOver`; reacts
  to player events and key presses, tracks `Outcome { None, Hit, Miss }`, `speed`, and
  `loopCount`. The speed-up (`SPEEDUP_FACTOR` ≈ 0.9, `MIN_SPEED` floor ≈ 0.3) lives here.
- **`silly_arduino_game.ino`** — `Buzzer buzzer(...); Jingle jingle(buzzer); Rhythm rhythm;
  Game game(buzzer, jingle, rhythm);` (the two helpers are created inside `Game`).
- **`Buzzer.h` / `Buzzer.cpp`** — no changes needed.
- **`Makefile`** — unchanged (`build` / `up` / `monitor`).

## Verification

- `make build` compiles for the Uno.
- `make up` then `make monitor`: start, confirm the rhythm loops and audibly speeds up each
  loop; hitting the target advances (happy melody) and the loop keeps playing through to the
  end before the reveal; a mistimed press or a missed target plays the sad melody and prints
  `Loops cleared: N`, then returns to idle.
