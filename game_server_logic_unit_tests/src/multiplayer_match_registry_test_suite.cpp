#include "stdafx.hpp"

#include "reversi/remoting/multiplayer_match.hpp"
#include "reversi/remoting/multiplayer_match_registry.hpp"
#include "reversi/remoting/testing/game_logger_spy_factory.hpp"

namespace reversi { namespace remoting { namespace testing
{

using ::testing::Contains;
using ::testing::Eq;
using ::testing::Test;

class MultiplayerMatchRegistry : public Test
{

protected:

    game_logger_spy_factory logger_factory;

    multiplayer_match_registry registry{logger_factory};
    
};

TEST_THAT(MultiplayerMatchRegistry,
     WHAT(CreateNewMatch),
     WHEN(GivenAUniqueNameAndABoardSize),
     THEN(CreatesAMatchWithThatNameAndBoardSize))
{
    auto const match_name = "MATCH";
    auto const board_size = 42;

    auto m = registry.create_new_match(match_name, board_size);

    EXPECT_THAT(m->get_name(), Eq(match_name));
    EXPECT_THAT(m->get_board_size(), Eq(board_size));
}

TEST_THAT(MultiplayerMatchRegistry,
     WHAT(CreateNewMatch),
     WHEN(GivenANameWhichIsIdenticalToTheNameOfARegisteredMatch),
     THEN(Throws))
{
    auto const match_name = "MATCH";
    auto m = registry.create_new_match(match_name, 4);
    
    EXPECT_THROW(registry.create_new_match(match_name, 6), match_name_not_unique_exception);
}

TEST_THAT(MultiplayerMatchRegistry,
     WHAT(GetMatch),
     WHEN(GivenTheNameOfAPreviouslyCreatedMatch),
     THEN(ReturnsThatMatch))
{
    auto const match_name = "MATCH";

    auto m = registry.create_new_match(match_name, 4);

    auto found_match = registry.get_match(match_name);

    EXPECT_THAT(found_match, Eq(m));
}

TEST_THAT(MultiplayerMatchRegistry,
     WHAT(GetMatch),
     WHEN(GivenANameThatIsNotTheNameOfAnyRegisteredMatch),
     THEN(Throws))
{
    EXPECT_THROW(registry.get_match("BOGUS MATCH"), match_not_found_exception);
}

TEST_THAT(MultiplayerMatchRegistry,
     WHAT(CreateNewMatch),
     WHEN(OnSuccess),
     THEN(ReturnsASharedPointerWithADeleterThatAutomaticallyPerformsUnregistrationOnDeletion))
{
    auto const match_name = "MATCH";
    auto const board_size = 42;

    {
        registry.create_new_match(match_name, board_size);
    }

    EXPECT_THROW(registry.get_match(match_name), match_not_found_exception);
}

TEST_THAT(MultiplayerMatchRegistry,
     WHAT(GetAllMatches),
     WHEN(Always),
     THEN(ReturnsAllTheCurrentlyRegisteredMatches))
{
    auto m1 = registry.create_new_match("MATCH 1", 4);
    auto m2 = registry.create_new_match("MATCH 2", 6);
    auto m3 = registry.create_new_match("MATCH 3", 8);

    auto all_matches = registry.get_all_matches();

    ASSERT_THAT(all_matches.size(), Eq(3u));

    EXPECT_THAT(all_matches, Contains(m1));
    EXPECT_THAT(all_matches, Contains(m2));
    EXPECT_THAT(all_matches, Contains(m3));
}

} } }