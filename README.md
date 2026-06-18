# mini-redis Qt GUI Manager

A feature-rich desktop GUI for managing a [mini-redis](https://github.com/bellgrell/mini-redis) server, built with **Qt 5.15.2** and **C++17**. Dark Catppuccin Mocha theme throughout.

## Screenshots

<!-- Add screenshots here -->

## Features

### Console
- Type any Redis command, see responses in real time
- Command history (↑/↓ arrow keys)
- Tab autocomplete for 40+ Redis commands
- Timestamped output with color-coded responses

### Key Browser
- List all keys with `KEYS *`
- **Filter by type**: String, List, Set, Hash, Sorted Set
- **Search** through keys by name
- View key details: type badge, value, TTL, size
- Copy value to clipboard, delete key with confirmation

### Server Info
- Fetch and display `INFO` output
- Parsed into color-coded sections (`# Server`, `# Keyspace`, etc.)
- One-click refresh

### Monitor
- Real-time command capture (like Redis `MONITOR`)
- Toggle start/stop, timestamped log with command count
- Captures commands sent from any panel

### Command Reference
- 42 built-in Redis commands organized by group
- Search by command name
- Filter by category (Connection, Strings, Lists, Sets, Hashes, etc.)
- Click any command to see syntax, summary, and description

## Requirements

- Qt 5.15.2 (MinGW 8.1.0 64-bit)
- MinGW 8.1.0 or newer
- A running mini-redis server (default: `127.0.0.1:6379`)

## Build

```bash
cd qt-ui
qmake mini-redis-ui.pro
mingw32-make -j4
```

Or open `mini-redis-ui.pro` in **Qt Creator** and click Build.

## Usage

1. Start the mini-redis server:
   ```bash
   cd mini-redis-main
   make
   ./mini-redis
   ```
2. Launch the GUI:
   ```bash
   cd qt-ui/release
   ./mini-redis-ui.exe
   ```
3. Connect (host and port default to `127.0.0.1:6379`)

## Project Structure

```
├── src/               # C++ source files
│   ├── main.cpp
│   ├── mainwindow.cpp
│   ├── consolepanel.cpp
│   ├── keybrowserpanel.cpp
│   ├── infopanel.cpp
│   ├── monitorpanel.cpp
│   ├── cheatsheetpanel.cpp
│   ├── redisclient.cpp
│   └── resparser.cpp
├── include/           # C++ headers
│   └── ...           # (same names as .cpp files)
├── forms/             # Qt Designer .ui files
│   └── ...           # (design-mode editable)
├── mini-redis-ui.pro  # Qt project file
└── resources.qrc      # Qt resources
```

## License

MIT
