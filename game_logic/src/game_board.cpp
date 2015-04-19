#include "stdafx.hpp"

#include "reversi/cell_position.hpp"
#include "reversi/game_board.hpp"

namespace reversi
{

game_board::game_board(int const size)
    : size{size}
    , board(size * size)
{    
}

int game_board::get_size() const
{
    return size;
}

void game_board::mark_cell(cell_position const pos, player const p)
{
    throw_if_cell_position_is_not_valid(pos);

    board[pos.row * size + pos.col] = p;
}

bool game_board::is_valid_cell_position(cell_position const pos) const
{
    return ((pos.row >= 0) && 
            (pos.row < size) && 
            (pos.col >= 0) && 
            (pos.col < size));
}

boost::optional<player> game_board::get_cell_mark(cell_position const pos) const
{
    throw_if_cell_position_is_not_valid(pos);

    return board[pos.row * size + pos.col];
}

void game_board::throw_if_cell_position_is_not_valid(
    cell_position const pos) const
{
    if (!is_valid_cell_position(pos))
    {
        throw bad_cell_coordinates_exception{};
    }
}

bool is_cell_occupied(game_board const& b, cell_position const pos)
{
    auto const optional_mark = b.get_cell_mark(pos);

    return bool{optional_mark};
}

}