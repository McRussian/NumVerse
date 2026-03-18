# Архитектура NumVerse

## Контекст и цели

MVP реализуется на Qt/QWidgets (Linux Desktop) с единственной целью —
**отработать игровые механики**. Мобильная версия будет написана с нуля
на другом фреймворке, используя наработки MVP как референс.

Главный архитектурный принцип: **Qt только для отрисовки**.
Любой класс, содержащий логику (правила, подсчёт, сортировка, валидация) —
чистый C++ без Qt-зависимостей. Такой код покрывается unit-тестами (GTest)
и без изменений переносится на любую платформу.

---

## Принятые решения

| Вопрос | Решение |
|--------|---------|
| UI-фреймворк | Qt/QWidgets |
| Сборка | CMake |
| Отрисовка поля | QGraphicsScene |
| Стилизация UI | QSS + QPainter для игрового поля |
| Владение объектами | `std::unique_ptr` |
| Уровни | JSON-файлы (`data/levels/`) |
| Статистика | JSON-файл (`data/stats.json`) |
| Типы данных | минимально подходящие (`uint8_t`, `uint16_t`, `uint32_t`) |
| Unit-тесты | GTest (логика — чистый C++, Qt Test не нужен) |

---

## Принцип разделения Qt / чистый C++

| Что | Где | Тестируется |
|-----|-----|-------------|
| Правила игры | `IGameRules` / `*Rules` | да |
| Движок игры | `GameLogic` | да |
| Структуры данных | `Board`, `GameCell`, `Selection`, `LevelData`, `GameResult` | да |
| Подсчёт и сортировка результатов | `ScoreBoard` | да |
| Валидация уровня | `LevelData` | да |
| Отрисовка поля | `GameBoard` (QGraphicsScene) | нет |
| Таймер и UI-контейнер | `GameWindow` | нет |
| Меню и диалоги | `MainWindow`, `MenuWidget` | нет |
| Чтение/запись файлов | `LevelLoader`, `StatisticsManager` | нет |

---

## Паттерны

| Паттерн | Применение |
|---------|-----------|
| Strategy (Logic Layer) | `IGameRules` — интерфейс правил, каждая игра — конкретная стратегия |
| Strategy (Game Layer) | `AbstractGame` — интерфейс Qt-обёртки |
| Wrapper / Adapter | `*Game` классы оборачивают `GameLogic` в Qt-сигналы |
| MVC (упрощённый) | `GameLogic` — Model, `GameBoard` — View, `AbstractGame` — Controller |
| Singleton | `StatisticsManager` |

---

## Слои

```
┌──────────────────────────────────────────────────────────┐
│  UI Layer (Qt/QWidgets)                                  │
│  MainWindow · GameWindow · MenuWidget · Dialogs          │
├──────────────────────────────────────────────────────────┤
│  Game Layer (Qt/QObject)                                 │
│  AbstractGame · *Game (обёртки) · GameBoard              │
├──────────────────────────────────────────────────────────┤
│  Logic Layer (чистый C++) ← покрыт GTest                │
│  GameLogic · IGameRules · *Rules                         │
│  ScoreBoard · GameResult                                 │
│  GameConfig · GameState · GameCell · Board · Selection   │
│  LevelData                                               │
├──────────────────────────────────────────────────────────┤
│  Infrastructure (Qt)                                     │
│  StatisticsManager · LevelLoader                         │
│  (читают файлы, строят объекты Logic Layer)              │
└──────────────────────────────────────────────────────────┘
```

---

## Структура файлов

