#pragma once

#include <cinder/gl/Texture.h>
#include <cinder/Text.h>

namespace reversi
{

struct game_score;

class game;
    
class player_info_renderer
{
    
public:

    player_info_renderer(game& g, float board_display_size);

    void draw_player_info(bool game_over, player next_mover, game_score score);

private:

    void create_fonts();

    void load_winner_picture();

    void draw_black_player_info(int black_score) const;

    void draw_black_player_name() const;

    void draw_black_player_score(int black_score) const;

    void draw_white_player_info(int white_score) const;

    void draw_white_player_name() const;

    void draw_white_player_score(int white_score) const;

    void draw_turn_indicator(bool game_over, player next_mover) const;

    void draw_black_player_turn_indicator() const;

    void draw_white_player_turn_indicator() const;

    void draw_winner_indicator(bool game_over, game_score score) const;

    void draw_black_player_winner_indicator() const;

    void draw_white_player_winner_indicator() const;

private:

    game& current_game;

    float board_display_size;

    cinder::Font player_name_font;

    cinder::Font score_font;

    cinder::gl::Texture winner_picture;

};

}