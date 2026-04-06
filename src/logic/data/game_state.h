#pragma once

#include "game_status.h"
#include "board.h"
#include "selection.h"
#include <cstdint>

// Снимок состояния игры в конкретный момент времени.
// Возвращается из GameLogic::getState() и передаётся в GameBoard для отрисовки.
// Не содержит логики — только данные.
struct GameState {
    GameStatus status         = GameStatus::Idle;
    uint32_t   score          = 0;
    uint16_t   movesLeft      = 0;
    uint32_t   secondsElapsed = 0;
    Board      board;
    Selection  selection;
};
