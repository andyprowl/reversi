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
    switch (e.getCode())
    {
        case cinder::app::KeyEvent::KEY_F4:
        case cinder::app::KeyEvent::KEY_F6:
        case cinder::app::KeyEvent::KEY_F8:
        case cinder::app::KeyEvent::KEY_F10:
        case cinder::app::KeyEvent::KEY_F12:
        {
            auto const new_size = 4 + (e.getCode() - cinder::app::KeyEvent::KEY_F4);

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
        auto const pos = get_cell_position_from_window_position(e.getPos());

        current_game->place(pos);
    }
    catch (std::exception const&)
    {        
        set_shown_hint_message("This is not a valid move :-( Please try again...");
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
    cinder::app::addAssetDirectory(boost::filesystem::current_path());

    cinder::app::addAssetDirectory(boost::filesystem::current_path() / "assets");    
}

void application::load_pictures()
{
    load_background_picture();

    load_title_picture();

    load_game_board_picture();

    load_game_board_frame_picture();

    load_token_pictures();    

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

void application::load_game_board_picture()
{
    auto const asset = cinder::app::loadAsset("game board.jpg");

    game_board_picture = cinder::loadImage(asset);      
}

void application::load_game_board_frame_picture()
{
    auto const asset = cinder::app::loadAsset("frame.png");

    game_board_frame_picture = cinder::loadImage(asset);      
}

void application::load_token_pictures()
{
    auto const wt_asset = cinder::app::loadAsset("white token.png");

    white_token_picture = cinder::loadImage(wt_asset);

    auto const bt_asset = cinder::app::loadAsset("black token.png");

    black_token_picture = cinder::loadImage(bt_asset);
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
    player_name_font = cinder::Font{"Arial", 50.f};

    score_font = cinder::Font{"Arial", 150.f};

    message_font = cinder::Font{"Arial", 30.f};
}

void application::start_new_game(int const board_size)
{
    current_game = std::make_unique<local_game>(board_size, "Player 1", "Player 2", logger);

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

    draw_white_player_info();

    draw_black_player_info();

    draw_turn_indicator();

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
    auto const center = cinder::Vec2f{getWindowCenter().x, title_picture.getSize().y / 2 + 5.f};

    auto const top_left = center - cinder::Vec2f{title_picture.getSize()} / 2.8f;

    auto const bottom_right = center + cinder::Vec2f{title_picture.getSize()} / 2.8f;

    cinder::gl::draw(title_picture, {top_left, bottom_right});    
}

void application::draw_white_player_info() const
{
    draw_white_player_name();

    draw_white_player_score();
}

void application::draw_white_player_name() const
{
    auto const name = current_game->get_player_name(player::white);

    auto const margin = 10.f;

    auto const left = margin;

    auto const right = getWindowCenter().x - board_size / 2.f - margin;

    auto const y_center = getWindowCenter().y - board_size / 4.f;

    auto const center = cinder::Vec2f{(right + left) / 2.f, y_center};

    cinder::gl::drawStringCentered(name, center, {1.f, 1.f, 1.f, 1.f}, player_name_font);
}

void application::draw_white_player_score() const
{
    auto const score = std::to_string(white_score);

    auto const margin = 10.f;

    auto const left = margin;

    auto const right = getWindowCenter().x - board_size / 2.f - margin;

    auto const y_center = getWindowCenter().y;

    auto const center = cinder::Vec2f{(right + left) / 2.f, y_center};

    cinder::gl::drawStringCentered(score, center, {1.f, 1.f, 1.f, 1.f}, score_font);    
}

void application::draw_black_player_info() const
{
    draw_black_player_name();

    draw_black_player_score();
}

void application::draw_black_player_name() const
{
    auto const name = current_game->get_player_name(player::black);

    auto const margin = 10.f;

    auto const left = getWindowCenter().x + board_size / 2.f + margin;

    auto const right = getWindowSize().x - margin;

    auto const y_center = getWindowCenter().y - board_size / 4.f;

    auto const center = cinder::Vec2f{(right + left) / 2.f, y_center};

    cinder::gl::drawStringCentered(name, center, {0.f, 0.f, 0.f, 1.f}, player_name_font);    
}

void application::draw_black_player_score() const
{
    auto const score = std::to_string(black_score);

    auto const margin = 10.f;

    auto const left = getWindowCenter().x + board_size / 2.f + margin;

    auto const right = getWindowSize().x - margin;

    auto const y_center = getWindowCenter().y;

    auto const center = cinder::Vec2f{(right + left) / 2.f, y_center};

    cinder::gl::drawStringCentered(score, center, {0.f, 0.f, 0.f, 1.f}, score_font);    
}

void application::draw_turn_indicator() const
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

void application::draw_white_player_turn_indicator() const
{
    auto const margin = 10.f;
        
    auto const right = getWindowCenter().x - board_size / 2.f - margin - 20.f;

    auto const y_center = getWindowCenter().y - board_size / 4.f;

    auto const top_left = cinder::Vec2f{margin, y_center - 5.f};

    auto const bottom_right = cinder::Vec2f{right + 5.f, y_center + 45.f};

    cinder::gl::color(cinder::ColorA{1.f, 1.f, 0.f, 1.f});

    cinder::gl::drawStrokedRoundedRect({top_left, bottom_right}, 5.0);

    cinder::gl::color(cinder::ColorA{1.f, 1.f, 1.f, 1.f});    
}

void application::draw_black_player_turn_indicator() const
{
    auto const margin = 10.f;
        
    auto const right = getWindowSize().x - margin;

    auto const y_center = getWindowCenter().y - board_size / 4.f;

    auto const top_left = cinder::Vec2f{getWindowCenter().x + board_size / 2.f + margin + 15.f, 
                                        y_center - 5.f};

    auto const bottom_right = cinder::Vec2f{right, y_center + 45.f};

    cinder::gl::color(cinder::ColorA{1.f, 1.f, 0.f, 1.f});

    cinder::gl::drawStrokedRoundedRect({top_left, bottom_right}, 5.0);

    cinder::gl::color(cinder::ColorA{1.f, 1.f, 1.f, 1.f});     
}

void application::draw_game_board() const
{
    draw_game_board_background();     
    
    draw_game_board_grid();
    
    draw_game_board_frame();    

    draw_tokens();

    draw_currently_hovered_cell();
}

void application::draw_game_board_background() const
{
    auto const top_left = get_game_board_origin();

    auto const bottom_right = top_left + cinder::Vec2f{board_size, board_size};
    
    cinder::gl::draw(game_board_picture, {top_left, bottom_right});    
}

void application::draw_game_board_grid() const
{
    cinder::gl::color(1.f, 1.f, 1.f);

    draw_game_board_grid_rows();

    draw_game_board_grid_columns();
}

void application::draw_game_board_grid_rows() const
{
    auto const cell_size = get_cell_size();

    auto const board_top_left = get_game_board_origin();

    auto const num_of_cells = current_game->get_board_size();

    for (auto const row : util::sequence(0, num_of_cells))
    {
        auto const cell_origin_y = board_top_left.y + row * cell_size;

        auto const from = cinder::Vec2f{board_top_left.x, cell_origin_y};

        auto const to = cinder::Vec2f{board_top_left.x + board_size, cell_origin_y};

        cinder::gl::drawLine(from, to);
    }    
}

void application::draw_game_board_grid_columns() const
{
    auto const cell_size = get_cell_size();

    auto const board_top_left = get_game_board_origin();

    auto const num_of_cells = current_game->get_board_size();

    for (auto const col : util::sequence(0, num_of_cells))
    {
        auto const cell_origin_x = board_top_left.x + col * cell_size;

        auto const from = cinder::Vec2f{cell_origin_x, board_top_left.y};

        auto const to = cinder::Vec2f{cell_origin_x, board_top_left.y + board_size};

        cinder::gl::drawLine(from, to);
    }    
}

void application::draw_game_board_frame() const
{
    auto const center = getWindowCenter();

    auto padding = 15.f;

    auto radius = cinder::Vec2f{board_size / 2.f + padding, board_size / 2.f + padding};

    auto const top_left = center - radius;

    auto const bottom_right = center + radius;
    
    cinder::gl::draw(game_board_frame_picture, {top_left, bottom_right});    
}

void application::draw_tokens() const
{
    auto const num_of_cells = current_game->get_board_size();

    for (auto const row : util::sequence(0, num_of_cells))
    {
        for (auto const col : util::sequence(0, num_of_cells))
        {
            draw_cell_content({row, col});
        }
    }
}

void application::draw_cell_content(cell_position const pos) const
{
    auto const mark = current_game->get_board_cell_mark(pos);
    if (!mark)
    {
        return;
    }

    draw_player_token(pos.row, pos.col, *mark);
}

void application::draw_player_token(int const row, int const col, player const p) const
{
    auto const cell_size = get_cell_size();

    auto const cell_origin = get_cell_origin(row, col);

    auto const margin = cell_size * 0.1f;

    auto const top_left = cell_origin + cinder::Vec2f{margin, margin};

    auto const bottom_right = cell_origin + cinder::Vec2f{cell_size - margin, cell_size - margin};

    draw_token_shape(p, {top_left, bottom_right});        
}

void application::draw_token_shape(player const p, cinder::Rectf const bounds) const
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

void application::draw_currently_hovered_cell() const
{
    if (game_over)
    {
        return;
    }

    auto const cell = get_currently_hovered_cell();
    if (cell)
    {
        cinder::gl::color(1.0, 0.f, 0.f, 1.f);

        cinder::gl::drawStrokedRoundedRect(get_cell_bounds(cell->row, cell->col), 5.f);

        cinder::gl::color(1.0, 0.f, 0.f, 1.f);
    }    
}

void application::draw_hint_message() const
{
    auto const center = getWindowCenter();

    auto const pos = cinder::Vec2f{center.x, center.y + board_size / 2.f + 25.f};

    auto const elapsed_seconds = cinder::app::getElapsedSeconds() - message_seconds;

    auto const transparency = ((elapsed_seconds < 2.0) || game_over) ? 1.f : 0.f;

    cinder::gl::drawStringCentered(hint_message, pos, {1.0, 1.0, 0.0, transparency}, message_font);
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
    auto const x_center = (getWindowCenter().x - board_size / 2) / 2;

    auto const y_center = getWindowCenter().y - board_size / 2.f + 10.f;

    auto const center = cinder::Vec2d{x_center - 5.f, y_center};

    auto const top_left = center - cinder::Vec2f{winner_picture.getSize()} / 4.5;

    auto const bottom_right = center + cinder::Vec2f{winner_picture.getSize()} / 4.5;

    cinder::gl::draw(winner_picture, {top_left, bottom_right});        
}

void application::draw_black_player_winner_indicator() const
{
    auto const x_center = (getWindowSize().x + (getWindowCenter().y + board_size)) / 2;

    auto const y_center = getWindowCenter().y - board_size / 2.f + 10.f;

    auto const center = cinder::Vec2d{x_center - 25.f, y_center};

    auto const top_left = center - cinder::Vec2f{winner_picture.getSize()} / 4.5;

    auto const bottom_right = center + cinder::Vec2f{winner_picture.getSize()} / 4.5;

    cinder::gl::draw(winner_picture, {top_left, bottom_right});    
}

void application::draw_game_over_label() const
{
    if (!game_over)
    {
        return;
    }

    auto const center = getWindowCenter();

    auto const label_center = center + cinder::Vec2f{0.f, center.y * 0.75f + 20.f};
    
    auto const top_left = label_center - cinder::Vec2f{game_over_picture.getSize()} / 2.f;

    auto const bottom_right = label_center + game_over_picture.getSize() / 2;

    cinder::gl::draw(game_over_picture, {top_left, bottom_right});    
}

void application::set_shown_hint_message(std::string msg)
{
    message_seconds = cinder::app::getElapsedSeconds();

    hint_message = std::move(msg);
}

cinder::Rectf application::get_cell_bounds(int const row, int const col) const
{
    auto const cell_size = get_cell_size();

    auto const top_left = get_cell_origin(row, col);

    auto const bottom_right = top_left + cinder::Vec2f{cell_size, cell_size};

    return {top_left, bottom_right};
}

cinder::Vec2f application::get_cell_origin(int const row, int const col) const
{
    auto const cell_size = get_cell_size();

    auto const board_top_left = get_game_board_origin();

    auto const cell_origin_x = board_top_left.x + col * cell_size;

    auto const cell_origin_y = board_top_left.y + row * cell_size;

    return {cell_origin_x, cell_origin_y};    
}

boost::optional<cell_position> application::get_currently_hovered_cell() const
{
    auto const mouse_position = getMousePos() - getWindowPos();

    auto const pos = get_cell_position_from_window_position(mouse_position);

    auto const game_size = current_game->get_board_size();

    if ((pos.row >= 0) && (pos.col >= 0) && (pos.row < game_size) && (pos.col < game_size))
    {
        return pos;
    }
    else
    {
        return boost::none;
    }
}

cinder::Vec2f application::get_game_board_origin() const
{
    auto const center = getWindowCenter();

    return (center - cinder::Vec2f{board_size / 2.f, board_size / 2.f});
}

cell_position application::get_cell_position_from_window_position(cinder::Vec2f const pos) const
{
    auto const board_origin = get_game_board_origin();

    auto const cell_size = get_cell_size();

    auto const relative_pos = (pos - board_origin);

    auto const cell_pos = (relative_pos / cell_size);

    return {cell_pos.y, cell_pos.x};
}

float application::get_cell_size() const
{
    return (board_size / current_game->get_board_size());
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

void application::on_placement(cell_position pos, 
                               player p, 
                               placement_outcome outcome, 
                               util::value_ref<std::vector<cell_position>> reversals)
{
    auto const scorer = (outcome == placement_outcome::turn_switched) ? get_opponent_of(p) : p;

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
            " Press F4, F6, F8, F10, or F12 to start a new game of the corresponding size...");
    }
    else
    {
        set_shown_hint_message("");
    }    
}

void application::show_turn_skipped_message()
{
    auto next_mover_name = current_game->get_player_name(next_mover);

    auto skipped_player_name = current_game->get_player_name(get_opponent_of(next_mover));

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
