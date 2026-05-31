#include <gtest/gtest.h>
#include <algorithm>
#include "logic/rules/number_chaos_rules.h"
#include "logic/data/game_cell.h"

using V = std::vector<uint16_t>;

// Создаёт GameState с заполненным полем rows×cols, значения 1..N (row-major)
static GameState makeState(uint8_t rows, uint8_t cols) {
    GameState state;
    state.status = GameStatus::Playing;
    state.board = Board(rows, cols);
    uint16_t n = 0;
    for (uint8_t r = 0; r < rows; ++r)
        for (uint8_t c = 0; c < cols; ++c)
            state.board.at(r, c) = GameCell(++n);
    return state;
}

// --- isValidSequence: слишком короткая ---

TEST(NumberChaosRulesTest, TooShortReturnsFalse) {
    EXPECT_FALSE(NumberChaosRules::isValidSequence(V{1, 2}));
    EXPECT_FALSE(NumberChaosRules::isValidSequence(V{5}));
    EXPECT_FALSE(NumberChaosRules::isValidSequence(V{}));
}

// --- Арифметическая ---

TEST(NumberChaosRulesTest, ArithmeticAscending) {
    EXPECT_TRUE(NumberChaosRules::isValidSequence(V{2, 5, 8, 11}));
}

TEST(NumberChaosRulesTest, ArithmeticDescending) {
    EXPECT_TRUE(NumberChaosRules::isValidSequence(V{11, 8, 5, 2}));
}

TEST(NumberChaosRulesTest, RepeatedValuesInvalid) {
    // {9,9,9}: арифм. diff=0 — нет; 9 не простое. Невалидно.
    EXPECT_FALSE(NumberChaosRules::isValidSequence(V{9, 9, 9}));
}

TEST(NumberChaosRulesTest, ArithmeticBrokenInvalid) {
    // diff непостоянный, и ни под один другой тип не подходит
    EXPECT_FALSE(NumberChaosRules::isValidSequence(V{3, 7, 12}));
}

// --- Геометрическая ---

TEST(NumberChaosRulesTest, GeometricAscending) {
    EXPECT_TRUE(NumberChaosRules::isValidSequence(V{3, 6, 12, 24}));
}

TEST(NumberChaosRulesTest, GeometricDescending) {
    EXPECT_TRUE(NumberChaosRules::isValidSequence(V{24, 12, 6, 3}));
}

TEST(NumberChaosRulesTest, GeometricPowersOfTwo) {
    EXPECT_TRUE(NumberChaosRules::isValidSequence(V{2, 4, 8, 16}));
}

TEST(NumberChaosRulesTest, GeometricNonIntegerRatioInvalid) {
    // 6→9→13: не геометрическая, и ни под другой тип не подходит
    EXPECT_FALSE(NumberChaosRules::isValidSequence(V{4, 6, 9}));
}

// --- Квадраты ---

TEST(NumberChaosRulesTest, SquaresAscending) {
    EXPECT_TRUE(NumberChaosRules::isValidSequence(V{1, 4, 9, 16}));
}

TEST(NumberChaosRulesTest, SquaresDescending) {
    EXPECT_TRUE(NumberChaosRules::isValidSequence(V{9, 4, 1}));
}

TEST(NumberChaosRulesTest, SquaresMiddleRange) {
    EXPECT_TRUE(NumberChaosRules::isValidSequence(V{4, 9, 16, 25}));
}

TEST(NumberChaosRulesTest, SquaresNonConsecutiveInvalid) {
    // {1,9,25}: корни 1,3,5 — шаг 2, isSquares=false; AllOdd удалён. Невалидно.
    EXPECT_FALSE(NumberChaosRules::isValidSequence(V{1, 9, 25}));
}

TEST(NumberChaosRulesTest, SquaresNotAllSquaresInvalid) {
    // {3,8,15}: не квадраты, не арифм.(diff 5,7), не геом., не фибо,
    //           не чётные, не нечётные (8 чётное), не простые (15=3×5), не факториалы
    EXPECT_FALSE(NumberChaosRules::isValidSequence(V{3, 8, 15}));
}

// --- Removed: AllEven, AllOdd ---
// Mixed arbitrary even/odd values no longer form valid sequences.

TEST(NumberChaosRulesTest, MixedEvenOddInvalid) {
    EXPECT_FALSE(NumberChaosRules::isValidSequence(V{4, 5, 7}));
}

// --- Все простые ---

TEST(NumberChaosRulesTest, AllPrime) {
    EXPECT_TRUE(NumberChaosRules::isValidSequence(V{2, 11, 5, 7}));
}

TEST(NumberChaosRulesTest, AllPrimeUnordered) {
    EXPECT_TRUE(NumberChaosRules::isValidSequence(V{13, 2, 17, 5}));
}

TEST(NumberChaosRulesTest, NotAllPrimeInvalid) {
    // 9 = 3² — не простое; и ни под другой тип не подходит
    EXPECT_FALSE(NumberChaosRules::isValidSequence(V{2, 3, 9}));
}

// --- Факториалы ---

