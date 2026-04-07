#include "stub_rules.h"
#include "logic/data/game_cell.h"

void StubRules::initBoard(Board& board, const GameConfig& config) {
    for (uint8_t r = 0; r < config.gridRows; ++r)
        for (uint8_t c = 0; c < config.gridCols; ++c)
            board.at(r, c) = GameCell(r * config.gridCols + c + 1);
}

void StubRules::applySelection(GameState& state, const GameConfig&) {
    state.score += 10;
    state.selection.clear();
}

Selection StubRules::getHint(const Board&) const {
    return {};
}
