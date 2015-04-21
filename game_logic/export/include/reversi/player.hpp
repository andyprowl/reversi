#pragma once

#include <ostream>
#include <string>

namespace reversi
{
    
enum class player
{
    black,
    white
};

inline std::ostream& operator << (std::ostream& out, player const p)
{
    out << ((p == player::black) ? "black" : "white");

    return out;
}

inline player get_opponent_of(player const p)
{
    return (p == player::black) ? player::white : player::black;
}

}