#include "game_catalog.h"
#include "game/number_chaos_game.h"
#include "game/ten_match_game.h"

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

// ── Ten Match ────────────────────────────────────────────────────────────────

void paintTenMatch(QPainter& p, const QRect& r)
{
    // 3×3 grid: show two highlighted pairs — equal (7,7) and sum-10 (3,7)
    static const int kValues[3][3] = {
        {3, 5, 7},
        {6, 4, 7},
        {8, 2, 1}
    };
    auto highlight = [](int row, int col) -> QColor {
        // Blue: (0,0)=3 and (2,1)=2 → wait, 3+2≠10, let me fix pairs
        // Blue: (0,0)=3 and (0,2)=7  → 3+7=10 ✓
        if (row == 0 && (col == 0 || col == 2)) return QColor(70, 130, 200);
        // Green: (1,2)=7 and (0,2)=7 → equal ✓  but (0,2) already blue
        // Green: (1,1)=4 and (2,2)=1... 4+1≠10
        // Green: (1,0)=6 and (2,2)=1... no
        // Use: (1,1)=4 and (2,0)=8 → no. Let's just use blue for sum pair
        // and green for equal pair on a different row
        // Values: row1=(6,4,7), row2=(8,2,1) → 4 and 6 sum to 10, both in diff rows
        // Let's simplify: row0=(3,5,7) - highlight col 0 and col 2 for 3+7
        //                 row1=(6,4,7) - highlight col 0,1 for 6+4
        if (row == 1 && (col == 0 || col == 1)) return QColor(100, 180, 100);
        return QColor(55, 55, 70);
    };

    constexpr int cells = 3;
    constexpr int cellW = 36, cellH = 36, gap = 3;
    const int gridW = cells * cellW + (cells - 1) * gap;
    const int gridH = cells * cellH + (cells - 1) * gap;
    const int ox = r.left() + (r.width()  - gridW) / 2;
    const int oy = r.top()  + (r.height() - gridH) / 2;

    QFont f = p.font();
    f.setPointSize(11);
    p.setFont(f);

    for (int row = 0; row < cells; ++row) {
        for (int col = 0; col < cells; ++col) {
            QRect cell(ox + col * (cellW + gap), oy + row * (cellH + gap), cellW, cellH);
            QColor bg = highlight(row, col);
            p.fillRect(cell, bg);
            bool bright = (bg != QColor(55, 55, 70));
            p.setPen(bright ? Qt::white : QColor(160, 160, 180));
            p.drawText(cell, Qt::AlignCenter, QString::number(kValues[row][col]));
        }
    }
}

GameConfig tenMatchConfig(Difficulty d)
{
    GameConfig c;
    c.difficulty = d;
    switch (d) {
    case Difficulty::Beginner: c.gridRows = 4; c.gridCols = 5; c.scoreMultiplierPct = 100; break;
    case Difficulty::Easy:     c.gridRows = 5; c.gridCols = 7; c.scoreMultiplierPct = 120; break;
    case Difficulty::Medium:   c.gridRows = 7; c.gridCols = 7; c.scoreMultiplierPct = 150; break;
    case Difficulty::Hard:     c.gridRows = 7; c.gridCols = 9; c.scoreMultiplierPct = 200; break;
    case Difficulty::Expert:   c.gridRows = 9; c.gridCols = 9; c.scoreMultiplierPct = 300; break;
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
        {
            1,
            "Десятки",
            "Убирай равные и дающие сумму 10",
            GameFeature::AppendRows,
            paintTenMatch,
            tenMatchConfig,
            [](std::string name, GameConfig cfg) -> AbstractGame* {
                return new TenMatchGame(std::move(name), cfg);
            }
        },
    };
}
