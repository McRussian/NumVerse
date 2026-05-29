#pragma once

#include <QObject>
#include <memory>
#include <string>

#include "logic/data/board.h"
#include "logic/data/game_config.h"
#include "logic/data/game_result.h"
#include "logic/data/game_state.h"
#include "logic/data/selection.h"
#include "logic/game_logic.h"
#include "logic/rules/i_game_rules.h"

// Базовый Qt-класс игровой сессии.
// Связывает GameLogic с UI: транслирует изменения состояния в сигналы.
// Подклассы реализуют createRules() — возвращают конкретные правила.
class AbstractGame : public QObject {
    Q_OBJECT
public:
    explicit AbstractGame(std::string playerName, GameConfig config, QObject* parent = nullptr);
    ~AbstractGame() override = default;

    void start();
    void selectCell(int row, int col);
    void applySelection();
    void tick(uint32_t secs);
    void hint();
    void reset();
    void shuffle();
    void surrender();

    const GameState& state() const;

signals:
    void boardChanged(const Board& board);
    void stateChanged(const GameState& state);
    void gameOver(const GameResult& result);
    void hintReady(const Selection& hint);

protected:
    virtual std::unique_ptr<IGameRules> createRules() const = 0;

private:
    std::string            m_playerName;
    GameConfig             m_config;
    GameLogic              m_logic;
    std::vector<Selection> m_hints;
    size_t                 m_hintIndex = 0;

    void emitStateSignals();
    void resetHints();
};
