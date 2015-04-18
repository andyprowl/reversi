#include "stdafx.hpp"

#include "reversi/hint_message_renderer.hpp"

namespace reversi
{

hint_message_renderer::hint_message_renderer(float const board_display_size)
    : board_display_size{board_display_size}
{
    create_fonts();
}

void hint_message_renderer::draw_hint(bool const game_over)
{
    auto const center = cinder::app::getWindowCenter();

    auto const pos = cinder::Vec2f{center.x, 
                                   center.y + board_display_size / 2.f + 25.f};

    auto const duration = cinder::app::getElapsedSeconds() - hint_timestamp;

    auto const transparency = ((duration < 2.0) || game_over) ? 1.f : 0.f;

    cinder::gl::drawStringCentered(hint, 
                                   pos, 
                                   {1.0, 1.0, 0.0, transparency}, 
                                   hint_font);
}

void hint_message_renderer::set_hint(std::string message)
{
    hint_timestamp = cinder::app::getElapsedSeconds();

    hint = std::move(message);
}

void hint_message_renderer::create_fonts()
{
    hint_font = cinder::Font{"Arial", 30.f};
}

}