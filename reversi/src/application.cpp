#include "stdafx.hpp"

#include "reversi/application.hpp"
#include "reversi/cell_position.hpp"
#include "reversi/file_game_logger.hpp"
#include "reversi/local_game.hpp"
#include "reversi/placement_outcome.hpp"
#include "reversi/player.hpp"
#include <cinder/ImageIo.h>

namespace reversi
{
  
application::application()
    : logger{"reversi.log"}
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

    start_new_game(6);
}   

void application::update()
{   
    if (!placement_target)
    {
        return;
    }

    try
    {
        current_game->place(*placement_target);
    }
    catch (std::exception const&)
    {        
        auto error_message = "This is not a valid move :-( Please try again...";

        set_shown_hint_message(std::move(error_message));
    }

    placement_target = boost::none;
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
            this->board_renderer->show_next_move_token(e.isControlDown());

            return;
        }
    }
}

void application::keyUp(cinder::app::KeyEvent const e)
{
    this->board_renderer->show_next_move_token(e.isControlDown());
}

void application::mouseDown(cinder::app::MouseEvent const e)
{
    auto const game_over = current_game->is_over();
    if (game_over)
    {
        return;
    }

    placement_target = board_renderer->get_currently_hovered_cell();
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

void application::load_game_over_picture()
{
    auto const asset = cinder::app::loadAsset("game over.png");

    game_over_picture = cinder::loadImage(asset);      
}

void application::start_new_game(int const board_size)
{
    current_game = create_new_game(board_size);

    create_renderers_for_game(*current_game);

    register_for_placement_notifications_from_current_game();    
}

std::unique_ptr<game> application::create_new_game(int board_size) const
{
    return std::make_unique<local_game>(board_size, 
                                        "Player 1", 
                                        "Player 2", 
                                        logger);
}

void application::create_renderers_for_game(game& g)
{
    board_renderer = 
        std::make_unique<game_board_renderer>(g, board_display_size);

    player_renderer = 
        std::make_unique<player_info_renderer>(g, board_display_size);
    
    hint_renderer = std::make_unique<hint_message_renderer>(board_display_size);
}

void application::draw_frame() const
{
    draw_background();

    draw_title();

    draw_game_board();

    draw_player_info();

    draw_game_over_label();

    draw_hint_message();
}

void application::draw_background() const
{
    auto const bounds = getWindowBounds();

    if (current_game->is_over())
    {
        cinder::gl::color(1.0, 1.0, 1.0, 0.5f);
    }
    else
    {
        cinder::gl::color(1.0, 1.0, 1.0, 1.0f);
    }

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
    board_renderer->draw_board();
}

void application::draw_player_info() const
{
    auto const next_mover = current_game->get_next_moving_player();

    auto const score = current_game->get_score();

    auto const game_over = current_game->is_over();

    player_renderer->draw_player_info(game_over, 
                                      next_mover, 
                                      score);
}

void application::draw_hint_message() const
{
    auto const game_over = current_game->is_over();

    hint_renderer->draw_hint(game_over);
}

void application::draw_game_over_label() const
{
    auto const game_over = current_game->is_over();
    if (!game_over)
    {
        return;
    }

    auto const center = getWindowCenter();

    auto const label_center = 
        center + cinder::Vec2f{0.f, center.y * 0.75f + 20.f};
    
    auto const top_left = 
        label_center - cinder::Vec2f{game_over_picture.getSize()} / 2.f;

    auto const bottom_right = label_center + game_over_picture.getSize() / 2;

    cinder::gl::draw(game_over_picture, {top_left, bottom_right});    
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
    cell_position const pos, 
    player const p, 
    placement_outcome const outcome, 
    util::value_ref<std::vector<cell_position>> reversals)
{
    process_placement_outcome(outcome);
}

void application::process_placement_outcome(placement_outcome const outcome)
{
    if (outcome == placement_outcome::turn_skipped)
    {
        show_turn_skipped_message();
    }
    else if (outcome == placement_outcome::game_over)
    {
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
    auto const next_mover = current_game->get_next_moving_player();

    auto const next_mover_name = current_game->get_player_name(next_mover);

    auto skipped_player_name = 
        current_game->get_player_name(get_opponent_of(next_mover));

    auto msg = std::move(skipped_player_name) + 
               " cannot move: " + 
               std::move(next_mover_name) + 
               " shall move again!";

    set_shown_hint_message(std::move(msg));    
}

void application::set_shown_hint_message(std::string message) const
{
    hint_renderer->set_hint(std::move(message));
}

std::string application::get_game_result_description() const
{
    auto const winner = get_winning_player(*current_game);
    if (!winner)
    {
        return "Nobody wins.";
    }

    return current_game->get_player_name(*winner) + " wins!";
}

}

CINDER_APP_NATIVE(reversi::application, ci::app::RendererGl)