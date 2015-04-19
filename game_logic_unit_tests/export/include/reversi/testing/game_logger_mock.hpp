#pragma once

#include "reversi/cell_position.hpp"
#include "reversi/game_logger.hpp"

namespace reversi { namespace testing
{

class game_logger_strict_mock : public game_logger
{
    
public:

    MOCK_METHOD2(log_successful_placement, void(cell_position, player));

    MOCK_METHOD1(log_turn_switched_message, void(player));

    MOCK_METHOD1(log_turn_skipped_message, void(player));

    MOCK_METHOD1(log_game_over_message, void(game const&));

};

using game_logger_mock = ::testing::NiceMock<game_logger_strict_mock>;

} }