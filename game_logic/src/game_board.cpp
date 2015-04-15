#include "stdafx.hpp"

#include "reversi/game_board.hpp"

namespace reversi
{

game_board::game_board(int const size)
    : size{size}
    , board(size)
{    
}

int game_board::get_size() const
{
    return size;
}
    
void game_board::mark_board_cell(int const row, int const col, player const p)
{
    if (are_cell_coordinates_valid(row, col))
    {
        throw bad_cell_coordinates_exception{};
    }

    if (is_cell_occupied(row, col))
    {
        throw cell_busy_exception{};
    }

    board[row * size + col] = p;
}

bool game_board::are_cell_coordinates_valid(int const row, int const col) const
{
    return ((row < 0) || (row >= size) || (col < 0) || (col >= size));
}

bool game_board::is_cell_occupied(int const row, int const col) const
{
    auto const optional_mark = board[row * size + col];

    return bool{optional_mark};
}

}