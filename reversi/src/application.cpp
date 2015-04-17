#include "stdafx.hpp"

#include "reversi/application.hpp"
#include "reversi/cell_position.hpp"
#include "reversi/local_game.hpp"
#include "reversi/file_game_logger.hpp"
#include "reversi/placement_outcome.hpp"
#include "reversi/player.hpp"
#include "util/sequence.hpp"
#include <cinder/ImageIo.h>

namespace reversi
{
  
application::application()
    : logger{"log.txt"}
{    
}

void application::prepareSettings(Settings* const settings)
{
    settings->setWindowSize(1024, 768);

    settings->setResizable(false);

    settings->setFrameRate(30);

    settings->setTitle("Reversi");
}

void application::setup()
{
    disable_maximize_button();

    setup_asset_directories();

    load_pictures();

    create_fonts();

    start_new_game(6);
}   

void application::update()
{    
}   

void application::draw()
{
    cinder::gl::clear({0.f, 0.f, 0.0f}, true);

    cinder::gl::enableAlphaBlending();

    cinder::gl::setMatricesWindow(cinder::app::getWindowSize());

    draw_frame();

    cinder::gl::disableAlphaBlending();    
}

void application::keyDown(cinder::app::KeyEvent const e)
{
    using cinder::app::KeyEvent;

    switch (e.getCode())
    {
        case KeyEvent::KEY_F4:
        case KeyEvent::KEY_F6:
        case KeyEvent::KEY_F8:
        case KeyEvent::KEY_F10:
        case KeyEvent::KEY_F12:
        {
            auto const new_size = 4 + (e.getCode() - KeyEvent::KEY_F4);

            start_new_game(new_size);

            return;
        }

        default:
        {
            return;
        }
    }
}

void application::mouseMove(cinder::app::MouseEvent const e)
{
    
}

void application::mouseDown(cinder::app::MouseEvent const e)
{
    if (game_over)
    {
        return;
    }

    try
    {
        auto const pos = board_renderer->get_currently_hovered_cell();
        if (pos)
        {
            current_game->place(*pos);
        }
    }
    catch (std::exception const&)
    {        
        set_shown_hint_message(
            "This is not a valid move :-( Please try again...");
    }
}

void application::mouseDrag(cinder::app::MouseEvent const e)
{
    
}

void application::mouseWheel(cinder::app::MouseEvent const e)
{
    
}

void application::resize()
{
    
}

void application::disable_maximize_button() const
{
    // Could not find a portable way of doing this...

    auto const windowHandle = ::GetActiveWindow();
    
    auto const style = ::GetWindowLong(windowHandle, GWL_STYLE);
   
    ::SetWindowLong(windowHandle, GWL_STYLE, style & ~WS_MAXIMIZEBOX);    
}

void application::setup_asset_directories() const
{
    auto const current_path = boost::filesystem::current_path();

    cinder::app::addAssetDirectory(current_path);

    cinder::app::addAssetDirectory(current_path / "assets");    
}

void application::load_pictures()
{
    load_background_picture();

    load_title_picture();

    load_winner_picture();

    load_game_over_picture();
}

void application::load_background_picture()
{
    auto const asset = cinder::app::loadAsset("background.jpg");

    background_picture = cinder::loadImage(asset);         
}

void application::load_title_picture()
{
    auto const asset = cinder::app::loadAsset("title.png");

    title_picture = cinder::loadImage(asset);         
}

void application::load_winner_picture()
{
    auto const asset = cinder::app::loadAsset("winner.png");

    winner_picture = cinder::loadImage(asset);      
}

void application::load_game_over_picture()
{
    auto const asset = cinder::app::loadAsset("game over.png");

    game_over_picture = cinder::loadImage(asset);      
}

void application::create_fonts()
{
    message_font = cinder::Font{"Arial", 30.f};
}

void application::start_new_game(int const board_size)
{
    current_game = std::make_unique<local_game>(board_size, 
                                                "Player 1", 
                                                "Player 2", 
                                                logger);

    board_renderer = std::make_unique<game_board_renderer>(*current_game, 
                                                           board_display_size);

    player_renderer = std::make_unique<player_info_renderer>(
        *current_game, 
        board_display_size);

    game_over = false;

    white_score = 2;

    black_score = 2;

    next_mover = player::black;

    register_for_placement_notifications_from_current_game();    
}

void application::draw_frame() const
{
    draw_background();

    draw_title();

    draw_game_board();

    draw_player_info();

    draw_winner_indicator();

    draw_game_over_label();

    draw_hint_message();
}

void application::draw_background() const
{
    auto const bounds = getWindowBounds();

    cinder::gl::draw(background_picture, bounds);
}

void application::draw_title() const
{
    auto const center = cinder::Vec2f{getWindowCenter().x, 
                                      title_picture.getSize().y / 2 + 5.f};

    auto const top_left = center - 
                          cinder::Vec2f{title_picture.getSize()} / 2.8f;

    auto const bottom_right = center + 
                              cinder::Vec2f{title_picture.getSize()} / 2.8f;

    cinder::gl::draw(title_picture, {top_left, bottom_right});    
}

void application::draw_game_board() const
{
    board_renderer->draw_board(game_over);
}

void application::draw_player_info() const
{
    player_renderer->draw_player_info(game_over, 
                                      next_mover, 
                                      white_score, 
                                      black_score);
}

void application::draw_hint_message() const
{
    auto const center = getWindowCenter();

    auto const pos = cinder::Vec2f{center.x, 
                                   center.y + board_display_size / 2.f + 25.f};

    auto const duration = cinder::app::getElapsedSeconds() - message_seconds;

    auto const transparency = ((duration < 2.0) || game_over) ? 1.f : 0.f;

    cinder::gl::drawStringCentered(hint_message, 
                                   pos, 
                                   {1.0, 1.0, 0.0, transparency}, 
                                   message_font);
}

void application::draw_winner_indicator() const
{
    if (!game_over || (white_score == black_score))
    {
        return; 
    }

    if (white_score > black_score)
    {
        draw_white_player_winner_indicator();
    }
    else
    {        
        draw_black_player_winner_indicator();
    }
}

void application::draw_white_player_winner_indicator() const
{
    auto const x_center = (getWindowCenter().x - board_display_size / 2) / 2;

    auto const y_center = getWindowCenter().y - board_display_size / 2.f + 10.f;

    auto const center = cinder::Vec2d{x_center - 5.f, y_center};

    auto const top_left = center - 
                          cinder::Vec2f{winner_picture.getSize()} / 4.5;

    auto const bottom_right = center + 
                              cinder::Vec2f{winner_picture.getSize()} / 4.5;

    cinder::gl::draw(winner_picture, {top_left, bottom_right});        
}

void application::draw_black_player_winner_indicator() const
{
    auto const window_center = getWindowCenter();

    auto const window_size = getWindowSize();

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

void application::draw_game_over_label() const
{
    if (!game_over)
    {
        return;
    }

    auto const center = getWindowCenter();

    auto const label_center = center + 
                              cinder::Vec2f{0.f, center.y * 0.75f + 20.f};
    
    auto const top_left = label_center - 
                          cinder::Vec2f{game_over_picture.getSize()} / 2.f;

    auto const bottom_right = label_center + game_over_picture.getSize() / 2;

    cinder::gl::draw(game_over_picture, {top_left, bottom_right});    
}

void application::set_shown_hint_message(std::string msg)
{
    message_seconds = cinder::app::getElapsedSeconds();

    hint_message = std::move(msg);
}

void application::register_for_placement_notifications_from_current_game()
{
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::placeholders::_4;

    auto handler = std::bind(&application::on_placement, this, _1, _2, _3, _4);

    current_game->register_placement_event_handler(std::move(handler));
}

void application::on_placement(
    cell_position pos, 
    player p, 
    placement_outcome outcome, 
    util::value_ref<std::vector<cell_position>> reversals)
{
    auto const scorer = (outcome == placement_outcome::turn_switched) ? 
        get_opponent_of(p) : 
        p;

    update_score(scorer, static_cast<int>(reversals.size()));

    next_mover = p;

    process_placement_outcome(outcome);
}

void application::update_score(player const scorer, int const num_of_reversals)
{
    if (scorer == player::white)
    {
        white_score += static_cast<int>(num_of_reversals + 1);
        black_score -= static_cast<int>(num_of_reversals);
    }
    else
    {
        black_score += static_cast<int>(num_of_reversals + 1);
        white_score -= static_cast<int>(num_of_reversals);
    }    
}

void application::process_placement_outcome(placement_outcome const outcome)
{
    if (outcome == placement_outcome::turn_skipped)
    {
        show_turn_skipped_message();
    }
    else if (outcome == placement_outcome::game_over)
    {
        game_over = true;

        set_shown_hint_message(
            get_game_result_description() + 
            " Press F4, F6, F8, F10, or F12 "
            "to start a new game of the corresponding size...");
    }
    else
    {
        set_shown_hint_message("");
    }    
}

void application::show_turn_skipped_message()
{
    auto next_mover_name = current_game->get_player_name(next_mover);

    auto skipped_player_name = 
        current_game->get_player_name(get_opponent_of(next_mover));

    auto msg = std::move(skipped_player_name) + 
               " cannot move: " + 
               std::move(next_mover_name) + 
               " shall move again!";

    set_shown_hint_message(std::move(msg));    
}

std::string application::get_game_result_description() const
{
    if (white_score == black_score)
    {
        return "Nobody wins.";
    }
    else if (white_score > black_score)
    {
        return current_game->get_player_name(player::white) + " wins!";
    }
    else
    {
        return current_game->get_player_name(player::black) + " wins!";
    }
}

}

CINDER_APP_NATIVE(reversi::application, ci::app::RendererGl)
