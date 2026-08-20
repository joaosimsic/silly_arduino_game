#pragma once

#include "Tuning.h"

struct Step {
    unsigned int freq;
    unsigned long duration;
    unsigned long pause;
};

class Rhythm {
  public:
    explicit Rhythm(Tuning &tuning);

    void generate();

    int count() const { return stepCount; }
    const Step &step(int i) const { return steps[i]; }
    int specialIndex() const { return specialIndex_; }

  private:
    static constexpr int MAX_STEPS = 9;

    Tuning &tuning;
    Step steps[MAX_STEPS];
    int stepCount = 0;
    int specialIndex_ = -1;

    int randomIn(int min, int max);
};
