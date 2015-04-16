#include "stdafx.hpp"

#include "reversi/game_logger.hpp"
#include "reversi/remoting/game_logger_factory.hpp"
#include "reversi/remoting/multiplayer_match.hpp"
#include "reversi/remoting/multiplayer_match_registry.hpp"

namespace reversi { namespace remoting
{

multiplayer_match_registry::multiplayer_match_registry(game_logger_factory& logger_factory)
    : logger_factory{logger_factory}
{    
}

std::shared_ptr<multiplayer_match> multiplayer_match_registry::create_new_match(
    std::string name, 
    int const board_size)
{
    throw_if_match_name_is_not_unique(name);

    auto m = create_tracked_match(name, board_size);

    matches.emplace(std::move(name), m);

    return m;
}

std::shared_ptr<multiplayer_match> multiplayer_match_registry::get_match(
    util::value_ref<std::string> name) const
{
    auto const it = matches.find(name);
    if (it == std::cend(matches))
    {
        throw match_not_found_exception{};
    }

    return it->second.lock();
}

std::vector<std::shared_ptr<multiplayer_match>> multiplayer_match_registry::get_all_matches() const
{
    auto result = std::vector<std::shared_ptr<multiplayer_match>>{};

    result.reserve(matches.size());

    std::transform(std::cbegin(matches), 
                   std::cend(matches), 
                   std::back_inserter(result),
                   [] (util::value_ref<decltype(matches)::value_type> entry)
    {
        return entry.second.lock();  
    });

    return result;
}

void multiplayer_match_registry::throw_if_match_name_is_not_unique(
    util::value_ref<std::string> name) const
{
    auto const it = matches.find(name);
    if (it != std::cend(matches))
    {
        throw match_name_not_unique_exception{};
    }    
}

std::shared_ptr<multiplayer_match> multiplayer_match_registry::create_tracked_match(
    std::string name,
    int const board_size)
{
    auto logger = logger_factory.make_logger_for_game(name);

    auto m = std::shared_ptr<multiplayer_match>(
        new multiplayer_match{std::move(name), board_size, std::move(logger)},
        [this] (multiplayer_match* const match)
    {
        matches.erase(match->get_name());

        delete match;
    });

    return m;
}

} }