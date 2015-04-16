#pragma once

#include "util/value_ref.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

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