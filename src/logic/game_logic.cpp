#include "game_logic.h"

void GameLogic::init(GameConfig config, std::unique_ptr<IGameRules> rules) {
    m_config = config;
    m_rules = std::move(rules);
    reset();
}

void GameLogic::reset() {
    m_state = GameState{};
    m_state.board = Board(m_config.gridRows, m_config.gridCols);
    m_state.movesLeft = m_config.maxMoves;
    m_rules->initBoard(m_state.board, m_config);
    m_state.status = GameStatus::Playing;
}

void GameLogic::select(uint8_t row, uint8_t col) {
    if (m_state.status != GameStatus::Playing)
        return;
    if (!m_state.board.isValid(row, col))
        return;

    if (m_state.selection.contains(row, col))
        m_state.selection.remove(row, col);
    else
        m_state.selection.add(row, col);
}

void GameLogic::applySelection() {
    if (m_state.status != GameStatus::Playing)
        return;
    if (m_state.selection.empty())
        return;

    m_rules->applySelection(m_state, m_config);

    if (m_state.status != GameStatus::Won && m_config.maxMoves > 0) {
        --m_state.movesLeft;
        if (m_state.movesLeft == 0)
            m_state.status = GameStatus::Lost;
    }
}

void GameLogic::tick(uint32_t secs) {
    if (m_state.status != GameStatus::Playing)
        return;

    m_state.secondsElapsed += secs;
    if (m_config.timeLimitSecs > 0 && m_state.secondsElapsed >= m_config.timeLimitSecs)
        m_state.status = GameStatus::Lost;
}

const GameState& GameLogic::getState() const {
    return m_state;
}

GameResult GameLogic::buildResult(const std::string& playerName) const {
    return GameResult{
        playerName,
        m_state.score,
        m_state.secondsElapsed,
        m_state.status == GameStatus::Won
    };
}
