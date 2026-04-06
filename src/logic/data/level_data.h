#pragma once

#include "difficulty.h"
#include "game_config.h"
#include <vector>
#include <cstdint>

// Данные уровня, загруженные из JSON-файла.
// Используется LevelLoader для построения объекта, затем конвертируется
// в GameConfig через toConfig(). Перед использованием проверяется isValid().
class LevelData {
public:
    LevelData() = default;
    LevelData(uint8_t level, Difficulty difficulty,
              uint8_t gridRows, uint8_t gridCols,
              uint16_t maxMoves, uint16_t timeLimitSecs,
              uint16_t targetSum, std::vector<uint16_t> board);

    uint8_t level() const;
    Difficulty difficulty() const;
    uint8_t gridRows() const;
    uint8_t gridCols() const;
    uint16_t maxMoves() const;
    uint16_t timeLimitSecs() const;
    uint16_t targetSum() const;
    const std::vector<uint16_t>& board() const;

    bool isValid() const;
    GameConfig toConfig() const;

private:
    uint8_t m_level = 0;
    Difficulty m_difficulty = Difficulty::Beginner;
    uint8_t m_gridRows = 0;
    uint8_t m_gridCols = 0;
    uint16_t m_maxMoves = 0;
    uint16_t m_timeLimitSecs = 0;
    uint16_t m_targetSum = 0;
    std::vector<uint16_t> m_board;
};
