#ifndef TWOKILLONE_GAMEAI_H
#define TWOKILLONE_GAMEAI_H

#include "Board.h"


class GameAI {
private:
    friend struct UCTComparator;

public:
    void think(const Board &, Player, int milliseconds);

    const Board::Position &getSrcPos() const { return srcPosition; }
    const Board::Position &getDestPos() const { return destPosition; }

private:
    struct Node;

private:
    Node *selectPromisingNode(Node *);

    void expandNode(Node *);

    Player simulate(const Node *);

    void randomMove(Board &board, Player player);

    void backPropagate(Node *, Player winningPlayer);

    void extractPositions(const Board &orgBoard, const Board &curBoard, Player player);

private:
    Board::Position srcPosition;
    Board::Position destPosition;
};


#endif //TWOKILLONE_GAMEAI_H
