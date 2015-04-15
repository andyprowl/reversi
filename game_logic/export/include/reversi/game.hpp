#pragma once

namespace reversi
{

struct score;
    
class game
{
    
public:

    game(int board_size);

    int get_board_size() const;

    score get_score() const;

private:

    int board_size;

};

}