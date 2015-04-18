#include "stdafx.hpp"

#include "reversi/cell_position.hpp"
#include "reversi/game.hpp"
#include "reversi/game_board_renderer.hpp"
#include "reversi/player.hpp"
#include "util/sequence.hpp"
#include <cinder/ImageIo.h>

namespace reversi
{
    
game_board_renderer::game_board_renderer(game& g, float const board_size)
    : current_game{g}
    , board_display_size{board_size}
{
    load_pictures();
}

void game_board_renderer::draw_board(bool const game_over) const
{
    draw_game_board_background();     
    
    draw_game_board_grid();
    
    draw_game_board_frame();    

    draw_tokens();

    draw_currently_hovered_cell(game_over);    
}

boost::optional<cell_position> 
    game_board_renderer::get_currently_hovered_cell() const
{
    auto const mouse_position = cinder::app::App::getMousePos() - 
                                cinder::app::getWindowPos();

    auto const pos = get_cell_position_from_window_position(mouse_position);

    auto const game_size = current_game.get_board_size();

    if ((pos.row >= 0) && 
        (pos.col >= 0) && 
        (pos.row < game_size) && 
        (pos.col < game_size))
    {
        return pos;
    }
    else
    {
        return boost::none;
    }
}

cell_position game_board_renderer::get_cell_position_from_window_position(
    cinder::Vec2f const pos) const
{
    auto const board_origin = get_game_board_origin();

    auto const cell_size = get_cell_size();

    auto const relative_pos = (pos - board_origin);

    auto const cell_pos = (relative_pos / cell_size);

    return {cell_pos.y, cell_pos.x};
}

void game_board_renderer::load_pictures()
{
    load_game_board_picture();

    load_game_board_frame_picture();

    load_token_pictures();
}

void game_board_renderer::load_game_board_picture()
{
    auto const asset = cinder::app::loadAsset("game board.jpg");

    game_board_picture = cinder::loadImage(asset);      
}

void game_board_renderer::load_game_board_frame_picture()
{
    auto const asset = cinder::app::loadAsset("frame.png");

    game_board_frame_picture = cinder::loadImage(asset);      
}

void game_board_renderer::load_token_pictures()
{
    auto const wt_asset = cinder::app::loadAsset("white token.png");

    white_token_picture = cinder::loadImage(wt_asset);

    auto const bt_asset = cinder::app::loadAsset("black token.png");

    black_token_picture = cinder::loadImage(bt_asset);
}

void game_board_renderer::draw_game_board_background() const
{
    auto const top_left = get_game_board_origin();

    auto const bottom_right = top_left + cinder::Vec2f{board_display_size, 
                                                       board_display_size};
    
    cinder::gl::draw(game_board_picture, {top_left, bottom_right});    
}

void game_board_renderer::draw_game_board_grid() const
{
    cinder::gl::color(1.f, 1.f, 1.f);

    draw_game_board_grid_rows();

    draw_game_board_grid_columns();
}

void game_board_renderer::draw_game_board_grid_rows() const
{
    auto const cell_size = get_cell_size();

    auto const board_top_left = get_game_board_origin();

    auto const num_of_cells = current_game.get_board_size();

    for (auto const row : util::sequence(0, num_of_cells))
    {
        auto const cell_origin_y = board_top_left.y + row * cell_size;

        auto const from = cinder::Vec2f{board_top_left.x, cell_origin_y};

        auto const to = cinder::Vec2f{board_top_left.x + board_display_size, 
                                      cell_origin_y};

        cinder::gl::drawLine(from, to);
    }    
}

void game_board_renderer::draw_game_board_grid_columns() const
{
    auto const cell_size = get_cell_size();

    auto const board_top_left = get_game_board_origin();

    auto const num_of_cells = current_game.get_board_size();

    for (auto const col : util::sequence(0, num_of_cells))
    {
        auto const cell_origin_x = board_top_left.x + col * cell_size;

        auto const from = cinder::Vec2f{cell_origin_x, board_top_left.y};

        auto const to = cinder::Vec2f{cell_origin_x, 
                                      board_top_left.y + board_display_size};

        cinder::gl::drawLine(from, to);
    }    
}

void game_board_renderer::draw_game_board_frame() const
{
    auto const center = cinder::app::getWindowCenter();

    auto const padding = 15.f;

    auto const radius = cinder::Vec2f{board_display_size / 2.f + padding, 
                                      board_display_size / 2.f + padding};

    auto const top_left = center - radius;

    auto const bottom_right = center + radius;
    
    cinder::gl::draw(game_board_frame_picture, {top_left, bottom_right});    
}

void game_board_renderer::draw_tokens() const
{
    auto const num_of_cells = current_game.get_board_size();

    for (auto const row : util::sequence(0, num_of_cells))
    {
        for (auto const col : util::sequence(0, num_of_cells))
        {
            draw_cell_content({row, col});
        }
    }
}

void game_board_renderer::draw_cell_content(cell_position const pos) const
{
    auto const mark = current_game.get_board_cell_mark(pos);
    if (!mark)
    {
        return;
    }

    draw_player_token(pos.row, pos.col, *mark);
}

void game_board_renderer::draw_player_token(int const row, 
                                            int const col, 
                                            player const p) const
{
    auto const cell_size = get_cell_size();

    auto const cell_origin = get_cell_origin(row, col);

    auto const margin = cell_size * 0.1f;

    auto const top_left = cell_origin + cinder::Vec2f{margin, margin};

    auto const bottom_right = cell_origin + 
                              cinder::Vec2f{cell_size - margin, 
                                            cell_size - margin};

    draw_token_shape(p, {top_left, bottom_right});        
}

void game_board_renderer::draw_token_shape(player const p, 
                                           cinder::Rectf const bounds) const
{
    if (p == player::white)
    {
        cinder::gl::draw(white_token_picture, bounds);    
    }
    else
    {
        cinder::gl::draw(black_token_picture, bounds);    
    }    
}

void game_board_renderer::draw_currently_hovered_cell(
    bool const game_over) const
{
    if (game_over)
    {
        return;
    }

    auto const cell = get_currently_hovered_cell();
    if (cell)
    {
        cinder::gl::color(1.0, 0.f, 0.f, 1.f);

        auto const cell_bounds = get_cell_bounds(cell->row, cell->col);

        cinder::gl::drawStrokedRoundedRect(cell_bounds, 5.f);

        cinder::gl::color(1.0, 0.f, 0.f, 1.f);
    }    
}

cinder::Rectf game_board_renderer::get_cell_bounds(int const row, 
                                                   int const col) const
{
    auto const cell_size = get_cell_size();

    auto const top_left = get_cell_origin(row, col);

    auto const bottom_right = top_left + cinder::Vec2f{cell_size, cell_size};

    return {top_left, bottom_right};
}

cinder::Vec2f game_board_renderer::get_cell_origin(int const row, 
                                                   int const col) const
{
    auto const cell_size = get_cell_size();

    auto const board_top_left = get_game_board_origin();

    auto const cell_origin_x = board_top_left.x + col * cell_size;

    auto const cell_origin_y = board_top_left.y + row * cell_size;

    return {cell_origin_x, cell_origin_y};    
}

cinder::Vec2f game_board_renderer::get_game_board_origin() const
{
    auto const center = cinder::app::getWindowCenter();

    return (center - cinder::Vec2f{board_display_size / 2.f, 
                                   board_display_size / 2.f});
}

float game_board_renderer::get_cell_size() const
{
    return (board_display_size / current_game.get_board_size());
}

}