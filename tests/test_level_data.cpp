#include <gtest/gtest.h>
#include "logic/level_data.h"

static LevelData makeValid(uint8_t rows = 4, uint8_t cols = 4)
{
    std::vector<uint16_t> board(rows * cols, 1);
    return LevelData(1, Difficulty::Easy, rows, cols, 10, 180, 0, board);
}

TEST(LevelDataTest, DefaultIsInvalid)
{
    LevelData ld;
    EXPECT_FALSE(ld.isValid());
}

TEST(LevelDataTest, ValidData)
{
    EXPECT_TRUE(makeValid().isValid());
}

TEST(LevelDataTest, InvalidLevel)
{
    std::vector<uint16_t> board(16, 1);
    LevelData ld(0, Difficulty::Easy, 4, 4, 0, 0, 0, board);
    EXPECT_FALSE(ld.isValid());
}

TEST(LevelDataTest, InvalidGridRows)
{
    std::vector<uint16_t> board;
    LevelData ld(1, Difficulty::Easy, 0, 4, 0, 0, 0, board);
    EXPECT_FALSE(ld.isValid());
}

TEST(LevelDataTest, InvalidGridCols)
{
    std::vector<uint16_t> board;
    LevelData ld(1, Difficulty::Easy, 4, 0, 0, 0, 0, board);
    EXPECT_FALSE(ld.isValid());
}

TEST(LevelDataTest, InvalidBoardSize)
{
    std::vector<uint16_t> board(10, 1); // должно быть 16
    LevelData ld(1, Difficulty::Easy, 4, 4, 0, 0, 0, board);
    EXPECT_FALSE(ld.isValid());
}

TEST(LevelDataTest, Getters)
{
    std::vector<uint16_t> board(6, 5);
    LevelData ld(3, Difficulty::Hard, 2, 3, 8, 120, 24, board);

    EXPECT_EQ(ld.level(), 3);
    EXPECT_EQ(ld.difficulty(), Difficulty::Hard);
    EXPECT_EQ(ld.gridRows(), 2);
    EXPECT_EQ(ld.gridCols(), 3);
    EXPECT_EQ(ld.maxMoves(), 8);
    EXPECT_EQ(ld.timeLimitSecs(), 120);
    EXPECT_EQ(ld.targetSum(), 24);
    EXPECT_EQ(ld.board().size(), 6u);
}

TEST(LevelDataTest, ToConfig)
{
    std::vector<uint16_t> board(6, 1);
    LevelData ld(2, Difficulty::Medium, 2, 3, 5, 90, 12, board);
    GameConfig cfg = ld.toConfig();

    EXPECT_EQ(cfg.level, 2);
    EXPECT_EQ(cfg.difficulty, Difficulty::Medium);
    EXPECT_EQ(cfg.gridRows, 2);
    EXPECT_EQ(cfg.gridCols, 3);
    EXPECT_EQ(cfg.maxMoves, 5);
    EXPECT_EQ(cfg.timeLimitSecs, 90);
    EXPECT_EQ(cfg.targetSum, 12);
}
