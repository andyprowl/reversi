#include "stdafx.hpp"

#include "reversi/remoting/game_server.hpp"
#include "reversi/remoting/multiplayer_match_messenger.hpp"
#include <iostream>

namespace reversi { namespace remoting
{

game_server::game_server(boost::asio::io_service& service)
    : message_server{service, 4543, std::cout}
    , match_registry{logger_factory}
{
}

networking::client_connection::message_processor game_server::get_message_processor_for_connection(
    networking::client_connection& c)
{
    auto messenger = std::make_shared<multiplayer_match_messenger>(match_registry);

    return [messenger] (util::value_ref<std::string> msg)
    {
        if (msg == "STOP")
        {
            return false;
        }

        messenger->execute_command(msg);

        return true;
    };
}

} }