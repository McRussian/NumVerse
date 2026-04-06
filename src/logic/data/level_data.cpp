#include "level_data.h"

LevelData::LevelData(uint8_t level, Difficulty difficulty,
                     uint8_t gridRows, uint8_t gridCols,
                     uint16_t maxMoves, uint16_t timeLimitSecs,
                     uint16_t targetSum, std::vector<uint16_t> board)
    : m_level(level)
    , m_difficulty(difficulty)
    , m_gridRows(gridRows)
    , m_gridCols(gridCols)
    , m_maxMoves(maxMoves)
    , m_timeLimitSecs(timeLimitSecs)
    , m_targetSum(targetSum)
    , m_board(std::move(board))
{}

uint8_t LevelData::level() const { return m_level; }
Difficulty LevelData::difficulty() const { return m_difficulty; }
uint8_t LevelData::gridRows() const { return m_gridRows; }
uint8_t LevelData::gridCols() const { return m_gridCols; }
uint16_t LevelData::maxMoves() const { return m_maxMoves; }
uint16_t LevelData::timeLimitSecs() const { return m_timeLimitSecs; }
uint16_t LevelData::targetSum() const { return m_targetSum; }
const std::vector<uint16_t>& LevelData::board() const { return m_board; }

bool LevelData::isValid() const
{
    if (m_level == 0) return false;
    if (m_gridRows == 0 || m_gridCols == 0) return false;
    if (m_board.size() != static_cast<size_t>(m_gridRows * m_gridCols)) return false;
    return true;
}

GameConfig LevelData::toConfig() const
{
    GameConfig cfg;
    cfg.level = m_level;
    cfg.difficulty = m_difficulty;
    cfg.gridRows = m_gridRows;
    cfg.gridCols = m_gridCols;
    cfg.maxMoves = m_maxMoves;
    cfg.timeLimitSecs = m_timeLimitSecs;
    cfg.targetSum = m_targetSum;
    return cfg;
}
