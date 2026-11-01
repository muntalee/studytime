#include "timer.hpp"

#include "notifications.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

Timer::Timer(Settings value) : settings(value) {
  Reset();
}

int Timer::Duration() const {
  if (phase == Phase::Study) return settings.study_minutes;
  if (phase == Phase::Break) return settings.break_minutes;
  return settings.long_break_minutes;
}

std::string Timer::PhaseName() const {
  if (phase == Phase::Study) return "FOCUS";
  if (phase == Phase::Break) return "BREAK";
  return "LONG BREAK";
}

void Timer::Reset() {
  running = false;
  remaining = std::chrono::minutes(Duration());
  last_tick = Clock::now();
}

void Timer::Toggle() {
  running = !running;
  last_tick = Clock::now();
}

void Timer::ApplySettings(Settings value) {
  settings = value;
  Reset();
}

void Timer::Update() {
  if (!running) return;
  const auto now = Clock::now();
  const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_tick);
  if (elapsed.count() <= 0) return;
  last_tick += elapsed;
  remaining -= elapsed;
  if (remaining.count() > 0) return;

  Notify(PhaseName());
  if (phase == Phase::Study) {
    ++completed_sessions;
    phase = (completed_sessions % settings.sessions == 0) ? Phase::LongBreak : Phase::Break;
  } else {
    phase = Phase::Study;
  }
  Reset();
}

std::string FormatTime(std::chrono::seconds value) {
  const auto seconds = std::max<int64_t>(0, value.count());
  std::ostringstream result;
  result << std::setfill('0') << std::setw(2) << seconds / 60 << ':'
         << std::setw(2) << seconds % 60;
  return result.str();
}
