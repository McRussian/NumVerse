#pragma once

#include "game_cell.h"
#include <vector>
#include <cstdint>
#include <stdexcept>

class Board {
public:
    Board() = default;
    Board(uint8_t rows, uint8_t cols);

    GameCell&       at(uint8_t row, uint8_t col);
    const GameCell& at(uint8_t row, uint8_t col) const;
    bool            isValid(uint8_t row, uint8_t col) const;

    uint8_t rows() const;
    uint8_t cols() const;
    size_t  size() const;
    bool    empty() const;

private:
    std::vector<GameCell> m_cells;
    uint8_t m_rows = 0;
    uint8_t m_cols = 0;
};
