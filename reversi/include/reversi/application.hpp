#pragma once

#include "reversi/file_game_logger.hpp"
#include "reversi/game.hpp"
#include "reversi/game_board_renderer.hpp"
#include "reversi/player.hpp"
#include <cinder/gl/Texture.h>
#include <cinder/Font.h>
#include <boost/optional.hpp>

namespace reversi
{
    
class application : public cinder::app::AppNative
{

public:

    application();

private:
    
    virtual void prepareSettings(Settings* settings) override;

    virtual void setup() override;
        
    virtual void update() override;
    
    virtual void draw() override;

    virtual void keyDown(cinder::app::KeyEvent e) override;

    virtual void mouseMove(cinder::app::MouseEvent e) override;

    virtual void mouseDown(cinder::app::MouseEvent e) override;

    virtual void mouseDrag(cinder::app::MouseEvent e) override;

    virtual void mouseWheel(cinder::app::MouseEvent e) override;

    virtual void resize() override;

private:

    void disable_maximize_button() const;

    void setup_asset_directories() const;

    void load_pictures();

    void load_background_picture();

    void load_title_picture();

    void load_winner_picture();

    void load_game_over_picture();

    void create_fonts();

    void start_new_game(int board_size);

    void draw_frame() const;

    void draw_background() const;

    void draw_title() const;

    void draw_game_board() const;

    void draw_white_player_info() const;

    void draw_white_player_name() const;

    void draw_white_player_score() const;

    void draw_black_player_info() const;

    void draw_black_player_name() const;

    void draw_black_player_score() const;

    void draw_turn_indicator() const;

    void draw_white_player_turn_indicator() const;

    void draw_black_player_turn_indicator() const;

    void draw_hint_message() const;

    void draw_winner_indicator() const;

    void draw_white_player_winner_indicator() const;

    void draw_black_player_winner_indicator() const;

    void draw_game_over_label() const;

    void set_shown_hint_message(std::string msg);

    void register_for_placement_notifications_from_current_game();

    void on_placement(cell_position pos, 
                      player p, 
                      placement_outcome outcome, 
                      util::value_ref<std::vector<cell_position>> reversals);

    void update_score(player scorer, int num_of_reversals);

    void process_placement_outcome(placement_outcome outcome);

    void show_turn_skipped_message();

    std::string get_game_result_description() const;

private:

    std::unique_ptr<game_board_renderer> board_renderer;

    cinder::gl::Texture background_picture;

    cinder::gl::Texture title_picture;

    cinder::gl::Texture winner_picture;

    cinder::gl::Texture game_over_picture;

    cinder::Font player_name_font;

    cinder::Font score_font;

    cinder::Font message_font;

    float board_display_size = 400.f;

    std::unique_ptr<game> current_game;

    player next_mover = player::black;

    bool game_over = false;

    int white_score = 2;

    int black_score = 2;

    double message_seconds = 0.0;

    std::string hint_message;

    file_game_logger logger;

};

}