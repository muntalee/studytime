#pragma once

#include "timer.hpp"

#include <ftxui/component/screen_interactive.hpp>

class TimerUi {
 public:
  explicit TimerUi(Timer& timer);
  void Run();

 private:
  Timer& timer_;
  ftxui::ScreenInteractive screen_;
};
