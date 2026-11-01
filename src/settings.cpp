#include "settings.hpp"

#include <cstdlib>
#include <fstream>
#include <string>

std::filesystem::path ConfigPath() {
#ifdef _WIN32
  const char* appdata = std::getenv("APPDATA");
  return std::filesystem::path(appdata ? appdata : ".") / "studytime" / "config";
#elif __APPLE__
  const char* home = std::getenv("HOME");
  return std::filesystem::path(home ? home : ".") / "Library/Application Support/studytime/config";
#else
  const char* config = std::getenv("XDG_CONFIG_HOME");
  if (config) return std::filesystem::path(config) / "studytime/config";
  const char* home = std::getenv("HOME");
  return std::filesystem::path(home ? home : ".") / ".config/studytime/config";
#endif
}

Settings LoadSettings() {
  Settings settings;
  std::ifstream input(ConfigPath());
  std::string key;
  int value;
  while (input >> key >> value) {
    if (key == "study_minutes") settings.study_minutes = value;
    if (key == "break_minutes") settings.break_minutes = value;
    if (key == "long_break_minutes") settings.long_break_minutes = value;
    if (key == "sessions") settings.sessions = value;
    if (key == "background_enabled") settings.background_enabled = value != 0;
    if (key == "study_red") settings.study_color.red = value;
    if (key == "study_green") settings.study_color.green = value;
    if (key == "study_blue") settings.study_color.blue = value;
    if (key == "break_red") settings.break_color.red = value;
    if (key == "break_green") settings.break_color.green = value;
    if (key == "break_blue") settings.break_color.blue = value;
    if (key == "long_break_red") settings.long_break_color.red = value;
    if (key == "long_break_green") settings.long_break_color.green = value;
    if (key == "long_break_blue") settings.long_break_color.blue = value;
  }
  return settings;
}

void SaveSettings(const Settings& settings) {
  const auto path = ConfigPath();
  std::filesystem::create_directories(path.parent_path());
  std::ofstream output(path);
  output << "study_minutes " << settings.study_minutes << '\n'
         << "break_minutes " << settings.break_minutes << '\n'
         << "long_break_minutes " << settings.long_break_minutes << '\n'
         << "sessions " << settings.sessions << '\n'
         << "background_enabled " << (settings.background_enabled ? 1 : 0) << '\n'
         << "study_red " << settings.study_color.red << '\n'
         << "study_green " << settings.study_color.green << '\n'
         << "study_blue " << settings.study_color.blue << '\n'
         << "break_red " << settings.break_color.red << '\n'
         << "break_green " << settings.break_color.green << '\n'
         << "break_blue " << settings.break_color.blue << '\n'
         << "long_break_red " << settings.long_break_color.red << '\n'
         << "long_break_green " << settings.long_break_color.green << '\n'
         << "long_break_blue " << settings.long_break_color.blue << '\n';
}
