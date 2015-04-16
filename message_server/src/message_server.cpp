#include "stdafx.hpp"

#include "networking/message_server.hpp"
#include "networking/client_connection.hpp"

namespace networking
{

message_server::message_server(boost::asio::io_service& service, 
                               int const port_number,
                               std::ostream& log)
    : log{log}
    , listener{service, boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), 
                                                       static_cast<unsigned short>(port_number)}}
    
{
    start_accepting_connections();
}

message_server::~message_server() = default;

void message_server::dispose_connection(int const client_id)
{
    connections.erase(client_id);
}

void message_server::start_accepting_connections()
{
    auto& connection = create_new_connection();

    listener.async_accept(connection.get_socket(), 
                          [this, &connection] (boost::system::error_code const& e)
    {
        if (!e)
        {
            auto processor = get_message_processor_for_connection(connection);

            connection.start_reading_messages(std::move(processor));
        }

        start_accepting_connections(); 
    });
}

client_connection::message_processor message_server::get_message_processor_for_connection(
    client_connection& c)
{
    return [] (util::value_ref<std::string>) { return true; };
}

client_connection& message_server::create_new_connection()
{
    auto client_id = ++next_client_id;

    auto connection = std::make_unique<client_connection>(next_client_id, 
                                                          listener.get_io_service(),
                                                          *this,
                                                          log);

    auto& new_connection = *connection;

    connections.emplace(client_id, std::move(connection));

    return new_connection;
}

}