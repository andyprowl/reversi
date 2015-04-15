#pragma once

namespace reversi
{

enum class player;
    
struct cell_position;

class game_logger
{
    
public:

    virtual ~game_logger() = default;

    virtual void log_successful_placement(cell_position pos, player p) = 0;

    virtual void log_turn_switched_message(player p) = 0;

    virtual void log_turn_skipped_message(player p) = 0;

    virtual void log_game_over_message(game const& g) = 0;

};

}