#pragma once

#include <QWidget>
#include "logic/data/board.h"

// Базовый класс для всех игровых досок.
// Определяет интерфейс: updateBoard + сигнал cellClicked.
// GridGameBoard — реализация для сеточных игр.
// SceneGameBoard — будущая реализация на QGraphicsScene (Tetris и подобные).
class AbstractGameBoard : public QWidget {
    Q_OBJECT
public:
    explicit AbstractGameBoard(QWidget* parent = nullptr) : QWidget(parent) {}
    ~AbstractGameBoard() override = default;

    virtual void updateBoard(const Board& board) = 0;

signals:
    void cellClicked(int row, int col);
};
