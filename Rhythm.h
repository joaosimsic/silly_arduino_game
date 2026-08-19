#pragma once

struct Step {
    unsigned int freq;
    unsigned long duration;
    unsigned long pause;
};

class Rhythm {
  public:
    Rhythm();

    void generate();

    int count() const { return stepCount; }
    const Step &step(int i) const { return steps[i]; }
    int specialIndex() const { return specialIndex_; }

  private:
    static constexpr unsigned int BASE_FREQ = 300;
    static constexpr unsigned int SPECIAL_FREQ = 900;
    static constexpr int MIN_STEPS = 4;
    static constexpr int MAX_STEPS = 9;
    static constexpr unsigned long MIN_TONE_MS = 150;
    static constexpr unsigned long MAX_TONE_MS = 400;
    static constexpr unsigned long MIN_PAUSE_MS = 150;
    static constexpr unsigned long MAX_PAUSE_MS = 700;

    Step steps[MAX_STEPS];
    int stepCount = 0;
    int specialIndex_ = -1;

    int randomIn(int min, int max);
};
