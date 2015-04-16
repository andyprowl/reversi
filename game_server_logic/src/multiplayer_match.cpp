#include "stdafx.hpp"

#include "reversi/game_logger.hpp"
#include "reversi/local_game.hpp"
#include "reversi/remoting/multiplayer_match.hpp"

namespace reversi { namespace remoting
{

multiplayer_match::multiplayer_match(std::string name, 
                                     int const board_size, 
                                     std::unique_ptr<game_logger>&& logger)
    : name(std::move(name))
    , board_size{board_size}
    , logger{std::move(logger)}
{   
}

multiplayer_match::~multiplayer_match() = default;

std::string multiplayer_match::get_name() const
{
    return name;
}

int multiplayer_match::get_board_size() const
{
    return board_size;
}

void multiplayer_match::join(std::string player_name)
{
    if (is_full())
    {
        throw match_full_exception{};
    }

    add_player(std::move(player_name));

    if (is_full())
    {
        create_game_and_fire_event();
    }
}

game& multiplayer_match::get_game() const
{
    if (played_game == nullptr)
    {
        throw match_not_started_exception{};
    }

    return *played_game;
}

boost::signals2::connection multiplayer_match::register_match_full_handler(match_full_handler h)
{
    return on_match_full.connect(std::move(h));
}

bool multiplayer_match::is_full() const
{
    return (player_names.size() == 2u);
}

void multiplayer_match::add_player(std::string player_name)
{
    player_names.push_back(std::move(player_name));
}

void multiplayer_match::create_game_and_fire_event()
{
    played_game = std::make_unique<local_game>(board_size, 
                                               player_names[0], 
                                               player_names[1], 
                                               *logger);

    on_match_full(*played_game);    
}

} }