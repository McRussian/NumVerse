#pragma once

#include <QWidget>
#include "logic/data/board.h"
#include "logic/data/selection.h"

// Базовый класс для всех игровых досок.
// Определяет интерфейс: updateBoard + сигналы кликов.
// GridGameBoard — реализация для сеточных игр.
// SceneGameBoard — будущая реализация на QGraphicsScene (Tetris и подобные).
class AbstractGameBoard : public QWidget {
    Q_OBJECT
public:
    explicit AbstractGameBoard(QWidget* parent = nullptr) : QWidget(parent) {}
    ~AbstractGameBoard() override = default;

    virtual void updateBoard(const Board& board) = 0;
    virtual void highlightHint(const Selection& hint) = 0;

signals:
    void cellClicked(int row, int col);
    void cellRightClicked(int row, int col);
};
