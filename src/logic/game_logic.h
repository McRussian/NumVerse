#pragma once

#include "data/game_config.h"
#include "data/game_state.h"
#include "data/game_result.h"
#include "rules/i_game_rules.h"
#include <memory>
#include <string>

// Центральный контроллер игровой сессии.
// Управляет состоянием, таймером, счётчиком ходов.
// Игровая логика (заполнение поля, оценка хода) делегируется IGameRules.
class GameLogic {
public:
    // Инициализировать новую сессию. Поле заполняется, статус → Playing.
    void init(GameConfig config, std::unique_ptr<IGameRules> rules);

    // Добавить/убрать ячейку из selection (toggle). Игнорируется если не Playing.
    void select(uint8_t row, uint8_t col);

    // Передать текущую selection в rules. Затем декрементировать movesLeft,
    // если лимит ненулевой и правила не выставили Won. Игнорируется если не Playing.
    void applySelection();

    // Продвинуть таймер на secs секунд. При достижении timeLimitSecs → Lost.
    // Игнорируется если не Playing.
    void tick(uint32_t secs);

    // Перезапустить сессию с теми же config и rules (без пересоздания).
    void reset();

    // Перетасовать значения непустых ячеек — выход из тупика.
    // Игнорируется если не Playing.
    void shuffle();

    // Завершить сессию поражением (игрок сдался).
    void forfeit();

    const GameState& getState() const;

    GameResult buildResult(const std::string& playerName) const;

    std::vector<Selection> getHint() const;

private:
    GameConfig m_config;
    GameState m_state;
    std::unique_ptr<IGameRules> m_rules;
};
