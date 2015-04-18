#pragma once

#include <memory>
#include <string>

namespace reversi
{

class game_logger;

}

namespace reversi { namespace remoting
{
    
class game_logger_factory
{
    
public:

    virtual ~game_logger_factory() = default;

    virtual std::unique_ptr<game_logger> make_logger_for_game(std::string match_name) = 0;

};

} }