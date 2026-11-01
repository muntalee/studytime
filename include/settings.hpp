#pragma once

#include <filesystem>

struct Settings {
  int study_minutes = 25;
  int break_minutes = 5;
  int long_break_minutes = 15;
  int sessions = 4;
  bool background_enabled = true;
  struct Rgb {
    int red;
    int green;
    int blue;
  };
  Rgb study_color{255, 0, 0};
  Rgb break_color{0, 180, 0};
  Rgb long_break_color{40, 100, 255};
};

std::filesystem::path ConfigPath();
Settings LoadSettings();
void SaveSettings(const Settings& settings);
