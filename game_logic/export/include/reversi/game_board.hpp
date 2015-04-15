#pragma once

#include <boost/optional.hpp>
#include <stdexcept>
#include <vector>

namespace reversi
{

enum class player;

class bad_cell_coordinates_exception : public virtual std::exception
{
};

class cell_busy_exception : public virtual std::exception
{    
};

class game_board
{
    
public:

    game_board(int size);

    int get_size() const;

    void mark_board_cell(int row, int col, player p);

private:

    bool are_cell_coordinates_valid(int row, int col) const;

    bool is_cell_occupied(int row, int col) const;

private:

    int size;

    std::vector<boost::optional<player>> board;

};

}