#pragma once

#include "util/value_ref.hpp"
#include <functional>
#include <memory>
#include <string>

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

    multiplayer_match_registry& registry;

    client_communication_channel channel;

    std::shared_ptr<multiplayer_match> joined_match;

    std::string player_name;

};

} }