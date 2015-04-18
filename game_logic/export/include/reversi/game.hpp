#pragma once

#include "util/value_ref.hpp"
#include <boost/optional.hpp>
#include <boost/signals2/connection.hpp>
#include <functional>
#include <string>

namespace reversi
{
    
enum class placement_outcome;
enum class player;

struct cell_position;
struct game_score;

class game
{

public:

    using placement_event_handler = 
        std::function<void(cell_position, 
                           player, 
                           placement_outcome, 
                           util::value_ref<std::vector<cell_position>>)>;

public:

    virtual ~game() = default;

    virtual int get_board_size() const = 0;

    virtual std::string get_player_name(player p) const = 0;

    virtual placement_outcome place(cell_position pos) = 0;
    
    virtual boost::optional<player> get_board_cell_mark(
        cell_position pos) const = 0;

    virtual game_score get_score() const = 0;

    virtual player get_next_moving_player() const = 0;

    virtual bool is_over() const = 0;

    virtual boost::signals2::connection register_placement_event_handler(
        placement_event_handler h) = 0;

}; 

boost::optional<player> get_winning_player(game const& g);

}