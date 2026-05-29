#pragma once

#include <QMainWindow>
#include <QVector>
#include <map>
#include "game_descriptor.h"
#include "high_score_dialog.h"
#include "logic/data/difficulty.h"
#include "logic/data/game_result.h"
#include "logic/data/game_state.h"

class QAction;
class QComboBox;
class QLabel;
class QStackedWidget;
class QTimer;
class QToolBar;
class MenuWidget;
class GameWindow;
class AbstractGame;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

    Difficulty currentDifficulty() const;

private:
    // Pages
    QStackedWidget* m_stack;
    MenuWidget*     m_menu;
    GameWindow*     m_gameWindow  = nullptr;
    AbstractGame*   m_currentGame = nullptr;
    int             m_currentGameId = -1;
    GameFeature     m_currentFeatures = GameFeature::None;

    // Toolbar
    QToolBar* m_toolbar;
    QAction*  m_applyAction;
    QAction*  m_hintAction;
    QAction*  m_shuffleAction;
    QAction*  m_surrenderAction;
    QAction*  m_newGameAction;
    QAction*  m_undoAction;

    // Status bar
    QComboBox* m_diffBox;
    QLabel*    m_scoreLabel;
    QLabel*    m_movesLabel;
    QLabel*    m_timeLabel;

    // Scores: keyed by gameId
    std::map<int, GameScoreData> m_scores;

    // Timer
    QTimer*   m_gameTimer;
    uint32_t  m_elapsedSecs = 0;
    QVector<QMetaObject::Connection> m_gameConnections;

    void startGame(int gameId);
    void showMenu();
    void onPlayerChanged(const QString& name);

    void onApplyTriggered();
    void onUndoTriggered();
    void onHintTriggered();
    void onShuffleTriggered();
    void onSurrenderTriggered();
    void onNewGameTriggered();
    void onGameOver(const GameResult& result);
    void updateGameStatus(const GameState& state);

    void connectGame(AbstractGame* game);
    void disconnectGame();
    void setGameControlsVisible(bool visible);
    void showHighScores();

    static QString formatTime(uint32_t secs);
};