```
NumVerse/
├── CMakeLists.txt
│
├── src/
│   ├── main.cpp
│   │
│   ├── logic/                        # чистый C++, без Qt
│   │   ├── GameConfig.h
│   │   ├── GameState.h
│   │   ├── GameCell.h
│   │   ├── Board.h/.cpp
│   │   ├── Selection.h
│   │   ├── LevelData.h/.cpp          # структура уровня + валидация
│   │   ├── GameResult.h              # результат одной игры
│   │   ├── ScoreBoard.h/.cpp         # хранение, поиск лучшего, сортировка
│   │   ├── IGameRules.h
│   │   ├── GameLogic.h/.cpp
│   │   └── rules/
│   │       ├── NumberChaosRules.h/.cpp
│   │       ├── SumBoomRules.h/.cpp
│   │       ├── NumberTetrisRules.h/.cpp
│   │       ├── MagicSquareRules.h/.cpp
│   │       ├── Make24Rules.h/.cpp
│   │       └── NumberMazeRules.h/.cpp
│   │
│   ├── core/                         # Qt-ядро
│   │   ├── AbstractGame.h
│   │   ├── GameBoard.h/.cpp          # QGraphicsScene
│   │   ├── Make24Board.h/.cpp
│   │   ├── GameWindow.h/.cpp
│   │   ├── LevelLoader.h/.cpp        # QFile + QJsonDocument → LevelData
│   │   └── StatisticsManager.h/.cpp  # QFile + QJsonDocument → ScoreBoard
│   │
│   ├── games/
│   │   ├── NumberChaosGame.h/.cpp
│   │   ├── SumBoomGame.h/.cpp
│   │   ├── NumberTetrisGame.h/.cpp
│   │   ├── MagicSquareGame.h/.cpp
│   │   ├── Make24Game.h/.cpp
│   │   └── NumberMazeGame.h/.cpp
│   │
│   └── ui/
│       ├── MainWindow.h/.cpp
│       ├── MenuWidget.h/.cpp
│       └── SettingsDialog.h/.cpp
│
├── tests/
│   ├── CMakeLists.txt
│   ├── test_board.cpp
│   ├── test_game_logic.cpp
│   ├── test_number_chaos_rules.cpp
│   ├── test_sum_boom_rules.cpp
│   ├── test_number_tetris_rules.cpp
│   ├── test_magic_square_rules.cpp
│   ├── test_make24_rules.cpp
│   ├── test_number_maze_rules.cpp
│   ├── test_score_board.cpp
│   └── test_level_data.cpp
│
├── data/
│   ├── stats.json
│   └── levels/
│       ├── number-chaos.json
│       └── ...
│
├── resources/
│   ├── resources.qrc
│   └── styles/main.qss
│
└── doc/
```

---

## Logic Layer — детали

### Типы данных

| Сущность | Тип |
|----------|-----|
| Индекс строки/столбца | `uint8_t` |
| Значение ячейки | `uint16_t` |
| Очки | `uint32_t` |
| Время (секунды) | `uint32_t` |
| Число ходов | `uint16_t` |
| Целевая сумма | `uint16_t` |
| Уровень сложности | `uint8_t` (0=Easy, 1=Medium, 2=Hard) |
| Номер уровня | `uint16_t` |

### GameConfig

```cpp
struct GameConfig {
    uint8_t  gridRows;
    uint8_t  gridCols;
    uint8_t  difficulty;
    uint16_t maxMoves;        // 0 = без ограничений
    uint16_t timeLimitSecs;   // 0 = без ограничений
    uint16_t targetSum;
    uint16_t level;
};
```

### GameCell

```cpp
enum class CellState : uint8_t {
    Normal,
    Selected,
    Highlighted,
    Locked,
    Empty
};

struct GameCell {
    uint16_t  value;
    CellState state;
};
```

### Board

```cpp
struct Board {
    std::vector<GameCell> cells;
    uint8_t rows;
    uint8_t cols;

    GameCell&       at(uint8_t row, uint8_t col);
    const GameCell& at(uint8_t row, uint8_t col) const;
    bool            isValid(uint8_t row, uint8_t col) const;
};
```

### Selection

```cpp
struct Selection {
    std::vector<std::pair<uint8_t, uint8_t>> cells;

    void add(uint8_t row, uint8_t col);
    void remove(uint8_t row, uint8_t col);
    void clear();
    bool contains(uint8_t row, uint8_t col) const;
    bool empty() const;
};
```

### GameState

```cpp
enum class GameStatus : uint8_t {
    Idle,
    Playing,
    Paused,
    Won,
    Lost
};

struct GameState {
    GameStatus status;
    uint32_t   score;
    uint16_t   movesLeft;
    uint32_t   secondsElapsed;
    Board      board;
    Selection  selection;
};
```

