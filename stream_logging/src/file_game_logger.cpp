#include "stdafx.hpp"

#include "reversi/file_game_logger.hpp"

namespace reversi
{
    
file_game_logger::file_game_logger(std::string file_name)
    : std::ofstream{std::move(file_name)}
    , stream_game_logger{static_cast<std::ostream&>(*this)}
{    
}

}