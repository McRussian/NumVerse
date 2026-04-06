#pragma once

#include <cstdint>

enum class CellState : uint8_t {
    Normal,
    Selected,
    Highlighted,
    Locked,
    Empty
};

struct GameCell {
    uint16_t  value = 0;
    CellState state = CellState::Normal;

    bool operator==(const GameCell&) const = default;
};
