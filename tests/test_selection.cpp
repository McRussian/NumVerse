#include <gtest/gtest.h>
#include "logic/selection.h"

TEST(SelectionTest, DefaultEmpty)
{
    Selection s;
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0u);
}

TEST(SelectionTest, AddCell)
{
    Selection s;
    s.add(1, 2);
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.size(), 1u);
    EXPECT_TRUE(s.contains(1, 2));
}

TEST(SelectionTest, AddMultipleCells)
{
    Selection s;
    s.add(0, 0);
    s.add(1, 1);
    s.add(2, 2);
    EXPECT_EQ(s.size(), 3u);
}

TEST(SelectionTest, AddDuplicateIgnored)
{
    Selection s;
    s.add(1, 2);
    s.add(1, 2);
    EXPECT_EQ(s.size(), 1u);
}

TEST(SelectionTest, RemoveExisting)
{
    Selection s;
    s.add(1, 2);
    s.add(3, 4);
    s.remove(1, 2);
    EXPECT_EQ(s.size(), 1u);
    EXPECT_FALSE(s.contains(1, 2));
    EXPECT_TRUE(s.contains(3, 4));
}

TEST(SelectionTest, RemoveNonExistingNoEffect)
{
    Selection s;
    s.add(1, 2);
    s.remove(9, 9);
    EXPECT_EQ(s.size(), 1u);
}

TEST(SelectionTest, ContainsMissing)
{
    Selection s;
    s.add(1, 2);
    EXPECT_FALSE(s.contains(1, 3));
    EXPECT_FALSE(s.contains(2, 2));
}

TEST(SelectionTest, Clear)
{
    Selection s;
    s.add(0, 0);
    s.add(1, 1);
    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0u);
}

TEST(SelectionTest, CellsReturnsAllAdded)
{
    Selection s;
    s.add(0, 1);
    s.add(2, 3);
    const auto& cells = s.cells();
    EXPECT_EQ(cells.size(), 2u);
    const auto p0 = std::make_pair<uint8_t, uint8_t>(0, 1);
    const auto p1 = std::make_pair<uint8_t, uint8_t>(2, 3);
    EXPECT_EQ(cells[0], p0);
    EXPECT_EQ(cells[1], p1);
}
