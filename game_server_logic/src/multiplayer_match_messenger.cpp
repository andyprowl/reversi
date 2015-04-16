#include "stdafx.hpp"

#include "reversi/cell_position.hpp"
#include "reversi/game.hpp"
#include "reversi/remoting/multiplayer_match.hpp"
#include "reversi/remoting/multiplayer_match_messenger.hpp"
#include "reversi/remoting/multiplayer_match_registry.hpp"
#include "util/tokenize.hpp"

namespace reversi { namespace remoting
{

multiplayer_match_messenger::multiplayer_match_messenger(multiplayer_match_registry& registry)
    : registry{registry}
    , player_name("Anonymous")
{    
}

void multiplayer_match_messenger::execute_command(std::string command)
{
    auto const tokens = util::tokenize(command, ";");
    if (tokens[0] == "CREATE")
    {
        joined_match = registry.create_new_match(tokens[1], std::stoi(tokens[2]));

        joined_match->join(player_name);
    }
    else if (tokens[0] == "JOIN")
    {
        auto m = registry.get_match(tokens[1]);

        m->join(player_name);

        joined_match = m;
    }
    else if (tokens[0] == "NAME")
    {
        player_name = tokens[1];
    }
    else if (tokens[0] == "PLACE")
    {
        auto& g = joined_match->get_game();

        g.place({std::stoi(tokens[1]), std::stoi(tokens[2])});
    }
}

std::shared_ptr<multiplayer_match> multiplayer_match_messenger::get_joined_match() const
{
    return joined_match;
}

std::string multiplayer_match_messenger::get_player_name() const
{
    return player_name;
}

} }