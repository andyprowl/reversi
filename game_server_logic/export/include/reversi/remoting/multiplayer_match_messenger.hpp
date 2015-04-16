#pragma once

#include <memory>
#include <string>

namespace reversi { namespace remoting
{

class multiplayer_match;
class multiplayer_match_registry;

class multiplayer_match_messenger
{

public:

    explicit multiplayer_match_messenger(multiplayer_match_registry& registry);
    
    void execute_command(std::string command);

    std::shared_ptr<multiplayer_match> get_joined_match() const;

    std::string get_player_name() const;

    game& get_current_game() const;

private:

    multiplayer_match_registry& registry;

    std::shared_ptr<multiplayer_match> joined_match;

    std::string player_name;

};

} }