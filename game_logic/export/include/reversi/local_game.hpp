#pragma once

#include "reversi/game.hpp"
#include "reversi/game_board.hpp"
#include "reversi/game_score.hpp"
#include "util/signal_traits.hpp"

namespace reversi
{

enum class direction;

struct game_score;
    
class game_logger;

class no_reversal_triggered_exception : public virtual std::exception
{    
};

class cell_busy_exception : public virtual std::exception
{    
};

class local_game : public game
{

public:

    local_game(int board_size, game_logger& logger);

    virtual int get_board_size() const override;

    virtual placement_outcome place(cell_position pos) override;
    
    virtual boost::signals2::connection register_placement_event_handler(
        placement_event_handler h) override;

    boost::optional<player> get_board_cell_mark(cell_position pos) const;

    game_score get_score() const;

    player get_next_moving_player() const;

private:

    void place_initial_marks();

    void throw_if_cell_is_occupied(cell_position pos) const;
    
    std::vector<cell_position> apply_reversals_or_throw_if_none_is_triggered(cell_position pos);

    std::vector<cell_position> get_reversals_for_placement(cell_position pos, player p) const;

    std::vector<cell_position> get_reversals_for_placement_in_direction(cell_position pos,
                                                                        player p,
                                                                        direction d) const;
    
    bool is_cell_occupied_by_opponent_of_player(cell_position pos, player p) const;

    bool is_cell_occupied_by_player(cell_position pos, player p) const;

    void update_score_after_reversals(util::value_ref<std::vector<cell_position>> reversals);

    void mark_placement_cell_and_update_score(cell_position pos);

    bool can_player_move(player p) const;

    placement_outcome update_game_state();

    placement_outcome determine_last_placement_outcome() const;

    void log_placement_outcome(placement_outcome p) const;

private:

    using placement_event = util::signal_type_t<placement_event_handler>;

private:

    game_board board;

    game_logger& logger;

    player next_moving_player;

    game_score score;

    placement_event on_placement;

};

}