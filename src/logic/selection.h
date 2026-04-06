#pragma once

#include <vector>
#include <cstdint>

class Selection {
public:
    void add(uint8_t row, uint8_t col);
    void remove(uint8_t row, uint8_t col);
    void clear();

    bool contains(uint8_t row, uint8_t col) const;
    bool empty() const;
    size_t size() const;

    const std::vector<std::pair<uint8_t, uint8_t>>& cells() const;

private:
    std::vector<std::pair<uint8_t, uint8_t>> m_cells;
};
