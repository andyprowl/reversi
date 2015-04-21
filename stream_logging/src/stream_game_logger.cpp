#include "stdafx.hpp"

#include "reversi/cell_position.hpp"
#include "reversi/game_score.hpp"
#include "reversi/game.hpp"
#include "reversi/player.hpp"
#include "reversi/stream_game_logger.hpp"
#include <algorithm>

namespace reversi
{
    
stream_game_logger::stream_game_logger(std::ostream& log)
    : log{log}
{
    
}
    
void stream_game_logger::log_successful_placement(cell_position const pos, 
                                                  player const p)
{
    log << "A " 
        << p
        << " mark has been placed at position " 
        << pos 
        << "." 
        << std::endl;
}

void stream_game_logger::log_turn_switched(player const p)
{
    log << "Turn switched, " << p << " moves." << std::endl;
}

void stream_game_logger::log_turn_skipped(player const p)
{
    auto const opponent = get_opponent_of(p);

    log << "Skipping " 
        << opponent
        << "'s turn because they cannot move; " 
        << p
        << " moves again." 
        << std::endl;
}

void stream_game_logger::log_game_over(game const& g)
{
    auto const score = g.get_score();
    auto const winner = get_winning_player(g);

    if (winner)
    {
        log << "Game over: " 
            << *winner
            << " wins with score " 
            << std::max(score.black, score.white)
            << " - "
            << std::min(score.black, score.white)
            << std::endl;
    }
    else
    {
        log << "Game over: there is no winner. The final score is " 
            << score.black 
            << " - " 
            << score.white
            << "." << std::endl;
    }
}

}