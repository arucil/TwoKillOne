#include "Board.h"
#include <stdio.h>


const Board::Delta Board::Directions[] = { { 0, -1 }, { 0, 1 }, { -1, 0 }, { 1, 0 } };

Board::Board() {
    for (auto &y : array) {
        for (auto &x : y) {
            x = Player::None;
        }
    }
}

void Board::initBoard() {
    for (int i = 0; i < SIZE; ++i) {
        array[0][i] = Player::Black;
        array[3][i] = Player::White;
    }
    array[1][0] = array[1][3] = Player::Black;
    array[2][0] = array[2][3] = Player::White;

    array[1][1] = array[1][2] = array[2][1] = array[2][2] = Player::None;
}

void Board::move(const Board::Position &srcPos, const Board::Position &destPos, Player player) {
    auto opponent = nextPlayer(player);

    array[destPos.y][destPos.x] = player;
    array[srcPos.y][srcPos.x] = Player::None;

    if (Player::None == array[destPos.y][0] && player == array[destPos.y][2]) {
        if (opponent == array[destPos.y][1] && player == array[destPos.y][3]) {
            array[destPos.y][1] = Player::None;
        } else if (opponent == array[destPos.y][3] && player == array[destPos.y][1]) {
            array[destPos.y][3] = Player::None;
        }
    } else if (Player::None == array[destPos.y][3] && player == array[destPos.y][1]) {
        if (opponent == array[destPos.y][0] && player == array[destPos.y][2]) {
            array[destPos.y][0] = Player::None;
        } else if (opponent == array[destPos.y][2] && player == array[destPos.y][0]) {
            array[destPos.y][2] = Player::None;
        }
    }

    if (Player::None == array[0][destPos.x] && player == array[2][destPos.x]) {
        if (opponent == array[1][destPos.x] && player == array[3][destPos.x]) {
            array[1][destPos.x] = Player::None;
        } else if (opponent == array[3][destPos.x] && player == array[1][destPos.x]) {
            array[3][destPos.x] = Player::None;
        }
    } else if (Player::None == array[3][destPos.x] && player == array[1][destPos.x]) {
        if (opponent == array[0][destPos.x] && player == array[2][destPos.x]) {
            array[0][destPos.x] = Player::None;
        } else if (opponent == array[2][destPos.x] && player == array[0][destPos.x]) {
            array[2][destPos.x] = Player::None;
        }
    }
}

bool Board::isWinning(Player player) const {
    auto opponent = nextPlayer(player);
    int num = 0;
    bool canMove = false;

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            if (array[y][x] == opponent) {
                ++num;
                for (auto &d : Directions) {
                    int x1 = x + d.x;
                    int y1 = y + d.y;
                    if (x1 >= 0 && x1 < SIZE && y1 >= 0 && y1 < SIZE && array[y1][x1] == Player::None) {
                        canMove = true;
                        break;
                    }
                }
            }
        }
    }

    // 只要至少有2个棋子且可以移动的棋子不少于1个就不算输
    return num < 2 || !canMove;

}