#pragma once

namespace reversi
{
        
enum class direction
{
    east,
    north_east,
    north,
    north_west,
    west,
    south_west,
    south,
    south_east
};

template<typename F>
void for_all_directions(F f)
{
    f(direction::east);
    f(direction::north_east);
    f(direction::north);
    f(direction::north_west);
    f(direction::west);
    f(direction::south_west);
    f(direction::south);
    f(direction::south_east);
}

}