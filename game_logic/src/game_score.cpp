#include "stdafx.hpp"

#include "reversi/game_score.hpp"
#include "reversi/player.hpp"

namespace reversi
{
    
game_score::game_score(int const white, int const black)
    : white{white}
    , black{black}
{    
}

bool operator == (game_score lhs, game_score rhs)
{
    return ((lhs.white == rhs.white) && (lhs.black == rhs.black));
            
}

bool operator != (game_score lhs, game_score rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator << (std::ostream& out, game_score s)
{
    out << "{ " << player::white << ": " << s.white << "; ";
    
    out << player::black << ": " << s.black << " }";

    return out;
}

game_score increase_score(game_score const s, player const p, int const points)
{
    if (p == player::white)
    {
        return {s.white + points, s.black};
    }
    else
    {
        return {s.white, s.black + points};
    }
}

boost::optional<player> get_winning_player(game_score const s)
{
    if (s.white > s.black) { return player::white; }

    if (s.black > s.white) { return player::black; }

    return boost::none;
}

}