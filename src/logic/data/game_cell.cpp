#include "game_cell.h"

GameCell::GameCell(uint16_t value, CellState state)
    : m_value(value)
    , m_state(state)
{}

uint16_t GameCell::value() const
{
    return m_value;
}

CellState GameCell::state() const
{
    return m_state;
}

void GameCell::setValue(uint16_t value)
{
    m_value = value;
}

void GameCell::setState(CellState state)
{
    m_state = state;
}
