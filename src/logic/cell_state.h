#pragma once

#include <cstdint>

// Визуальное состояние ячейки на игровом поле.
// Используется в GameCell и читается GameBoard для выбора стиля отрисовки.
enum class CellState : uint8_t {
    Normal,
    Selected,
    Highlighted,
    Locked,
    Empty
};
