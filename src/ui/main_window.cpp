#include "main_window.h"
#include "menu_widget.h"
#include "game_window.h"
#include "grid_game_board.h"
#include "game_catalog.h"
#include "game/abstract_game.h"

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QLabel>
#include <QAbstractButton>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTimer>
#include <QToolBar>
#include <algorithm>

static const char* kDiffLabels[] = {"Новичок", "Лёгкий", "Средний", "Сложный", "Эксперт"};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("NumVerse");
    resize(680, 580);

    // Central stack
    m_stack = new QStackedWidget(this);
    m_menu  = new MenuWidget;
    m_stack->addWidget(m_menu);
    setCentralWidget(m_stack);

    // Toolbar
    m_toolbar = addToolBar("Игра");
    m_toolbar->setMovable(false);
    m_toolbar->setFloatable(false);
    m_newGameAction   = m_toolbar->addAction("Новая игра");
    m_undoAction      = m_toolbar->addAction("Отменить");
    m_toolbar->addSeparator();
    m_hintAction      = m_toolbar->addAction("Подсказка");
    m_shuffleAction   = m_toolbar->addAction("Перемешать");
    m_toolbar->addSeparator();
    m_surrenderAction = m_toolbar->addAction("Сдаться");

    auto* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_toolbar->addWidget(spacer);
    m_appendAction = m_toolbar->addAction("Достроить");
    m_applyAction  = m_toolbar->addAction("Применить");
    m_toolbar->hide();

    // Status bar: game info (left) + difficulty (right)
    m_scoreLabel = new QLabel(this);
    m_movesLabel = new QLabel(this);
    m_timeLabel  = new QLabel(this);
    statusBar()->addWidget(m_scoreLabel);
    statusBar()->addWidget(m_movesLabel);
    statusBar()->addWidget(m_timeLabel);

    auto* diffLabel = new QLabel("  Сложность: ", this);
    m_diffBox = new QComboBox(this);
    for (const char* label : kDiffLabels)
        m_diffBox->addItem(label);
    statusBar()->addPermanentWidget(diffLabel);
    statusBar()->addPermanentWidget(m_diffBox);

    setGameControlsVisible(false);

    // Game timer
    m_gameTimer = new QTimer(this);
    m_gameTimer->setInterval(1000);
    connect(m_gameTimer, &QTimer::timeout, this, [this] {
        if (m_currentGame) {
            m_currentGame->tick(1);
            m_timeLabel->setText("  " + formatTime(++m_elapsedSecs));
        }
    });

    // Toolbar actions
    connect(m_newGameAction,   &QAction::triggered, this, &MainWindow::onNewGameTriggered);
    connect(m_undoAction,      &QAction::triggered, this, &MainWindow::onUndoTriggered);
    connect(m_appendAction,    &QAction::triggered, this, &MainWindow::onAppendTriggered);
    connect(m_applyAction,     &QAction::triggered, this, &MainWindow::onApplyTriggered);
    connect(m_hintAction,      &QAction::triggered, this, &MainWindow::onHintTriggered);
    connect(m_shuffleAction,   &QAction::triggered, this, &MainWindow::onShuffleTriggered);
    connect(m_surrenderAction, &QAction::triggered, this, &MainWindow::onSurrenderTriggered);

    // Menu signals
    connect(m_menu, &MenuWidget::gameSelected,    this, &MainWindow::startGame);
    connect(m_menu, &MenuWidget::quitRequested,  qApp, &QApplication::quit);
    connect(m_menu, &MenuWidget::playerChanged,  this, &MainWindow::onPlayerChanged);
    connect(m_menu, &MenuWidget::recordsRequested, this, &MainWindow::showHighScores);

    // Difficulty combo
    connect(m_diffBox, &QComboBox::currentIndexChanged, this, [this](int idx) {
        QSettings().setValue("players/" + m_menu->currentPlayerName() + "/difficulty", idx);
    });

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

    // Clean up previous game
    disconnectGame();
    if (m_gameWindow) {
        m_stack->removeWidget(m_gameWindow);
        m_gameWindow->deleteLater();
        m_gameWindow = nullptr;
    }
    delete m_currentGame;
    m_currentGame = nullptr;

    // Create new game
    m_currentGame     = it->createGame(m_menu->currentPlayerName().toStdString(), config);
    m_currentGameId   = gameId;
    m_currentFeatures = it->features;

    // Ensure score entry exists for this game
    if (!m_scores.count(gameId))
        m_scores[gameId] = GameScoreData{gameId, it->name};
    m_applyAction->setVisible(hasFeature(it->features,  GameFeature::ApplySelection));
    m_appendAction->setVisible(hasFeature(it->features, GameFeature::AppendRows));
    auto* board     = new GridGameBoard;
    m_gameWindow    = new GameWindow(m_currentGame, board);

    m_stack->addWidget(m_gameWindow);
    m_stack->setCurrentWidget(m_gameWindow);

    connectGame(m_currentGame);
    m_toolbar->show();
    setGameControlsVisible(true);

    m_elapsedSecs = 0;
    m_timeLabel->setText("  " + formatTime(0));
    m_gameTimer->start();

    m_currentGame->start();
}

