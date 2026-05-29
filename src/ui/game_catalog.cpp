#include "game_catalog.h"

namespace {

// Mini 4x3 grid preview: row 1 shows a geometric sequence 4→8→16 highlighted.
void paintNumberChaos(QPainter& p, const QRect& r)
{
    static const int kValues[3][4] = {
        { 7,  3, 11,  5},
        { 4,  8, 16,  2},
        { 9,  1,  6, 14}
    };
    // highlighted = geometric sequence 4,8,16 at row 1 cols 0-2
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
    f.setBold(false);
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

} // namespace

std::vector<GameDescriptor> GameCatalog::allGames()
{
    return {
        {0, "Number Chaos", "Числовые последовательности", paintNumberChaos},
    };
}
