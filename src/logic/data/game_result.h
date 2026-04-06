#pragma once

#include <cstdint>
#include <string>

// Итог завершённой игровой сессии.
// Формируется в GameLogic::buildResult() и передаётся в ScoreBoard::add(level, result).
// Уровень не хранится здесь — он является ключом в ScoreBoard и JSON-словаре.
struct GameResult {
    std::string playerName;
    uint32_t score = 0;
    uint32_t timeSecs = 0;
    bool won = false;
};
