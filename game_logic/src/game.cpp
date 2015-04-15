#include "stdafx.hpp"

#include "reversi/cell_position.hpp"
#include "reversi/direction.hpp"
#include "reversi/game.hpp"
#include "reversi/player.hpp"
#include "reversi/game_score.hpp"

namespace reversi
{

game::game(int const board_size)
    : board{board_size}
    , next_moving_player{player::black}
{    
    place_initial_marks();
}

int game::get_board_size() const
{
    return board.get_size();
}

boost::optional<player> game::get_board_cell_mark(cell_position const pos) const
{
    return board.get_cell_mark(pos);
}

void game::place(cell_position const pos)
{
    throw_if_cell_is_occupied(board, pos);

    apply_reversals_or_throw_if_none_is_triggered(pos);
    
    board.mark_cell(pos, next_moving_player);

    switch_turn();
}

game_score game::get_score() const
{
    return {2, 2};
}

player game::get_next_moving_player() const
{
    return next_moving_player;
}

void game::switch_turn()
{
    next_moving_player = get_opponent_of(next_moving_player);
}

void game::place_initial_marks()
{
    auto const board_size = board.get_size();

    board.mark_cell({board_size / 2 - 1, board_size / 2 - 1}, player::white);
    
    board.mark_cell({board_size / 2, board_size / 2}, player::white);
    
    board.mark_cell({board_size / 2 - 1, board_size / 2}, player::black);
    
    board.mark_cell({board_size / 2, board_size / 2 - 1}, player::black);    
}
    
void game::throw_if_cell_is_occupied(game_board const& b, cell_position const pos) const
{
    if (b.is_cell_occupied(pos))
    {
        throw cell_busy_exception{}; 
    }   
}

void game::apply_reversals_or_throw_if_none_is_triggered(cell_position const pos)
{
    auto const reversals = get_reversals_for_placement(pos);
    
    if (reversals.empty())
    {
        throw no_reversal_triggered_exception{};
    }

    for (auto const r : reversals)
    {
        board.mark_cell(r, next_moving_player);    
    }    
}

std::vector<cell_position> game::get_reversals_for_placement(cell_position const pos) const
{
    auto reversals = std::vector<cell_position>{};

    for_all_directions([&reversals, this, pos] (direction const d)
    {
        auto revs = get_reversals_for_placement_in_direction(pos, d);

        reversals.insert(std::end(reversals), std::cbegin(revs), std::cend(revs));
    });

    return reversals;
}

std::vector<cell_position> game::get_reversals_for_placement_in_direction(
    cell_position const pos, 
    direction const d) const
{
    auto reversals = std::vector<cell_position>{};

    auto next_pos = get_next_cell_position(pos, d);

    while (board.is_valid_cell_position(next_pos) &&
           (board.get_cell_mark(next_pos) == get_opponent_of(next_moving_player)))
    {
        reversals.push_back(next_pos);
            
        next_pos = get_next_cell_position(next_pos, d);
    }

    auto valid = (board.is_valid_cell_position(next_pos) && 
                  (board.get_cell_mark(next_pos) == next_moving_player));

    return valid ? reversals : std::vector<cell_position>{};
}

}