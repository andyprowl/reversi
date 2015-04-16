#include "stdafx.hpp"

#include "reversi/game.hpp"
#include "reversi/player.hpp"
#include "reversi/stream_game_logger.hpp"
#include "reversi/remoting/multiplayer_match.hpp"

namespace reversi { namespace remoting { namespace testing
{

using ::testing::Eq;
using ::testing::Lt;
using ::testing::Ref;
using ::testing::Test;

class MultiplayerMatch : public Test
{
    
protected:

    std::string match_name = "NAME";

    int board_size = 4;

    std::stringstream log;

    std::unique_ptr<game_logger> logger = std::make_unique<stream_game_logger>(log);

    multiplayer_match the_match{match_name, board_size, std::move(logger)};

};

TEST_THAT(MultiplayerMatch,
     WHAT(GetName),
     WHEN(Always),
     THEN(ReturnsTheNamePassedAtConstruction))
{
    EXPECT_THAT(the_match.get_name(), Eq(match_name));
}

TEST_THAT(MultiplayerMatch,
     WHAT(Join),
     WHEN(AfterTwoPlayersHaveAlreadyJoined),
     THEN(Throws))
{
    the_match.join("Mark");
    the_match.join("John");

    EXPECT_THROW(the_match.join("Luke"), match_full_exception);
}

TEST_THAT(MultiplayerMatch,
     WHAT(Join),
     WHEN(WhenTheSecondPlayerJoins),
     THEN(CreatesAGameWithTheBoardSizeSpecifiedAtConstructionAndInvokesRegisteredEventHandlers))
{
    auto invoked = false;
    the_match.register_match_full_handler([&invoked, this] (game& g)
    {
        invoked = true;

        EXPECT_THAT(g.get_player_name(player::white), Eq("Mark"));
        EXPECT_THAT(g.get_player_name(player::black), Eq("John"));
        EXPECT_THAT(g.get_board_size(), Eq(board_size));
    });

    the_match.join("Mark");

    EXPECT_FALSE(invoked);

    the_match.join("John");    

    EXPECT_TRUE(invoked);
}

TEST_THAT(MultiplayerMatch,
     WHAT(GetGame),
     WHEN(BeforeTheMatchHasStarted),
     THEN(Throws))
{
    the_match.join("Mark");

    EXPECT_THROW(the_match.get_game(), match_not_started_exception);
}

TEST_THAT(MultiplayerMatch,
     WHAT(GetGame),
     WHEN(AfterTheMatchHasStarted),
     THEN(ReturnsTheMatchGame))
{
    auto started_game = static_cast<game*>(nullptr);
    the_match.register_match_full_handler([&started_game] (game& g)
    {
        started_game = &g;
    });

    the_match.join("Mark");
    the_match.join("John");    

    EXPECT_THAT(the_match.get_game(), Ref(*started_game));
}

} } }