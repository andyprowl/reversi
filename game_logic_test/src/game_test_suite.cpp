#include "stdafx.hpp"

#include "reversi/cell_position.hpp"
#include "reversi/game.hpp"
#include "reversi/player.hpp"
#include "reversi/game_score.hpp"

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
     THEN(ReturnsTwoForBothWhiteAndBlack))
{
    auto const s = the_game.get_score();

    EXPECT_THAT(s.white, Eq(2));
    EXPECT_THAT(s.black, Eq(2));
}

TEST_THAT(Game,
     WHAT(GetNextMovingPlayer),
     WHEN(ImmediatelyAfterConstruction),
     THEN(ReturnsBlack))
{
    EXPECT_THAT(the_game.get_next_moving_player(), Eq(player::black));
}

TEST_THAT(Game,
     WHAT(GetBoardCellMark),
     WHEN(GivenTheCoordinatesOfAnEmptyCell),
     THEN(ReturnsAnEmptyOptionalValue))
{
    auto const optional_mark = the_game.get_board_cell_mark({0, 0});

    auto const is_empty = bool{optional_mark};

    EXPECT_FALSE(is_empty);
}

TEST_THAT(Game,
     WHAT(GetBoardCellMark),
     WHEN(GivenTheCoordinatesOfTheNorthWestCentralCell),
     THEN(ReturnsWhiteMark))
{
    EXPECT_THAT(the_game.get_board_cell_mark({3, 3}), Eq(player::white));
}

TEST_THAT(Game,
     WHAT(GetBoardCellMark),
     WHEN(GivenTheCoordinatesOfTheSouthEastCentralCell),
     THEN(ReturnsWhiteMark))
{
    EXPECT_THAT(the_game.get_board_cell_mark({4, 4}), Eq(player::white));
}

TEST_THAT(Game,
     WHAT(GetBoardCellMark),
     WHEN(GivenTheCoordinatesOfTheNorthEastCentralCell),
     THEN(ReturnsBlackMark))
{
    EXPECT_THAT(the_game.get_board_cell_mark({3, 4}), Eq(player::black));
}

TEST_THAT(Game,
     WHAT(GetBoardCellMark),
     WHEN(GivenTheCoordinatesOfTheSouthWestCentralCell),
     THEN(ReturnsBlackMark))
{
    EXPECT_THAT(the_game.get_board_cell_mark({4, 3}), Eq(player::black));
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(GivenARowIndexWhichIsLowerThanZero),
     THEN(Throws))
{
    EXPECT_THROW((the_game.place({-1, 3})), bad_cell_coordinates_exception);
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(GivenARowIndexWhichIsNotLowerThanTheBoardSize),
     THEN(Throws))
{
    EXPECT_THROW((the_game.place({board_size, 3})), bad_cell_coordinates_exception);
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(GivenAColumnIndexWhichIsLowerThanZero),
     THEN(Throws))
{
    EXPECT_THROW((the_game.place({1, -2})), bad_cell_coordinates_exception);
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(GivenAColumnIndexWhichIsNotLowerThanTheBoardSize),
     THEN(Throws))
{
    EXPECT_THROW((the_game.place({2, board_size})), bad_cell_coordinates_exception);
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(GivenTheCoordinatesOfANonEmptyCell),
     THEN(Throws))
{
    EXPECT_THROW((the_game.place({3, 3})), cell_busy_exception);
}

TEST_THAT(Game, 
     WHAT(Place),
     WHEN(GivenTheCoordinatesOfAFreeCellThatDoesNotLeadToAnyReversal),
     THEN(Throws))
{
    EXPECT_THROW(the_game.place({1, 1}), no_reversal_triggered_exception);
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(GivenTheCoordinatesOfAFreeCellThatLeadsToSomeReversalEastOfThePlacePosition),
     THEN(PlacesTheMarkAndPerformsTheReversals))
{
    the_game.place({3, 2});

    EXPECT_THAT(the_game.get_board_cell_mark({3, 2}), Eq(player::black));

    EXPECT_THAT(the_game.get_board_cell_mark({3, 3}), Eq(player::black));
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(GivenTheCoordinatesOfAFreeCellThatLeadsToSomeReversalWestOfThePlacePosition),
     THEN(PlacesTheMarkAndPerformsTheReversals))
{
    the_game.place({5, 4});

    EXPECT_THAT(the_game.get_board_cell_mark({5, 4}), Eq(player::black));

    EXPECT_THAT(the_game.get_board_cell_mark({4, 4}), Eq(player::black));
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(OnSuccess),
     THEN(SwitchesTurn))
{
    the_game.place({3, 2});

    EXPECT_THAT(the_game.get_next_moving_player(), Eq(player::white));

    the_game.place({2, 4});

    EXPECT_THAT(the_game.get_next_moving_player(), Eq(player::black));
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(OnFailure),
     THEN(DoesNotSwitchTurn))
{
    the_game.place({3, 2});

    try
    {
        the_game.place({0, 0});
    }
    catch (std::exception const&)
    {
        EXPECT_THAT(the_game.get_next_moving_player(), Eq(player::white));
    }
}

} }