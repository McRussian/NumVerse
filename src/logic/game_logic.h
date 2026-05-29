#pragma once

#include "data/game_config.h"
#include "data/game_state.h"
#include "data/game_result.h"
#include "rules/i_game_rules.h"
#include <deque>
#include <memory>
#include <string>

class GameLogic {
public:
    void init(GameConfig config, std::unique_ptr<IGameRules> rules);

    void select(uint8_t row, uint8_t col);
    void applySelection();
    void tick(uint32_t secs);
    void reset();
    void shuffle();
    void forfeit();

    // Откат последнего валидного хода. Игнорируется если история пуста или не Playing.
    void undo();
    bool canUndo() const;

    // Replace board (e.g. append rows). Clears selection and history.
    void replaceBoard(Board newBoard);

    const GameState& getState() const;
    GameResult buildResult(const std::string& playerName) const;
    std::vector<Selection> getHint() const;

private:
    struct Snapshot {
        Board    board;
        uint32_t score;
        uint16_t movesLeft;
    };

    GameConfig                  m_config;
    GameState                   m_state;
    std::unique_ptr<IGameRules> m_rules;
    std::deque<Snapshot>        m_history;
};
