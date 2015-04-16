#include "stdafx.hpp"

#include "reversi/cell_position.hpp"
#include "reversi/game.hpp"
#include "reversi/remoting/multiplayer_match.hpp"
#include "reversi/remoting/multiplayer_match_messenger.hpp"
#include "reversi/remoting/multiplayer_match_registry.hpp"
#include "util/tokenize.hpp"

namespace reversi { namespace remoting
{

multiplayer_match_messenger::multiplayer_match_messenger(multiplayer_match_registry& registry,
                                                         client_communication_channel channel)
    : registry{registry}
    , channel{std::move(channel)}
    , player_name("Anonymous")
{    
    setup_command_handlers();
}

void multiplayer_match_messenger::execute_command(std::string command)
{
    auto const tokens = util::tokenize(command, ";");
    
    auto const command_name = tokens[0];

    auto& handler = processors[command_name];

    handler(tokens);
}

std::shared_ptr<multiplayer_match> multiplayer_match_messenger::get_joined_match() const
{
    return joined_match;
}

std::string multiplayer_match_messenger::get_player_name() const
{
    return player_name;
}

void multiplayer_match_messenger::setup_command_handlers()
{
    using std::placeholders::_1;
    using self = multiplayer_match_messenger;

    processors["NAME"] = std::bind(&self::process_set_name_command, this, _1);
    processors["CREATE"] = std::bind(&self::process_create_match_command, this, _1);
    processors["JOIN"] = std::bind(&self::process_join_match_command, this, _1);
    processors["PLACE"] = std::bind(&self::process_place_mark_command, this, _1);    
}

void multiplayer_match_messenger::process_set_name_command(
    util::value_ref<std::vector<std::string>> tokens)
{
    player_name = tokens[1];
}

void multiplayer_match_messenger::process_create_match_command(
    util::value_ref<std::vector<std::string>> tokens)
{
    try
    {
        joined_match = registry.create_new_match(tokens[1], std::stoi(tokens[2]));

        joined_match->join(player_name);

        channel("OK");
    }
    catch (std::exception const&)
    {
        channel("ERROR;NAME NOT UNIQUE");
    }    
}

void multiplayer_match_messenger::process_join_match_command(
    util::value_ref<std::vector<std::string>> tokens)
{
    try
    {
        auto m = registry.get_match(tokens[1]);

        m->join(player_name);

        joined_match = m;

        channel("OK");
    }
    catch (std::exception const&)
    {            
        channel("ERROR;MATCH NOT FOUND");
    }    
}

void multiplayer_match_messenger::process_place_mark_command(
    util::value_ref<std::vector<std::string>> tokens)
{
    try
    {
        auto& g = joined_match->get_game();

        g.place({std::stoi(tokens[1]), std::stoi(tokens[2])});

        channel("OK");
    }
    catch (std::exception const&)
    {
        channel("ERROR;INVALID POSITION");
    }
}

} }