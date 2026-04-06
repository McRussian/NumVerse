#pragma once

#include <cstdint>

enum class CellState : uint8_t {
    Normal,
    Selected,
    Highlighted,
    Locked,
    Empty
};
