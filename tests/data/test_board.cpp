#include <gtest/gtest.h>
#include "logic/data/board.h"

TEST(BoardTest, DefaultConstruction)
{
    Board b;
    EXPECT_EQ(b.rows(), 0);
    EXPECT_EQ(b.cols(), 0);
    EXPECT_TRUE(b.empty());
}

TEST(BoardTest, ConstructionWithSize)
{
    Board b(4, 5);
    EXPECT_EQ(b.rows(), 4);
    EXPECT_EQ(b.cols(), 5);
    EXPECT_EQ(b.size(), 20u);
}

TEST(BoardTest, CellsDefaultInitialized)
{
    Board b(3, 3);
    for (uint8_t r = 0; r < b.rows(); ++r)
        for (uint8_t c = 0; c < b.cols(); ++c) {
            EXPECT_EQ(b.at(r, c).value(), 0);
            EXPECT_EQ(b.at(r, c).state(), CellState::Normal);
        }
}

TEST(BoardTest, IsValidInsideBounds)
{
    Board b(4, 5);
    EXPECT_TRUE(b.isValid(0, 0));
    EXPECT_TRUE(b.isValid(3, 4));
    EXPECT_TRUE(b.isValid(2, 2));
}

TEST(BoardTest, IsValidOutsideBounds)
{
    Board b(4, 5);
    EXPECT_FALSE(b.isValid(4, 0));
    EXPECT_FALSE(b.isValid(0, 5));
    EXPECT_FALSE(b.isValid(4, 5));
}

TEST(BoardTest, AtReadWrite)
{
    Board b(3, 3);
    b.at(1, 2).setValue(7);
    b.at(1, 2).setState(CellState::Selected);

    EXPECT_EQ(b.at(1, 2).value(), 7);
    EXPECT_EQ(b.at(1, 2).state(), CellState::Selected);
}

TEST(BoardTest, AtConstVersion)
{
    Board b(2, 2);
    b.at(0, 1).setValue(42);

    const Board& cb = b;
    EXPECT_EQ(cb.at(0, 1).value(), 42);
}

TEST(BoardTest, AtOutOfRangeThrows)
{
    Board b(3, 3);
    EXPECT_THROW(b.at(3, 0), std::out_of_range);
    EXPECT_THROW(b.at(0, 3), std::out_of_range);
}

TEST(BoardTest, DifferentPositionsAreIndependent)
{
    Board b(2, 3);
    b.at(0, 0).setValue(1);
    b.at(0, 1).setValue(2);
    b.at(0, 2).setValue(3);
    b.at(1, 0).setValue(4);

    EXPECT_EQ(b.at(0, 0).value(), 1);
    EXPECT_EQ(b.at(0, 1).value(), 2);
    EXPECT_EQ(b.at(0, 2).value(), 3);
    EXPECT_EQ(b.at(1, 0).value(), 4);
}
