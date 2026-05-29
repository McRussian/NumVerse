#include "game_window.h"
#include "abstract_game_board.h"
#include "game/abstract_game.h"

#include <QVBoxLayout>

GameWindow::GameWindow(AbstractGame* game, AbstractGameBoard* board, QWidget* parent)
    : QWidget(parent)
    , m_game(game)
    , m_board(board)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_board);
    setLayout(layout);

    setupConnections();
}

AbstractGame* GameWindow::game() const
{
    return m_game;
}

void GameWindow::setupConnections()
{
    connect(m_game, &AbstractGame::boardChanged,
            m_board, &AbstractGameBoard::updateBoard);
    connect(m_game, &AbstractGame::hintReady,
            m_board, &AbstractGameBoard::highlightHint);
    connect(m_board, &AbstractGameBoard::cellClicked,
            m_game, &AbstractGame::selectCell);
    connect(m_board, &AbstractGameBoard::cellRightClicked,
            m_game, &AbstractGame::applySelection);
}
