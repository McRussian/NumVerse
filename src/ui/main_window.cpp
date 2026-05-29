#include "main_window.h"
#include "menu_widget.h"
#include "settings_dialog.h"
#include "game_window.h"
#include "grid_game_board.h"
#include "game/abstract_game.h"
#include "game/number_chaos_game.h"

#include <QApplication>
#include <QStackedWidget>

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
    SettingsDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    auto ps = dlg.settings();

    if (m_gameWindow) {
        m_stack->removeWidget(m_gameWindow);
        m_gameWindow->deleteLater();
        m_gameWindow = nullptr;
    }

    AbstractGame* game = nullptr;
    switch (gameId) {
    case 0:
    default:
        game = new NumberChaosGame(ps.playerName, ps.config);
        break;
    }

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
