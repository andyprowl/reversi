#pragma once

#include <boost/optional.hpp>
#include <stdexcept>
#include <vector>

namespace reversi
{

enum class player;

struct cell_position;

class bad_cell_coordinates_exception : public virtual std::exception
{
};

class game_board
{

public:

    using cell_mark_change_event_handler = std::function<void(cell_position, player)>;

public:

    game_board(int size);

    int get_size() const;

    void mark_cell(cell_position pos, player p);

    bool is_valid_cell_position(cell_position pos) const;

    bool is_cell_occupied(cell_position const pos) const;

    boost::optional<player> get_cell_mark(cell_position pos) const;

private:

    void throw_if_cell_position_is_not_valid(cell_position pos) const;

private:

    int size;

    std::vector<boost::optional<player>> board;

};

}