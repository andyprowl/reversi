#include "stdafx.hpp"

#include "reversi/cell_position.hpp"
#include "reversi/game.hpp"
#include "reversi/player.hpp"
#include "reversi/stream_game_logger.hpp"
#include <sstream>

namespace reversi { namespace testing
{

using ::testing::AnyOf;
using ::testing::HasSubstr;
using ::testing::Test;

class StreamGameLogger : public Test
{

protected:

    void make_black_win()
    {
        // See http://reversi-nxn.blogspot.cz/2008/04/reversi-4x4-games.html
        
        the_game.place({0, 1}); // black
        the_game.place({0, 2}); // white
        the_game.place({0, 3}); // black
        the_game.place({0, 0}); // white
        the_game.place({2, 0}); // black
        the_game.place({3, 3}); // black wins
    }
    
protected:
    
    std::stringstream log;

    stream_game_logger logger{log};

    game the_game{4, "BLACK PLAYER", "WHITE PLAYER", logger};

};

TEST_THAT(StreamGameLogger,
     WHAT(LogSuccessfulPlacement),
     WHEN(GivenACellPositionAndAPlayer),
     THEN(InsertsTheCoordinatesOfThatPositionAndARepresentationOfThePlayer))
{
    logger.log_successful_placement({15, 42}, player::white);

    auto const content = log.str();

    EXPECT_THAT(content, HasSubstr("15"));
    EXPECT_THAT(content, HasSubstr("42"));
    EXPECT_THAT(content, HasSubstr(to_string(player::white)));
}

TEST_THAT(StreamGameLogger,
     WHAT(LogTurnSwitchedMessage),
     WHEN(GivenAPlayer),
     THEN(InsertsThePlayersDefaultRepresentationAndTheWordSwitch))
{
    logger.log_turn_switched(player::white);

    auto const content = log.str();

    EXPECT_THAT(content, AnyOf(HasSubstr("switch"), HasSubstr("Switch")));
    EXPECT_THAT(content, HasSubstr(to_string(player::white)));
}

TEST_THAT(StreamGameLogger,
     WHAT(LogTurnSkippedMessage),
     WHEN(GivenAPlayer),
     THEN(InsertsThePlayersDefaultRepresentationAndTheWordSkip))
{
    logger.log_turn_skipped(player::white);

    auto const content = log.str();

    EXPECT_THAT(content, AnyOf(HasSubstr("skip"), HasSubstr("Skip")));
    EXPECT_THAT(content, HasSubstr(to_string(player::white)));
}

TEST_THAT(StreamGameLogger,
     WHAT(LogGameOverMessage),
     WHEN(GivenAGameWhereOneOfThePlayersWon),
     THEN(LogsTheNameOfTheWinnerScoreOfTheGameAndTheWordsOverAndWins))
{
    make_black_win();

    logger.log_game_over(the_game);

    auto const content = log.str();

    auto const score = the_game.get_score();

    EXPECT_THAT(content, HasSubstr("over"));
    EXPECT_THAT(content, HasSubstr("wins"));
    EXPECT_THAT(content, HasSubstr(to_string(player::black)));
    EXPECT_THAT(content, HasSubstr(std::to_string(score.white)));
    EXPECT_THAT(content, HasSubstr(std::to_string(score.black)));
}

TEST_THAT(StreamGameLogger,
     WHAT(LogGameOverMessage),
     WHEN(GivenAGameWhereNoneOfThePlayersWon),
     THEN(LogsTheScoreOfTheGameAndTheWordsOver))
{
    logger.log_game_over(the_game);

    auto const content = log.str();

    auto const score = the_game.get_score();

    EXPECT_THAT(content, HasSubstr("over"));
    EXPECT_THAT(content, HasSubstr(std::to_string(score.white)));
    EXPECT_THAT(content, HasSubstr(std::to_string(score.black)));
}

} }