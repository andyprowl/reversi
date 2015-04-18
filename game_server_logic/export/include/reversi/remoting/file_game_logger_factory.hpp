#pragma once

#include "reversi/remoting/game_logger_factory.hpp"

namespace reversi { namespace remoting
{

class file_game_logger_factory : public game_logger_factory
{
    
public:

    virtual std::unique_ptr<game_logger> make_logger_for_game(std::string match_name) override;

};

} }