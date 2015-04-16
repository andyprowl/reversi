#pragma once

#include "reversi/cell_position.hpp"
#include "reversi/game_logger.hpp"
#include "reversi/player.hpp"
#include <string>
#include <vector>

namespace reversi { namespace testing
{

class game_logger_spy : public game_logger
{

public:

    virtual void log_successful_placement(cell_position pos, player p) override
    {
        messages.emplace_back("SP " + 
                              to_string(p) +
                              " " +
                              std::to_string(pos.row) + 
                              " " + 
                              std::to_string(pos.col));
    }

    virtual void log_turn_switched_message(player p) override
    {
        messages.emplace_back("TS " + to_string(p));
    }

    virtual void log_turn_skipped_message(player p) override
    {
        messages.emplace_back("TSK " + to_string(p));
    }

    virtual void log_game_over_message(local_game const& g) override
    {
        messages.emplace_back("GO");
    }

public:

    std::vector<std::string> messages;
    
};

} }