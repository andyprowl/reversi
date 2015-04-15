#pragma once

#include "reversi/game_board.hpp"

namespace reversi
{

enum class player;

struct score;
    
class game
{
    
public:

    game(int board_size);

    int get_board_size() const;

    void place(int row, int col);
    
    score get_score() const;

    player get_next_moving_player() const;

private:

    void switch_turn();

private:

    game_board board;

    player next_moving_player;

};

}