#ifndef TWOKILLONE_BOARD_H
#define TWOKILLONE_BOARD_H

#include <stdexcept>


enum class Player {
    None, Black, White
};

inline Player nextPlayer(Player player) {
    switch (player) {
    case Player::Black:
        return Player::White;
    case Player::White:
        return Player::Black;
    default:
        throw std::logic_error("unreachable");
    }
}

class Board {
public:
    Board();

public:
    Player operator()(int y, int x) {
        return array[y][x];
    }

    void initBoard();

public:
    static const int SIZE = 4;

private:
    Player array[SIZE][SIZE];
};


#endif //TWOKILLONE_BOARD_H
