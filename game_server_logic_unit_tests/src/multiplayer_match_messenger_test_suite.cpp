#include "stdafx.hpp"

#include "reversi/game.hpp"
#include "reversi/player.hpp"
#include "reversi/remoting/multiplayer_match.hpp"
#include "reversi/remoting/multiplayer_match_messenger.hpp"
#include "reversi/remoting/multiplayer_match_registry.hpp"
#include "reversi/remoting/testing/game_logger_spy_factory.hpp"

namespace reversi { namespace remoting { namespace testing
{

using ::testing::Contains;
using ::testing::Eq;
using ::testing::Ne;
using ::testing::Test;

class MultiplayerMatchMessenger : public Test
{

protected:

    virtual void SetUp() override
    {
    }

    void set_player_name(std::string name)
    {
        messenger.execute_command("NAME;" + std::move(name));
    }

    std::shared_ptr<multiplayer_match> create_match(util::value_ref<std::string> name)
    {
        messenger.execute_command("CREATE;" + name + ";4");

        return registry.get_match(name);        
    }

    multiplayer_match_messenger::client_communication_channel make_client_communication_channel()
    {
        return [this] (std::string msg)
        {
            last_messages_for_client.push_back(std::move(msg));
        };
    }

protected:

    std::vector<std::string> last_messages_for_client;

    game_logger_spy_factory logger_factory;

    multiplayer_match_registry registry{logger_factory};

