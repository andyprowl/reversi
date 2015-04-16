#include "stdafx.hpp"

#include "networking/client_connection.hpp"
#include "networking/message_server.hpp"

namespace networking
{

client_connection::client_connection(int const client_id, 
                                     boost::asio::io_service& service,
                                     message_server& server,
                                     std::ostream& log)
    : client_id{client_id}
    , socket{service}
    , server{server}
    , log{log}
{
}

boost::asio::ip::tcp::socket& client_connection::get_socket()
{
    return socket;
}

void client_connection::start_reading_messages(message_processor p)
{
    log << "Client " << client_id << " connected, reading messages..." << std::endl;

    try
    {
        processor = p;

        read_next_message();
    }
    catch (std::exception const& error)
    {            
        log << "Problem on client connection: ";

        log << error.what() << std::endl;
    }
}

void client_connection::send_message(util::value_ref<std::string> msg)
{
    auto& service = socket.get_io_service();

    service.post([this, msg]
    {
        write(socket, boost::asio::buffer(msg));
    });
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
    log << "Received message from client " << client_id << ": " << msg;

    return processor(std::move(msg));
}

void client_connection::remove_self_from_server()
{
    log << "Client " << client_id << " terminated, disposing connection..." << std::endl;

    server.dispose_connection(client_id);    
}

}