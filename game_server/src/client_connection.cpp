#include "stdafx.hpp"

#include "reversi/remoting/client_connection.hpp"
#include "reversi/remoting/game_server.hpp"
#include <iostream>

client_connection::client_connection(int const client_id, 
                                     boost::asio::io_service& service,
                                     game_server& server)
    : client_id{client_id}
    , socket{service}
    , server{server}
{
}

tcp::socket& client_connection::get_socket()
{
    return socket;
}

void client_connection::start_reading_messages()
{
    std::cout << "Client " << client_id << " connected, reading messages..." << std::endl;

    try
    {
        read_next_message();
    }
    catch (std::exception const& error)
    {            
        std::cout << "Problem on client connection: ";

        std::cout << error.what() << std::endl;
    }
}

void client_connection::read_next_message()
{
    socket.async_read_some(
        boost::asio::buffer(message_bytes),
        [this] (boost::system::error_code const& ec, std::size_t const bytes_read)
    {
        if (ec) { return; }

        on_message_received(bytes_read);
    });
}

void client_connection::on_message_received(std::size_t const size_in_bytes)
{
    auto msg = std::string{std::begin(message_bytes), 
                            std::next(std::begin(message_bytes), size_in_bytes)};

    auto const keep_reading = process_message(std::move(msg));

    if (keep_reading)
    {
        read_next_message();
    }
    else
    {
        remove_self_from_server();
    }
}

bool client_connection::process_message(std::string msg)
{
    std::cout << "Message received from client " << client_id << ": " << msg << std::endl;

    return (msg != "STOP");
}

void client_connection::remove_self_from_server()
{
    std::cout << "Client " << client_id << " terminated, disposing connection..." << std::endl;

    server.dispose_connection(client_id);    
}