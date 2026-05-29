#include "number_chaos_rules.h"
#include "logic/data/game_cell.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

// ─── Generator helpers ───────────────────────────────────────────────────────

namespace {

static const std::vector<uint16_t> FACTORIALS = {1, 2, 6, 24, 120, 720, 5040, 40320};

enum class SeqType {
    Arithmetic, Geometric, AllEven, AllOdd, AllPrime, Squares, Factorial, SecondOrder
};

std::vector<SeqType> availableTypes(Difficulty d)
{
    std::vector<SeqType> t = {SeqType::Arithmetic, SeqType::AllEven, SeqType::AllOdd};
    if (d >= Difficulty::Easy)   t.push_back(SeqType::Geometric);
    if (d >= Difficulty::Medium) { t.push_back(SeqType::Squares); t.push_back(SeqType::AllPrime); }
    if (d >= Difficulty::Hard)   { t.push_back(SeqType::Factorial); t.push_back(SeqType::SecondOrder); }
    return t;
}

uint16_t maxValue(Difficulty d)
{
    switch (d) {
    case Difficulty::Beginner: return 20;
    case Difficulty::Easy:     return 40;
    case Difficulty::Medium:   return 80;
    case Difficulty::Hard:     return 200;
    case Difficulty::Expert:   return 500;
    }
    return 40;
}

// Returns empty vector if generation fails for given parameters.
std::vector<uint16_t> generateSeq(std::mt19937& rng, SeqType type, int len, uint16_t maxVal)
{
    auto ri = [&](int lo, int hi) -> int {
        return lo <= hi ? std::uniform_int_distribution<int>(lo, hi)(rng) : lo;
    };
    auto coin = [&]() { return std::bernoulli_distribution(0.5)(rng); };

    switch (type) {

    case SeqType::Arithmetic: {
        int step = ri(1, std::max(1, maxVal / (len * 3)));
        if (coin()) step = -step;
        int start = (step > 0) ? ri(1, maxVal - step * (len - 1))
                                : ri(1 - step * (len - 1), maxVal);
        std::vector<uint16_t> v;
        for (int i = 0; i < len; ++i) {
            int val = start + step * i;
            if (val < 1 || val > maxVal) return {};
            v.push_back(static_cast<uint16_t>(val));
        }
        return v;
    }

    case SeqType::Geometric: {
        int ratio = ri(2, 3);
        int maxStart = maxVal;
        for (int i = 0; i < len - 1; ++i) maxStart /= ratio;
        if (maxStart < 1) return {};
        int start = ri(1, maxStart);
        bool asc = coin();
        std::vector<uint16_t> v;
        if (asc) {
            int val = start;
            for (int i = 0; i < len; ++i) {
                if (val < 1 || val > maxVal) return {};
                v.push_back(static_cast<uint16_t>(val));
                val *= ratio;
            }
        } else {
            int val = start;
            for (int i = 0; i < len - 1; ++i) val *= ratio;
            for (int i = 0; i < len; ++i) {
                if (val < 1 || val > maxVal) return {};
                v.push_back(static_cast<uint16_t>(val));
                val /= ratio;
            }
        }
        return v;
    }

    case SeqType::AllEven: {
        std::vector<uint16_t> pool;
        for (uint16_t i = 2; i <= maxVal; i += 2) pool.push_back(i);
        if ((int)pool.size() < len) return {};
        std::shuffle(pool.begin(), pool.end(), rng);
        return {pool.begin(), pool.begin() + len};
    }

    case SeqType::AllOdd: {
        std::vector<uint16_t> pool;
        for (uint16_t i = 1; i <= maxVal; i += 2) pool.push_back(i);
        if ((int)pool.size() < len) return {};
        std::shuffle(pool.begin(), pool.end(), rng);
        return {pool.begin(), pool.begin() + len};
    }

    case SeqType::AllPrime: {
        std::vector<uint16_t> pool;
        uint16_t cap = std::min(maxVal, uint16_t{300});
        for (uint16_t n = 2; n <= cap; ++n) {
            bool prime = true;
            for (uint16_t j = 2; j * j <= n; ++j)
                if (n % j == 0) { prime = false; break; }
            if (prime) pool.push_back(n);
        }
        if ((int)pool.size() < len) return {};
        std::shuffle(pool.begin(), pool.end(), rng);
        return {pool.begin(), pool.begin() + len};
    }

    case SeqType::Squares: {
        int maxRoot = static_cast<int>(std::sqrt(static_cast<double>(maxVal)));
        if (maxRoot < len) return {};
        int startRoot = ri(1, maxRoot - len + 1);
        bool asc = coin();
        std::vector<uint16_t> v;
        for (int i = 0; i < len; ++i) {
            int r = asc ? startRoot + i : startRoot + (len - 1 - i);
            uint16_t sq = static_cast<uint16_t>(r * r);
            if (sq < 1 || sq > maxVal) return {};
            v.push_back(sq);
        }
        return v;
    }

    case SeqType::Factorial: {
        if (len < 3) return {};
        // Find valid start such that all len factorials fit in maxVal
        int maxStart = -1;
        for (int i = 0; i + len - 1 < (int)FACTORIALS.size(); ++i)
            if (FACTORIALS[i + len - 1] <= maxVal) maxStart = i;
        if (maxStart < 0) return {};
        int start = ri(0, maxStart);
        if (FACTORIALS[start + len - 1] > maxVal) return {};
        std::vector<uint16_t> v;
        for (int i = 0; i < len; ++i) v.push_back(FACTORIALS[start + i]);
        return v;
    }

    case SeqType::SecondOrder: {
        if (len < 4) return {};
        int d1 = ri(1, 3);   // first difference
        int d2 = ri(1, 2);   // second difference (acceleration)
        int start = ri(1, 10);
        std::vector<uint16_t> v;
        v.push_back(static_cast<uint16_t>(start));
        int cur = start, diff = d1;
        for (int i = 1; i < len; ++i) {
            cur += diff;
            diff += d2;
            if (cur < 1 || cur > maxVal) return {};
            v.push_back(static_cast<uint16_t>(cur));
        }
        return v;
    }
    }
    return {};
}

} // namespace

