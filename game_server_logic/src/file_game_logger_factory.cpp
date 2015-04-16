#include "stdafx.hpp"

#include "reversi/file_game_logger.hpp"
#include "reversi/remoting/file_game_logger_factory.hpp"

namespace reversi { namespace remoting
{

std::unique_ptr<game_logger> file_game_logger_factory::make_logger_for_game(
    std::string match_name)
{
    return std::make_unique<file_game_logger>(std::move(match_name));   
}

} }