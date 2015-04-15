#include "stdafx.hpp"

#include "reversi/game.hpp"
#include "reversi/player.hpp"
#include "reversi/score.hpp"

namespace reversi { namespace testing
{

using ::testing::Eq;
using ::testing::Test;

class Game : public Test
{
    
protected:

    int board_size = 8;

    game the_game{board_size};

};

TEST_THAT(Game,
     WHAT(GetBoardSize),
     WHEN(Always),
     THEN(ReturnsTheSizePassedAtConstruction))
{
    EXPECT_THAT(the_game.get_board_size(), Eq(board_size));
}

TEST_THAT(Game,
     WHAT(GetScore),
     WHEN(ImmediatelyAfterConstruction),
     THEN(ReturnsZeroForBothWhiteAndBlack))
{
    auto const s = the_game.get_score();

    EXPECT_THAT(s.white, Eq(0));
    EXPECT_THAT(s.black, Eq(0));
}

TEST_THAT(Game,
     WHAT(GetNextMovingPlayer),
     WHEN(ImmediatelyAfterConstruction),
     THEN(ReturnsBlack))
{
    EXPECT_THAT(the_game.get_next_moving_player(), Eq(player::black));
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(GivenARowIndexWhichIsLowerThanZero),
     THEN(Throws))
{
    EXPECT_THROW((the_game.place(-1, 3)), bad_cell_coordinates_exception);
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(GivenARowIndexWhichIsNotLowerThanTheBoardSize),
     THEN(Throws))
{
    EXPECT_THROW((the_game.place(board_size, 3)), bad_cell_coordinates_exception);
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(GivenAColumnIndexWhichIsLowerThanZero),
     THEN(Throws))
{
    EXPECT_THROW((the_game.place(1, -2)), bad_cell_coordinates_exception);
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(GivenAColumnIndexWhichIsNotLowerThanTheBoardSize),
     THEN(Throws))
{
    EXPECT_THROW((the_game.place(2, board_size)), bad_cell_coordinates_exception);
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(GivenValidCoordinates),
     THEN(DoesNotThrow))
{
    EXPECT_NO_THROW(the_game.place(0, 0));
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(OnSuccess),
     THEN(SwitchesTurn))
{
    the_game.place(0, 0);

    EXPECT_THAT(the_game.get_next_moving_player(), Eq(player::white));

    the_game.place(0, 1);

    EXPECT_THAT(the_game.get_next_moving_player(), Eq(player::black));
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(OnFailure),
     THEN(DoesNotSwitchTurn))
{
    the_game.place(0, 0);

    try
    {
        the_game.place(0, 0);
    }
    catch (std::exception const&)
    {
        EXPECT_THAT(the_game.get_next_moving_player(), Eq(player::white));
    }
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(GivenTheCoordinatesOfANonEmptyCell),
     THEN(Throws))
{
    the_game.place(0, 0);

    EXPECT_THROW((the_game.place(0, 0)), cell_busy_exception);
}

} }