// ─── NumberChaosRules ────────────────────────────────────────────────────────

void NumberChaosRules::initBoard(Board& board, const GameConfig& config)
{
    std::mt19937 rng{std::random_device{}()};

    const int rows   = config.gridRows;
    const int cols   = config.gridCols;
    const int total  = rows * cols;
    const uint16_t maxVal = maxValue(config.difficulty);
    const int maxSeqLen   = (config.difficulty >= Difficulty::Medium) ? 5 : 4;

    const auto types = availableTypes(config.difficulty);
    // Target: ~60% of cells covered by placed sequences
    const int numSeqs = std::max(2, total * 3 / 20);

    // Free positions (row-major indices)
    std::vector<int> free(total);
    std::iota(free.begin(), free.end(), 0);

    for (int s = 0; s < numSeqs && (int)free.size() >= 3; ++s) {
        bool placed = false;
        for (int attempt = 0; attempt < 20 && !placed; ++attempt) {
            SeqType type = types[std::uniform_int_distribution<int>(
                0, static_cast<int>(types.size()) - 1)(rng)];
            int len = std::uniform_int_distribution<int>(
                3, std::min(maxSeqLen, (int)free.size()))(rng);

            auto values = generateSeq(rng, type, len, maxVal);
            if (values.size() < 3) continue;
            len = static_cast<int>(values.size());
            if (len > (int)free.size()) continue;

            // Pick len random free positions and sort to row-major order
            std::shuffle(free.begin(), free.end(), rng);
            std::vector<int> chosen(free.begin(), free.begin() + len);
            std::sort(chosen.begin(), chosen.end());

            for (int i = 0; i < len; ++i)
                board.at(chosen[i] / cols, chosen[i] % cols) = GameCell(values[i]);

            free.erase(std::remove_if(free.begin(), free.end(), [&](int p) {
                return std::find(chosen.begin(), chosen.end(), p) != chosen.end();
            }), free.end());

            placed = true;
        }
    }

    // Fill remaining cells with noise
    std::uniform_int_distribution<uint16_t> noise(1, maxVal);
    for (int pos : free)
        board.at(pos / cols, pos % cols) = GameCell(noise(rng));
}

void NumberChaosRules::applySelection(GameState& state, const GameConfig&)
{
    const auto& cells = state.selection.cells();

    auto resetStates = [&] {
        for (auto [r, c] : cells)
            state.board.at(r, c).setState(CellState::Normal);
    };

    if (cells.size() < 3) {
        resetStates();
        state.selection.clear();
        return;
    }

    std::vector<uint16_t> values;
    values.reserve(cells.size());
    for (auto [r, c] : cells)
        values.push_back(state.board.at(r, c).value());

    if (!isValidSequence(values)) {
        resetStates();
        state.selection.clear();
        return;
    }

    for (auto [r, c] : cells)
        state.board.at(r, c) = GameCell(0, CellState::Empty);

    state.score += calcScore(cells.size());
    state.selection.clear();

    if (isBoardCleared(state.board))
        state.status = GameStatus::Won;
}

uint32_t NumberChaosRules::calcScore(size_t length) {
    return static_cast<uint32_t>(length) * 10;
}

bool NumberChaosRules::isValidSequence(const std::vector<uint16_t>& v) {
    if (v.size() < 3) return false;
    return isArithmetic(v)  ||
           isGeometric(v)   ||
           isSquares(v)     ||
           isAllEven(v)     ||
           isAllOdd(v)      ||
           isAllPrime(v)    ||
           isFactorial(v)   ||
           isSecondOrder(v);
}

bool NumberChaosRules::isArithmetic(const std::vector<uint16_t>& v) {
    int diff = (int)v[1] - (int)v[0];
    if (diff == 0) return false;
    for (size_t i = 2; i < v.size(); ++i)
        if ((int)v[i] - (int)v[i - 1] != diff) return false;
    return true;
}

