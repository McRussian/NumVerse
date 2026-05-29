#pragma once

#include <QWidget>

class AbstractGame;
class AbstractGameBoard;

class GameWindow : public QWidget {
    Q_OBJECT
public:
    explicit GameWindow(AbstractGame* game, AbstractGameBoard* board,
                        QWidget* parent = nullptr);

    AbstractGame* game() const;

signals:
    void backRequested();

private:
    AbstractGame*      m_game;
    AbstractGameBoard* m_board;

    void setupConnections();
};
