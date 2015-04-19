#pragma once

#include "reversi/stream_game_logger.hpp"
#include <fstream>
#include <string>

namespace reversi
{

class file_game_logger : private std::ofstream, public stream_game_logger
{

public:

    file_game_logger(std::string file_name);

    using std::ofstream::close;

};

}