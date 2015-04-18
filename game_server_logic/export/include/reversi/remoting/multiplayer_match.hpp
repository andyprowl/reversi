#pragma once

#include "util/signal_traits.hpp"
#include <boost/signals2.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace reversi
{
    
class game;
class game_logger;

}

namespace reversi { namespace remoting
{

class match_full_exception : public virtual std::exception
{    
};

class match_not_started_exception : public virtual std::exception
{    
};

class multiplayer_match
{

public:

    using match_full_handler = std::function<void(game&)>;
    
public:

    multiplayer_match(std::string name, int board_size, std::unique_ptr<game_logger>&& logger);

    ~multiplayer_match();

    std::string get_name() const;

    int get_board_size() const;

    void join(std::string player_name);

    bool is_full() const;

    game& get_game() const;
    
    boost::signals2::connection register_match_full_handler(match_full_handler h);

private:

    void add_player(std::string player_name);

    void create_game_and_fire_event();

private:

    using match_full_event = util::signal_type_t<match_full_handler>;

private:

    std::string name;

    int board_size;

    std::vector<std::string> player_names;

    std::unique_ptr<game> played_game;

    match_full_event on_match_full;

    std::unique_ptr<game_logger> logger;

};

} }