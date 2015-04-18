#include "stdafx.hpp"

#include "reversi/cell_position.hpp"
#include "reversi/direction.hpp"
#include <cassert>

namespace reversi
{
    
cell_position::cell_position(int const row, int const col)
    : row{row}
    , col{col}
{    
}

bool operator == (cell_position lhs, cell_position rhs)
{
    return ((lhs.row == rhs.row) && (lhs.col == rhs.col));
}

bool operator != (cell_position lhs, cell_position rhs)
{
    return !(lhs == rhs);
}

cell_position get_next_cell_position(cell_position const p, direction const d)
{
    switch (d)
    {
        default: { assert(false); } // Intentional fallback
        case direction::east: { return {p.row, p.col + 1}; }
        case direction::north_east: { return {p.row - 1, p.col + 1}; }
        case direction::north: { return {p.row - 1, p.col}; }
        case direction::north_west: { return {p.row - 1, p.col - 1}; }
        case direction::west: { return {p.row, p.col - 1}; }
        case direction::south_west: { return {p.row + 1, p.col - 1}; }
        case direction::south: { return {p.row + 1, p.col}; }
        case direction::south_east: { return {p.row + 1, p.col + 1}; }
    }
}

std::string to_string(cell_position const p)
{
    return "(row = " + 
           std::to_string(p.row) + 
           ", col = " + 
           std::to_string(p.col) + ")";
}

}