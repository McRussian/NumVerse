#include <gtest/gtest.h>
#include "logic/game_cell.h"

TEST(GameCellTest, DefaultConstruction)
{
    GameCell cell;
    EXPECT_EQ(cell.value, 0);
    EXPECT_EQ(cell.state, CellState::Normal);
}

TEST(GameCellTest, ValueAssignment)
{
    GameCell cell;
    cell.value = 42;
    EXPECT_EQ(cell.value, 42);
}

TEST(GameCellTest, StateAssignment)
{
    GameCell cell;
    cell.state = CellState::Selected;
    EXPECT_EQ(cell.state, CellState::Selected);

    cell.state = CellState::Locked;
    EXPECT_EQ(cell.state, CellState::Locked);
}

TEST(GameCellTest, AggregateInit)
{
    GameCell cell{7, CellState::Highlighted};
    EXPECT_EQ(cell.value, 7);
    EXPECT_EQ(cell.state, CellState::Highlighted);
}

TEST(GameCellTest, Equality)
{
    GameCell a{5, CellState::Normal};
    GameCell b{5, CellState::Normal};
    GameCell c{5, CellState::Selected};
    GameCell d{9, CellState::Normal};

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
    EXPECT_NE(a, d);
}
