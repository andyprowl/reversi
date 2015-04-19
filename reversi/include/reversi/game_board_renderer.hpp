#pragma once

#include <cinder/gl/Texture.h>

namespace reversi
{

struct cell_position;

class game;
    
class game_board_renderer
{
    
public:

    game_board_renderer(game& g, float board_size);

    void draw_board() const;

    boost::optional<cell_position> get_currently_hovered_cell() const;

    cell_position get_cell_position_from_window_position(
        cinder::Vec2f pos) const;

    void show_next_move_token(bool show);

private:

    void load_pictures();

    void load_game_board_picture();

    void load_game_board_frame_picture();

    void load_token_pictures();

    void draw_game_board_background() const;

    void draw_game_board_grid() const;

    void draw_game_board_grid_rows() const;

    void draw_game_board_grid_columns() const;

    void draw_game_board_frame() const;

    void draw_tokens() const;

    void draw_cell_content(cell_position pos) const;

    void draw_empty_cell(cell_position pos) const;

    void draw_player_token(int row, int col, player p) const;

    void draw_next_move_token() const;

    void draw_token_shape(player p, cinder::Rectf bounds) const;

    void draw_currently_hovered_cell() const;

    cinder::Rectf get_cell_bounds(int row, int col) const;

    cinder::Vec2f get_cell_origin(int row, int col) const;

    cinder::Vec2f get_game_board_origin() const;

    float get_cell_size() const;

private:

    game& current_game;

    cinder::gl::Texture game_board_picture;

    cinder::gl::Texture game_board_frame_picture;

    cinder::gl::Texture black_token_picture;

    cinder::gl::Texture white_token_picture;

    float board_display_size;

    bool display_next_move_token;

};

}