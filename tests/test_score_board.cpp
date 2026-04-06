#include <gtest/gtest.h>
#include "logic/score_board.h"

static GameResult makeResult(uint32_t score, uint32_t timeSecs, bool won,
                             const std::string& name = "player")
{
    return GameResult{name, score, timeSecs, won};
}

// --- ByScore mode ---

TEST(ScoreBoardByScore, EmptyBoard)
{
    ScoreBoard sb;
    EXPECT_TRUE(sb.all().empty());
    EXPECT_TRUE(sb.top(1).empty());
    EXPECT_FALSE(sb.best(1).has_value());
}

TEST(ScoreBoardByScore, LostResultIgnored)
{
    ScoreBoard sb;
    sb.add(1, makeResult(500, 30, false));
    EXPECT_TRUE(sb.top(1).empty());
}

TEST(ScoreBoardByScore, SortedByScoreDesc)
{
    ScoreBoard sb;
    sb.add(1, makeResult(100, 60, true));
    sb.add(1, makeResult(300, 90, true));
    sb.add(1, makeResult(200, 30, true));

    auto list = sb.top(1);
    EXPECT_EQ(list[0].score, 300u);
    EXPECT_EQ(list[1].score, 200u);
    EXPECT_EQ(list[2].score, 100u);
}

TEST(ScoreBoardByScore, TieBreakByTimeAsc)
{
    ScoreBoard sb;
    sb.add(1, makeResult(200, 90, true));
    sb.add(1, makeResult(200, 40, true));

    auto list = sb.top(1);
    EXPECT_EQ(list[0].timeSecs, 40u);
    EXPECT_EQ(list[1].timeSecs, 90u);
}

TEST(ScoreBoardByScore, BetterEntersWorstDropped)
{
    ScoreBoard sb(RankingMode::ByScore, 3);
    sb.add(1, makeResult(100, 60, true));
    sb.add(1, makeResult(200, 60, true));
    sb.add(1, makeResult(300, 60, true));
    sb.add(1, makeResult(150, 60, true));

    auto list = sb.top(1);
    EXPECT_EQ(list.size(), 3u);
    EXPECT_EQ(list.back().score, 150u);
}

TEST(ScoreBoardByScore, WorseIgnored)
{
    ScoreBoard sb(RankingMode::ByScore, 3);
    sb.add(1, makeResult(100, 60, true));
    sb.add(1, makeResult(200, 60, true));
    sb.add(1, makeResult(300, 60, true));
    sb.add(1, makeResult(50, 60, true));

    EXPECT_EQ(sb.top(1).size(), 3u);
    EXPECT_EQ(sb.top(1).back().score, 100u);
}

TEST(ScoreBoardByScore, AddReturnsTrue)
{
    ScoreBoard sb(RankingMode::ByScore, 2);
    EXPECT_TRUE(sb.add(1, makeResult(200, 60, true)));
    EXPECT_TRUE(sb.add(1, makeResult(100, 60, true)));
    EXPECT_TRUE(sb.add(1, makeResult(150, 60, true)));   // вытесняет 100
    EXPECT_FALSE(sb.add(1, makeResult(50, 60, true)));   // хуже худшего
    EXPECT_FALSE(sb.add(1, makeResult(100, 60, false))); // проигрыш
}

TEST(ScoreBoardByScore, Best)
{
    ScoreBoard sb;
    sb.add(1, makeResult(100, 60, true));
    sb.add(1, makeResult(300, 90, true));

    auto best = sb.best(1);
    ASSERT_TRUE(best.has_value());
    EXPECT_EQ(best->score, 300u);
}

TEST(ScoreBoardByScore, PlayerNamePreserved)
{
    ScoreBoard sb;
    sb.add(1, makeResult(300, 60, true, "Alice"));
    sb.add(1, makeResult(100, 30, true, "Bob"));

    auto list = sb.top(1);
    EXPECT_EQ(list[0].playerName, "Alice");
    EXPECT_EQ(list[1].playerName, "Bob");
}

// --- clear ---

TEST(ScoreBoardClear, ClearLevel)
{
    ScoreBoard sb;
    sb.add(1, makeResult(100, 60, true));
    sb.add(2, makeResult(200, 30, true));

    sb.clear(1);

    EXPECT_TRUE(sb.top(1).empty());
    EXPECT_FALSE(sb.top(2).empty());
}

TEST(ScoreBoardClear, ClearNonExistentLevelNoEffect)
{
    ScoreBoard sb;
    sb.add(1, makeResult(100, 60, true));
    sb.clear(9);
    EXPECT_EQ(sb.top(1).size(), 1u);
}

TEST(ScoreBoardClear, ClearAll)
{
    ScoreBoard sb;
    sb.add(1, makeResult(100, 60, true));
    sb.add(2, makeResult(200, 30, true));

    sb.clearAll();

    EXPECT_TRUE(sb.all().empty());
}

// --- ByTime mode ---

TEST(ScoreBoardByTime, SortedByTimeAsc)
{
    ScoreBoard sb(RankingMode::ByTime);
    sb.add(1, makeResult(100, 60, true));
    sb.add(1, makeResult(200, 30, true));
    sb.add(1, makeResult(150, 45, true));

    auto list = sb.top(1);
    EXPECT_EQ(list[0].timeSecs, 30u);
    EXPECT_EQ(list[1].timeSecs, 45u);
    EXPECT_EQ(list[2].timeSecs, 60u);
}

TEST(ScoreBoardByTime, TieBreakByScoreDesc)
{
    ScoreBoard sb(RankingMode::ByTime);
    sb.add(1, makeResult(100, 40, true));
    sb.add(1, makeResult(200, 40, true));

    auto list = sb.top(1);
    EXPECT_EQ(list[0].score, 200u);
    EXPECT_EQ(list[1].score, 100u);
}

TEST(ScoreBoardByTime, BetterEntersWorstDropped)
{
    ScoreBoard sb(RankingMode::ByTime, 3);
    sb.add(1, makeResult(100, 30, true));
    sb.add(1, makeResult(100, 45, true));
    sb.add(1, makeResult(100, 60, true));
    sb.add(1, makeResult(100, 40, true));

    auto list = sb.top(1);
    EXPECT_EQ(list.size(), 3u);
    EXPECT_EQ(list.back().timeSecs, 45u);
}

TEST(ScoreBoardByTime, AddReturnsTrue)
{
    ScoreBoard sb(RankingMode::ByTime, 2);
    sb.add(1, makeResult(100, 30, true));
    sb.add(1, makeResult(100, 60, true));

    EXPECT_TRUE(sb.add(1, makeResult(100, 50, true)));  // 50 < 60 (худший) — войдёт
    EXPECT_FALSE(sb.add(1, makeResult(100, 90, true))); // 90 > 60 (худший) — не войдёт
}
