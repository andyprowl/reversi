#include "stdafx.hpp"

#include "reversi/game.hpp"
#include "reversi/game_score.hpp"

namespace reversi
{
    
boost::optional<player> get_winning_player(game const& g)
{
    auto const score = g.get_score();

    return get_winning_player(score);
}

}