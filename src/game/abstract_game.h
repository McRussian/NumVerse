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

class AbstractGame : public QObject {
    Q_OBJECT
public:
    explicit AbstractGame(std::string playerName, GameConfig config, QObject* parent = nullptr);
    ~AbstractGame() override = default;

    void start();
    virtual void selectCell(int row, int col);
    void applySelection();
    void tick(uint32_t secs);
    void hint();
    void reset();
    void shuffle();
    void undo();
    void surrender();
    virtual void append() {}

    const GameState& state() const;

signals:
    void boardChanged(const Board& board);
    void stateChanged(const GameState& state);
    void gameOver(const GameResult& result);
    void hintReady(const Selection& hint);

protected:
    virtual std::unique_ptr<IGameRules> createRules() const = 0;

    void emitStateSignals();
    void resetHints();
    void replaceBoardAndEmit(Board newBoard);
    void applyNeighborHighlights(const Selection& cells);

private:
    std::string            m_playerName;
    GameConfig             m_config;
    GameLogic              m_logic;
    std::vector<Selection> m_hints;
    size_t                 m_hintIndex = 0;
};
