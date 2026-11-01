#include "settings.hpp"
#include "timer.hpp"
#include "ui.hpp"

int main() {
  Timer timer(LoadSettings());
  TimerUi ui(timer);
  ui.Run();
}
