#include "board.h"

Board::Board(uint8_t rows, uint8_t cols)
    : m_cells(rows * cols)
    , m_rows(rows)
    , m_cols(cols)
{}

GameCell& Board::at(uint8_t row, uint8_t col)
{
    if (!isValid(row, col))
        throw std::out_of_range("Board::at — index out of range");
    return m_cells[row * m_cols + col];
}

const GameCell& Board::at(uint8_t row, uint8_t col) const
{
    if (!isValid(row, col))
        throw std::out_of_range("Board::at — index out of range");
    return m_cells[row * m_cols + col];
}

bool Board::isValid(uint8_t row, uint8_t col) const
{
    return row < m_rows && col < m_cols;
}

uint8_t Board::rows() const { return m_rows; }
uint8_t Board::cols() const { return m_cols; }
size_t  Board::size() const { return m_cells.size(); }
bool    Board::empty() const { return m_cells.empty(); }
