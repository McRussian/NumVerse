#pragma once

#include <cstdint>

struct GameConfig {
    uint8_t  gridRows      = 0;
    uint8_t  gridCols      = 0;
    uint8_t  difficulty    = 0;  // 0 = Easy, 1 = Medium, 2 = Hard
    uint16_t maxMoves      = 0;  // 0 = no limit
    uint16_t timeLimitSecs = 0;  // 0 = no limit
    uint16_t targetSum     = 0;
    uint16_t level         = 0;
};
