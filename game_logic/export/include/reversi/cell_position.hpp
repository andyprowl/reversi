#pragma once

#include <string>

namespace reversi
{

enum class direction;
    
struct cell_position
{

public:

    cell_position(int row, int col);

public:
    
    int row;

    int col;

};

cell_position get_next_cell_position(cell_position p, direction d);

std::string to_string(cell_position p);

}