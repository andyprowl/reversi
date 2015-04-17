#pragma once

#include "reversi/cell_position.hpp"
#include "reversi/file_game_logger.hpp"
#include "reversi/game.hpp"
#include "reversi/game_board_renderer.hpp"
#include "reversi/game_score.hpp"
#include "reversi/player.hpp"
#include "reversi/player_info_renderer.hpp"
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

    virtual void mouseDown(cinder::app::MouseEvent e) override;

private:

    void disable_maximize_button() const;

    void setup_asset_directories() const;

    void load_pictures();

    void load_background_picture();

    void load_title_picture();

    void load_game_over_picture();

    void create_fonts();

    void start_new_game(int board_size);

    void draw_frame() const;

    void draw_background() const;

    void draw_title() const;

    void draw_game_board() const;

    void draw_player_info() const;

    void draw_hint_message() const;

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

    std::unique_ptr<game> current_game;

    std::unique_ptr<game_board_renderer> board_renderer;

    std::unique_ptr<player_info_renderer> player_renderer;

    boost::optional<cell_position> placement_target;

    cinder::gl::Texture background_picture;

    cinder::gl::Texture title_picture;

    cinder::gl::Texture game_over_picture;

    cinder::Font message_font;

    float board_display_size = 400.f;

    player next_mover = player::black;

    bool game_over = false;

    game_score score = {2, 2};

    double message_seconds = 0.0;

    std::string hint_message;

    file_game_logger logger;

};

}