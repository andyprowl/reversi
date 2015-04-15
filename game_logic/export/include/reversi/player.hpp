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

inline std::string to_string(player const p)
{
    return ((p == player::black) ? "black" : "white");
}

inline std::ostream& operator << (std::ostream& out, player const p)
{
    out << to_string(p);

    return out;
}

inline player get_opponent_of(player const p)
{
    return (p == player::black) ? player::white : player::black;
}

}