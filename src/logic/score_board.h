#pragma once

#include "data/game_result.h"
#include <vector>
#include <map>
#include <optional>
#include <cstdint>

enum class RankingMode {
    ByScore,  // score desc, timeSecs asc при равном счёте
    ByTime    // timeSecs asc, score desc при равном времени
};

// Хранит топ-N результатов отдельно по каждому уровню.
// Учитываются только выигранные сессии.
// Критерий ранжирования и вытеснения задаётся через RankingMode.
// StatisticsManager держит два экземпляра на каждую игру — по очкам и по времени.
class ScoreBoard {
public:
    static constexpr size_t DEFAULT_MAX_RECORDS = 10;

    explicit ScoreBoard(RankingMode mode = RankingMode::ByScore,
                        size_t maxRecords = DEFAULT_MAX_RECORDS);

    // Добавить результат. Проигранные сессии игнорируются.
    // Возвращает true если результат вошёл в таблицу (можно показать "новый рекорд!").
    bool add(uint8_t level, const GameResult& result);

    // Вернуть топ-N записей уровня, отсортированных по RankingMode.
    // Пустой список если для уровня ещё нет записей.
    std::vector<GameResult> top(uint8_t level) const;

    // Вернуть лучшую запись уровня (первую в топе).
    // std::nullopt если для уровня ещё нет записей.
    std::optional<GameResult> best(uint8_t level) const;

    // Вернуть все записи по всем уровням (ключ — номер уровня).
    // Используется StatisticsManager для сохранения в JSON.
    const std::map<uint8_t, std::vector<GameResult>>& all() const;

    // Очистить таблицу конкретного уровня.
    void clear(uint8_t level);

    // Очистить таблицы всех уровней.
    void clearAll();

private:
    bool isBetter(const GameResult& a, const GameResult& b) const;

    std::map<uint8_t, std::vector<GameResult>> m_records;
    RankingMode m_mode;
    size_t m_maxRecords;
};