### LevelData

Хранит данные уровня и умеет себя валидировать — без Qt.

```cpp
struct LevelData {
    uint16_t             level;
    uint8_t              difficulty;
    uint8_t              gridRows;
    uint8_t              gridCols;
    uint16_t             maxMoves;
    uint16_t             timeLimitSecs;
    uint16_t             targetSum;
    std::vector<uint16_t> board;   // плоский массив значений

    bool        isValid() const;   // размер board == rows*cols и т.д.
    GameConfig  toConfig() const;
};
```

### GameResult

```cpp
struct GameResult {
    uint16_t level;
    uint32_t score;
    uint32_t timeSecs;
    bool     won;
};
```

### ScoreBoard

Вся логика работы с результатами — без Qt:

```cpp
class ScoreBoard {
    std::vector<GameResult> m_results;

public:
    void             add(const GameResult& result);

    uint32_t         bestScore() const;
    uint32_t         bestTime() const;    // минимальное время победы
    uint16_t         totalPlayed() const;
    uint16_t         totalWon() const;

    // лучший результат по конкретному уровню
    std::optional<GameResult> bestForLevel(uint16_t level) const;

    // отсортированные результаты
    std::vector<GameResult> sortedByScore() const;
    std::vector<GameResult> sortedByTime() const;

    const std::vector<GameResult>& all() const;
};
```

### IGameRules

```cpp
class IGameRules {
public:
    virtual ~IGameRules() = default;

    virtual void     initBoard(Board& board, const GameConfig& cfg) = 0;
    virtual bool     isValidSelection(const Board& board,
                                      const Selection& sel) const = 0;
    virtual uint32_t applySelection(Board& board,
                                    const Selection& sel) const = 0;
    virtual bool     isWon(const Board& board) const = 0;
    virtual bool     isLost(const Board& board, uint16_t movesLeft) const = 0;
    virtual Selection getHint(const Board& board) const = 0;
    virtual void     tick(Board& board, uint32_t deltaMs) {}
};
```

### GameLogic

```cpp
class GameLogic {
    std::unique_ptr<IGameRules> m_rules;
    Board      m_board;
    Selection  m_selection;
    GameStatus m_status    = GameStatus::Idle;
    uint32_t   m_score     = 0;
    uint16_t   m_movesLeft = 0;
    uint32_t   m_elapsed   = 0;

public:
    explicit GameLogic(std::unique_ptr<IGameRules> rules);

    void      init(const GameConfig& cfg);
    void      reset();
    bool      selectCell(uint8_t row, uint8_t col);
    bool      confirmSelection();
    void      cancelSelection();
    void      tick(uint32_t deltaMs);
    void      addSecond();

    GameState  getState() const;
    GameResult buildResult() const;
    Selection  getHint() const;
};
```

---

## Infrastructure Layer — детали

### LevelLoader (Qt)

Читает JSON → строит `LevelData` (чистый C++) → валидирует:

```cpp
class LevelLoader {
public:
    // бросает std::runtime_error если файл не найден или LevelData::isValid() == false
    static LevelData load(const QString& gameName, uint16_t level);

private:
    static QString filePath(const QString& gameName);
    static LevelData parse(const QJsonObject& obj);
};
```

### StatisticsManager (Qt)

Читает/пишет JSON. Внутри держит `ScoreBoard` (чистый C++) на каждую игру:

```cpp
class StatisticsManager {
public:
    static StatisticsManager& instance();

    void             record(const QString& game, const GameResult& result);
    const ScoreBoard& scoreBoard(const QString& game) const;

    void load(const QString& path);
    void save(const QString& path);

private:
    std::unordered_map<std::string, ScoreBoard> m_boards;
    QString m_path;
};
```

Qt-виджет меню запрашивает `scoreBoard("NumberChaos")` и отображает —
вся логика сортировки и поиска лучшего уже в `ScoreBoard`.

---

## Game Layer — детали

### AbstractGame

