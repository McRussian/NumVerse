#pragma once

#include "abstract_game_board.h"
#include <vector>

class GameCellWidget;
class QGridLayout;

class GridGameBoard : public AbstractGameBoard {
    Q_OBJECT
public:
    explicit GridGameBoard(QWidget* parent = nullptr);

    void updateBoard(const Board& board) override;
    void highlightHint(const Selection& hint) override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QGridLayout*                              m_layout;
    std::vector<std::vector<GameCellWidget*>> m_cells;
    std::vector<std::pair<int,int>>           m_hintCells;
    int m_rows     = 0;
    int m_cols     = 0;
    int m_cellSize = 0; // locked on first resize, 0 = not yet calculated

    void rebuildGrid(const Board& board);
    void updateCellSizes();
};
