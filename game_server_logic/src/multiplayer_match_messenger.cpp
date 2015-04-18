#include "stdafx.hpp"

#include "reversi/cell_position.hpp"
#include "reversi/game.hpp"
#include "reversi/placement_outcome.hpp"
#include "reversi/player.hpp"
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
    processors["SIZE"] = std::bind(&self::process_board_size_query_command, this, _1);
    processors["PLAYER"] = std::bind(&self::process_player_name_query_command, this, _1);
    processors["CELL"] = std::bind(&self::process_cell_mark_query_command, this, _1);
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
        auto m = registry.create_new_match(tokens[1], std::stoi(tokens[2]));

        join_match(std::move(m));
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
        
        join_match(std::move(m));
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

void multiplayer_match_messenger::process_board_size_query_command(
    util::value_ref<std::vector<std::string>> tokens)
{
    if (joined_match == nullptr)
    {
        channel("ERROR;NO MATCH JOINED");

        return;
    }

    auto const board_size = joined_match->get_board_size();    

    channel("OK;" + std::to_string(board_size));
}

void multiplayer_match_messenger::process_player_name_query_command(
    util::value_ref<std::vector<std::string>> tokens)
{
    if (joined_match == nullptr)
    {
        channel("ERROR;NO MATCH JOINED");

        return;
    }

    try
    {
        auto& g = joined_match->get_game();

        auto const p = (tokens[1] == "WHITE") ? player::white : player::black;
    
        channel("OK;" + g.get_player_name(p));
    }
    catch (std::exception const&)
    {
        channel("ERROR;GAME NOT STARTED");
    }
}

void multiplayer_match_messenger::process_cell_mark_query_command(
    util::value_ref<std::vector<std::string>> tokens)
{
    if (joined_match == nullptr)
    {
        channel("ERROR;NO MATCH JOINED");

        return;
    }

    try
    {
        auto& g = joined_match->get_game();

        auto content = g.get_board_cell_mark({std::stoi(tokens[1]), std::stoi(tokens[2])});

        channel("OK;" + format_optional_player(content));
    }
    catch (std::exception const&)
    {
        channel("ERROR;GAME NOT STARTED");
    }
}

void multiplayer_match_messenger::join_match(std::shared_ptr<multiplayer_match> m)
{
    m->join(player_name);

    joined_match = std::move(m);

    if (joined_match->is_full())
    {
        auto& g = joined_match->get_game();

        register_for_placement_notifications(g);
    }
    else
    {
        register_for_full_match_notifications_from_joined_match();
    }

    channel("OK");
}

void multiplayer_match_messenger::register_for_full_match_notifications_from_joined_match()
{
    joined_match->register_match_full_handler([this] (game& g)
    {
        register_for_placement_notifications(g);
    });
}

void multiplayer_match_messenger::register_for_placement_notifications(game& g)
{
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::placeholders::_4;
    using self = multiplayer_match_messenger;

    auto handler = std::bind(&self::on_placement_in_joined_game, this, _1, _2, _3, _4);

    g.register_placement_event_handler(handler);
}

void multiplayer_match_messenger::on_placement_in_joined_game(
    cell_position const pos, 
    player const p, 
    placement_outcome const outcome, 
    util::value_ref<std::vector<cell_position>> reversals)
{
    auto formatted_position = format_position(pos);

    auto formatted_player = format_optional_player(p);

    auto formatted_outcome = format_placement_outcome(outcome);

    auto formatted_reversals = format_all_positions(reversals);

    auto msg = "PLACEMENT;" + 
               std::move(formatted_position) + 
               ";" + 
               std::move(formatted_player) +
               ";" +
               std::move(formatted_outcome) +
               ";" +
               std::move(formatted_reversals);

    channel(std::move(msg));
}

std::string multiplayer_match_messenger::format_all_positions(
    util::value_ref<std::vector<cell_position>> positions) const
{
    auto result = std::string{};

    auto first = true;    
    for (auto const pos : positions)
    {
        if (!first)
        {
            result += ";";
        }

        result += format_position(pos);

        first = false;
    }

    return result;
}

std::string multiplayer_match_messenger::format_position(cell_position const pos) const
{
    return std::to_string(pos.row) + ";" + std::to_string(pos.col);
}

std::string multiplayer_match_messenger::format_optional_player(
    boost::optional<player> const content) const
{
    if (!content)
    {
        return "EMPTY";
    }
    else if (content == player::white)
    {
        return "WHITE";
    }
    else
    {
        return "BLACK";
    }    
}

std::string multiplayer_match_messenger::format_placement_outcome(
    placement_outcome const outcome) const
{
    if (outcome == placement_outcome::turn_switched)
    {
        return "SWITCH";
    }
    else if (outcome == placement_outcome::turn_skipped)
    {
        return "SKIP";
    }
    else
    {
        return "OVER";
    }
}

} }