```cpp
class AbstractGame : public QObject {
    Q_OBJECT
public:
    virtual void      start(const GameConfig& config) = 0;
    virtual void      reset() = 0;
    virtual void      pause() = 0;
    virtual void      resume() = 0;

    virtual void      onCellClicked(uint8_t row, uint8_t col) = 0;
    virtual void      onSelectionConfirmed() = 0;
    virtual void      onHintRequested() = 0;
    virtual void      tick() = 0;

    virtual GameState getState() const = 0;
    virtual QString   gameName() const = 0;

signals:
    void boardChanged();
    void gameFinished(bool won, uint32_t score, uint32_t timeSecs);
    void scoreChanged(uint32_t score);
    void movesLeftChanged(uint16_t movesLeft);
    void hintReady(QVector<QPoint> cells);
};
```

### Пример — NumberChaosGame

```cpp
class NumberChaosGame : public AbstractGame {
    Q_OBJECT
    GameLogic m_logic{ std::make_unique<NumberChaosRules>() };

public:
    QString gameName() const override { return "NumberChaos"; }

    void start(const GameConfig& cfg) override {
        m_logic.init(cfg);
        emit boardChanged();
    }

    void onCellClicked(uint8_t row, uint8_t col) override {
        if (m_logic.selectCell(row, col))
            emit boardChanged();
    }

    void onSelectionConfirmed() override {
        if (m_logic.confirmSelection()) {
            const auto state = m_logic.getState();
            emit scoreChanged(state.score);
            emit boardChanged();
            if (state.status == GameStatus::Won || state.status == GameStatus::Lost)
                emit gameFinished(state.status == GameStatus::Won,
                                  state.score, state.secondsElapsed);
        }
    }

    void onHintRequested() override {
        const auto sel = m_logic.getHint();
        QVector<QPoint> pts;
        pts.reserve(static_cast<qsizetype>(sel.cells.size()));
        for (auto [r, c] : sel.cells)
            pts << QPoint(r, c);
        emit hintReady(pts);
    }

    void tick() override {}

    GameState getState() const override { return m_logic.getState(); }
};
```

---

## Core Layer — детали

### GameBoard (QGraphicsScene)

```cpp
class GameBoard : public QGraphicsView {
    Q_OBJECT
    QGraphicsScene* m_scene;

public:
    void setBoard(const Board& board);
    void setSelection(const Selection& sel);
    void highlightCells(const QVector<QPoint>& cells);

signals:
    void cellClicked(uint8_t row, uint8_t col);
    void selectionConfirmed();
};
```

### GameWindow

```cpp
class GameWindow : public QWidget {
    Q_OBJECT

    std::unique_ptr<AbstractGame> m_game;
    GameBoard*  m_board;
    QTimer*     m_ticker;   // 100 мс → m_game->tick()
    QTimer*     m_clock;    // 1 с   → m_game->addSecond()
};
```

Связки сигнал-слот:
```
m_board::cellClicked        → m_game::onCellClicked
m_board::selectionConfirmed → m_game::onSelectionConfirmed
m_game::boardChanged        → GameWindow: обновить m_board из getState()
m_game::hintReady           → m_board::highlightCells
m_game::gameFinished        → GameWindow: показать результат + StatisticsManager::record
m_game::scoreChanged        → обновить QLabel
m_game::movesLeftChanged    → обновить QLabel
m_ticker::timeout           → m_game::tick
m_clock::timeout            → m_game::addSecond + обновить QLabel таймера
```

---

## Тесты

### Фреймворк: GTest

Вся тестируемая логика — чистый C++, Qt Test не нужен.
GTest подключается через CMake `FetchContent` или системный пакет (`gtest-devel`).

### Структура тестов

```
tests/
├── CMakeLists.txt
├── test_board.cpp              # Board::at, isValid, граничные случаи
├── test_selection.cpp          # add, remove, contains
├── test_level_data.cpp         # LevelData::isValid, toConfig
├── test_score_board.cpp        # bestScore, bestTime, sortedByScore, bestForLevel
├── test_game_logic.cpp         # init, selectCell, confirmSelection, tick
├── test_number_chaos_rules.cpp
├── test_sum_boom_rules.cpp
├── test_number_tetris_rules.cpp
├── test_magic_square_rules.cpp
├── test_make24_rules.cpp
└── test_number_maze_rules.cpp
```

