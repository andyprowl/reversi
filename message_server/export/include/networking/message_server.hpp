#pragma once

#include "networking/client_connection.hpp"
#include <boost/asio.hpp>
#include <unordered_map>

namespace networking
{

class message_server
{

public:
  
    explicit message_server(boost::asio::io_service& service, int port_number, std::ostream& log);
    
    virtual ~message_server();

    void dispose_connection(int const client_id);

private:

    virtual client_connection::message_processor get_message_processor_for_connection(
        client_connection& c);

    void start_accepting_connections();

    client_connection& create_new_connection();

private:

    std::ostream& log;

    boost::asio::ip::tcp::acceptor listener;

    std::unordered_map<int, std::unique_ptr<client_connection>> connections;

    int next_client_id = 0;

};

}