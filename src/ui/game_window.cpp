#include "game_window.h"
#include "game_panel.h"
#include "game_timer_widget.h"
#include "abstract_game_board.h"
#include "game/abstract_game.h"

#include <QMessageBox>
#include <QVBoxLayout>

GameWindow::GameWindow(AbstractGame* game, AbstractGameBoard* board, QWidget* parent)
    : QWidget(parent)
    , m_game(game)
    , m_board(board)
    , m_panel(new GamePanel(this))
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_board, 1);
    layout->addWidget(m_panel, 0);
    setLayout(layout);

    setupConnections();
}

void GameWindow::setupConnections()
{
    // Логика → UI
    connect(m_game, &AbstractGame::boardChanged,
            m_board, &AbstractGameBoard::updateBoard);

    connect(m_game, &AbstractGame::stateChanged,
            m_panel, &GamePanel::update);

    connect(m_game, &AbstractGame::hintReady,
            m_board, &AbstractGameBoard::highlightHint);

    connect(m_game, &AbstractGame::gameOver, this, [this](const GameResult& result) {
        m_panel->timer()->stop();
        QString msg = result.won
            ? QString("Победа! Счёт: %1\nВремя: %2 сек.")
                  .arg(result.score).arg(result.timeSecs)
            : QString("Игра окончена. Счёт: %1\nВремя: %2 сек.")
                  .arg(result.score).arg(result.timeSecs);
        QMessageBox::information(this, "Конец игры", msg);
    });

    // UI → Логика
    connect(m_board, &AbstractGameBoard::cellClicked,
            m_game, &AbstractGame::selectCell);

    connect(m_board, &AbstractGameBoard::cellRightClicked,
            m_game, &AbstractGame::applySelection);

    connect(m_panel, &GamePanel::hintClicked,
            m_game, &AbstractGame::hint);

    connect(m_panel, &GamePanel::surrenderClicked, this, [this] {
        auto btn = QMessageBox::question(this, "Сдаться", "Завершить игру?");
        if (btn == QMessageBox::Yes)
            m_game->surrender();
    });

    // Таймер
    connect(m_panel->timer(), &GameTimerWidget::ticked, this, [this] {
        m_game->tick(1);
    });
}
