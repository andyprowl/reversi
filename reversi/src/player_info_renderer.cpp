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

void player_info_renderer::draw_player_info()
{
    auto const score = current_game.get_score();

    draw_black_player_info(score.black);

    draw_white_player_info(score.white);

    draw_turn_indicator();

    draw_winner_indicator();
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

void player_info_renderer::draw_black_player_info(int const black_score) const
{
    draw_black_player_name();

    draw_black_player_score(black_score);
}

void player_info_renderer::draw_black_player_name() const
{
    auto const name = current_game.get_player_name(player::black);

    auto const margin = 10.f;

    auto const left = margin;

    auto const window_center = cinder::app::getWindowCenter();

    auto const right = window_center.x - board_display_size / 2.f - margin;

    auto const y_center = window_center.y - board_display_size / 4.f;

    auto const center = cinder::Vec2f{(right + left) / 2.f, y_center};

    auto const black_color = get_black_player_color();

    cinder::gl::drawStringCentered(name, center, black_color, player_name_font);
}

void player_info_renderer::draw_black_player_score(int const black_score) const
{
    auto const score = std::to_string(black_score);

    auto const margin = 10.f;

    auto const left = margin;

    auto const window_center = cinder::app::getWindowCenter();

    auto const right = window_center.x - board_display_size / 2.f - margin;

    auto const y_center = window_center.y;

    auto const center = cinder::Vec2f{(right + left) / 2.f, y_center};

    auto const black_color = get_black_player_color();

    cinder::gl::drawStringCentered(score, center, black_color, score_font);    
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

    auto const window_center = cinder::app::getWindowCenter();

    auto const left = window_center.x + board_display_size / 2.f + margin;

    auto const right = cinder::app::getWindowSize().x - margin;

    auto const y_center = window_center.y - board_display_size / 4.f;

    auto const center = cinder::Vec2f{(right + left) / 2.f, y_center};

    auto const white_color = get_white_player_color();

    cinder::gl::drawStringCentered(name, center, white_color, player_name_font);    
}

void player_info_renderer::draw_white_player_score(int const white_score) const
{
    auto const score = std::to_string(white_score);

    auto const margin = 10.f;

    auto const window_center = cinder::app::getWindowCenter();

    auto const left = window_center.x + board_display_size / 2.f + margin;

    auto const right = cinder::app::getWindowSize().x - margin;

    auto const y_center = window_center.y;

    auto const center = cinder::Vec2f{(right + left) / 2.f, y_center};

    auto const white_color = get_white_player_color();

    cinder::gl::drawStringCentered(score, center, white_color, score_font);    
}

void player_info_renderer::draw_turn_indicator() const
{
    if (current_game.is_over()) { return; }

    glLineWidth(3.0);

    if (current_game.get_next_moving_player() == player::black)
    {
        draw_black_player_turn_indicator();
    }
    else
    {
        draw_white_player_turn_indicator();
    }

    glLineWidth(1.0);
}

void player_info_renderer::draw_black_player_turn_indicator() const
{
    auto const margin = 10.f;

    auto const center = cinder::app::getWindowCenter();
        
    auto const right = center.x - board_display_size / 2.f - margin - 20.f;

    auto const y_center = center.y - board_display_size / 4.f;

    auto const top_left = cinder::Vec2f{margin, y_center - 5.f};

    auto const bottom_right = cinder::Vec2f{right + 5.f, y_center + 45.f};

    cinder::gl::color(1.f, 1.f, 0.f, 1.f);

    cinder::gl::drawStrokedRoundedRect({top_left, bottom_right}, 5.0);

    cinder::gl::color(1.f, 1.f, 1.f, 1.f);    
}

void player_info_renderer::draw_white_player_turn_indicator() const
{
    auto const margin = 10.f;
        
    auto const right = cinder::app::getWindowSize().x - margin;

    auto const window_center = cinder::app::getWindowCenter();

    auto const y_center = window_center.y - board_display_size / 4.f;

    auto const top_left = cinder::Vec2f{
        window_center.x + board_display_size / 2.f + margin + 15.f, 
        y_center - 5.f};

    auto const bottom_right = cinder::Vec2f{right, y_center + 45.f};

    cinder::gl::color(1.f, 1.f, 0.f, 1.f);

    cinder::gl::drawStrokedRoundedRect({top_left, bottom_right}, 5.0);

    cinder::gl::color(1.f, 1.f, 1.f, 1.f);     
}

void player_info_renderer::draw_winner_indicator() const
{
    auto const winner = get_winning_player(current_game);
    if (!current_game.is_over() || !winner)
    {
        return; 
    }

    if (winner == player::black)
    {
        draw_black_player_winner_indicator();
    }
    else
    {        
        draw_white_player_winner_indicator();
    }
}

void player_info_renderer::draw_black_player_winner_indicator() const
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

void player_info_renderer::draw_white_player_winner_indicator() const
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

cinder::ColorA player_info_renderer::get_black_player_color() const
{
    if (!current_game.is_over())
    {
        return cinder::ColorA::black();        
    }

    if (get_winning_player(current_game) != player::black)
    {
        return {0.f, 0.f, 0.f, 0.75f};
    }

    if (static_cast<int>(cinder::app::getElapsedSeconds()) % 2)
    {
        return cinder::ColorA::black();
    }
    else
    {
        return {1.f, 1.f, 0.f, 1.f};
    }
}

cinder::ColorA player_info_renderer::get_white_player_color() const
{
    if (!current_game.is_over())
    {
        return cinder::ColorA::white();        
    }

    if (get_winning_player(current_game) != player::white)
    {
        return {1.f, 1.f, 1.f, 0.25f};
    }

    if (static_cast<int>(cinder::app::getElapsedSeconds()) % 2)
    {
        return cinder::ColorA::white();
    }
    else
    {
        return {1.f, 1.f, 0.f, 1.f};
    }
}

}