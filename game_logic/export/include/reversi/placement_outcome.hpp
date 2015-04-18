#pragma once

#include <cassert>

namespace reversi
{
    
enum class placement_outcome
{
    turn_switched,
    turn_skipped,
    game_over
};

inline std::ostream& operator << (std::ostream& out, 
                                  placement_outcome const outcome)
{
    switch (outcome)
    {
        default: { assert(false); } // Intentional fallthrough
        case placement_outcome::turn_switched: 
        { 
            return (out << "turn switched"); 
        }
        
        case placement_outcome::turn_skipped: 
        { 
            return (out << "turn skipped"); 
        }

        case placement_outcome::game_over: 
        { 
            return (out << "game over"); 
        }
    }
}

}