#include "stdafx.hpp"

#include "reversi/game.hpp"
#include "reversi/player.hpp"
#include "reversi/remoting/multiplayer_match.hpp"
#include "reversi/remoting/multiplayer_match_messenger.hpp"
#include "reversi/remoting/multiplayer_match_registry.hpp"
#include "reversi/remoting/testing/game_logger_spy_factory.hpp"

namespace reversi { namespace remoting { namespace testing
{

using ::testing::Eq;
using ::testing::Ne;
using ::testing::Test;

class MultiplayerMatchMessenger : public Test
{

protected:

    void set_player_name(std::string name)
    {
        messenger.execute_command("NAME;" + std::move(name));
    }

    std::shared_ptr<multiplayer_match> create_match(util::value_ref<std::string> name)
    {
        messenger.execute_command("CREATE;" + name + ";4");

        return registry.get_match(name);        
    }

protected:

    game_logger_spy_factory logger_factory;

    multiplayer_match_registry registry{logger_factory};

    multiplayer_match_messenger messenger{registry};
    
};

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAStringThatEncodesASetPlayerNameCommand),
     THEN(StoresThePlayerNameToBeUsedWhenJoiningAGame))
{
    messenger.execute_command("NAME;COOL PLAYER");

    EXPECT_THAT(messenger.get_player_name(), Eq("COOL PLAYER"));
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAStringThatEncodesACreateMatchCommand),
     THEN(LetsTheRegistryCreateANewMatch))
{
    messenger.execute_command("CREATE;MY MATCH;4");

    auto m = registry.get_match("MY MATCH");
    ASSERT_THAT(m, Ne(nullptr));

    EXPECT_THAT(m->get_name(), Eq("MY MATCH"));
    EXPECT_THAT(m->get_board_size(), Eq(4));
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAStringThatEncodesACreateMatchCommand),
     THEN(JoinsTheNewlyCreatedMatchWithThePreviouslySetPlayerName))
{
    auto const name = "COOL PLAYER";

    set_player_name(name);

    auto m = create_match("MATCH");

    EXPECT_THAT(messenger.get_joined_match(), Eq(m));

    auto invoked = true;
    m->register_match_full_handler([name, &invoked] (game& g)
    {
        EXPECT_THAT(g.get_player_name(player::white), Eq(name));
    });

    m->join("SECOND PLAYER");

    EXPECT_TRUE(invoked);
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAJoinMatchCommand),
     THEN(JoinsTheMatchWithTheEncodedName))
{
    set_player_name("PLAYER");

    auto m = registry.create_new_match("NEW MATCH", 4);    

    messenger.execute_command("JOIN;NEW MATCH");

    ASSERT_THAT(messenger.get_joined_match(), Eq(m));

    m->join("SECOND PLAYER");

    auto& g = m->get_game();

    EXPECT_THAT(g.get_player_name(player::white), Eq("PLAYER"));
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAPlaceMarkCommandAfterTheJoinedGameHasStarted),
     THEN(LetTheGamePlaceTheMarkInTheEncodedPosition))
{
    set_player_name("PLAYER");

    auto m = create_match("MATCH");

    m->join("SECOND PLAYER");

    messenger.execute_command("PLACE;0;1");

    auto& g = m->get_game();

    EXPECT_TRUE(bool{g.get_board_cell_mark({0, 1})});
}

} } }