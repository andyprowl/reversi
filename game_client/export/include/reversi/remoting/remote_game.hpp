#pragma once

#include "reversi/game.hpp"
#include "util/signal_traits.hpp"
#include <boost/asio.hpp>
#include <string>
#include <thread>

namespace reversi { namespace remoting
{

class remote_game : public game
{
    
public:

    explicit remote_game(std::unique_ptr<boost::asio::ip::tcp::socket> channel);

    virtual int get_board_size() const override;

    virtual std::string get_player_name(player p) const override;

    virtual placement_outcome place(cell_position pos) override;
    
    virtual boost::optional<player> get_board_cell_mark(cell_position pos) const override;

    virtual boost::signals2::connection register_placement_event_handler(
        placement_event_handler h) override;

private:

    std::string send_command(std::string msg) const;

    void send_message_to_server(std::string msg) const;

    std::string receive_message_from_server() const;

    void start_listening_to_placement_notifications() const;

    void stop_listening_to_placement_notifications() const;

    int parse_board_size_query_result(util::value_ref<std::string> response) const;

    std::string parse_player_name_query_result(util::value_ref<std::string> response) const;

    boost::optional<player> parse_board_cell_content_query_result(
        util::value_ref<std::string> response) const;

    placement_outcome parse_placement_response(util::value_ref<std::string> response) const;

    void handle_placement_notification(util::value_ref<std::string> msg) const;

    boost::optional<player> parse_board_cell_content(util::value_ref<std::string> content) const;

    placement_outcome parse_placement_outcome(
        util::value_ref<std::string> formatted_outcome) const;

    std::vector<cell_position> parse_all_positions(
        util::value_ref<std::vector<std::string>> positions) const;

    player parse_player(util::value_ref<std::string> formatted_player) const;

    std::string format_player(player p) const;

    std::string format_position(cell_position pos) const;

    void throw_if_error_report(util::value_ref<std::string> response) const;

private:

    using placement_event = util::signal_type_t<placement_event_handler>;

private:

    std::unique_ptr<boost::asio::ip::tcp::socket> channel;

    placement_event on_placement;

};

} }