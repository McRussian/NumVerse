#include "main_window.h"
#include "menu_widget.h"
#include "game_window.h"
#include "grid_game_board.h"
#include "game_catalog.h"
#include "game/abstract_game.h"

#include <QApplication>
#include <QStackedWidget>
#include <algorithm>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("NumVerse");
    resize(680, 580);

    m_stack = new QStackedWidget(this);
    m_menu  = new MenuWidget;
    m_stack->addWidget(m_menu);
    setCentralWidget(m_stack);

    connect(m_menu, &MenuWidget::gameSelected, this, &MainWindow::startGame);
    connect(m_menu, &MenuWidget::quitRequested, qApp, &QApplication::quit);
}

void MainWindow::startGame(int gameId)
{
    const auto games = GameCatalog::allGames();
    const auto it = std::find_if(games.begin(), games.end(),
                                 [gameId](const GameDescriptor& d) { return d.id == gameId; });
    if (it == games.end())
        return;

    const QString    playerName = m_menu->currentPlayerName();
    const Difficulty difficulty = m_menu->currentDifficulty();
    const GameConfig config     = it->makeConfig(difficulty);

    if (m_gameWindow) {
        m_stack->removeWidget(m_gameWindow);
        m_gameWindow->deleteLater();
        m_gameWindow = nullptr;
    }

    auto* game  = it->createGame(playerName.toStdString(), config);
    auto* board = new GridGameBoard;
    m_gameWindow = new GameWindow(game, board);
    m_stack->addWidget(m_gameWindow);
    m_stack->setCurrentWidget(m_gameWindow);

    connect(m_gameWindow, &GameWindow::backRequested, this, &MainWindow::showMenu);

    game->start();
}

void MainWindow::showMenu()
{
    m_stack->setCurrentWidget(m_menu);
    if (m_gameWindow) {
        m_stack->removeWidget(m_gameWindow);
        m_gameWindow->deleteLater();
        m_gameWindow = nullptr;
    }
}
