#ifndef TWOKILLONE_AI_WORKER_H
#define TWOKILLONE_AI_WORKER_H

#include "Board.h"

struct RequestData {
    Board board;
    Player player;
    int milliseconds;
};

struct ResponseData {
    Board::Position srcPos;
    Board::Position destPos;
};


#endif //TWOKILLONE_AI_WORKER_H
