#include "ui.hpp"

#include "settings.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include <array>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <string>
#include <thread>

namespace {
using namespace ftxui;

Color PhaseColor(const Timer& timer, bool muted) {
  const auto& rgb = timer.phase == Timer::Phase::Study
      ? timer.settings.study_color
      : timer.phase == Timer::Phase::Break ? timer.settings.break_color
                                           : timer.settings.long_break_color;
  const int factor = muted ? 70 : 100;
  return Color::RGB(rgb.red * factor / 100, rgb.green * factor / 100,
                    rgb.blue * factor / 100);
}

Element TimerPanel(const Timer& timer, int width, int height) {
  const auto total = std::chrono::seconds(timer.Duration() * 60);
  const float progress = total.count() == 0
      ? 0.0F
      : 1.0F - static_cast<float>(timer.remaining.count()) / total.count();
  const int filled_columns = std::clamp(static_cast<int>(progress * width), 0, width);
  const Color fill_color = timer.settings.background_enabled
      ? PhaseColor(timer, !timer.running)
      : Color::Default;
  const Color foreground = timer.running ? Color::White : Color::RGB(178, 178, 178);
  Elements rows;
  for (int row = 0; row < height; ++row) {
    const int unfilled_columns = width - filled_columns;
    auto bar = hbox({
        text(std::string(filled_columns, ' ')) | bgcolor(fill_color),
        text(std::string(unfilled_columns, ' ')),
    });
    rows.push_back(std::move(bar));
  }
  auto content = vbox({
      text(FormatTime(timer.remaining)) | bold | center | color(foreground),
  }) | center;
  return dbox({vbox(rows), content}) |
         size(WIDTH, EQUAL, width) |
         size(HEIGHT, EQUAL, height) |
         borderRounded | color(foreground);
}
}  // namespace

TimerUi::TimerUi(Timer& timer)
    : timer_(timer), screen_(ScreenInteractive::Fullscreen()) {}

void TimerUi::Run() {
  bool settings_open = false;
  int selected = 0;
  Settings draft = timer_.settings;
  std::atomic<bool> active = true;

  auto component = CatchEvent(Renderer([&] {
    timer_.Update();
    const int panel_width = std::max(20, screen_.dimx() / 2);
    const int panel_height = std::max(8, screen_.dimy() / 2);
    auto view = vbox({
        filler(),
        TimerPanel(timer_, panel_width, panel_height) | center,
        filler(),
    }) | flex;
    const auto footer = text("Ctrl+P settings   Space start/pause   R reset   Q quit") |
                        dim | center;
    if (!settings_open) {
      return vbox({view, footer});
    }

    const std::array<std::string, 14> names = {
        "Background", "Study minutes", "Break minutes", "Long break minutes", "Sessions",
        "Study red", "Study green", "Study blue", "Break red", "Break green", "Break blue",
        "Long break red", "Long break green", "Long break blue"};
    const std::array<int, 14> values = {
        draft.background_enabled ? 1 : 0, draft.study_minutes, draft.break_minutes,
        draft.long_break_minutes, draft.sessions, draft.study_color.red, draft.study_color.green,
        draft.study_color.blue, draft.break_color.red, draft.break_color.green, draft.break_color.blue,
        draft.long_break_color.red, draft.long_break_color.green, draft.long_break_color.blue};
    Elements fields;
    for (int i = 0; i < 14; ++i) {
      fields.push_back(hbox({text(i == selected ? "> " : "  ") | bold,
                             text(names[i]) | size(WIDTH, EQUAL, 20),
                             text(i == 0 ? (values[i] ? "on" : "off") : std::to_string(values[i])) | bold}));
    }
    auto settings = window(
        text(" Settings (arrows change, Enter save, Esc cancel) "), vbox(fields)) |
        clear_under;
    return vbox({dbox({view, settings | center}) | flex, footer});
  }), [&](Event event) {
    if (event == Event::CtrlP) {
      settings_open = !settings_open;
      draft = timer_.settings;
      return true;
    }
    if (settings_open) {
      if (event == Event::Escape) {
        settings_open = false;
        return true;
      }
      if (event == Event::ArrowUp) selected = (selected + 13) % 14;
      if (event == Event::ArrowDown) selected = (selected + 1) % 14;
      int* value = nullptr;
      if (selected == 1) value = &draft.study_minutes;
      if (selected == 2) value = &draft.break_minutes;
      if (selected == 3) value = &draft.long_break_minutes;
      if (selected == 4) value = &draft.sessions;
      if (selected == 5) value = &draft.study_color.red;
      if (selected == 6) value = &draft.study_color.green;
      if (selected == 7) value = &draft.study_color.blue;
      if (selected == 8) value = &draft.break_color.red;
      if (selected == 9) value = &draft.break_color.green;
      if (selected == 10) value = &draft.break_color.blue;
      if (selected == 11) value = &draft.long_break_color.red;
      if (selected == 12) value = &draft.long_break_color.green;
      if (selected == 13) value = &draft.long_break_color.blue;
      if (selected == 0 && (event == Event::ArrowLeft || event == Event::ArrowRight)) {
        draft.background_enabled = !draft.background_enabled;
      } else if (value) {
        const int minimum = selected == 4 ? 1 : 0;
        const int maximum = selected <= 4 ? 240 : 255;
        if (event == Event::ArrowLeft) *value = std::max(minimum, *value - 1);
        if (event == Event::ArrowRight) *value = std::min(maximum, *value + 1);
      }
      if (event == Event::Return) {
        timer_.ApplySettings(draft);
        SaveSettings(timer_.settings);
        settings_open = false;
      }
      return true;
    }
    if (event == Event::Character("q") || event == Event::Character("Q")) {
      active = false;
      screen_.Exit();
      return true;
    }
    if (event == Event::Character(" ")) {
      timer_.Toggle();
      return true;
    }
    if (event == Event::Character("r") || event == Event::Character("R")) {
      timer_.Reset();
      return true;
    }
    return false;
  });

  std::thread ticker([&] {
    while (active) {
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      if (active) screen_.PostEvent(Event::Custom);
    }
  });
  screen_.Loop(component);
  active = false;
  ticker.join();
}
