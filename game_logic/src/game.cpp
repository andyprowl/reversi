#include "stdafx.hpp"

#include "reversi/cell_position.hpp"
#include "reversi/direction.hpp"
#include "reversi/game.hpp"
#include "reversi/player.hpp"
#include "reversi/placement_outcome.hpp"
#include "reversi/game_score.hpp"
#include "util/sequence.hpp"

namespace reversi
{

game::game(int const board_size)
    : board{board_size}
    , next_moving_player{player::black}
    , score{2, 2}
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

placement_outcome game::place(cell_position const pos)
{
    throw_if_cell_is_occupied(pos);

    apply_reversals_or_throw_if_none_is_triggered(pos);
    
    mark_placement_cell_and_update_score(pos);

    return determine_placement_outcome();
}

game_score game::get_score() const
{
    return score;
}

player game::get_next_moving_player() const
{
    return next_moving_player;
}

void game::place_initial_marks()
{
    auto const board_size = board.get_size();

    board.mark_cell({board_size / 2 - 1, board_size / 2 - 1}, player::white);
    
    board.mark_cell({board_size / 2, board_size / 2}, player::white);
    
    board.mark_cell({board_size / 2 - 1, board_size / 2}, player::black);
    
    board.mark_cell({board_size / 2, board_size / 2 - 1}, player::black);    
}
    
void game::throw_if_cell_is_occupied(cell_position const pos) const
{
    if (board.is_cell_occupied(pos))
    {
        throw cell_busy_exception{}; 
    }   
}

void game::apply_reversals_or_throw_if_none_is_triggered(cell_position const pos)
{
    auto const reversals = get_reversals_for_placement(pos, next_moving_player);
    
    if (reversals.empty())
    {
        throw no_reversal_triggered_exception{};
    }

    for (auto const r : reversals)
    {
        board.mark_cell(r, next_moving_player);    
    }

    update_score_after_reversals(reversals);
}

std::vector<cell_position> game::get_reversals_for_placement(cell_position const pos,
                                                             player const p) const
{
    if (board.is_cell_occupied(pos)) { return {}; }

    auto reversals = std::vector<cell_position>{};

    for_all_directions([&reversals, this, p, pos] (direction const d)
    {
        auto revs = get_reversals_for_placement_in_direction(pos, p, d);

        reversals.insert(std::end(reversals), std::cbegin(revs), std::cend(revs));
    });

    return reversals;
}

std::vector<cell_position> game::get_reversals_for_placement_in_direction(
    cell_position const pos, 
    player const p,
    direction const d) const
{
    auto reversals = std::vector<cell_position>{};

    auto next_pos = get_next_cell_position(pos, d);

    while (is_cell_occupied_by_opponent_of_player(next_pos, p))
    {
        reversals.push_back(next_pos);
            
        next_pos = get_next_cell_position(next_pos, d);
    }

    auto valid_move = is_cell_occupied_by_player(next_pos, p);

    return valid_move ? reversals : std::vector<cell_position>{};
}

bool game::is_cell_occupied_by_opponent_of_player(cell_position const pos, player const p) const
{
    auto const opponent = get_opponent_of(p);

    return is_cell_occupied_by_player(pos, opponent);
}

bool game::is_cell_occupied_by_player(cell_position const pos, player const p) const
{
    return ((board.is_valid_cell_position(pos) && (board.get_cell_mark(pos) == p)));
}

void game::update_score_after_reversals(util::value_ref<std::vector<cell_position>> reversals)
{
    auto num_of_reversals = static_cast<int>(reversals.size());

    score = increase_score(score, next_moving_player, num_of_reversals);

    score = increase_score(score, get_opponent_of(next_moving_player), -num_of_reversals);    
}

void game::mark_placement_cell_and_update_score(cell_position const pos)
{
    board.mark_cell(pos, next_moving_player);

    score = increase_score(score, next_moving_player, 1);    
}

placement_outcome game::determine_placement_outcome()
{
    auto opponent = get_opponent_of(next_moving_player);

    if (can_player_move(opponent))
    {
        next_moving_player = opponent;

        return placement_outcome::turn_switched;
    }
    
    return can_player_move(next_moving_player) ? placement_outcome::turn_skipped :
                                                 placement_outcome::game_over;
}

bool game::can_player_move(player p) const
{
    for (auto const row : util::sequence(0, board.get_size()))
    {
        for (auto const col : util::sequence(0, board.get_size()))
        {
            auto const reversals = get_reversals_for_placement({row, col}, p);

            if (!reversals.empty())
            {
                return true;
            }
        }
    }

    return false;
}

}