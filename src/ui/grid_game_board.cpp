#include "grid_game_board.h"
#include "game_cell_widget.h"
#include "theme.h"
#include "logic/data/board.h"
#include "logic/data/cell_state.h"
#include "logic/data/selection.h"

#include <QGridLayout>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>
#include <algorithm>

GridGameBoard::GridGameBoard(QWidget* parent)
    : AbstractGameBoard(parent)
    , m_layout(new QGridLayout(this))
{
    m_layout->setSpacing(Theme::CellSpacing);
    m_layout->setContentsMargins(Theme::GridPadding, Theme::GridPadding,
                                 Theme::GridPadding, Theme::GridPadding);
    setLayout(m_layout);
}

void GridGameBoard::updateBoard(const Board& board)
{
    if (board.rows() != m_rows || board.cols() != m_cols)
        rebuildGrid(board);

    for (int r = 0; r < m_rows; ++r)
        for (int c = 0; c < m_cols; ++c) {
            const auto& cell = board.at(r, c);
            m_cells[r][c]->setValue(cell.value());
            m_cells[r][c]->setState(cell.state());
        }
}

void GridGameBoard::highlightHint(const Selection& hint)
{
    for (auto [row, col] : m_hintCells)
        if (row < m_rows && col < m_cols)
            if (m_cells[row][col]->currentState() == CellState::Hinted)
                m_cells[row][col]->setState(CellState::Normal);
    m_hintCells.clear();

    for (auto [row, col] : hint.cells()) {
        if (row < m_rows && col < m_cols) {
            auto* cell = m_cells[row][col];
            if (cell->currentState() != CellState::Empty) {
                cell->setState(CellState::Hinted);
                m_hintCells.push_back({row, col});
            }
        }
    }
}

void GridGameBoard::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void GridGameBoard::rebuildGrid(const Board& board)
{
    while (auto* item = m_layout->takeAt(0)) {
        delete item->widget();
        delete item;
    }
    m_cells.clear();
    m_hintCells.clear();

    m_rows = board.rows();
    m_cols = board.cols();
    m_cells.assign(m_rows, std::vector<GameCellWidget*>(m_cols, nullptr));

    for (int r = 0; r < m_rows; ++r)
        for (int c = 0; c < m_cols; ++c) {
            auto* cell = new GameCellWidget(r, c, this);
            connect(cell, &GameCellWidget::clicked,      this, &GridGameBoard::cellClicked);
            connect(cell, &GameCellWidget::rightClicked, this, &GridGameBoard::cellRightClicked);
            m_layout->addWidget(cell, r, c);
            m_cells[r][c] = cell;
        }

    updateCellSizes();
}

QSize GridGameBoard::minimumSizeHint() const
{
    if (m_rows == 0 || m_cols == 0)
        return QWidget::minimumSizeHint();
    int w = m_cols * (Theme::CellMinSize + Theme::CellSpacing)
            - Theme::CellSpacing + 2 * Theme::GridPadding;
    int h = m_rows * (Theme::CellMinSize + Theme::CellSpacing)
            - Theme::CellSpacing + 2 * Theme::GridPadding;
    return QSize(w, h);
}

void GridGameBoard::resizeEvent(QResizeEvent* event)
{
    AbstractGameBoard::resizeEvent(event);
    updateCellSizes();
}

void GridGameBoard::updateCellSizes()
{
    if (m_rows == 0 || m_cols == 0)
        return;

    int spacingW = Theme::CellSpacing * (m_cols - 1) + Theme::GridPadding * 2;
    int spacingH = Theme::CellSpacing * (m_rows - 1) + Theme::GridPadding * 2;
    int cellW    = (width()  - spacingW) / m_cols;
    int cellH    = (height() - spacingH) / m_rows;
    int cellSize = std::max(Theme::CellMinSize, std::min(cellW, cellH));

    int fontSize = std::max(10, cellSize / 3);
    for (auto& row : m_cells)
        for (auto* cell : row) {
            cell->setFixedSize(cellSize, cellSize);
            cell->setFontSize(fontSize);
        }
}
