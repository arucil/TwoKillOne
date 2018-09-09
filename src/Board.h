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
    static const int SIZE = 4;

    struct Position {
        int x, y;
    };

    struct Delta {
        int x;
        int y;
    };

public:
    Board();

public:
    const Player &operator()(int x, int y) const {
        return array[y][x];
    }

    const Player &operator()(const Position &p) const {
        return array[p.y][p.x];
    }

    Player &operator()(int x, int y) {
        return array[y][x];
    }

    Player &operator()(const Position &p) {
        return array[p.y][p.x];
    }

    void initBoard();

    /**
     * 移动棋子. 负责吃棋子.
     * 不检查参数的position是否合法
     */
    void move(const Position &srcPos, const Position &destPos, Player player);

    bool isWinning(Player) const;

private:
    static const Delta Directions[4];

private:
    Player array[SIZE][SIZE];
};


#endif //TWOKILLONE_BOARD_H
