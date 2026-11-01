#include "notifications.hpp"

#include <cstdio>
#include <cstdlib>

void Notify(const std::string& phase) {
  std::fputs("\a", stdout);
  std::fflush(stdout);
#ifdef _WIN32
  const std::string command = "powershell -NoProfile -Command \"New-BurntToastNotification -Text 'Studytime', '" + phase + " complete'\"";
#elif __APPLE__
  const std::string command = "osascript -e 'display notification \"" + phase + " complete\" with title \"Studytime\"'";
#else
  const std::string command = "notify-send -a studytime 'Studytime' '" + phase + " complete' >/dev/null 2>&1";
#endif
  std::system(command.c_str());
}
