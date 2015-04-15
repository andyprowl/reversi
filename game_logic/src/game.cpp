#include "stdafx.hpp"

#include "reversi/game.hpp"
#include "reversi/score.hpp"

namespace reversi
{
    
game::game(int const board_size)
    : board_size{board_size}
{    
}

int game::get_board_size() const
{
    return board_size;
}

score game::get_score() const
{
    return {0, 0};
}


}