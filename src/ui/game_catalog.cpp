#include "game_catalog.h"
#include "game/number_chaos_game.h"

namespace {

void paintNumberChaos(QPainter& p, const QRect& r)
{
    static const int kValues[3][4] = {
        { 7,  3, 11,  5},
        { 4,  8, 16,  2},
        { 9,  1,  6, 14}
    };
    auto highlighted = [](int row, int col) {
        return row == 1 && col >= 0 && col <= 2;
    };

    constexpr int cols = 4, rows = 3;
    constexpr int cellW = 32, cellH = 32, gap = 3;
    const int gridW = cols * cellW + (cols - 1) * gap;
    const int gridH = rows * cellH + (rows - 1) * gap;
    const int ox = r.left() + (r.width()  - gridW) / 2;
    const int oy = r.top()  + (r.height() - gridH) / 2;

    QFont f = p.font();
    f.setPointSize(10);
    p.setFont(f);

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            QRect cell(ox + col * (cellW + gap), oy + row * (cellH + gap), cellW, cellH);
            bool hi = highlighted(row, col);
            p.fillRect(cell, hi ? QColor(70, 130, 200) : QColor(55, 55, 70));
            p.setPen(hi ? Qt::white : QColor(160, 160, 180));
            p.drawText(cell, Qt::AlignCenter, QString::number(kValues[row][col]));
        }
    }
}

GameConfig numberChaosConfig(Difficulty d)
{
    GameConfig c;
    c.difficulty = d;
    switch (d) {
    case Difficulty::Beginner: c.gridRows = 3; c.gridCols = 4; c.scoreMultiplierPct = 100; break;
    case Difficulty::Easy:     c.gridRows = 4; c.gridCols = 5; c.scoreMultiplierPct = 120; break;
    case Difficulty::Medium:   c.gridRows = 5; c.gridCols = 6; c.scoreMultiplierPct = 150; break;
    case Difficulty::Hard:     c.gridRows = 5; c.gridCols = 7; c.scoreMultiplierPct = 200; break;
    case Difficulty::Expert:   c.gridRows = 6; c.gridCols = 8; c.scoreMultiplierPct = 300; break;
    }
    return c;
}

} // namespace

std::vector<GameDescriptor> GameCatalog::allGames()
{
    return {
        {
            0,
            "Number Chaos",
            "Числовые последовательности",
            GameFeature::ApplySelection,
            paintNumberChaos,
            numberChaosConfig,
            [](std::string name, GameConfig cfg) -> AbstractGame* {
                return new NumberChaosGame(std::move(name), cfg);
            }
        },
    };
}
