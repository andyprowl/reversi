#pragma once

#include "util/value_ref.hpp"
#include <boost/asio.hpp>
#include <array>
#include <ostream>

namespace networking
{
    
class message_server;

class client_connection
{

public:

    using message_processor = std::function<bool(util::value_ref<std::string>)>;

public:

    client_connection(int const client_id, 
                      boost::asio::io_service& service, 
                      message_server& server,
                      std::ostream& log);

    boost::asio::ip::tcp::socket& get_socket();

    void start_reading_messages(message_processor p);

    void send_message(util::value_ref<std::string> msg);

private:
    
    void read_next_message();
    
    void on_message_received(std::size_t size_in_bytes);

    bool process_message(std::string message);

    void remove_self_from_server();

private:

    int client_id;

    boost::asio::ip::tcp::socket socket;

    message_server& server;

    std::ostream& log;

    message_processor processor;

    std::array<char, 4096> message_bytes;

};

}