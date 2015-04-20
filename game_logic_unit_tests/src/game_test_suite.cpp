#include "stdafx.hpp"

#include "reversi/cell_position.hpp"
#include "reversi/game.hpp"
#include "reversi/placement_outcome.hpp"
#include "reversi/player.hpp"
#include "reversi/game_score.hpp"

#ifdef USE_LOGGER_SPY
#include "reversi/testing/game_logger_spy.hpp"
#else
#include "reversi/testing/game_logger_mock.hpp"
#endif

namespace reversi { namespace testing
{

using ::testing::Contains;
using ::testing::Eq;
using ::testing::Exactly;
using ::testing::Ref;
using ::testing::Test;

class Game : public Test
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

    std::string black_player_name = "PLAYER 1";

    std::string white_player_name = "PLAYER 2";

    int board_size = 4;

#ifdef USE_LOGGER_SPY
    game_logger_spy logger;
#else
    game_logger_mock logger;
#endif

    game the_game{board_size, black_player_name, white_player_name, logger};

};

TEST_THAT(Game,
     WHAT(GetBoardSize),
     WHEN(Always),
     THEN(ReturnsTheSizePassedAtConstruction))
{
    EXPECT_THAT(the_game.get_board_size(), Eq(board_size));
}

TEST_THAT(Game,
     WHAT(GetPlayerName),
     WHEN(GivenTheBlackPlayerIdentifier),
     THEN(ReturnsTheBlackPlayerNamePassedAtConstruction))
{
    EXPECT_THAT(the_game.get_player_name(player::white), Eq(white_player_name));
}

TEST_THAT(Game,
     WHAT(GetPlayerName),
     WHEN(GivenTheWhitePlayerIdentifier),
     THEN(ReturnsTheWhitePlayerNamePassedAtConstruction))
{
    EXPECT_THAT(the_game.get_player_name(player::white), Eq(white_player_name));
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
    EXPECT_THAT(the_game.get_board_cell_mark({1, 1}), Eq(player::white));
}

TEST_THAT(Game,
     WHAT(GetBoardCellMark),
     WHEN(GivenTheCoordinatesOfTheSouthEastCentralCell),
     THEN(ReturnsWhiteMark))
{
    EXPECT_THAT(the_game.get_board_cell_mark({2, 2}), Eq(player::white));
}

TEST_THAT(Game,
     WHAT(GetBoardCellMark),
     WHEN(GivenTheCoordinatesOfTheNorthEastCentralCell),
     THEN(ReturnsBlackMark))
{
    EXPECT_THAT(the_game.get_board_cell_mark({1, 2}), Eq(player::black));
}

TEST_THAT(Game,
     WHAT(GetBoardCellMark),
     WHEN(GivenTheCoordinatesOfTheSouthWestCentralCell),
     THEN(ReturnsBlackMark))
{
    EXPECT_THAT(the_game.get_board_cell_mark({2, 1}), Eq(player::black));
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
    EXPECT_THROW((the_game.place({board_size, 3})), 
                 bad_cell_coordinates_exception);
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
    EXPECT_THROW((the_game.place({2, board_size})), 
                 bad_cell_coordinates_exception);
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(GivenTheCoordinatesOfANonEmptyCell),
     THEN(Throws))
{
    EXPECT_THROW((the_game.place({2, 2})), cell_busy_exception);
}

TEST_THAT(Game, 
     WHAT(Place),
     WHEN(GivenTheCoordinatesOfAFreeCellThatDoesNotLeadToAnyReversal),
     THEN(Throws))
{
    EXPECT_THROW(the_game.place({0, 0}), no_reversal_triggered_exception);
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(GivenTheCoordinatesOfAFreeCellThatLeadsToSomeReversalTowardsEast),
     THEN(PlacesTheMarkAndPerformsTheReversals))
{
    the_game.place({1, 0});

    EXPECT_THAT(the_game.get_board_cell_mark({1, 0}), Eq(player::black));

    EXPECT_THAT(the_game.get_board_cell_mark({1, 1}), Eq(player::black));
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(GivenTheCoordinatesOfAFreeCellThatLeadsToSomeReversalTowardsWest),
     THEN(PlacesTheMarkAndPerformsTheReversals))
{
    the_game.place({2, 3});

    EXPECT_THAT(the_game.get_board_cell_mark({2, 3}), Eq(player::black));

    EXPECT_THAT(the_game.get_board_cell_mark({2, 2}), Eq(player::black));
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(OnSuccess),
     THEN(SwitchesTurn))
{
    the_game.place({0, 1});

    EXPECT_THAT(the_game.get_next_moving_player(), Eq(player::white));

    the_game.place({2, 0});

    EXPECT_THAT(the_game.get_next_moving_player(), Eq(player::black));
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(OnSuccess),
     THEN(UpdatesTheScore))
{
    the_game.place({0, 1});

    EXPECT_THAT(the_game.get_score(), Eq(game_score{1, 4}));
}

TEST_THAT(Game,
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

TEST_THAT(Game,
     WHAT(Place),
     WHEN(WhenAfterPlacementTheNextPlayerCannotMakeAValidMoveButTheOpponentCan),
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

TEST_THAT(Game,
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

TEST_THAT(Game,
     WHAT(CanPlace),
     WHEN(GivenThePositionOfAnOccupiedCell),
     THEN(ReturnsFalse))
{
    EXPECT_FALSE(the_game.can_place({1, 1}));
}

TEST_THAT(Game,
     WHAT(CanPlace),
     WHEN(GivenThePositionOfAFreeCellThatWouldNotLeadToAnyReversalIfMarked),
     THEN(ReturnsFalse))
{
    EXPECT_FALSE(the_game.can_place({2, 0}));
}

TEST_THAT(Game,
     WHAT(CanPlace),
     WHEN(GivenThePositionOfACallThatWouldLeadToAtLeastOneReversalIfMarked),
     THEN(ReturnsTrue))
{
    EXPECT_TRUE(the_game.can_place({1, 0}));
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(OnSuccess),
     THEN(LogsASuccessfulPlacementMessage))
{
#ifndef USE_LOGGER_SPY
    EXPECT_CALL(logger, 
                log_successful_placement(cell_position{2, 3}, player::black))
                .Times(Exactly(1));
#endif

        the_game.place({2, 3});

#ifdef USE_LOGGER_SPY
    EXPECT_THAT(logger.messages, Contains("SP black 2 3"));
#endif
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(OnATurnSwitch),
     THEN(LogsATurnSwitchedMessage))
{
#ifndef USE_LOGGER_SPY
    EXPECT_CALL(logger, log_turn_switched(player::white)).Times(Exactly(1));
#endif

    the_game.place({2, 3});

#ifdef USE_LOGGER_SPY
    EXPECT_THAT(logger.messages, Contains("TS white"));
#endif
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(OnATurnSkip),
     THEN(LogsATurnSkippedMessage))
{
#ifndef USE_LOGGER_SPY
    EXPECT_CALL(logger, log_turn_skipped(player::black)).Times(Exactly(1));
#endif

    make_white_unable_to_move();

#ifdef USE_LOGGER_SPY
    EXPECT_THAT(logger.messages, Contains("TSK black"));
#endif
}

TEST_THAT(Game,
     WHAT(Place),
     WHEN(OnSuccess),
     THEN(LogsALocalGameOverMessage))
{
#ifndef USE_LOGGER_SPY
    EXPECT_CALL(logger, log_game_over(Ref(the_game))).Times(Exactly(1));
#endif

    make_black_win();

#ifdef USE_LOGGER_SPY
    EXPECT_THAT(logger.messages, Contains("GO"));
#endif
}

TEST_THAT(Game,
     WHAT(IsOver),
     WHEN(WhenTheGameIsNotOver),
     THEN(ReturnsFalse))
{
    EXPECT_FALSE(the_game.is_over());
}

TEST_THAT(Game,
     WHAT(IsOver),
     WHEN(WhenTheGameIsOver),
     THEN(ReturnsTrue))
{
    make_black_win();

    EXPECT_TRUE(the_game.is_over());
}

} }