void MainWindow::showMenu()
{
    m_gameTimer->stop();
    disconnectGame();

    m_stack->setCurrentWidget(m_menu);
    if (m_gameWindow) {
        m_stack->removeWidget(m_gameWindow);
        m_gameWindow->deleteLater();
        m_gameWindow = nullptr;
    }
    delete m_currentGame;
    m_currentGame = nullptr;

    m_toolbar->hide();
    setGameControlsVisible(false);
}

void MainWindow::connectGame(AbstractGame* game)
{
    m_gameConnections << connect(game, &AbstractGame::stateChanged,
                                 this, &MainWindow::updateGameStatus);
    m_gameConnections << connect(game, &AbstractGame::gameOver,
                                 this, &MainWindow::onGameOver);
}

void MainWindow::disconnectGame()
{
    for (const auto& c : m_gameConnections)
        disconnect(c);
    m_gameConnections.clear();
}

void MainWindow::setGameControlsVisible(bool visible)
{
    m_scoreLabel->setVisible(visible);
    m_movesLabel->setVisible(visible);
    m_timeLabel->setVisible(visible);
}

void MainWindow::onUndoTriggered()
{
    if (m_currentGame)
        m_currentGame->undo();
}

void MainWindow::onAppendTriggered()
{
    if (m_currentGame)
        m_currentGame->append();
}

void MainWindow::onApplyTriggered()
{
    if (m_currentGame)
        m_currentGame->applySelection();
}

void MainWindow::onShuffleTriggered()
{
    if (m_currentGame)
        m_currentGame->shuffle();
}

void MainWindow::onHintTriggered()
{
    if (m_currentGame)
        m_currentGame->hint();
}

void MainWindow::onSurrenderTriggered()
{
    if (!m_currentGame) return;
    auto btn = QMessageBox::question(this, "Сдаться", "Завершить игру?");
    if (btn == QMessageBox::Yes)
        m_currentGame->surrender();
}

void MainWindow::onNewGameTriggered()
{
    if (m_currentGameId >= 0)
        startGame(m_currentGameId);
}

void MainWindow::onGameOver(const GameResult& result)
{
    m_gameTimer->stop();

    const uint8_t lvl = static_cast<uint8_t>(currentDifficulty());
    auto& gsd = m_scores[m_currentGameId];
    const bool r1 = gsd.byScore.add(lvl, result);
    const bool r2 = gsd.byTime.add(lvl, result);
    const bool newRecord = r1 || r2;

    QString msg = result.won
        ? QString("Победа!\nСчёт: %1   Время: %2%3")
              .arg(result.score)
              .arg(formatTime(result.timeSecs))
              .arg(newRecord ? "\n\nНовый рекорд!" : "")
        : QString("Игра окончена.\nСчёт: %1   Время: %2")
              .arg(result.score).arg(formatTime(result.timeSecs));

    QMessageBox box(this);
    box.setWindowTitle(result.won ? "Победа!" : "Конец игры");
    box.setText(msg);
    box.addButton("Заново",  QMessageBox::AcceptRole);
    QAbstractButton* menuBtn = box.addButton("В меню", QMessageBox::RejectRole);
    box.exec();

    if (newRecord)
        showHighScores();

    if (box.clickedButton() == menuBtn)
        showMenu();
    else
        onNewGameTriggered();
}

void MainWindow::showHighScores()
{
    HighScoreDialog dlg(m_scores, this);
    dlg.exec();
}

void MainWindow::updateGameStatus(const GameState& state)
{
    m_scoreLabel->setText(QString("  Счёт: %1  ").arg(state.score));
    if (state.movesLeft > 0)
        m_movesLabel->setText(QString("Ходы: %1  ").arg(state.movesLeft));
    else
        m_movesLabel->clear();
}

QString MainWindow::formatTime(uint32_t secs)
{
    return QString("%1:%2")
        .arg(secs / 60, 2, 10, QChar('0'))
        .arg(secs % 60, 2, 10, QChar('0'));
}
