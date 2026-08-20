#pragma once

#include <Arduino.h>

class Tuning {
  public:
    static constexpr int MAX_NOTES = 8;

    unsigned int baseFreq = 300;
    unsigned int specialFreq = 900;
    int minSteps = 4;
    int maxSteps = 9;
    unsigned long minToneMs = 150;
    unsigned long maxToneMs = 400;
    unsigned long minPauseMs = 150;
    unsigned long maxPauseMs = 700;

    float speedupFactor = 0.9f;
    float minSpeed = 0.3f;
    unsigned long preRollMs = 1000;

    unsigned int happyNotes[MAX_NOTES] = {262, 330, 392, 523, 659, 784, 1047};
    int happyCount = 7;
    unsigned long happyMs = 150;

    unsigned int sadNotes[MAX_NOTES] = {523, 392, 330, 262};
    int sadCount = 4;
    unsigned long sadMs = 250;

    static bool freqValid(unsigned int f) { return f >= 31 && f <= 65535; }

    static void printFactor(Stream &out, float f) {
        long v = (long)(f * 1000.0f);
        if (v < 0) {
            out.print('-');
            v = -v;
        }
        out.print(v / 1000);
        out.print('.');
        int frac = (int)(v % 1000);
        if (frac < 100)
            out.print('0');
        if (frac < 10)
            out.print('0');
        out.print(frac);
    }

    void setDefaults() {
        baseFreq = 300;
        specialFreq = 900;
        minSteps = 4;
        maxSteps = 9;
        minToneMs = 150;
        maxToneMs = 400;
        minPauseMs = 150;
        maxPauseMs = 700;
        speedupFactor = 0.9f;
        minSpeed = 0.3f;
        preRollMs = 1000;
        happyNotes[0] = 262;
        happyNotes[1] = 330;
        happyNotes[2] = 392;
        happyNotes[3] = 523;
        happyNotes[4] = 659;
        happyNotes[5] = 784;
        happyNotes[6] = 1047;
        happyCount = 7;
        happyMs = 150;
        sadNotes[0] = 523;
        sadNotes[1] = 392;
        sadNotes[2] = 330;
        sadNotes[3] = 262;
        sadCount = 4;
        sadMs = 250;
    }

    void printTo(Stream &out) const {
        out.println(F("--- current tuning ---"));
        out.print(F("base="));
        out.print(baseFreq);
        out.print(F(" special="));
        out.println(specialFreq);
        out.print(F("steps="));
        out.print(minSteps);
        out.print(F(".."));
        out.println(maxSteps);
        out.print(F("tone="));
        out.print(minToneMs);
        out.print(F(".."));
        out.println(maxToneMs);
        out.print(F("pause="));
        out.print(minPauseMs);
        out.print(F(".."));
        out.println(maxPauseMs);
        out.print(F("speedup="));
        printFactor(out, speedupFactor);
        out.print(F(" minSpeed="));
        printFactor(out, minSpeed);
        out.print(F("preRoll="));
        out.println(preRollMs);
        out.print(F("happyMs="));
        out.print(happyMs);
        out.print(F(" sadMs="));
        out.println(sadMs);
        out.print(F("happyNotes("));
        out.print(happyCount);
        out.print(F("): "));
        for (int i = 0; i < happyCount; i++) {
            out.print(happyNotes[i]);
            out.print(' ');
        }
        out.println();
        out.print(F("sadNotes("));
        out.print(sadCount);
        out.print(F("): "));
        for (int i = 0; i < sadCount; i++) {
            out.print(sadNotes[i]);
            out.print(' ');
        }
        out.println();
    }
};
