#pragma once

#include "cell_state.h"

class GameCell {
public:
    GameCell() = default;
    GameCell(uint16_t value, CellState state = CellState::Normal);

    uint16_t  value() const;
    CellState state() const;

    void setValue(uint16_t value);
    void setState(CellState state);

    bool operator==(const GameCell&) const = default;

private:
    uint16_t  m_value = 0;
    CellState m_state = CellState::Normal;
};
