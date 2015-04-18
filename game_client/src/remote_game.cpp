#include "stdafx.hpp"

#include "reversi/cell_position.hpp"
#include "reversi/placement_outcome.hpp"
#include "reversi/player.hpp"
#include "reversi/remoting/remote_game.hpp"
#include "util/sequence.hpp"
#include "util/tokenize.hpp"

namespace reversi { namespace remoting
{

remote_game::remote_game(std::unique_ptr<boost::asio::ip::tcp::socket> channel)
    : channel{std::move(channel)}
{
    start_listening_to_placement_notifications();
}

int remote_game::get_board_size() const
{
    auto const answer = send_command("SIZE");

    return parse_board_size_query_result(answer);
}

std::string remote_game::get_player_name(player const p) const
{
    auto command = "PLAYER;" + format_player(p);

    auto const answer = send_command(std::move(command));

    return parse_player_name_query_result(answer);
}

placement_outcome remote_game::place(cell_position const pos)
{
    auto const command = "PLACE;" + format_position(pos);

    stop_listening_to_placement_notifications();

    send_message_to_server(std::move(command));

    auto const answer = receive_message_from_server();
    
    throw_if_error_report(answer);

    auto const placement = receive_message_from_server();

    handle_placement_notification(placement);

    auto const outcome = receive_message_from_server();

    start_listening_to_placement_notifications();

    return parse_placement_response(outcome);
}    
    
boost::optional<player> remote_game::get_board_cell_mark(cell_position const pos) const
{
    auto const command = "CELL;" + format_position(pos);

    auto const answer = send_command(std::move(command));

    return parse_board_cell_content_query_result(answer);
}

boost::signals2::connection remote_game::register_placement_event_handler(
    placement_event_handler h)
{
    return on_placement.connect(std::move(h));
}

std::string remote_game::send_command(std::string msg) const
{
    stop_listening_to_placement_notifications();

    send_message_to_server(std::move(msg));

    auto answer = receive_message_from_server();

    start_listening_to_placement_notifications();    

    return answer;
}

void remote_game::send_message_to_server(std::string msg) const
{
    write(*channel, boost::asio::buffer(msg));
}

std::string remote_game::receive_message_from_server() const
{
    auto bytes = std::array<char, 256>{};

    auto const bytes_read = read(*channel, boost::asio::buffer(bytes));

    return {std::cbegin(bytes), std::next(std::cbegin(bytes), bytes_read)};
}

void remote_game::start_listening_to_placement_notifications() const
{ 
    static auto bytes = std::array<char, 4096>{};

    async_read(*channel, 
               boost::asio::buffer(bytes), 
               [this] (boost::system::error_code const& error, std::size_t const bytes_read)
    {
        if (error) { return; }
        
        auto msg = std::string{std::cbegin(bytes), std::next(std::cbegin(bytes), bytes_read)};

        handle_placement_notification(msg);

        start_listening_to_placement_notifications();
    });
}

void remote_game::stop_listening_to_placement_notifications() const
{
    channel->cancel();
}

int remote_game::parse_board_size_query_result(util::value_ref<std::string> response) const
{
    throw_if_error_report(response);

    auto const tokens = util::tokenize(response, ";");

    return std::stoi(tokens[1]);
}

std::string remote_game::parse_player_name_query_result(
    util::value_ref<std::string> response) const
{
    throw_if_error_report(response);

    auto const tokens = util::tokenize(response, ";");

    return tokens[1];
}

boost::optional<player> remote_game::parse_board_cell_content_query_result(
    util::value_ref<std::string> response) const
{
    throw_if_error_report(response);

    auto const tokens = util::tokenize(response, ";");

    return parse_board_cell_content(tokens[1]);
}

placement_outcome remote_game::parse_placement_response(
    util::value_ref<std::string> response) const
{
    auto const tokens = util::tokenize(response, ";");

    return parse_placement_outcome(tokens[1]);
}

void remote_game::handle_placement_notification(util::value_ref<std::string> msg) const
{
    auto tokens = util::tokenize(msg, ";");

    auto const pos = cell_position{std::stoi(tokens[1]), std::stoi(tokens[2])};

    auto const p = parse_player(tokens[2]);

    auto const outcome = parse_placement_outcome(tokens[3]);

    auto formatted_reversals = std::vector<std::string>{};
    
    std::move(std::next(std::begin(tokens), 4), 
              std::end(tokens), 
              std::back_inserter(formatted_reversals));

    auto const reversals = parse_all_positions(formatted_reversals);

    on_placement(pos, p, outcome, reversals);
}

boost::optional<player> remote_game::parse_board_cell_content(
    util::value_ref<std::string> content) const
{
    if (content == "EMPTY")
    {
        return boost::none;
    }
    else
    {
        return parse_player(content);
    }
}

placement_outcome remote_game::parse_placement_outcome(
    util::value_ref<std::string> formatted_outcome) const
{
    if (formatted_outcome == "SWITCH")
    {
        return placement_outcome::turn_switched;
    }
    else if (formatted_outcome == "SKIP")
    {
        return placement_outcome::turn_skipped;
    }
    else
    {
        return placement_outcome::game_over;
    }    
}

std::vector<cell_position> remote_game::parse_all_positions(
    util::value_ref<std::vector<std::string>> positions) const
{
    auto result = std::vector<cell_position>{};

    auto const num_of_positions = static_cast<int>(positions.size() / 2);

    result.reserve(num_of_positions);

    for (auto const i : util::sequence(0, num_of_positions))
    {
        result.emplace_back(std::stoi(positions[i * 2]), std::stoi(positions[i * 2 + 1]));
    }

    return result;
}

player remote_game::parse_player(util::value_ref<std::string> formatted_player) const
{
    return (formatted_player == "WHITE") ? player::white : player::black;
}

std::string remote_game::format_player(player const p) const
{
    return (p == player::white) ? "WHITE" : "BLACK";
}

std::string remote_game::format_position(cell_position pos) const
{
    return std::to_string(pos.row) + ";" + std::to_string(pos.col);
}

void remote_game::throw_if_error_report(util::value_ref<std::string> response) const
{
    if (response.find("OK") != 0)
    {
        throw std::exception{};
    }     
}

} }