#pragma once

#include <boost/asio.hpp>
#include <unordered_map>

using boost::asio::ip::tcp;

class client_connection;

class game_server
{

public:
  
    explicit game_server(boost::asio::io_service& service);
    
    ~game_server();

    void dispose_connection(int const client_id);

private:

    void start_accepting_connections();

    client_connection& create_new_connection();

private:

    tcp::acceptor listener;

    std::unordered_map<int, std::unique_ptr<client_connection>> connections;

    int next_client_id = 0;

};