TEST(NumberChaosRulesTest, FactorialFromStart) {
    EXPECT_TRUE(NumberChaosRules::isValidSequence(V{1, 2, 6, 24}));
}

TEST(NumberChaosRulesTest, FactorialSubsequence) {
    EXPECT_TRUE(NumberChaosRules::isValidSequence(V{2, 6, 24, 120}));
}

TEST(NumberChaosRulesTest, FactorialNotConsecutiveInvalid) {
    // 1! затем пропуск 2!, сразу 3!
    EXPECT_FALSE(NumberChaosRules::isValidSequence(V{1, 6, 24}));
}

TEST(NumberChaosRulesTest, FactorialWrongValueInvalid) {
    EXPECT_FALSE(NumberChaosRules::isValidSequence(V{1, 2, 6, 25}));
}

// --- Второй порядок ---

TEST(NumberChaosRulesTest, SecondOrderAscending) {
    // diffs: 1,2,3,4 — арифм. с шагом +1
    EXPECT_TRUE(NumberChaosRules::isValidSequence(V{1, 2, 4, 7, 11}));
}

TEST(NumberChaosRulesTest, SecondOrderFourElements) {
    // diffs: 2,3,4 — арифм. с шагом +1
    EXPECT_TRUE(NumberChaosRules::isValidSequence(V{1, 3, 6, 10}));
}

TEST(NumberChaosRulesTest, SecondOrderTooShortInvalid) {
    // n=3 < 4 — недостаточно для второго порядка
    // {4,5,7}: не арифм.(diff 1,2), не геом., не фибо(4+5≠7), не квадраты(5 не кв.),
    //          не чётные (5,7 нечётные), не нечётные (4 чётное),
    //          не простые (4 не простое), не факториалы, не 2-й порядок
    EXPECT_FALSE(NumberChaosRules::isValidSequence(V{4, 5, 7}));
}

TEST(NumberChaosRulesTest, SecondOrderWithArithmeticDiffsInvalid) {
    // diffs постоянные (second_diff=0) → это просто арифм., isSecondOrder вернёт false,
    // но isArithmetic вернёт true → isValidSequence = true
    EXPECT_TRUE(NumberChaosRules::isValidSequence(V{1, 3, 5, 7}));
}

// --- calcScore ---

TEST(NumberChaosRulesTest, CalcScore) {
    EXPECT_EQ(NumberChaosRules::calcScore(3), 30u);  // 3*2*5
    EXPECT_EQ(NumberChaosRules::calcScore(4), 60u);  // 4*3*5
    EXPECT_EQ(NumberChaosRules::calcScore(5), 100u); // 5*4*5
}

// --- applySelection ---

TEST(NumberChaosRulesTest, ValidSelectionRemovesCells) {
    // Поле 2×3: значения 1,2,3,4,5,6
    // Выделяем (0,0)=1, (0,1)=2, (0,2)=3 → арифм. +1
    NumberChaosRules rules;
    GameConfig cfg; cfg.gridRows = 2; cfg.gridCols = 3;
    auto state = makeState(2, 3);

    state.selection.add(0, 0);
    state.selection.add(0, 1);
    state.selection.add(0, 2);
    rules.applySelection(state, cfg);

    EXPECT_EQ(state.board.at(0, 0).state(), CellState::Empty);
    EXPECT_EQ(state.board.at(0, 1).state(), CellState::Empty);
    EXPECT_EQ(state.board.at(0, 2).state(), CellState::Empty);
    // Остальные не тронуты
    EXPECT_NE(state.board.at(1, 0).state(), CellState::Empty);
}

TEST(NumberChaosRulesTest, ValidSelectionAddsScore) {
    NumberChaosRules rules;
    GameConfig cfg; cfg.gridRows = 2; cfg.gridCols = 3;
    auto state = makeState(2, 3);

    state.selection.add(0, 0); // 1
    state.selection.add(0, 1); // 2
    state.selection.add(0, 2); // 3
    rules.applySelection(state, cfg);

    EXPECT_EQ(state.score, 30u);
}

TEST(NumberChaosRulesTest, ValidSelectionClearsSelection) {
    NumberChaosRules rules;
    GameConfig cfg; cfg.gridRows = 2; cfg.gridCols = 3;
    auto state = makeState(2, 3);

    state.selection.add(0, 0);
    state.selection.add(0, 1);
    state.selection.add(0, 2);
    rules.applySelection(state, cfg);

    EXPECT_TRUE(state.selection.empty());
}

TEST(NumberChaosRulesTest, InvalidSelectionLeavesCellsIntact) {
    // {3,7,12} — не является никакой последовательностью
    NumberChaosRules rules;
    GameConfig cfg; cfg.gridRows = 1; cfg.gridCols = 3;
    GameState state;
    state.status = GameStatus::Playing;
    state.board = Board(1, 3);
    state.board.at(0, 0) = GameCell(3);
    state.board.at(0, 1) = GameCell(7);
    state.board.at(0, 2) = GameCell(12);

    state.selection.add(0, 0);
    state.selection.add(0, 1);
    state.selection.add(0, 2);
    rules.applySelection(state, cfg);

    EXPECT_NE(state.board.at(0, 0).state(), CellState::Empty);
    EXPECT_NE(state.board.at(0, 1).state(), CellState::Empty);
    EXPECT_NE(state.board.at(0, 2).state(), CellState::Empty);
    EXPECT_EQ(state.score, 0u);
    EXPECT_TRUE(state.selection.empty());
}

