#include "number_chaos_rules.h"
#include "logic/data/game_cell.h"
#include <cmath>
#include <algorithm>

// Факториалы, умещающиеся в uint16_t: 1!..8!
static const std::vector<uint16_t> FACTORIALS = {1, 2, 6, 24, 120, 720, 5040, 40320};

void NumberChaosRules::initBoard(Board& board, const GameConfig& config) {
    uint16_t n = 0;
    for (uint8_t r = 0; r < config.gridRows; ++r)
        for (uint8_t c = 0; c < config.gridCols; ++c)
            board.at(r, c) = GameCell(++n);
}

void NumberChaosRules::applySelection(GameState& state, const GameConfig&) {
    const auto& cells = state.selection.cells();

    if (cells.size() < 3) {
        state.selection.clear();
        return;
    }

    std::vector<uint16_t> values;
    values.reserve(cells.size());
    for (auto [r, c] : cells)
        values.push_back(state.board.at(r, c).value());

    if (!isValidSequence(values)) {
        state.selection.clear();
        return;
    }

    for (auto [r, c] : cells)
        state.board.at(r, c) = GameCell(0, CellState::Empty);

    state.score += calcScore(cells.size());
    state.selection.clear();

    if (isBoardCleared(state.board))
        state.status = GameStatus::Won;
}

uint32_t NumberChaosRules::calcScore(size_t length) {
    return static_cast<uint32_t>(length) * 10;
}

bool NumberChaosRules::isValidSequence(const std::vector<uint16_t>& v) {
    if (v.size() < 3) return false;
    return isArithmetic(v)  ||
           isGeometric(v)   ||
           isFibonacci(v)   ||
           isSquares(v)     ||
           isAllEven(v)     ||
           isAllOdd(v)      ||
           isAllPrime(v)    ||
           isFactorial(v)   ||
           isSecondOrder(v);
}

bool NumberChaosRules::isArithmetic(const std::vector<uint16_t>& v) {
    int diff = (int)v[1] - (int)v[0];
    if (diff == 0) return false;
    for (size_t i = 2; i < v.size(); ++i)
        if ((int)v[i] - (int)v[i - 1] != diff) return false;
    return true;
}

bool NumberChaosRules::isGeometric(const std::vector<uint16_t>& v) {
    for (auto val : v)
        if (val == 0) return false;
    if (v[0] == v[1]) return false;

    if (v[0] < v[1]) {
        if (v[1] % v[0] != 0) return false;
        uint16_t r = v[1] / v[0];
        if (r < 2) return false;
        for (size_t i = 2; i < v.size(); ++i)
            if (v[i] % v[i - 1] != 0 || v[i] / v[i - 1] != r) return false;
    } else {
        if (v[0] % v[1] != 0) return false;
        uint16_t r = v[0] / v[1];
        if (r < 2) return false;
        for (size_t i = 2; i < v.size(); ++i)
            if (v[i - 1] % v[i] != 0 || v[i - 1] / v[i] != r) return false;
    }
    return true;
}

bool NumberChaosRules::isFibonacci(const std::vector<uint16_t>& v) {
    for (size_t i = 1; i < v.size(); ++i)
        if (v[i] < v[i - 1]) return false;
    for (size_t i = 2; i < v.size(); ++i)
        if ((uint32_t)v[i - 1] + (uint32_t)v[i - 2] != v[i]) return false;
    return true;
}

bool NumberChaosRules::isSquares(const std::vector<uint16_t>& v) {
    std::vector<uint16_t> roots;
    roots.reserve(v.size());
    for (auto val : v) {
        auto r = (uint16_t)std::round(std::sqrt((double)val));
        if (r * r != val) return false;
        roots.push_back(r);
    }
    int diff = (int)roots[1] - (int)roots[0];
    if (diff != 1 && diff != -1) return false;
    for (size_t i = 2; i < roots.size(); ++i)
        if ((int)roots[i] - (int)roots[i - 1] != diff) return false;
    return true;
}

bool NumberChaosRules::isAllEven(const std::vector<uint16_t>& v) {
    for (auto val : v)
        if (val % 2 != 0) return false;
    return true;
}

bool NumberChaosRules::isAllOdd(const std::vector<uint16_t>& v) {
    for (auto val : v)
        if (val % 2 != 1) return false;
    return true;
}

bool NumberChaosRules::isAllPrime(const std::vector<uint16_t>& v) {
    for (auto val : v)
        if (!isPrime(val)) return false;
    return true;
}

bool NumberChaosRules::isPrime(uint16_t n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (uint16_t i = 3; (uint32_t)i * i <= n; i += 2)
        if (n % i == 0) return false;
    return true;
}

bool NumberChaosRules::isFactorial(const std::vector<uint16_t>& v) {
    auto it = std::find(FACTORIALS.begin(), FACTORIALS.end(), v[0]);
    if (it == FACTORIALS.end()) return false;
    size_t idx = static_cast<size_t>(it - FACTORIALS.begin());
    for (size_t i = 1; i < v.size(); ++i) {
        if (idx + i >= FACTORIALS.size()) return false;
        if (v[i] != FACTORIALS[idx + i]) return false;
    }
    return true;
}

bool NumberChaosRules::isSecondOrder(const std::vector<uint16_t>& v) {
    if (v.size() < 4) return false;
    std::vector<int> diffs;
    diffs.reserve(v.size() - 1);
    for (size_t i = 1; i < v.size(); ++i)
        diffs.push_back((int)v[i] - (int)v[i - 1]);
    int second_diff = diffs[1] - diffs[0];
    if (second_diff == 0) return false;
    for (size_t i = 2; i < diffs.size(); ++i)
        if (diffs[i] - diffs[i - 1] != second_diff) return false;
    return true;
}

bool NumberChaosRules::isBoardCleared(const Board& board) {
    for (uint8_t r = 0; r < board.rows(); ++r)
        for (uint8_t c = 0; c < board.cols(); ++c)
            if (board.at(r, c).state() != CellState::Empty) return false;
    return true;
}
