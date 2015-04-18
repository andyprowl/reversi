#pragma once

#include "util/value_ref.hpp"
#include <boost/optional.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace reversi
{
    
enum class placement_outcome;
enum class player;

struct cell_position;

class game;

}

namespace reversi { namespace remoting
{

class multiplayer_match;
class multiplayer_match_registry;

class multiplayer_match_messenger
{

public:

    using client_communication_channel = std::function<void(util::value_ref<std::string>)>;;

public:

    explicit multiplayer_match_messenger(multiplayer_match_registry& registry,
                                         client_communication_channel channel);
    
    void execute_command(std::string command);

    std::shared_ptr<multiplayer_match> get_joined_match() const;

    std::string get_player_name() const;

private:

    void setup_command_handlers();

    void process_set_name_command(util::value_ref<std::vector<std::string>> tokens);

    void process_create_match_command(util::value_ref<std::vector<std::string>> tokens);

    void process_join_match_command(util::value_ref<std::vector<std::string>> tokens);

    void process_place_mark_command(util::value_ref<std::vector<std::string>> tokens);

    void process_board_size_query_command(util::value_ref<std::vector<std::string>> tokens);

    void process_player_name_query_command(util::value_ref<std::vector<std::string>> tokens);

    void process_cell_mark_query_command(util::value_ref<std::vector<std::string>> tokens);

    void join_match(std::shared_ptr<multiplayer_match> m);

    void register_for_full_match_notifications_from_joined_match();

    void register_for_placement_notifications(game& g);

    void on_placement_in_joined_game(cell_position pos, 
                                     player p, 
                                     placement_outcome outcome, 
                                     util::value_ref<std::vector<cell_position>> reversals);

    std::string format_all_positions(util::value_ref<std::vector<cell_position>> positions) const;

    std::string format_position(cell_position pos) const;

    std::string format_optional_player(boost::optional<player> content) const;

    std::string format_placement_outcome(placement_outcome outcome) const;

private:

    using command_handler = std::function<void(util::value_ref<std::vector<std::string>>)>;

private:

    multiplayer_match_registry& registry;

    client_communication_channel channel;

    std::shared_ptr<multiplayer_match> joined_match;

    std::string player_name;

    std::unordered_map<std::string, command_handler> processors;

};

} }