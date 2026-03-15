# NOX

NOX is a fast, keyboard-first application launcher and command palette for Linux, inspired by Raycast and built with C++17, Qt 6, and QML.

It is designed to feel lightweight, native, and immediate: open the palette, type, navigate with the keyboard, and launch what you need without breaking flow.

Repository: https://github.com/brsrk94/Nox

## Highlights

- Fast floating launcher UI with a centered command palette
- Linux desktop app search powered by `.desktop` files
- Command mode with `>` prefixes
- Instant calculator detection and preview
- Clipboard history search
- File search across common user directories
- Dark, modern Qt Quick interface with `Outfit` typography
- Plugin-oriented architecture for calculator, system, and future extensions
- AppImage and Debian package release targets

## Current Features

### Application launcher

NOX scans installed desktop applications and lets you search and launch them from one palette.

### Command runner

Use `>` to enter command mode:

```text
> shutdown
> restart
> kill firefox
> open downloads
```

### Calculator

Type a mathematical expression directly:

```text
52 * 8
(14 + 6) / 2
```

### Clipboard history

Recent clipboard entries are stored in memory and can be searched and copied back quickly.

### File search

NOX indexes common locations such as:

- Home
- Downloads
- Documents

## Keyboard Shortcuts

- `Up` / `Down`: Navigate results
- `Enter`: Run selected result
- `Tab`: Open action menu
- `Esc`: Close launcher

Global launcher shortcuts:

- `Alt + Space`
- `Super + N`

## Linux Session Notes

- Global shortcuts are implemented in-app for X11 sessions.
- Wayland compositors generally require compositor-specific APIs or desktop-specific bindings for true global shortcuts.
- A helper script is included for GNOME custom keybindings:

```bash
./scripts/setup_gnome_shortcut.sh "$HOME/Applications/NOX-0.1.0-x86_64.AppImage"
```

## Releases

Releases will be published here:

https://github.com/brsrk94/Nox/releases

Planned release assets:

- `nox_0.1.0_amd64.deb`
- `NOX-0.1.0-x86_64.AppImage`

### Install the Debian package

```bash
sudo dpkg -i nox_0.1.0_amd64.deb
```

If Debian or Ubuntu reports missing dependencies:

```bash
sudo apt -f install
```

### Run the AppImage

```bash
chmod +x NOX-0.1.0-x86_64.AppImage
./NOX-0.1.0-x86_64.AppImage
```

On systems where FUSE is unavailable:

```bash
APPIMAGE_EXTRACT_AND_RUN=1 ./NOX-0.1.0-x86_64.AppImage
```

## Build From Source

### Requirements

- CMake 3.21+
- C++17 compiler
- Qt 6 with:
  - Core
  - Gui
  - Qml
  - Quick
  - QuickControls2
  - Concurrent
- X11 development headers for X11 global shortcut support

### Build

```bash
cmake -S . -B build
cmake --build build -j
./build/nox
```

If Qt is installed in a custom location:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/qt
cmake --build build -j
```

## Packaging

Build release artifacts locally with:

```bash
./scripts/build_deb.sh
./scripts/build_appimage.sh
```

Generated packages are written to:

- `dist/nox_0.1.0_amd64.deb`
- `dist/NOX-0.1.0-x86_64.AppImage`

## Project Structure

```text
nox/
├── core/
├── plugins/
├── resources/
├── scripts/
├── ui/
└── main.cpp
```

## Status

NOX is functional today as a base launcher and packaging-ready desktop application. The project is still early and will continue to improve in search quality, plugin loading, Wayland support, and overall polish.

## Roadmap

- Dynamic plugin loading from `/plugins`
- Better fuzzy ranking and indexing
- Improved Wayland-friendly launcher activation
- More actions per result
- Better previews for files and clipboard content
- Additional system and developer-focused plugins

## License

This project should include a top-level license file before public release. If you want, the next step can be adding an explicit open-source license such as MIT or GPL-3.0.
