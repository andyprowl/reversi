#include "stdafx.hpp"

#include "reversi/remoting/game_server.hpp"
#include <iostream>

int main(int argc, wchar_t* argv[])
{
    try
    {
        auto service = boost::asio::io_service{};

        reversi::remoting::game_server server{service};
        
        service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}