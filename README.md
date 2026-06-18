# mini-redis Qt GUI Manager

**English** · [中文](#中文) · [Русский](#русский)

---

A feature-rich desktop GUI for managing a [mini-redis](https://github.com/bellgrell/mini-redis) server, built with **Qt 5.15.2** and **C++17**. Dark Catppuccin Mocha theme throughout.

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
│   └── ...
├── forms/             # Qt Designer .ui files
│   └── ...
├── mini-redis-ui.pro  # Qt project file
└── resources.qrc      # Qt resources
```

## License

MIT

---

<a id="中文"></a>

# mini-redis Qt 图形界面管理器

**中文** · [English](#english) · [Русский](#русский)

基于 **Qt 5.15.2** 和 **C++17** 构建的功能丰富的桌面 GUI，用于管理 [mini-redis](https://github.com/bellgrell/mini-redis) 服务器。全局 Catppuccin Mocha 暗色主题。

## 功能

### 控制台
- 输入任意 Redis 命令，实时查看响应
- 命令历史（↑/↓ 方向键）
- Tab 键自动补全 40+ Redis 命令
- 带时间戳的输出，响应颜色区分

### 键浏览器
- 使用 `KEYS *` 列出所有键
- **按类型过滤**：字符串、列表、集合、哈希、有序集合
- **搜索**键名
- 查看键详情：类型徽标、值、TTL、大小
- 复制值到剪贴板，确认后删除键

### 服务器信息
- 获取并显示 `INFO` 输出
- 解析为彩色分区（`# Server`、`# Keyspace` 等）
- 一键刷新

### 监视器
- 实时命令捕获（类似 Redis `MONITOR`）
- 启动/停止切换，带时间戳的日志和命令计数
- 捕获从任何面板发送的命令

### 命令参考
- 42 条内置 Redis 命令按组分类
- 按命令名称搜索
- 按类别过滤（连接、字符串、列表、集合等）
- 点击任意命令查看语法、摘要和描述

## 系统要求

- Qt 5.15.2 (MinGW 8.1.0 64-bit)
- MinGW 8.1.0 或更新版本
- 运行的 mini-redis 服务器（默认 `127.0.0.1:6379`）

## 构建

```bash
cd qt-ui
qmake mini-redis-ui.pro
mingw32-make -j4
```

或在 **Qt Creator** 中打开 `mini-redis-ui.pro` 并点击构建。

## 使用

1. 启动 mini-redis 服务器：
   ```bash
   cd mini-redis-main
   make
   ./mini-redis
   ```
2. 启动 GUI：
   ```bash
   cd qt-ui/release
   ./mini-redis-ui.exe
   ```
3. 连接（主机和端口默认 `127.0.0.1:6379`）

## 项目结构

```
├── src/               # C++ 源文件
├── include/           # C++ 头文件
├── forms/             # Qt Designer .ui 表单文件
├── mini-redis-ui.pro  # Qt 项目文件
└── resources.qrc      # Qt 资源文件
```

## 许可证

MIT

---

<a id="русский"></a>

# mini-redis Qt Менеджер

**Русский** · [English](#english) · [中文](#中文)

Многофункциональный desktop GUI для управления [mini-redis](https://github.com/bellgrell/mini-redis) сервером, созданный на **Qt 5.15.2** и **C++17**. Тёмная тема Catppuccin Mocha.

## Возможности

### Консоль
- Введите любую Redis команду, мгновенный ответ
- История команд (↑/↓)
- Автодополнение Tab для 40+ команд
- Вывод с временными метками и цветовой кодировкой

### Обзор ключей
- Список всех ключей через `KEYS *`
- **Фильтр по типу**: String, List, Set, Hash, Sorted Set
- **Поиск** по имени ключа
- Просмотр деталей: тип, значение, TTL, размер
- Копирование значения, удаление ключа с подтверждением

### Инфо сервера
- Получение и отображение `INFO`
- Разбор на цветные секции
- Обновление в один клик

### Монитор
- Захват команд в реальном времени (аналог Redis `MONITOR`)
- Вкл/выкл, логи с временными метками
- Перехватывает команды из любой панели

### Справка команд
- 42 встроенные Redis команды по группам
- Поиск по имени
- Фильтр по категории
- Клик для просмотра синтаксиса и описания

## Требования

- Qt 5.15.2 (MinGW 8.1.0 64-bit)
- MinGW 8.1.0 или новее
- Запущенный mini-redis сервер (по умолч. `127.0.0.1:6379`)

## Сборка

```bash
cd qt-ui
qmake mini-redis-ui.pro
mingw32-make -j4
```

Или откройте `mini-redis-ui.pro` в **Qt Creator** и нажмите Собрать.

## Использование

1. Запустите mini-redis сервер:
   ```bash
   cd mini-redis-main
   make
   ./mini-redis
   ```
2. Запустите GUI:
   ```bash
   cd qt-ui/release
   ./mini-redis-ui.exe
   ```
3. Подключитесь (хост и порт по умолч. `127.0.0.1:6379`)

## Структура проекта

```
├── src/               # C++ исходники
├── include/           # C++ заголовки
├── forms/             # .ui файлы Qt Designer
├── mini-redis-ui.pro  # Qt проект
└── resources.qrc      # Qt ресурсы
```

## Лицензия

MIT
