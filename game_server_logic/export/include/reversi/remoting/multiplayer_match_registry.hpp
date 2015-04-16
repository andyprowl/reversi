#pragma once

#include "util/value_ref.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace reversi { namespace remoting
{

class game_logger_factory;
class multiplayer_match;

class match_name_not_unique_exception : public virtual std::exception
{    
};

class match_not_found_exception : public virtual std::exception
{    
};

class multiplayer_match_registry
{
    
public:

    explicit multiplayer_match_registry(game_logger_factory& logger_factory);

    std::shared_ptr<multiplayer_match> create_new_match(std::string name, int board_size);

    std::shared_ptr<multiplayer_match> get_match(util::value_ref<std::string> name) const;

private:

    void throw_if_match_name_is_not_unique(util::value_ref<std::string> name) const;

    std::shared_ptr<multiplayer_match> create_tracked_match(std::string name, int board_size);

private:

    game_logger_factory& logger_factory;

    std::unordered_map<std::string, std::weak_ptr<multiplayer_match>> matches;

};

} }