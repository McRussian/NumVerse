#include <gtest/gtest.h>
#include "logic/game_logic.h"
#include "logic/rules/stub_rules.h"

// Правила для проверки ветки: rules выставляют Won
class WinningStubRules : public IGameRules {
public:
    void initBoard(Board& board, const GameConfig& config) override {
        for (uint8_t r = 0; r < config.gridRows; ++r)
            for (uint8_t c = 0; c < config.gridCols; ++c)
                board.at(r, c) = GameCell(1);
    }
    void applySelection(GameState& state, const GameConfig&) override {
        state.status = GameStatus::Won;
        state.selection.clear();
    }
};

static GameLogic makeGame(uint8_t rows = 3, uint8_t cols = 3,
                           uint16_t maxMoves = 0, uint16_t timeLimitSecs = 0) {
    GameConfig cfg;
    cfg.gridRows = rows;
    cfg.gridCols = cols;
    cfg.maxMoves = maxMoves;
    cfg.timeLimitSecs = timeLimitSecs;
    GameLogic gl;
    gl.init(cfg, std::make_unique<StubRules>());
    return gl;
}

// --- init ---

TEST(GameLogicTest, InitSetsPlayingStatus) {
    auto gl = makeGame();
    EXPECT_EQ(gl.getState().status, GameStatus::Playing);
}

TEST(GameLogicTest, InitPopulatesBoard) {
    auto gl = makeGame(2, 3);
    EXPECT_EQ(gl.getState().board.at(0, 0).value(), 1);
    EXPECT_EQ(gl.getState().board.at(0, 2).value(), 3);
    EXPECT_EQ(gl.getState().board.at(1, 0).value(), 4);
}

TEST(GameLogicTest, InitSetsMoveLimit) {
    auto gl = makeGame(3, 3, 5);
    EXPECT_EQ(gl.getState().movesLeft, 5);
}

// --- select ---

TEST(GameLogicTest, SelectAddsToSelection) {
    auto gl = makeGame();
    gl.select(0, 0);
    EXPECT_TRUE(gl.getState().selection.contains(0, 0));
}

TEST(GameLogicTest, SelectTogglesCell) {
    auto gl = makeGame();
    gl.select(1, 1);
    gl.select(1, 1);
    EXPECT_FALSE(gl.getState().selection.contains(1, 1));
}

TEST(GameLogicTest, SelectIgnoresOutOfBounds) {
    auto gl = makeGame(3, 3);
    gl.select(5, 5);
    EXPECT_TRUE(gl.getState().selection.empty());
}

TEST(GameLogicTest, SelectNoOpWhenNotPlaying) {
    auto gl = makeGame();
    gl.tick(1000); // не достигнет лимита (лимит 0), но просто проверим через Paused имитацию
    // Принудительно потеряем состояние через лимит ходов
    GameConfig cfg;
    cfg.gridRows = 2; cfg.gridCols = 2; cfg.maxMoves = 1;
    GameLogic gl2;
    gl2.init(cfg, std::make_unique<StubRules>());
    gl2.select(0, 0);
    gl2.applySelection();  // movesLeft → 0 → Lost
    ASSERT_EQ(gl2.getState().status, GameStatus::Lost);
    gl2.select(0, 1);
    EXPECT_FALSE(gl2.getState().selection.contains(0, 1));
}

// --- applySelection ---

TEST(GameLogicTest, ApplySelectionCallsRules) {
    auto gl = makeGame();
    gl.select(0, 0);
    gl.applySelection();
    EXPECT_EQ(gl.getState().score, 10);
}

TEST(GameLogicTest, ApplySelectionClearsSelection) {
    auto gl = makeGame();
    gl.select(0, 0);
    gl.applySelection();
    EXPECT_TRUE(gl.getState().selection.empty());
}

TEST(GameLogicTest, ApplySelectionDecrementsMovesLeft) {
    auto gl = makeGame(3, 3, 5);
    gl.select(0, 0);
    gl.applySelection();
    EXPECT_EQ(gl.getState().movesLeft, 4);
}

