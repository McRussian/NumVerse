#include "score_board.h"

#include <algorithm>

ScoreBoard::ScoreBoard(RankingMode mode, size_t maxRecords)
    : m_mode(mode)
    , m_maxRecords(maxRecords)
{}

bool ScoreBoard::isBetter(const GameResult& a, const GameResult& b) const
{
    if (m_mode == RankingMode::ByScore) {
        if (a.score != b.score) return a.score > b.score;
        return a.timeSecs < b.timeSecs;
    } else {
        if (a.timeSecs != b.timeSecs) return a.timeSecs < b.timeSecs;
        return a.score > b.score;
    }
}

bool ScoreBoard::add(uint8_t level, const GameResult& result)
{
    if (!result.won) return false;
    auto& list = m_records[level];
    if (list.size() >= m_maxRecords && !isBetter(result, list.back()))
        return false;
    auto pos = std::lower_bound(list.begin(), list.end(), result,
        [this](const GameResult& a, const GameResult& b) {
            return isBetter(a, b);
        });
    list.insert(pos, result);
    if (list.size() > m_maxRecords)
        list.resize(m_maxRecords);
    return true;
}

std::vector<GameResult> ScoreBoard::top(uint8_t level) const
{
    auto it = m_records.find(level);
    if (it == m_records.end()) return {};
    return it->second;
}

std::optional<GameResult> ScoreBoard::best(uint8_t level) const
{
    auto it = m_records.find(level);
    if (it == m_records.end() || it->second.empty()) return std::nullopt;
    return it->second.front();
}

const std::map<uint8_t, std::vector<GameResult>>& ScoreBoard::all() const
{
    return m_records;
}

void ScoreBoard::clear(uint8_t level)
{
    m_records.erase(level);
}

void ScoreBoard::clearAll()
{
    m_records.clear();
}
