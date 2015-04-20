#pragma once

#include "reversi/game_logger.hpp"
#include <ostream>

namespace reversi
{

class stream_game_logger : public game_logger
{

public:

    explicit stream_game_logger(std::ostream& log);
        
    virtual void log_successful_placement(cell_position pos, player p) override;
    
    virtual void log_turn_switched(player p) override;
    
    virtual void log_turn_skipped(player p) override;
    
    virtual void log_game_over(game const& g) override;

private:
    
    std::ostream& log;

};

}