bool NumberChaosRules::isGeometric(const std::vector<uint16_t>& v) {
    for (auto val : v) if (val == 0) return false;
    if (v[0] == v[1]) return false;
    if (v[0] < v[1]) {
        if (v[1] % v[0] != 0) return false;
        uint16_t r = v[1] / v[0];
        if (r < 2) return false;
        for (size_t i = 2; i < v.size(); ++i)
            if (v[i] % v[i-1] != 0 || v[i] / v[i-1] != r) return false;
    } else {
        if (v[0] % v[1] != 0) return false;
        uint16_t r = v[0] / v[1];
        if (r < 2) return false;
        for (size_t i = 2; i < v.size(); ++i)
            if (v[i-1] % v[i] != 0 || v[i-1] / v[i] != r) return false;
    }
    return true;
}

bool NumberChaosRules::isFibonacci(const std::vector<uint16_t>& v) {
    for (size_t i = 1; i < v.size(); ++i)
        if (v[i] < v[i-1]) return false;
    for (size_t i = 2; i < v.size(); ++i)
        if ((uint32_t)v[i-1] + (uint32_t)v[i-2] != v[i]) return false;
    return true;
}

bool NumberChaosRules::isSquares(const std::vector<uint16_t>& v) {
    std::vector<uint16_t> roots;
    roots.reserve(v.size());
    for (auto val : v) {
        auto r = (uint16_t)std::round(std::sqrt((double)val));
        if (r * r != val) return false;
        roots.push_back(r);
    }
    int diff = (int)roots[1] - (int)roots[0];
    if (diff != 1 && diff != -1) return false;
    for (size_t i = 2; i < roots.size(); ++i)
        if ((int)roots[i] - (int)roots[i-1] != diff) return false;
    return true;
}

bool NumberChaosRules::isAllEven(const std::vector<uint16_t>& v) {
    for (auto val : v) if (val % 2 != 0) return false;
    return true;
}

bool NumberChaosRules::isAllOdd(const std::vector<uint16_t>& v) {
    for (auto val : v) if (val % 2 != 1) return false;
    return true;
}

bool NumberChaosRules::isAllPrime(const std::vector<uint16_t>& v) {
    for (auto val : v) if (!isPrime(val)) return false;
    return true;
}

bool NumberChaosRules::isPrime(uint16_t n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (uint16_t i = 3; (uint32_t)i * i <= n; i += 2)
        if (n % i == 0) return false;
    return true;
}

bool NumberChaosRules::isFactorial(const std::vector<uint16_t>& v) {
    auto it = std::find(FACTORIALS.begin(), FACTORIALS.end(), v[0]);
    if (it == FACTORIALS.end()) return false;
    size_t idx = static_cast<size_t>(it - FACTORIALS.begin());
    for (size_t i = 1; i < v.size(); ++i) {
        if (idx + i >= FACTORIALS.size()) return false;
        if (v[i] != FACTORIALS[idx + i]) return false;
    }
    return true;
}

bool NumberChaosRules::isSecondOrder(const std::vector<uint16_t>& v) {
    if (v.size() < 4) return false;
    std::vector<int> diffs;
    diffs.reserve(v.size() - 1);
    for (size_t i = 1; i < v.size(); ++i)
        diffs.push_back((int)v[i] - (int)v[i-1]);
    int second_diff = diffs[1] - diffs[0];
    if (second_diff == 0) return false;
    for (size_t i = 2; i < diffs.size(); ++i)
        if (diffs[i] - diffs[i-1] != second_diff) return false;
    return true;
}

static bool nextCombination(std::vector<size_t>& idx, size_t n) {
    int i = static_cast<int>(idx.size()) - 1;
    while (i >= 0 && idx[i] == n - idx.size() + static_cast<size_t>(i)) --i;
    if (i < 0) return false;
    ++idx[i];
    for (size_t j = static_cast<size_t>(i) + 1; j < idx.size(); ++j)
        idx[j] = idx[j-1] + 1;
    return true;
}

std::vector<Selection> NumberChaosRules::getHint(const Board& board) const {
    std::vector<std::pair<uint8_t, uint8_t>> cells;
    for (uint8_t r = 0; r < board.rows(); ++r)
        for (uint8_t c = 0; c < board.cols(); ++c)
            if (board.at(r, c).state() != CellState::Empty)
                cells.push_back({r, c});

    size_t n = cells.size();
    if (n < 3) return {};

    std::vector<Selection> results;
    size_t maxSize = std::min(n, size_t{5});
    for (size_t size = 3; size <= maxSize; ++size) {
        std::vector<size_t> idx(size);
        std::iota(idx.begin(), idx.end(), 0);
        do {
            std::vector<uint16_t> values;
            values.reserve(size);
            for (size_t i : idx)
                values.push_back(board.at(cells[i].first, cells[i].second).value());
            if (isValidSequence(values)) {
                Selection sel;
                for (size_t i : idx) sel.add(cells[i].first, cells[i].second);
                results.push_back(sel);
            }
        } while (nextCombination(idx, n));
    }
    return results;
}

bool NumberChaosRules::isBoardCleared(const Board& board) {
    for (uint8_t r = 0; r < board.rows(); ++r)
        for (uint8_t c = 0; c < board.cols(); ++c)
            if (board.at(r, c).state() != CellState::Empty) return false;
    return true;
}
