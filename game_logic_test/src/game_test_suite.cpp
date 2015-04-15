#include "stdafx.hpp"

#include "reversi/game.hpp"
#include "reversi/score.hpp"

namespace reversi { namespace testing
{

using ::testing::Eq;
using ::testing::Test;

class Game : public Test
{
    
protected:

    game the_game;

};

TEST_THAT(Game,
     WHAT(GetScore),
     WHEN(ImmediatelyAfterConstruction),
     THEN(ReturnsZeroForBothWhiteAndBlack))
{
    auto const s = the_game.get_score();

    EXPECT_THAT(s.white, Eq(0));
    EXPECT_THAT(s.black, Eq(0));
}

} }