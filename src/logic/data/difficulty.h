#pragma once

#include <cstdint>

// Уровень сложности игровой сессии.
// Используется в GameConfig и LevelData.
enum class Difficulty : uint8_t {
    Beginner,
    Easy,
    Medium,
    Hard,
    Expert
};