TEST(NumberChaosRulesTest, ClearingBoardSetsWon) {
    // Поле 1×3: значения 1,2,3 → арифм. +1 → удаляем все → Won
    NumberChaosRules rules;
    GameConfig cfg; cfg.gridRows = 1; cfg.gridCols = 3;
    GameState state;
    state.status = GameStatus::Playing;
    state.board = Board(1, 3);
    state.board.at(0, 0) = GameCell(1);
    state.board.at(0, 1) = GameCell(2);
    state.board.at(0, 2) = GameCell(3);

    state.selection.add(0, 0);
    state.selection.add(0, 1);
    state.selection.add(0, 2);
    rules.applySelection(state, cfg);

    EXPECT_EQ(state.status, GameStatus::Won);
}

TEST(NumberChaosRulesTest, PartialClearDoesNotSetWon) {
    NumberChaosRules rules;
    GameConfig cfg; cfg.gridRows = 2; cfg.gridCols = 3;
    auto state = makeState(2, 3);

    state.selection.add(0, 0);
    state.selection.add(0, 1);
    state.selection.add(0, 2);
    rules.applySelection(state, cfg);

    EXPECT_EQ(state.status, GameStatus::Playing);
}

// --- getHint ---

TEST(NumberChaosRulesTest, HintEmptyBoard) {
    NumberChaosRules rules;
    Board board(2, 3);
    for (uint8_t r = 0; r < 2; ++r)
        for (uint8_t c = 0; c < 3; ++c)
            board.at(r, c) = GameCell(0, CellState::Empty);
    EXPECT_TRUE(rules.getHint(board).empty());
}

TEST(NumberChaosRulesTest, HintNoValidSequence) {
    // {3,7,12} — не образует валидную последовательность
    NumberChaosRules rules;
    Board board(1, 3);
    board.at(0, 0) = GameCell(3);
    board.at(0, 1) = GameCell(7);
    board.at(0, 2) = GameCell(12);
    EXPECT_TRUE(rules.getHint(board).empty());
}

TEST(NumberChaosRulesTest, HintFindsSequence) {
    // {2,5,8} — арифм. +3
    NumberChaosRules rules;
    Board board(1, 3);
    board.at(0, 0) = GameCell(2);
    board.at(0, 1) = GameCell(5);
    board.at(0, 2) = GameCell(8);
    auto hints = rules.getHint(board);
    ASSERT_FALSE(hints.empty());
    std::vector<uint16_t> values;
    for (auto [r, c] : hints[0].cells())
        values.push_back(board.at(r, c).value());
    EXPECT_TRUE(NumberChaosRules::isValidSequence(values));
}

TEST(NumberChaosRulesTest, HintFindsNonAdjacentSequence) {
    // Row: [2, noise, 5, noise, 8] — arithmetic seq at positions 0,2,4
    // Hint must find [2,5,8] across noise cells within the same row
    NumberChaosRules rules;
    Board board(1, 5);
    board.at(0, 0) = GameCell(2);
    GameCell n1(99); n1.setNoise(true); board.at(0, 1) = n1;
    board.at(0, 2) = GameCell(5);
    GameCell n2(77); n2.setNoise(true); board.at(0, 3) = n2;
    board.at(0, 4) = GameCell(8);
    auto hints = rules.getHint(board);
    ASSERT_FALSE(hints.empty());
    // The found hint must contain cells from the same row only
    for (const auto& h : hints) {
        const auto& cells = h.cells();
        bool sameRow = std::all_of(cells.begin(), cells.end(),
                                   [&](auto p) { return p.first == cells[0].first; });
        bool sameCol = std::all_of(cells.begin(), cells.end(),
                                   [&](auto p) { return p.second == cells[0].second; });
        EXPECT_TRUE(sameRow || sameCol);
    }
}

TEST(NumberChaosRulesTest, HintPrefersShorterSequence) {
    // {2,5,8,11}: есть тройки и четвёрка, hint должен вернуть 3 ячейки
    NumberChaosRules rules;
    Board board(1, 4);
    board.at(0, 0) = GameCell(2);
    board.at(0, 1) = GameCell(5);
    board.at(0, 2) = GameCell(8);
    board.at(0, 3) = GameCell(11);
    auto hints = rules.getHint(board);
    ASSERT_FALSE(hints.empty());
    EXPECT_EQ(hints[0].size(), 3u);
}

// --- initBoard ---

TEST(NumberChaosRulesTest, InitBoardFillsAllCells) {
    NumberChaosRules rules;
    GameConfig cfg; cfg.gridRows = 3; cfg.gridCols = 3;
    Board board(3, 3);
    rules.initBoard(board, cfg);

    for (uint8_t r = 0; r < 3; ++r)
        for (uint8_t c = 0; c < 3; ++c)
            EXPECT_NE(board.at(r, c).state(), CellState::Empty);
}
