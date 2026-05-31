#pragma once

#include "i_game_rules.h"

class TenMatchRules : public IGameRules {
public:
    void initBoard(Board& board, const GameConfig& config) override;
    void applySelection(GameState& state, const GameConfig& config) override;
    std::vector<Selection> getHint(const Board& board) const override;

    static bool isMatch(uint16_t a, uint16_t b);
    static bool isNeighbor(const Board& board, int r1, int c1, int r2, int c2);
    static Selection getAllNeighbors(const Board& board, int row, int col);

private:
    static bool isBoardCleared(const Board& board);
};
