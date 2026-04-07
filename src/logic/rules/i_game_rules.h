#pragma once

#include "logic/data/board.h"
#include "logic/data/game_config.h"
#include "logic/data/game_state.h"

// Интерфейс игровых правил. Каждая игра предоставляет свою реализацию.
// GameLogic владеет экземпляром и делегирует ему заполнение поля и обработку ходов.
class IGameRules {
public:
    virtual ~IGameRules() = default;

    // Заполнить поле начальными значениями перед стартом.
    virtual void initBoard(Board& board, const GameConfig& config) = 0;

    // Обработать текущую selection из state: начислить очки, убрать ячейки, выставить Won.
    // Обязан очистить state.selection перед возвратом.
    virtual void applySelection(GameState& state, const GameConfig& config) = 0;

    // Найти любую валидную последовательность на поле и вернуть её как Selection.
    // Ячейки в Selection идут в порядке чтения (row-major).
    // Возвращает пустой Selection если валидных ходов нет.
    virtual Selection getHint(const Board& board) const = 0;
};