TEST(GameLogicTest, ApplySelectionSetsLostWhenMovesExhausted) {
    auto gl = makeGame(3, 3, 1);
    gl.select(0, 0);
    gl.applySelection();
    EXPECT_EQ(gl.getState().status, GameStatus::Lost);
}

TEST(GameLogicTest, ApplySelectionNoOpWhenSelectionEmpty) {
    auto gl = makeGame(3, 3, 3);
    gl.applySelection();
    EXPECT_EQ(gl.getState().movesLeft, 3);
    EXPECT_EQ(gl.getState().score, 0);
}

TEST(GameLogicTest, ApplySelectionPreservesWonStatus) {
    GameConfig cfg;
    cfg.gridRows = 2; cfg.gridCols = 2; cfg.maxMoves = 1;
    GameLogic gl;
    gl.init(cfg, std::make_unique<WinningStubRules>());
    gl.select(0, 0);
    gl.applySelection();
    // rules выставили Won; movesLeft не должен стать Lost
    EXPECT_EQ(gl.getState().status, GameStatus::Won);
}

// --- tick ---

TEST(GameLogicTest, TickUpdatesSeconds) {
    auto gl = makeGame();
    gl.tick(10);
    EXPECT_EQ(gl.getState().secondsElapsed, 10u);
}

TEST(GameLogicTest, TickSetsLostOnTimeLimit) {
    GameConfig cfg;
    cfg.gridRows = 2; cfg.gridCols = 2; cfg.timeLimitSecs = 30;
    GameLogic gl;
    gl.init(cfg, std::make_unique<StubRules>());
    gl.tick(30);
    EXPECT_EQ(gl.getState().status, GameStatus::Lost);
}

TEST(GameLogicTest, TickNoLostWhenNoLimit) {
    auto gl = makeGame();
    gl.tick(9999);
    EXPECT_EQ(gl.getState().status, GameStatus::Playing);
}

TEST(GameLogicTest, TickNoOpWhenNotPlaying) {
    GameConfig cfg;
    cfg.gridRows = 2; cfg.gridCols = 2; cfg.timeLimitSecs = 10;
    GameLogic gl;
    gl.init(cfg, std::make_unique<StubRules>());
    gl.tick(10); // → Lost
    gl.tick(50); // не должно менять secondsElapsed
    EXPECT_EQ(gl.getState().secondsElapsed, 10u);
}

// --- buildResult ---

TEST(GameLogicTest, BuildResultWon) {
    GameConfig cfg;
    cfg.gridRows = 2; cfg.gridCols = 2;
    GameLogic gl;
    gl.init(cfg, std::make_unique<WinningStubRules>());
    gl.select(0, 0);
    gl.applySelection();
    auto r = gl.buildResult("Alice");
    EXPECT_EQ(r.playerName, "Alice");
    EXPECT_TRUE(r.won);
}

TEST(GameLogicTest, BuildResultNotWon) {
    auto gl = makeGame();
    auto r = gl.buildResult("Bob");
    EXPECT_FALSE(r.won);
    EXPECT_EQ(r.playerName, "Bob");
}

TEST(GameLogicTest, BuildResultScore) {
    auto gl = makeGame();
    gl.select(0, 0); gl.applySelection();
    gl.select(0, 1); gl.applySelection();
    EXPECT_EQ(gl.buildResult("").score, 20u);
}

// --- reset ---

TEST(GameLogicTest, ResetRestoresPlaying) {
    auto gl = makeGame(3, 3, 1);
    gl.select(0, 0);
    gl.applySelection(); // → Lost
    gl.reset();
    EXPECT_EQ(gl.getState().status, GameStatus::Playing);
    EXPECT_EQ(gl.getState().score, 0u);
    EXPECT_EQ(gl.getState().movesLeft, 1);
}

TEST(GameLogicTest, ResetClearsSelection) {
    auto gl = makeGame();
    gl.select(1, 1);
    gl.reset();
    EXPECT_TRUE(gl.getState().selection.empty());
}
