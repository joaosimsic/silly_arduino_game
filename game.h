#pragma once

class Game {
  public:
    explicit Game(int buzzerPin);

    void start();
    void update();

  private:
    int buzzerPin;

    enum class State {
      Idle,
      Waiting,
      Playing,
      WaitingForInput,
      Result,
    };

    State state = State::Idle;
};
