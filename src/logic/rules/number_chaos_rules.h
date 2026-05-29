#pragma once

#include "i_game_rules.h"
#include <vector>
#include <cstdint>

class NumberChaosRules : public IGameRules {
public:
    // Placeholder: заполняет поле значениями 1..N (row-major).
    // Будет заменено загрузкой из LevelData когда добавим initialBoard в GameConfig.
    void initBoard(Board& board, const GameConfig& config) override;

    // Извлекает значения из selection в порядке выделения.
    // Валидный ход: удаляет ячейки, начисляет очки, проверяет победу.
    // Невалидный ход: только очищает selection (score не меняется).
    void applySelection(GameState& state, const GameConfig& config) override;

    // Проверяет, образуют ли values (в данном порядке) корректную последовательность.
    // Для категориальных типов (чётные, нечётные, простые) порядок не важен.
    // Для остальных — важен: игрок выделяет ячейки в нужном порядке.
    static bool isValidSequence(const std::vector<uint16_t>& values);

    static uint32_t calcScore(size_t length); // 10 * length

    // Ищет кратчайшую валидную последовательность среди непустых ячеек.
    // Перебирает комбинации в порядке чтения (row-major) — без перестановок,
    // т.к. генерация гарантирует что ячейки последовательности идут в этом порядке.
    std::vector<Selection> getHint(const Board& board) const override;

private:
    static bool isArithmetic(const std::vector<uint16_t>& v);
    static bool isGeometric(const std::vector<uint16_t>& v);
    static bool isFibonacci(const std::vector<uint16_t>& v);
    static bool isSquares(const std::vector<uint16_t>& v);
    static bool isAllPrime(const std::vector<uint16_t>& v);
    static bool isFactorial(const std::vector<uint16_t>& v);
    static bool isSecondOrder(const std::vector<uint16_t>& v);
    static bool isPrime(uint16_t n);
    static bool isBoardCleared(const Board& board);
};
