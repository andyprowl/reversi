#pragma once

#include "reversi/game_board.hpp"

namespace reversi
{

enum class direction;
enum class player;

struct cell_position;
struct game_score;
    
class no_reversal_triggered_exception : public virtual std::exception
{    
};

class cell_busy_exception : public virtual std::exception
{    
};

class game
{
    
public:

    game(int board_size);

    int get_board_size() const;

    boost::optional<player> get_board_cell_mark(cell_position pos) const;

    void place(cell_position pos);
    
    game_score get_score() const;

    player get_next_moving_player() const;

private:

    void switch_turn();

    void place_initial_marks();

    void throw_if_cell_is_occupied(game_board const& b, cell_position const pos) const;
    
    void apply_reversals_or_throw_if_none_is_triggered(cell_position const pos);

    std::vector<cell_position> get_reversals_for_placement(cell_position const pos) const;

    std::vector<cell_position> get_reversals_for_placement_in_direction(cell_position pos, 
                                                                        direction d) const;
    
private:

    game_board board;

    player next_moving_player;

};

}