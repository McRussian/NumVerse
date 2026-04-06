#pragma once

#include <cstdint>

// Статус текущей игровой сессии.
// Используется в GameState и проверяется в GameLogic, AbstractGame и *Game-обёртках.
enum class GameStatus : uint8_t {
    Idle,     // сессия не начата
    Playing,  // идёт игра
    Paused,   // на паузе
    Won,      // победа
    Lost      // поражение
};
