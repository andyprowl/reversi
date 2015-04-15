#include "stdafx.hpp"

#include "reversi/cell_position.hpp"
#include "reversi/game.hpp"
#include "reversi/game_score.hpp"
#include "reversi/player.hpp"
#include "reversi/stream_game_logger.hpp"
#include <algorithm>

namespace reversi
{
    
stream_game_logger::stream_game_logger(std::ostream& log)
    : log{log}
{
    
}
    
void stream_game_logger::log_successful_placement(cell_position const pos, player const p)
{
    log << "A " 
        << to_string(p) 
        << " mark has been placed at position " 
        << to_string(pos) << "." 
        << std::endl;
}

void stream_game_logger::log_turn_switched_message(player const p)
{
    log << "Turn switched, " << to_string(p) << " moves." << std::endl;
}

void stream_game_logger::log_turn_skipped_message(player const p)
{
    auto const opponent = get_opponent_of(p);

    log << "Skipping " 
        << to_string(opponent) 
        << "'s turn because they cannot move; " 
        << to_string(p)
        << " moves again." 
        << std::endl;
}

void stream_game_logger::log_game_over_message(game const& g)
{
    auto const score = g.get_score();

    auto const winner = get_winning_player(score);

    if (winner)
    {
        log << "Game over: " 
            << to_string(*winner) 
            << " wins with score " 
            << std::max(score.white, score.black)
            << " - "
            << std::min(score.white, score.black)
            << std::endl;
    }
    else
    {
        log << "Game over: there is no winner. The final score is " 
            << score.white 
            << " - " 
            << score.black
            << "." << std::endl;
    }
}

}