#include "game_cell_widget.h"

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QVBoxLayout>

GameCellWidget::GameCellWidget(int row, int col, QWidget* parent)
    : QWidget(parent)
    , m_row(row)
    , m_col(col)
    , m_label(new QLabel(this))
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_label, 0, Qt::AlignCenter);
    setLayout(layout);

    m_label->setAlignment(Qt::AlignCenter);
    setState(CellState::Normal);
}

void GameCellWidget::setValue(uint16_t value)
{
    m_label->setText(value == 0 ? QString{} : QString::number(value));
}

CellState GameCellWidget::currentState() const
{
    return m_state;
}

void GameCellWidget::setState(CellState state)
{
    m_state = state;
    const char* str = "normal";
    switch (state) {
        case CellState::Normal:      str = "normal";      break;
        case CellState::Selected:    str = "selected";    break;
        case CellState::Highlighted: str = "highlighted"; break;
        case CellState::Hinted:      str = "hinted";      break;
        case CellState::Locked:      str = "locked";      break;
        case CellState::Empty:       str = "empty";       break;
        case CellState::Neighbor:    str = "neighbor";    break;
    }

    setProperty("cellState", str);
    style()->unpolish(this);
    style()->polish(this);
    update();
}

void GameCellWidget::setFontSize(int px)
{
    QFont f = m_label->font();
    f.setPixelSize(px);
    f.setBold(true);
    m_label->setFont(f);
}

void GameCellWidget::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void GameCellWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        emit clicked(m_row, m_col);
    else if (event->button() == Qt::RightButton)
        emit rightClicked(m_row, m_col);
}
