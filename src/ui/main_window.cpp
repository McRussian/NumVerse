#include "main_window.h"
#include "menu_widget.h"
#include "game_window.h"
#include "grid_game_board.h"
#include "game_catalog.h"
#include "game/abstract_game.h"

#include <QApplication>
#include <QComboBox>
#include <QLabel>
#include <QSettings>
#include <QStackedWidget>
#include <QStatusBar>
#include <algorithm>

static const char* kDiffLabels[] = {"Новичок", "Лёгкий", "Средний", "Сложный", "Эксперт"};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("NumVerse");
    resize(680, 580);

    m_stack = new QStackedWidget(this);
    m_menu  = new MenuWidget;
    m_stack->addWidget(m_menu);
    setCentralWidget(m_stack);

    // Status bar: difficulty selector
    auto* diffLabel = new QLabel("  Сложность: ", this);
    m_diffBox = new QComboBox(this);
    for (const char* label : kDiffLabels)
        m_diffBox->addItem(label);

    statusBar()->addPermanentWidget(diffLabel);
    statusBar()->addPermanentWidget(m_diffBox);

    connect(m_menu, &MenuWidget::gameSelected,   this, &MainWindow::startGame);
    connect(m_menu, &MenuWidget::quitRequested,  qApp, &QApplication::quit);
    connect(m_menu, &MenuWidget::playerChanged,  this, &MainWindow::onPlayerChanged);

    connect(m_diffBox, &QComboBox::currentIndexChanged, this, [this](int idx) {
        QSettings().setValue("players/" + m_menu->currentPlayerName() + "/difficulty", idx);
    });

    // Load difficulty for the initial player
    onPlayerChanged(m_menu->currentPlayerName());
}

Difficulty MainWindow::currentDifficulty() const
{
    return static_cast<Difficulty>(m_diffBox->currentIndex());
}

void MainWindow::onPlayerChanged(const QString& name)
{
    int idx = QSettings().value("players/" + name + "/difficulty", 1).toInt();
    QSignalBlocker blocker(m_diffBox);
    m_diffBox->setCurrentIndex(idx);
}

void MainWindow::startGame(int gameId)
{
    const auto games = GameCatalog::allGames();
    const auto it    = std::find_if(games.begin(), games.end(),
                                    [gameId](const GameDescriptor& d) { return d.id == gameId; });
    if (it == games.end())
        return;

    const GameConfig config = it->makeConfig(currentDifficulty());

    if (m_gameWindow) {
        m_stack->removeWidget(m_gameWindow);
        m_gameWindow->deleteLater();
        m_gameWindow = nullptr;
    }

    auto* game  = it->createGame(m_menu->currentPlayerName().toStdString(), config);
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
