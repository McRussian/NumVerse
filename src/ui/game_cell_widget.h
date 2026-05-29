#pragma once

#include <QWidget>
#include "logic/data/cell_state.h"

class QLabel;

class GameCellWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameCellWidget(int row, int col, QWidget* parent = nullptr);

    void      setValue(uint16_t value);
    void      setState(CellState state);
    CellState currentState() const;
    void      setFontSize(int px);

signals:
    void clicked(int row, int col);
    void rightClicked(int row, int col);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    int       m_row;
    int       m_col;
    QLabel*   m_label;
    CellState m_state = CellState::Normal;
};
