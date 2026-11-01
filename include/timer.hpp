#pragma once

#include "settings.hpp"

#include <chrono>
#include <string>

class Timer {
 public:
  enum class Phase { Study, Break, LongBreak };

  explicit Timer(Settings settings);

  void Update();
  void Reset();
  void Toggle();
  void ApplySettings(Settings settings);

  int Duration() const;
  std::string PhaseName() const;
  std::chrono::seconds remaining{0};
  Settings settings;
  Phase phase = Phase::Study;
  int completed_sessions = 0;
  bool running = false;

 private:
  using Clock = std::chrono::steady_clock;
  Clock::time_point last_tick = Clock::now();
};

std::string FormatTime(std::chrono::seconds remaining);
