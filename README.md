# Studytime

A small cross-platform Pomodoro timer for the terminal, built with C++ and [FTXUI](https://github.com/ArthurSonzogni/FTXUI).

The timer renders a large centered rounded panel with a horizontal fill that
advances from left to right inside its border as time passes. The countdown stays centered in the panel,
and settings are stored in the platform's user configuration directory.

## Demo

![Studytime demo](assets/demo.gif)

## Build

Clone with the submodule and build with CMake/Ninja:

```bash
git clone --recurse-submodules https://github.com/muntalee/studytime
cd studytime
make compile
make run
```

Or use CMake directly:

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/studytime
```

## Controls

| Key | Action |
|---|---|
| `Space` | Start or pause |
| `R` | Reset the current phase |
| `Ctrl+P` | Open settings |
| `Q` | Quit |


Inside settings, use the arrow keys to select and change values, `Enter` to
save, or `Escape` to cancel. The solid timer panel can be disabled, and each
phase has an RGB color editor. The defaults are red for focus, green for short
breaks, and blue for long breaks. This RGB editor is used instead of a
mouse-based color picker because it works consistently across terminals and
Wayland sessions.

## Notifications

When a phase ends, Studytime emits the terminal bell and sends a desktop
notification. On Linux it uses `notify-send`, which works on Wayland/Sway
through the desktop notification D-Bus service. Install and run a notification
daemon such as `mako` or `swaync` if notifications are not visible. The
terminal bell still works without one.
