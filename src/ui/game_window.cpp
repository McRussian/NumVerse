#include "game_window.h"
#include "game_panel.h"
#include "game_timer_widget.h"
#include "abstract_game_board.h"
#include "game/abstract_game.h"

#include <QAbstractButton>
#include <QMessageBox>
#include <QPushButton>
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
            ? QString("Победа!\nСчёт: %1   Время: %2 сек.")
                  .arg(result.score).arg(result.timeSecs)
            : QString("Игра окончена.\nСчёт: %1   Время: %2 сек.")
                  .arg(result.score).arg(result.timeSecs);

        QMessageBox box(this);
        box.setWindowTitle("Конец игры");
        box.setText(msg);
        box.addButton("Заново", QMessageBox::AcceptRole);
        QAbstractButton* menuBtn = box.addButton("В меню", QMessageBox::RejectRole);
        box.exec();

        if (box.clickedButton() == menuBtn) {
            emit backRequested();
        } else {
            m_panel->timer()->reset();
            m_game->reset();
            m_panel->timer()->start();
        }
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
