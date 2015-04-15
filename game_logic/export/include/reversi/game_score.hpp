#pragma once

#include <ostream>

namespace reversi
{
    
enum class player;

struct game_score
{

public:

    game_score(int white, int black);

public:
    
    int white;

    int black;

};

bool operator == (game_score lhs, game_score rhs);

bool operator != (game_score lhs, game_score rhs);

std::ostream& operator << (std::ostream& out, game_score s);

game_score increase_score(game_score s, player p, int points);

}