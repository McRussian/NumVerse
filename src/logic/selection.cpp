#include "selection.h"

#include <algorithm>

void Selection::add(uint8_t row, uint8_t col)
{
    if (!contains(row, col))
        m_cells.emplace_back(row, col);
}

void Selection::remove(uint8_t row, uint8_t col)
{
    auto it = std::find(m_cells.begin(), m_cells.end(), std::make_pair(row, col));
    if (it != m_cells.end())
        m_cells.erase(it);
}

void Selection::clear()
{
    m_cells.clear();
}

bool Selection::contains(uint8_t row, uint8_t col) const
{
    return std::find(m_cells.begin(), m_cells.end(), std::make_pair(row, col)) != m_cells.end();
}

bool Selection::empty() const
{
    return m_cells.empty();
}

size_t Selection::size() const
{
    return m_cells.size();
}

const std::vector<std::pair<uint8_t, uint8_t>>& Selection::cells() const
{
    return m_cells;
}
