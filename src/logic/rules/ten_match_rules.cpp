#include "ten_match_rules.h"
#include "logic/data/game_cell.h"
#include "logic/data/cell_state.h"

#include <algorithm>
#include <random>

namespace {

Board collapseEmptyRows(const Board& board)
{
    const int cols = board.cols();
    std::vector<uint8_t> kept;
    for (uint8_t r = 0; r < board.rows(); ++r)
        for (int c = 0; c < cols; ++c) {
            const auto& cell = board.at(r, static_cast<uint8_t>(c));
            if (cell.state() != CellState::Empty && cell.value() != 0) {
                kept.push_back(r);
                break;
            }
        }

    if (kept.size() == static_cast<size_t>(board.rows()))
        return board;

    Board result(static_cast<uint8_t>(kept.size()),
                 static_cast<uint8_t>(cols));
    for (size_t nr = 0; nr < kept.size(); ++nr)
        for (int c = 0; c < cols; ++c)
            result.at(static_cast<uint8_t>(nr), static_cast<uint8_t>(c))
                = board.at(kept[nr], static_cast<uint8_t>(c));
    return result;
}

} // namespace

void TenMatchRules::initBoard(Board& board, const GameConfig& config)
{
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<uint16_t> dist(1, 9);

    for (uint8_t r = 0; r < config.gridRows; ++r)
        for (uint8_t c = 0; c < config.gridCols; ++c)
            board.at(r, c) = GameCell(dist(rng));
}

bool TenMatchRules::isMatch(uint16_t a, uint16_t b)
{
    return a == b || a + b == 10;
}

bool TenMatchRules::isNeighbor(const Board& board, int r1, int c1, int r2, int c2)
{
    const int cols = board.cols();
    const int rows = board.rows();

    auto isEmpty = [&](int r, int c) {
        return r >= 0 && r < rows && c >= 0 && c < cols
            && board.at(static_cast<uint8_t>(r), static_cast<uint8_t>(c)).state()
               == CellState::Empty;
    };

    // Type 1: same row, clear path
    if (r1 == r2) {
        int cMin = std::min(c1, c2), cMax = std::max(c1, c2);
        bool ok = true;
        for (int c = cMin + 1; c < cMax && ok; ++c)
            ok = isEmpty(r1, c);
        if (ok) return true;
    }

    // Type 2: same column, clear path
    if (c1 == c2) {
        int rMin = std::min(r1, r2), rMax = std::max(r1, r2);
        bool ok = true;
        for (int r = rMin + 1; r < rMax && ok; ++r)
            ok = isEmpty(r, c1);
        if (ok) return true;
    }

    // Type 3: row-major linear clear path (includes row-wrap adjacency)
    {
        int p1 = r1 * cols + c1;
        int p2 = r2 * cols + c2;
        if (p1 > p2) std::swap(p1, p2);
        bool ok = true;
        for (int p = p1 + 1; p < p2 && ok; ++p)
            ok = isEmpty(p / cols, p % cols);
        if (ok) return true;
    }

    // Type 4: diagonal clear path (|dr| == |dc|)
    {
        int dr = r2 - r1, dc = c2 - c1;
        if (std::abs(dr) == std::abs(dc) && dr != 0) {
            int stepR = dr > 0 ? 1 : -1;
            int stepC = dc > 0 ? 1 : -1;
            bool ok = true;
            int r = r1 + stepR, c = c1 + stepC;
            while (r != r2 && ok) {
                ok = isEmpty(r, c);
                r += stepR;
                c += stepC;
            }
            if (ok) return true;
        }
    }

    return false;
}

void TenMatchRules::applySelection(GameState& state, const GameConfig& config)
{
    const auto& cells = state.selection.cells();

    auto resetStates = [&] {
        for (auto [r, c] : cells)
            state.board.at(r, c).setState(CellState::Normal);
        state.selection.clear();
    };

    if (cells.size() != 2) {
        resetStates();
        return;
    }

    auto [r1, c1] = cells[0];
    auto [r2, c2] = cells[1];
    uint16_t v1 = state.board.at(r1, c1).value();
    uint16_t v2 = state.board.at(r2, c2).value();

    if (!isMatch(v1, v2) || !isNeighbor(state.board, r1, c1, r2, c2)) {
        resetStates();
        return;
    }

    state.board.at(r1, c1) = GameCell(0, CellState::Empty);
    state.board.at(r2, c2) = GameCell(0, CellState::Empty);
    state.score += static_cast<uint32_t>(config.scoreMultiplierPct) * 10 / 100;
    state.selection.clear();

    state.board = collapseEmptyRows(state.board);

    if (isBoardCleared(state.board))
        state.status = GameStatus::Won;
}

std::vector<Selection> TenMatchRules::getHint(const Board& board) const
{
    std::vector<std::pair<uint8_t, uint8_t>> cells;
    for (uint8_t r = 0; r < board.rows(); ++r)
        for (uint8_t c = 0; c < board.cols(); ++c) {
            const auto& cell = board.at(r, c);
            if (cell.state() != CellState::Empty && cell.value() != 0)
                cells.push_back({r, c});
        }

    std::vector<Selection> results;
    for (size_t i = 0; i < cells.size(); ++i) {
        for (size_t j = i + 1; j < cells.size(); ++j) {
            auto [r1, c1] = cells[i];
            auto [r2, c2] = cells[j];
            if (isMatch(board.at(r1,c1).value(), board.at(r2,c2).value())
                && isNeighbor(board, r1, c1, r2, c2))
            {
                Selection sel;
                sel.add(r1, c1);
                sel.add(r2, c2);
                results.push_back(sel);
            }
        }
    }
    return results;
}

bool TenMatchRules::isBoardCleared(const Board& board)
{
    for (uint8_t r = 0; r < board.rows(); ++r)
        for (uint8_t c = 0; c < board.cols(); ++c)
            if (board.at(r, c).state() != CellState::Empty)
                return false;
    return true;
}
