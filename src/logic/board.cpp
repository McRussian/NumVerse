#include "board.h"

Board::Board(uint8_t rows, uint8_t cols)
    : cells(rows * cols)
    , rows(rows)
    , cols(cols)
{}

GameCell& Board::at(uint8_t row, uint8_t col)
{
    if (!isValid(row, col))
        throw std::out_of_range("Board::at — index out of range");
    return cells[row * cols + col];
}

const GameCell& Board::at(uint8_t row, uint8_t col) const
{
    if (!isValid(row, col))
        throw std::out_of_range("Board::at — index out of range");
    return cells[row * cols + col];
}

bool Board::isValid(uint8_t row, uint8_t col) const
{
    return row < rows && col < cols;
}
