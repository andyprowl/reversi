#pragma once

#include "networking/message_server.hpp"
#include "reversi/remoting/file_game_logger_factory.hpp"
#include "reversi/remoting/multiplayer_match_registry.hpp"

namespace reversi { namespace remoting
{
    
class game_server : public networking::message_server
{

public:
  
    explicit game_server(boost::asio::io_service& service);

private:

    virtual networking::client_connection::message_processor get_message_processor_for_connection(
        networking::client_connection& c) override;

private:

    file_game_logger_factory logger_factory;

    multiplayer_match_registry match_registry;

};

} }