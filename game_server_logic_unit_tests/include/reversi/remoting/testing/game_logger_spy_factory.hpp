#pragma once

#include "reversi/remoting/game_logger_factory.hpp"
#include "reversi/testing/game_logger_spy.hpp"

namespace reversi { namespace remoting { namespace testing
{

class game_logger_spy_factory : public game_logger_factory
{
    
public:

    virtual std::unique_ptr<game_logger> make_logger_for_game(std::string match_name) override
    {
        return std::make_unique<reversi::testing::game_logger_spy>();
    }

};

} } }