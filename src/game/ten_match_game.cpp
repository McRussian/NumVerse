#include "ten_match_game.h"
#include "logic/rules/ten_match_rules.h"
#include "logic/data/cell_state.h"

TenMatchGame::TenMatchGame(std::string playerName, GameConfig config, QObject* parent)
    : AbstractGame(std::move(playerName), std::move(config), parent)
{}

std::unique_ptr<IGameRules> TenMatchGame::createRules() const
{
    return std::make_unique<TenMatchRules>();
}

void TenMatchGame::selectCell(int row, int col)
{
    AbstractGame::selectCell(row, col);
    // Auto-apply when exactly 2 cells are selected
    if (state().selection.cells().size() == 2)
        AbstractGame::applySelection();
}

void TenMatchGame::append()
{
    const Board& current = state().board;
    const int rows = current.rows();
    const int cols = current.cols();

    std::vector<uint16_t> remaining;
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c) {
            const auto& cell = current.at(
                static_cast<uint8_t>(r), static_cast<uint8_t>(c));
            if (cell.state() != CellState::Empty && cell.value() != 0)
                remaining.push_back(cell.value());
        }

    if (remaining.empty()) return;

    const int extraRows = (static_cast<int>(remaining.size()) + cols - 1) / cols;
    Board newBoard(static_cast<uint8_t>(rows + extraRows),
                   static_cast<uint8_t>(cols));

    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            newBoard.at(static_cast<uint8_t>(r), static_cast<uint8_t>(c))
                = current.at(static_cast<uint8_t>(r), static_cast<uint8_t>(c));

    size_t idx = 0;
    for (int r = rows; r < rows + extraRows; ++r)
        for (int c = 0; c < cols; ++c, ++idx)
            newBoard.at(static_cast<uint8_t>(r), static_cast<uint8_t>(c))
                = (idx < remaining.size())
                    ? GameCell(remaining[idx])
                    : GameCell(0, CellState::Empty);

    replaceBoardAndEmit(std::move(newBoard));
}
