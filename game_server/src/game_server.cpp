#include "stdafx.hpp"

#include "reversi/remoting/game_server.hpp"
#include "reversi/remoting/client_connection.hpp"
#include <iostream>

game_server::game_server(boost::asio::io_service& io_service)
    : listener{io_service, tcp::endpoint(tcp::v4(), 4543)}
{
    start_accepting_connections();
}

game_server::~game_server() = default;

void game_server::dispose_connection(int const client_id)
{
    connections.erase(client_id);
}

void game_server::start_accepting_connections()
{
    std::cout << "Ready to accept new client connections..." << std::endl;

    auto& connection = create_new_connection();

    listener.async_accept(connection.get_socket(), 
                          [this, &connection] (boost::system::error_code const& e)
    {
        if (!e)
        {
            connection.start_reading_messages();
        }

        start_accepting_connections(); 
    });
}

client_connection& game_server::create_new_connection()
{
    auto client_id = ++next_client_id;

    auto connection = std::make_unique<client_connection>(next_client_id, 
                                                          listener.get_io_service(),
                                                          *this);

    auto& new_connection = *connection;

    connections.emplace(client_id, std::move(connection));

    return new_connection;
}