#include "game_logic.h"
#include "data/cell_state.h"
#include <algorithm>
#include <random>

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
    m_history.clear();
}

void GameLogic::select(uint8_t row, uint8_t col) {
    if (m_state.status != GameStatus::Playing)
        return;
    if (!m_state.board.isValid(row, col))
        return;
    const auto& cell = m_state.board.at(row, col);
    if (cell.state() == CellState::Empty || cell.value() == 0 || cell.isNoise())
        return;

    if (m_state.selection.contains(row, col)) {
        m_state.selection.remove(row, col);
        m_state.board.at(row, col).setState(CellState::Normal);
    } else {
        m_state.selection.add(row, col);
        m_state.board.at(row, col).setState(CellState::Selected);
    }
}

void GameLogic::applySelection() {
    if (m_state.status != GameStatus::Playing)
        return;
    if (m_state.selection.empty())
        return;

    // Snapshot before applying — saved only if move is valid
    Snapshot snap{m_state.board, m_state.score, m_state.movesLeft};

    uint32_t scoreBefore = m_state.score;
    m_rules->applySelection(m_state, m_config);
    bool validMove = m_state.score > scoreBefore;

    if (validMove) {
        // Apply difficulty multiplier to gained score
        if (m_config.scoreMultiplierPct != 100) {
            uint32_t gained = m_state.score - scoreBefore;
            m_state.score   = scoreBefore + gained * m_config.scoreMultiplierPct / 100;
        }

        if (m_config.historySize > 0) {
            if (m_history.size() >= m_config.historySize)
                m_history.pop_front();
            m_history.push_back(std::move(snap));
        }

        if (m_state.status != GameStatus::Won && m_config.maxMoves > 0) {
            --m_state.movesLeft;
            if (m_state.movesLeft == 0)
                m_state.status = GameStatus::Lost;
        }
    }
}

void GameLogic::undo() {
    if (m_history.empty() || m_state.status != GameStatus::Playing)
        return;

    const auto& snap  = m_history.back();
    m_state.board     = snap.board;
    m_state.score     = snap.score;
    m_state.movesLeft = snap.movesLeft;
    m_state.selection.clear();

    // Snapshot was taken while cells were Selected — restore them to Normal
    for (uint8_t r = 0; r < m_state.board.rows(); ++r)
        for (uint8_t c = 0; c < m_state.board.cols(); ++c) {
            auto& cell = m_state.board.at(r, c);
            if (cell.state() == CellState::Selected || cell.state() == CellState::Hinted)
                cell.setState(CellState::Normal);
        }

    m_history.pop_back();
}

bool GameLogic::canUndo() const {
    return !m_history.empty();
}

void GameLogic::replaceBoard(Board newBoard) {
    if (m_state.status != GameStatus::Playing) return;
    m_state.board = std::move(newBoard);
    m_state.selection.clear();
    m_history.clear();
}

void GameLogic::shuffle() {
    if (m_state.status != GameStatus::Playing)
        return;

    std::vector<std::pair<uint8_t, uint8_t>> positions;
    std::vector<uint16_t> values;

    for (uint8_t r = 0; r < m_state.board.rows(); ++r)
        for (uint8_t c = 0; c < m_state.board.cols(); ++c) {
            const auto& cell = m_state.board.at(r, c);
            if (cell.state() != CellState::Empty && cell.value() != 0 && !cell.isNoise()) {
                positions.push_back({r, c});
                values.push_back(cell.value());
            }
        }

    if (values.size() < 2) return;

    std::shuffle(values.begin(), values.end(), std::mt19937{std::random_device{}()});

    m_state.selection.clear();
    for (size_t i = 0; i < positions.size(); ++i) {
        auto [r, c] = positions[i];
        m_state.board.at(r, c) = GameCell(values[i], CellState::Normal);
    }
}

void GameLogic::forfeit() {
    if (m_state.status != GameStatus::Playing)
        return;

    // Penalty: score × (cleared / seqTotal), noise cells are excluded
    int seqActive = 0, cleared = 0;
    for (uint8_t r = 0; r < m_state.board.rows(); ++r)
        for (uint8_t c = 0; c < m_state.board.cols(); ++c) {
            const auto& cell = m_state.board.at(r, c);
            if (cell.state() == CellState::Empty)
                ++cleared;
            else if (!cell.isNoise())
                ++seqActive;
        }
    int seqTotal = seqActive + cleared;
    if (seqTotal > 0)
        m_state.score = m_state.score * static_cast<uint32_t>(cleared)
                        / static_cast<uint32_t>(seqTotal);

    m_state.status = GameStatus::Lost;
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

std::vector<Selection> GameLogic::getHint() const {
    auto hints = m_rules->getHint(m_state.board);

    const auto& selCells = m_state.selection.cells();
    if (selCells.empty()) return hints;

    // Keep only hints where selected cells appear as a subsequence (in order)
    std::vector<Selection> filtered;
    for (const auto& hint : hints) {
        const auto& hc = hint.cells();
        size_t si = 0;
        for (const auto& cell : hc) {
            if (si < selCells.size() && cell == selCells[si]) ++si;
        }
        if (si == selCells.size()) filtered.push_back(hint);
    }
    return filtered.empty() ? hints : filtered;
}

GameResult GameLogic::buildResult(const std::string& playerName) const {
    return GameResult{
        playerName,
        m_state.score,
        m_state.secondsElapsed,
        m_state.status == GameStatus::Won
    };
}
