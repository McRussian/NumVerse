#pragma once

#include <cstdint>

// Итог завершённой игровой сессии.
// Формируется в GameLogic::buildResult() и передаётся в ScoreBoard и StatisticsManager.
struct GameResult {
    uint8_t level = 0;
    uint32_t score = 0;
    uint32_t timeSecs = 0;
    bool won = false;
};
