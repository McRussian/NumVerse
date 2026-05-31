#include "number_chaos_game.h"
#include "logic/rules/number_chaos_rules.h"
#include <algorithm>

NumberChaosGame::NumberChaosGame(std::string playerName, GameConfig config, QObject* parent)
    : AbstractGame(std::move(playerName), std::move(config), parent)
{}

void NumberChaosGame::selectCell(int row, int col)
{
    // Enforce H/V: all selected cells must be in the same row or same column
    const auto& selCells = state().selection.cells();
    if (!selCells.empty()) {
        bool sameRow = std::all_of(selCells.begin(), selCells.end(),
                                   [row](auto p) { return p.first == row; });
        bool sameCol = std::all_of(selCells.begin(), selCells.end(),
                                   [col](auto p) { return p.second == col; });
        if (!sameRow && !sameCol) return;
    }
    AbstractGame::selectCell(row, col);
}

std::unique_ptr<IGameRules> NumberChaosGame::createRules() const
{
    return std::make_unique<NumberChaosRules>();
}
