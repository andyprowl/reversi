#include "stdafx.hpp"

#include "reversi/cell_position.hpp"
#include "reversi/local_game.hpp"
#include "reversi/placement_outcome.hpp"
#include "reversi/player.hpp"
#include "reversi/game_score.hpp"
#include "reversi/testing/game_logger_spy.hpp"

namespace reversi { namespace testing
{

using ::testing::Contains;
using ::testing::Eq;
using ::testing::Test;

class LocalGame : public Test
{

protected:

    void make_white_unable_to_move()
    {
        // See http://reversi-nxn.blogspot.cz/2008/04/reversi-4x4-games.html

        the_game.place({0, 1}); // black
        the_game.place({0, 2}); // white
        the_game.place({0, 3}); // black
        the_game.place({0, 0}); // white
        the_game.place({2, 0}); // black
    }

    void make_black_win()
    {
        // See http://reversi-nxn.blogspot.cz/2008/04/reversi-4x4-games.html

        make_white_unable_to_move();

        the_game.place({3, 3}); // black wins
    }
    
protected:

    std::string white_player_name = "PLAYER 2";

    std::string black_player_name = "PLAYER 1";

    int board_size = 4;

    game_logger_spy logger;

    local_game the_game{board_size, white_player_name, black_player_name, logger};

};

TEST_THAT(LocalGame,
     WHAT(GetBoardSize),
     WHEN(Always),
     THEN(ReturnsTheSizePassedAtConstruction))
{
    EXPECT_THAT(the_game.get_board_size(), Eq(board_size));
}

TEST_THAT(LocalGame,
     WHAT(GetPlayerName),
     WHEN(GivenTheBlackPlayerIdentifier),
     THEN(ReturnsTheBlackPlayerNamePassedAtConstruction))
{
    EXPECT_THAT(the_game.get_player_name(player::white), Eq(white_player_name));
}

TEST_THAT(LocalGame,
     WHAT(GetPlayerName),
     WHEN(GivenTheWhitePlayerIdentifier),
     THEN(ReturnsTheWhitePlayerNamePassedAtConstruction))
{
    EXPECT_THAT(the_game.get_player_name(player::white), Eq(white_player_name));
}

TEST_THAT(LocalGame,
     WHAT(GetScore),
     WHEN(ImmediatelyAfterConstruction),
     THEN(ReturnsTwoForBothWhiteAndBlack))
{
    auto const s = the_game.get_score();

    EXPECT_THAT(s.white, Eq(2));
    EXPECT_THAT(s.black, Eq(2));
}

TEST_THAT(LocalGame,
     WHAT(GetNextMovingPlayer),
     WHEN(ImmediatelyAfterConstruction),
     THEN(ReturnsBlack))
{
    EXPECT_THAT(the_game.get_next_moving_player(), Eq(player::black));
}

TEST_THAT(LocalGame,
     WHAT(GetBoardCellMark),
     WHEN(GivenTheCoordinatesOfAnEmptyCell),
     THEN(ReturnsAnEmptyOptionalValue))
{
    auto const optional_mark = the_game.get_board_cell_mark({0, 0});

    auto const is_empty = bool{optional_mark};

    EXPECT_FALSE(is_empty);
}

TEST_THAT(LocalGame,
     WHAT(GetBoardCellMark),
     WHEN(GivenTheCoordinatesOfTheNorthWestCentralCell),
     THEN(ReturnsWhiteMark))
{
    EXPECT_THAT(the_game.get_board_cell_mark({1, 1}), Eq(player::white));
}

TEST_THAT(LocalGame,
     WHAT(GetBoardCellMark),
     WHEN(GivenTheCoordinatesOfTheSouthEastCentralCell),
     THEN(ReturnsWhiteMark))
{
    EXPECT_THAT(the_game.get_board_cell_mark({2, 2}), Eq(player::white));
}

TEST_THAT(LocalGame,
     WHAT(GetBoardCellMark),
     WHEN(GivenTheCoordinatesOfTheNorthEastCentralCell),
     THEN(ReturnsBlackMark))
{
    EXPECT_THAT(the_game.get_board_cell_mark({1, 2}), Eq(player::black));
}

TEST_THAT(LocalGame,
     WHAT(GetBoardCellMark),
     WHEN(GivenTheCoordinatesOfTheSouthWestCentralCell),
     THEN(ReturnsBlackMark))
{
    EXPECT_THAT(the_game.get_board_cell_mark({2, 1}), Eq(player::black));
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(GivenARowIndexWhichIsLowerThanZero),
     THEN(Throws))
{
    EXPECT_THROW((the_game.place({-1, 3})), bad_cell_coordinates_exception);
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(GivenARowIndexWhichIsNotLowerThanTheBoardSize),
     THEN(Throws))
{
    EXPECT_THROW((the_game.place({board_size, 3})), bad_cell_coordinates_exception);
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(GivenAColumnIndexWhichIsLowerThanZero),
     THEN(Throws))
{
    EXPECT_THROW((the_game.place({1, -2})), bad_cell_coordinates_exception);
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(GivenAColumnIndexWhichIsNotLowerThanTheBoardSize),
     THEN(Throws))
{
    EXPECT_THROW((the_game.place({2, board_size})), bad_cell_coordinates_exception);
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(GivenTheCoordinatesOfANonEmptyCell),
     THEN(Throws))
{
    EXPECT_THROW((the_game.place({2, 2})), cell_busy_exception);
}

TEST_THAT(LocalGame, 
     WHAT(Place),
     WHEN(GivenTheCoordinatesOfAFreeCellThatDoesNotLeadToAnyReversal),
     THEN(Throws))
{
    EXPECT_THROW(the_game.place({0, 0}), no_reversal_triggered_exception);
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(GivenTheCoordinatesOfAFreeCellThatLeadsToSomeReversalEastOfThePlacePosition),
     THEN(PlacesTheMarkAndPerformsTheReversals))
{
    the_game.place({1, 0});

    EXPECT_THAT(the_game.get_board_cell_mark({1, 0}), Eq(player::black));

    EXPECT_THAT(the_game.get_board_cell_mark({1, 1}), Eq(player::black));
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(GivenTheCoordinatesOfAFreeCellThatLeadsToSomeReversalWestOfThePlacePosition),
     THEN(PlacesTheMarkAndPerformsTheReversals))
{
    the_game.place({2, 3});

    EXPECT_THAT(the_game.get_board_cell_mark({2, 3}), Eq(player::black));

    EXPECT_THAT(the_game.get_board_cell_mark({2, 2}), Eq(player::black));
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(OnSuccess),
     THEN(SwitchesTurn))
{
    the_game.place({0, 1});

    EXPECT_THAT(the_game.get_next_moving_player(), Eq(player::white));

    the_game.place({2, 0});

    EXPECT_THAT(the_game.get_next_moving_player(), Eq(player::black));
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(OnSuccess),
     THEN(UpdatesTheScore))
{
    the_game.place({0, 1});

    EXPECT_THAT(the_game.get_score(), Eq(game_score{1, 4}));
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(OnFailure),
     THEN(DoesNotSwitchTurn))
{
    the_game.place({0, 1});

    try
    {
        the_game.place({0, 0});
    }
    catch (std::exception const&)
    {
        EXPECT_THAT(the_game.get_next_moving_player(), Eq(player::white));
    }
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(WhenAfterAPlacementTheNextPlayerCannotMakeAValidMoveButTheOneWhoJustMovedCan),
     THEN(DoesNotSwitchTheTurnAndReturnsTheCorrespondingOutcome))
{
    // See http://reversi-nxn.blogspot.cz/2008/04/reversi-4x4-games.html

    the_game.place({0, 1}); // black
    the_game.place({0, 2}); // white
    the_game.place({0, 3}); // black
    the_game.place({0, 0}); // white
    
    auto const outcome = the_game.place({2, 0}); // black

    EXPECT_THAT(outcome, Eq(placement_outcome::turn_skipped));

    EXPECT_THAT(the_game.get_next_moving_player(), Eq(player::black));
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(WhenAfterAPlacementNeitherOfThePlayersCanMakeAValidMove),
     THEN(DoesNotSwitchTheTurnAndTheLocalGameIsOver))
{
    // See http://reversi-nxn.blogspot.cz/2008/04/reversi-4x4-games.html

    the_game.place({0, 1}); // black
    the_game.place({0, 2}); // white
    the_game.place({0, 3}); // black
    the_game.place({0, 0}); // white
    the_game.place({2, 0}); // black
        
    auto const outcome = the_game.place({3, 3}); // black again

    EXPECT_THAT(outcome, Eq(placement_outcome::game_over));
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(OnSuccess),
     THEN(LogsASuccessfulPlacementMessage))
{
    the_game.place({2, 3});

    EXPECT_THAT(logger.messages, Contains("SP black 2 3"));
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(OnATurnSwitch),
     THEN(LogsATurnSwitchedMessage))
{
    the_game.place({2, 3});

    EXPECT_THAT(logger.messages, Contains("TS white"));
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(OnATurnSkip),
     THEN(LogsATurnSkippedMessage))
{
    make_white_unable_to_move();

    EXPECT_THAT(logger.messages, Contains("TSK black"));
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(OnSuccess),
     THEN(LogsALocalGameOverMessage))
{
    make_black_win();

    EXPECT_THAT(logger.messages, Contains("GO"));
}

TEST_THAT(LocalGame,
     WHAT(Place),
     WHEN(Always),
     THEN(InvokesHandlersRegisteredForPlacementEventNotifications))
{
    the_game.place({3, 2}); // black
    the_game.place({3, 3}); // white
    the_game.place({2, 3}); // black

    auto invoked = false;
    the_game.register_placement_event_handler(
        [&invoked, this] (cell_position const pos, 
                          player const p, 
                          placement_outcome const outcome,
                          util::value_ref<std::vector<cell_position>> flipped_cells)
    {
        EXPECT_THAT(pos, Eq(cell_position{1, 3}));
        EXPECT_THAT(p, Eq(player::black));
        EXPECT_THAT(outcome, Eq(placement_outcome::turn_switched));

        ASSERT_THAT(flipped_cells.size(), Eq(2u));
        EXPECT_THAT(flipped_cells, Contains(cell_position{2, 3}));
        EXPECT_THAT(flipped_cells, Contains(cell_position{1, 2}));

        invoked = true;
    });

    the_game.place({1, 3}); // white

    EXPECT_TRUE(invoked);
}

} }