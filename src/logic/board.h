#pragma once

#include "game_cell.h"
#include <vector>
#include <cstdint>
#include <stdexcept>

struct Board {
    std::vector<GameCell> cells;
    uint8_t rows = 0;
    uint8_t cols = 0;

    Board() = default;
    Board(uint8_t rows, uint8_t cols);

    GameCell&       at(uint8_t row, uint8_t col);
    const GameCell& at(uint8_t row, uint8_t col) const;
    bool            isValid(uint8_t row, uint8_t col) const;
};