    multiplayer_match_messenger messenger{registry, make_client_communication_channel()};
    
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
     WHEN(GivenAStringThatEncodesACreateMatchCommandWithAUniqueName),
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
     WHEN(GivenAStringThatEncodesACreateMatchCommandWithAUniqueName),
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
     WHEN(GivenAStringThatEncodesACreateMatchCommandWithAUniqueName),
     THEN(CommunicatesASuccessCodeBackToTheClient))
{    
    messenger.execute_command("CREATE;MATCH;8");

    ASSERT_THAT(last_messages_for_client.size(), Eq(1u));

    EXPECT_THAT(last_messages_for_client, Contains("OK"));
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAStringThatEncodesACreateMatchCommandWithANonUniqueName),
     THEN(DoesNotThrowAndDoesNotCreateASecondGame))
{
    create_match("MATCH");

    EXPECT_NO_THROW(create_match("MATCH"));

    auto matches = registry.get_all_matches();

    EXPECT_THAT(matches.size(), Eq(1u));
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAStringThatEncodesACreateMatchCommandWithANonUniqueName),
     THEN(CommunicatesAnErrorBackToTheClient))
{
    create_match("MATCH");
    
    last_messages_for_client.clear();

    messenger.execute_command("CREATE;MATCH;8");

    ASSERT_THAT(last_messages_for_client.size(), Eq(1u));

    EXPECT_THAT(last_messages_for_client, Contains("ERROR;NAME NOT UNIQUE"));
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAJoinMatchCommandWithTheNameOfACurrentlyRegisteredMatch),
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
     WHEN(GivenAJoinMatchCommandWithTheNameOfACurrentlyRegisteredMatch),
     THEN(CommunicatesASuccessCodeBackToTheClient))
{    
    auto m = registry.create_new_match("NEW MATCH", 4);    

    last_messages_for_client.clear();

    messenger.execute_command("JOIN;NEW MATCH");

    ASSERT_THAT(last_messages_for_client.size(), Eq(1u));

    EXPECT_THAT(last_messages_for_client, Contains("OK"));
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAJoinMatchCommandWithANameWhichIsNotTheNameOfAnyRegisteredMatch),
     THEN(DoesNotThrowAndDoesNotJoinAnyMatch))
{
    set_player_name("PLAYER");

    auto m = registry.create_new_match("NEW MATCH", 4);    

    EXPECT_NO_THROW(messenger.execute_command("JOIN;BOGUS MATCH"));

    EXPECT_THAT(messenger.get_joined_match(), Eq(nullptr));
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAJoinMatchCommandWithANameWhichIsNotTheNameOfAnyRegisteredMatch),
     THEN(CommunicatesAnErrorBackToTheClient))
{
    create_match("MATCH");
    
    last_messages_for_client.clear();

    messenger.execute_command("JOIN;BOGUS MATCH");

    ASSERT_THAT(last_messages_for_client.size(), Eq(1u));

    EXPECT_THAT(last_messages_for_client, Contains("ERROR;MATCH NOT FOUND"));
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAPlaceMarkCommandWithAValidCellPositionAfterTheJoinedGameHasStarted),
     THEN(LetTheGamePlaceTheMarkInTheEncodedPosition))
{
    set_player_name("PLAYER");

    auto m = create_match("MATCH");

    m->join("SECOND PLAYER");

    messenger.execute_command("PLACE;0;1");

    auto& g = m->get_game();

    EXPECT_TRUE(bool{g.get_board_cell_mark({0, 1})});
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAPlaceMarkCommandWithAValidCellPositionAfterTheJoinedGameHasStarted),
     THEN(CommunicatesASuccessCodeBackToTheClient))
{
    set_player_name("PLAYER");

    auto m = create_match("MATCH");

    m->join("SECOND PLAYER");

    last_messages_for_client.clear();

    messenger.execute_command("PLACE;0;1");

    ASSERT_THAT(last_messages_for_client.size(), Eq(1u));

    EXPECT_THAT(last_messages_for_client, Contains("OK"));    
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAPlaceMarkCommandWithAnInvalidCellPositionAfterTheJoinedGameHasStarted),
     THEN(DoesNotThrowAndDoesNotPlaceAndyMark))
{
    set_player_name("PLAYER");

    auto m = create_match("MATCH");

    m->join("SECOND PLAYER");

    EXPECT_NO_THROW(messenger.execute_command("PLACE;0;0"));

    auto& g = m->get_game();

    EXPECT_FALSE(bool{g.get_board_cell_mark({0, 0})});
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAPlaceMarkCommandWithAnInvalidCellPositionAfterTheJoinedGameHasStarted),
     THEN(CommunicatesASuccessCodeBackToTheClient))
{
    set_player_name("PLAYER");

    auto m = create_match("MATCH");

    m->join("SECOND PLAYER");

    last_messages_for_client.clear();

    messenger.execute_command("PLACE;0;0");

    ASSERT_THAT(last_messages_for_client.size(), Eq(1u));

    EXPECT_THAT(last_messages_for_client, Contains("ERROR;INVALID POSITION"));    
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAGetBoardSizeCommandAfterAMatchHasBeenJoined),
     THEN(CommunicatesTheSizeBackToTheClient))
{
    messenger.execute_command("CREATE;NEW MATCH;16");

    last_messages_for_client.clear();

    messenger.execute_command("SIZE");

    ASSERT_THAT(last_messages_for_client.size(), Eq(1u));

    EXPECT_THAT(last_messages_for_client, Contains("OK;16"));    
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAGetBoardSizeCommandBeforeAMatchHasBeenJoined),
     THEN(DoesNotThrowAndCommunicatesAnErrorBackToTheClient))
{
    registry.create_new_match("NEW MATCH", 16);    
    
    messenger.execute_command("SIZE");

    ASSERT_THAT(last_messages_for_client.size(), Eq(1u));

    EXPECT_THAT(last_messages_for_client, Contains("ERROR;NO MATCH JOINED"));    
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAGetWhitePlayerNameCommandAfterStartingAMatchJoinedAsTheFirstPlayer),
     THEN(CommunicatesOwnNameBackToTheClient))
{
    set_player_name("COOL PLAYER");

    auto m = registry.create_new_match("NEW MATCH", 16);
    
    messenger.execute_command("JOIN;NEW MATCH");

    m->join("SECOND PLAYER");

    last_messages_for_client.clear();
    
    messenger.execute_command("PLAYER;WHITE");

    ASSERT_THAT(last_messages_for_client.size(), Eq(1u));

    EXPECT_THAT(last_messages_for_client, Contains("OK;COOL PLAYER"));
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAGetWhitePlayerNameCommandAfterStartingAMatchJoinedAsTheSecondPlayer),
     THEN(CommunicatesNameOfTheOpponentBackToTheClient))
{
    set_player_name("COOL PLAYER");

    auto m = registry.create_new_match("NEW MATCH", 16);
    
    m->join("OPPONENT");

    messenger.execute_command("JOIN;NEW MATCH");

    last_messages_for_client.clear();
    
    messenger.execute_command("PLAYER;WHITE");

    ASSERT_THAT(last_messages_for_client.size(), Eq(1u));

    EXPECT_THAT(last_messages_for_client, Contains("OK;OPPONENT"));
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAGetBlackPlayerNameCommandAfterStartingAMatchJoinedAsTheFirstPlayer),
     THEN(CommunicatesNameOfTheOpponentBackToTheClient))
{
    set_player_name("COOL PLAYER");

    auto m = registry.create_new_match("NEW MATCH", 16);
    
    messenger.execute_command("JOIN;NEW MATCH");

    m->join("OPPONENT");

    last_messages_for_client.clear();
    
    messenger.execute_command("PLAYER;BLACK");

    ASSERT_THAT(last_messages_for_client.size(), Eq(1u));

    EXPECT_THAT(last_messages_for_client, Contains("OK;OPPONENT"));
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAGetBlackPlayerNameCommandAfterStartingAMatchJoinedAsTheSecondPlayer),
     THEN(CommunicatesOwnNameBackToTheClient))
{
    set_player_name("COOL PLAYER");

    auto m = registry.create_new_match("NEW MATCH", 16);
    
    m->join("OPPONENT");

    messenger.execute_command("JOIN;NEW MATCH");

    last_messages_for_client.clear();
    
    messenger.execute_command("PLAYER;BLACK");

    ASSERT_THAT(last_messages_for_client.size(), Eq(1u));

    EXPECT_THAT(last_messages_for_client, Contains("OK;COOL PLAYER"));
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAGetPlayerNameCommandBeforeJoiningAMatch),
     THEN(DoesNotThrowAndCommunicatesAnErrorBackToTheClient))
{
    set_player_name("COOL PLAYER");

    auto m = registry.create_new_match("NEW MATCH", 16);
    
    m->join("OPPONENT");

    last_messages_for_client.clear();

    EXPECT_NO_THROW(messenger.execute_command("PLAYER;WHITE"));

    ASSERT_THAT(last_messages_for_client.size(), Eq(1u));

    EXPECT_THAT(last_messages_for_client, Contains("ERROR;NO MATCH JOINED"));
}

TEST_THAT(MultiplayerMatchMessenger,
     WHAT(ExecuteCommand),
     WHEN(GivenAGetPlayerNameCommandAfterAMatchWasJoinedButBeforeTheGameWasStarted),
     THEN(DoesNotThrowAndCommunicatesAnErrorBackToTheClient))
{
    set_player_name("COOL PLAYER");

    auto m = registry.create_new_match("NEW MATCH", 16);

    messenger.execute_command("JOIN;NEW MATCH");

    last_messages_for_client.clear();

    EXPECT_NO_THROW(messenger.execute_command("PLAYER;WHITE"));

    ASSERT_THAT(last_messages_for_client.size(), Eq(1u));

    EXPECT_THAT(last_messages_for_client, Contains("ERROR;GAME NOT STARTED"));
}

} } }