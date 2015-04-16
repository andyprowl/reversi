#pragma once

#include <boost/asio.hpp>
#include <array>

using boost::asio::ip::tcp;

class game_server;

class client_connection
{

public:

    client_connection(int const client_id, boost::asio::io_service& service, game_server& server);

    tcp::socket& get_socket();

    void start_reading_messages();

private:
    
    void read_next_message();

private:

    void on_message_received(std::size_t size_in_bytes);

    bool process_message(std::string message);

    void remove_self_from_server();

private:

    int client_id;

    tcp::socket socket;

    game_server& server;

    std::array<char, 4096> message_bytes;

};