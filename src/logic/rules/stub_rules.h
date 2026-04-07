#pragma once

#include "i_game_rules.h"

// Заглушка правил для тестирования GameLogic.
// initBoard: заполняет ячейки значениями 1..N (row-major).
// applySelection: добавляет 10 очков, очищает selection.
class StubRules : public IGameRules {
public:
    void initBoard(Board& board, const GameConfig& config) override;
    void applySelection(GameState& state, const GameConfig& config) override;
};
