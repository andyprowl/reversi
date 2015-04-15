#include "stdafx.hpp"

#include "reversi/game.hpp"
#include "reversi/player.hpp"
#include "reversi/score.hpp"

namespace reversi
{
    
game::game(int const board_size)
    : board{board_size}
    , next_moving_player{player::black}
{    
}

int game::get_board_size() const
{
    return board.get_size();
}

void game::place(int const row, int const col)
{
    board.mark_board_cell(row, col, next_moving_player);

    switch_turn();
}

score game::get_score() const
{
    return {0, 0};
}

player game::get_next_moving_player() const
{
    return next_moving_player;
}

void game::switch_turn()
{
    next_moving_player = (next_moving_player == player::black) ? player::white : player::black;
}

}