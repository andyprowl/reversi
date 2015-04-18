#pragma once

#include <cinder/Font.h>

namespace reversi
{
    
class hint_message_renderer
{
    
public:

    explicit hint_message_renderer(float board_display_size);

    void draw_hint(bool game_over);

    void set_hint(std::string hint);

private:

    void create_fonts();

private:

    cinder::Font hint_font;

    float board_display_size;

    std::string hint;

    double hint_timestamp;

};

}