### CMakeLists.txt для тестов

```cmake
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.14.0/googletest-1.14.0.tar.gz
)
FetchContent_MakeAvailable(googletest)

# Собираем только logic/ — без Qt
file(GLOB_RECURSE LOGIC_SOURCES ../src/logic/*.cpp)

add_executable(NumVerseTests
    ${LOGIC_SOURCES}
    test_board.cpp
    test_selection.cpp
    test_level_data.cpp
    test_score_board.cpp
    test_game_logic.cpp
    test_number_chaos_rules.cpp
    test_sum_boom_rules.cpp
    test_number_tetris_rules.cpp
    test_magic_square_rules.cpp
    test_make24_rules.cpp
    test_number_maze_rules.cpp
)

target_link_libraries(NumVerseTests PRIVATE GTest::gtest_main)
target_include_directories(NumVerseTests PRIVATE ../src)

include(GoogleTest)
gtest_discover_tests(NumVerseTests)
```

### Пример теста

```cpp
// test_score_board.cpp
#include <gtest/gtest.h>
#include "logic/ScoreBoard.h"

TEST(ScoreBoardTest, BestScoreReturnsMax) {
    ScoreBoard sb;
    sb.add({ .level = 1, .score = 100, .timeSecs = 60, .won = true });
    sb.add({ .level = 1, .score = 250, .timeSecs = 45, .won = true });
    sb.add({ .level = 2, .score = 80,  .timeSecs = 30, .won = false });

    EXPECT_EQ(sb.bestScore(), 250u);
}

TEST(ScoreBoardTest, BestTimeIgnoresLosses) {
    ScoreBoard sb;
    sb.add({ .level = 1, .score = 100, .timeSecs = 20, .won = false });
    sb.add({ .level = 1, .score = 150, .timeSecs = 60, .won = true });
    sb.add({ .level = 1, .score = 200, .timeSecs = 40, .won = true });

    EXPECT_EQ(sb.bestTime(), 40u);
}

TEST(ScoreBoardTest, BestForLevelReturnsOptional) {
    ScoreBoard sb;
    EXPECT_FALSE(sb.bestForLevel(1).has_value());

    sb.add({ .level = 1, .score = 300, .timeSecs = 50, .won = true });
    ASSERT_TRUE(sb.bestForLevel(1).has_value());
    EXPECT_EQ(sb.bestForLevel(1)->score, 300u);
}
```

---

## Корневой CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)
project(NumVerse VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

file(GLOB_RECURSE APP_SOURCES src/*.cpp)
file(GLOB_RECURSE APP_HEADERS src/*.h)

add_executable(NumVerse
    ${APP_SOURCES}
    ${APP_HEADERS}
    resources/resources.qrc
)

target_link_libraries(NumVerse PRIVATE Qt6::Widgets)
target_include_directories(NumVerse PRIVATE src)

enable_testing()
add_subdirectory(tests)
```

---

## Порядок разработки

1. **Скелет** — CMake (основной + тесты), проект и тесты компилируются
2. **Board, GameCell, Selection** — структуры + тесты
3. **LevelData, ScoreBoard** — чистая логика + тесты
4. **IGameRules, GameLogic** — движок + тесты
5. **NumberChaosRules** — первые правила + тесты
6. **GameBoard** — QGraphicsScene, рисует `Board`
7. **GameWindow** — таймер, кнопки, связки
8. **NumberChaosGame** — Qt-обёртка, первая играбельная игра
9. **MainWindow + MenuWidget** — выбор игры
10. **LevelLoader + StatisticsManager** — файлы
11. Следующие игры: `*Rules` + тесты + `*Game`

---

## Что НЕ входит в MVP

- Звуки и музыка
- Анимации (кроме подсветки)
- Редактор уровней
- Процедурная генерация уровней
- Онлайн-рекорды
- Локализация
