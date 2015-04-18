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
    auto channel = multiplayer_match_messenger::client_communication_channel{
        [this, &c] (std::string msg)
    {
        c.send_message(std::move(msg));        
    }};

    auto messenger = std::make_shared<multiplayer_match_messenger>(match_registry, 
                                                                   std::move(channel));

    return [this, messenger] (util::value_ref<std::string> msg)
    {
        return forward_non_terminal_message_to_messenger(msg, *messenger);
    };
}

bool game_server::forward_non_terminal_message_to_messenger(
    util::value_ref<std::string> msg,
    multiplayer_match_messenger& messenger)
{
    if (msg == "STOP")
    {
        return false;
    }

    messenger.execute_command(msg);

    return true;
}


} }