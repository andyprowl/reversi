#include "stdafx.hpp"

#include "reversi/game.hpp"
#include "reversi/game_score.hpp"
#include "reversi/player.hpp"
#include "reversi/player_info_renderer.hpp"
#include <cinder/ImageIo.h>

namespace reversi
{
    
player_info_renderer::player_info_renderer(game& g, 
                                           float const board_display_size)
    : current_game{g}
    , board_display_size{board_display_size}
{    
    create_fonts();

    load_winner_picture();
}

void player_info_renderer::draw_player_info(bool const game_over,
                                            player const next_mover,
                                            game_score const score)
{
    draw_white_player_info(score.white);

    draw_black_player_info(score.black);

    draw_turn_indicator(game_over, next_mover);

    draw_winner_indicator(game_over, score);
}

void player_info_renderer::create_fonts()
{
    player_name_font = cinder::Font{"Arial", 50.f};

    score_font = cinder::Font{"Arial", 150.f};    
}

void player_info_renderer::load_winner_picture()
{
    auto const asset = cinder::app::loadAsset("winner.png");

    winner_picture = cinder::loadImage(asset);      
}

void player_info_renderer::draw_white_player_info(int const white_score) const
{
    draw_white_player_name();

    draw_white_player_score(white_score);
}

void player_info_renderer::draw_white_player_name() const
{
    auto const name = current_game.get_player_name(player::white);

    auto const margin = 10.f;

    auto const left = margin;

    auto const window_center = cinder::app::getWindowCenter();

    auto const right = window_center.x - board_display_size / 2.f - margin;

    auto const y_center = window_center.y - board_display_size / 4.f;

    auto const center = cinder::Vec2f{(right + left) / 2.f, y_center};

    auto const white_color = cinder::ColorA{1.f, 1.f, 1.f, 1.f};

    cinder::gl::drawStringCentered(name, center, white_color, player_name_font);
}

void player_info_renderer::draw_white_player_score(int const white_score) const
{
    auto const score = std::to_string(white_score);

    auto const margin = 10.f;

    auto const left = margin;

    auto const window_center = cinder::app::getWindowCenter();

    auto const right = window_center.x - board_display_size / 2.f - margin;

    auto const y_center = window_center.y;

    auto const center = cinder::Vec2f{(right + left) / 2.f, y_center};

    auto const white_color = cinder::ColorA{1.f, 1.f, 1.f, 1.f};

    cinder::gl::drawStringCentered(score, center, white_color, score_font);    
}

void player_info_renderer::draw_black_player_info(int const black_score) const
{
    draw_black_player_name();

    draw_black_player_score(black_score);
}

void player_info_renderer::draw_black_player_name() const
{
    auto const name = current_game.get_player_name(player::black);

    auto const margin = 10.f;

    auto const window_center = cinder::app::getWindowCenter();

    auto const left = window_center.x + board_display_size / 2.f + margin;

    auto const right = cinder::app::getWindowSize().x - margin;

    auto const y_center = window_center.y - board_display_size / 4.f;

    auto const center = cinder::Vec2f{(right + left) / 2.f, y_center};

    auto const black_color = cinder::ColorA{0.f, 0.f, 0.f, 1.f};

    cinder::gl::drawStringCentered(name, center, black_color, player_name_font);    
}

void player_info_renderer::draw_black_player_score(int const black_score) const
{
    auto const score = std::to_string(black_score);

    auto const margin = 10.f;

    auto const window_center = cinder::app::getWindowCenter();

    auto const left = window_center.x + board_display_size / 2.f + margin;

    auto const right = cinder::app::getWindowSize().x - margin;

    auto const y_center = window_center.y;

    auto const center = cinder::Vec2f{(right + left) / 2.f, y_center};

    auto const black_color = cinder::ColorA{0.f, 0.f, 0.f, 1.f};

    cinder::gl::drawStringCentered(score, center, black_color, score_font);    
}

void player_info_renderer::draw_turn_indicator(bool const game_over,
                                               player const next_mover) const
{
    if (game_over)
    {
        return;
    }

    if (next_mover == player::white)
    {
        draw_white_player_turn_indicator();
    }
    else
    {
        draw_black_player_turn_indicator();
    }
}

void player_info_renderer::draw_white_player_turn_indicator() const
{
    auto const margin = 10.f;

    auto const center = cinder::app::getWindowCenter();
        
    auto const right = center.x - board_display_size / 2.f - margin - 20.f;

    auto const y_center = center.y - board_display_size / 4.f;

    auto const top_left = cinder::Vec2f{margin, y_center - 5.f};

    auto const bottom_right = cinder::Vec2f{right + 5.f, y_center + 45.f};

    cinder::gl::color(cinder::ColorA{1.f, 1.f, 0.f, 1.f});

    cinder::gl::drawStrokedRoundedRect({top_left, bottom_right}, 5.0);

    cinder::gl::color(cinder::ColorA{1.f, 1.f, 1.f, 1.f});    
}

void player_info_renderer::draw_black_player_turn_indicator() const
{
    auto const margin = 10.f;
        
    auto const right = cinder::app::getWindowSize().x - margin;

    auto const window_center = cinder::app::getWindowCenter();

    auto const y_center = window_center.y - board_display_size / 4.f;

    auto const top_left = cinder::Vec2f{
        window_center.x + board_display_size / 2.f + margin + 15.f, 
        y_center - 5.f};

    auto const bottom_right = cinder::Vec2f{right, y_center + 45.f};

    cinder::gl::color(cinder::ColorA{1.f, 1.f, 0.f, 1.f});

    cinder::gl::drawStrokedRoundedRect({top_left, bottom_right}, 5.0);

    cinder::gl::color(cinder::ColorA{1.f, 1.f, 1.f, 1.f});     
}

void player_info_renderer::draw_winner_indicator(bool const game_over,
                                                 game_score const score) const
{
    if (!game_over || (score.white == score.black))
    {
        return; 
    }

    if (score.white > score.black)
    {
        draw_white_player_winner_indicator();
    }
    else
    {        
        draw_black_player_winner_indicator();
    }
}

void player_info_renderer::draw_white_player_winner_indicator() const
{
    auto const window_center = cinder::app::getWindowCenter();

    auto const x_center = (window_center.x - board_display_size / 2) / 2;

    auto const y_center = window_center.y - board_display_size / 2.f + 10.f;

    auto const center = cinder::Vec2d{x_center - 5.f, y_center};

    auto const top_left = center - 
                          cinder::Vec2f{winner_picture.getSize()} / 4.5;

    auto const bottom_right = center + 
                              cinder::Vec2f{winner_picture.getSize()} / 4.5;

    cinder::gl::draw(winner_picture, {top_left, bottom_right});        
}

void player_info_renderer::draw_black_player_winner_indicator() const
{
    auto const window_center = cinder::app::getWindowCenter();

    auto const window_size = cinder::app::getWindowSize();

    auto const x_center = 
        (window_size.x + (window_center.y + board_display_size)) / 2;

    auto const y_center = window_center.y - board_display_size / 2.f + 10.f;

    auto const center = cinder::Vec2d{x_center - 25.f, y_center};

    auto const top_left = center - 
                          cinder::Vec2f{winner_picture.getSize()} / 4.5;

    auto const bottom_right = center + 
                              cinder::Vec2f{winner_picture.getSize()} / 4.5;

    cinder::gl::draw(winner_picture, {top_left, bottom_right});    
}

}