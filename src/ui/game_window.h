#pragma once

#include <QWidget>

class AbstractGame;
class AbstractGameBoard;
class GamePanel;

class GameWindow : public QWidget {
    Q_OBJECT
public:
    explicit GameWindow(AbstractGame* game, AbstractGameBoard* board,
                        QWidget* parent = nullptr);

private:
    AbstractGame*      m_game;
    AbstractGameBoard* m_board;
    GamePanel*         m_panel;

    void setupConnections();
};
