# NumVerse

**NumVerse** — контейнер числовых головоломок с единым интерфейсом и общей механикой.
Каждая игра использует поле с числами и набор математических правил, отличаясь темпом и логикой.

Аналог KSudoku, но для числовых паттернов: несколько игр — одно приложение.

---

## Игры

| # | Название | Механика | Статус |
|---|----------|----------|--------|
| 1 | [Числовой Хаос](doc/number-chaos.md) | Найди и удали скрытые числовые последовательности | В разработке |
| 2 | [Суммарный Бум](doc/sum-boom.md) | Выдели группы чисел с заданной суммой | Запланировано |
| 3 | [Числовой Тетрис](doc/number-tetris.md) | Числа падают сверху, очищай строки по сумме | Запланировано |
| 4 | [Магический Квадрат](doc/magic-square.md) | Расставь числа так, чтобы суммы совпадали | Запланировано |
| 5 | [Сделай 24](doc/make-24.md) | Получи 24 из 4 чисел с помощью арифметики | Запланировано |
| 6 | [Числовой Лабиринт](doc/number-maze.md) | Пройди по полю, двигаясь только на делители и кратные | Запланировано |

---

## Общие возможности

- Настройка сложности: 5 уровней (Beginner → Expert)
- Таймер и таблица рекордов по каждому уровню (топ по очкам и по времени)
- Система подсказок
- Единый дизайн: общие шрифты, цвета, анимации

---

## Технологии

- **Язык:** C++20
- **Фреймворк:** Qt6 / QWidgets
- **Сборка:** CMake
- **Тесты:** GTest (логика — чистый C++)
- **Платформа:** Linux (Desktop)

---

## Структура проекта

```
NumVerse/
├── src/
│   ├── logic/
│   │   ├── data/       — структуры данных (чистый C++)
│   │   ├── rules/      — правила конкретных игр (чистый C++)
│   │   ├── score_board — таблица рекордов
│   │   ├── i_game_rules, game_logic — игровой движок
│   ├── core/           — Qt-ядро (GameBoard, GameWindow)
│   ├── games/          — Qt-обёртки над логикой
│   └── ui/             — MainWindow, MenuWidget
├── tests/
│   ├── data/           — тесты структур данных
│   ├── rules/          — тесты правил игр
│   └── ...             — тесты движка и рекордов
├── data/               — JSON уровни и статистика
├── doc/                — описание игр и архитектуры
└── run_tests.sh        — запуск всех тестов
```

---

## План разработки

### Этап 1 — Скелет ✅
| | Задача |
|-|--------|
| ✅ | CMakeLists.txt (app + tests), main.cpp |
| ✅ | GTest, run_tests.sh |

### Этап 2 — Структуры данных ✅
| | Задача |
|-|--------|
| ✅ | `GameCell`, `CellState` |
| ✅ | `Board` |
| ✅ | `Selection` |
| ✅ | `GameConfig`, `Difficulty` |
| ✅ | `GameState`, `GameStatus` |
| ✅ | `GameResult` |

### Этап 3 — LevelData + ScoreBoard ✅
| | Задача |
|-|--------|
| ✅ | `LevelData` (isValid, toConfig) |
| ✅ | `ScoreBoard` (топ-N по уровням, RankingMode) |

### Этап 4 — Игровой движок
| | Задача |
|-|--------|
| ⬜ | `IGameRules` + `StubRules` |
| ⬜ | `GameLogic::init`, `reset`, `getState` |
| ⬜ | `GameLogic::selectCell`, `cancelSelection` |
| ⬜ | `GameLogic::confirmSelection` |
| ⬜ | `GameLogic::tick`, `addSecond` |
| ⬜ | `GameLogic::buildResult`, `getHint` |

### Этап 5 — NumberChaosRules
| | Задача |
|-|--------|
| ⬜ | `initBoard` |
| ⬜ | Типы последовательностей |
| ⬜ | `isValidSelection` |
| ⬜ | `applySelection` |
| ⬜ | `isWon`, `isLost` |
| ⬜ | `getHint` |

### Этап 6 — GameBoard (Qt)
| | Задача |
|-|--------|
| ⬜ | Отрисовка сетки по `Board` |
| ⬜ | Состояния ячеек |
| ⬜ | Клики → сигнал `cellClicked` |
| ⬜ | `setSelection`, `highlightCells` |

### Этап 7 — NumberChaosGame + GameWindow
| | Задача |
|-|--------|
| ⬜ | `AbstractGame` |
| ⬜ | `NumberChaosGame` |
| ⬜ | `GameWindow` (компоновка) |
| ⬜ | `GameWindow` таймеры + signal-slot |

### Этап 8 — UI-оболочка
| | Задача |
|-|--------|
| ⬜ | `MenuWidget` |
| ⬜ | `SettingsDialog` |
| ⬜ | `MainWindow` |

### Этап 9 — Persistence
| | Задача |
|-|--------|
| ⬜ | `LevelLoader` |
| ⬜ | `StatisticsManager` |
| ⬜ | Интеграция |

### Этап 10+ — Остальные игры
| | Задача |
|-|--------|
| ⬜ | SumBoom |
| ⬜ | NumberTetris |
| ⬜ | MagicSquare |
| ⬜ | Make24 |
| ⬜ | NumberMaze |
