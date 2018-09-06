#include "Board.h"

Board::Board() {
    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            array[y][x] = Player::None;
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
}
